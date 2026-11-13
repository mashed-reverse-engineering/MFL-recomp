/* RenderWare Audio (Criterion Software) stream source: re-interleave of a filled input block into the output block ring,
 * MFL.exe 0x005c22d0 (_RwaStreamSourceReinterleave).  Written from the MFL.exe disassembly (objdump); name from the symbol table.
 * Stream object layout used here: +0xec byte count of a short block, +0xf0 owner tag copied into the output block, +0x118 source
 * description, input ring +0x144 data / +0x148 blocks / +0x154, +0x158 pending / +0x15c read index / +0x164 size, output ring
 * +0x168 data / +0x16c blocks / +0x174 write index / +0x17c size, +0x18c the input ring's lock (semaphore).
 * Source description: +0x14 sub-stream count, +0x18 sub-stream table, +0x20 bytes per ring frame. */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef struct RwaStreamBlock {      /* 20-byte entry of a block ring */
  RwUInt32 id[2];
  RwUInt32 owner;
  RwUInt32 position;      /* 32-bit fraction of the block already consumed (input) / filled (output) */
  RwUInt32 state;         /* input: bit 2 = holds data, bit 0x10 = short block; 1 = free.  output: 2 = complete */
} RwaStreamBlock;

typedef struct RwaSubStream {      /* 0x28-byte entry of the source's sub-stream table */
  RwUInt8 pad00[0x14];
  RwUInt8 *format;            /* byte +0xd: channel count */
  RwUInt16 blockBytes;        /* bytes of one channel block */
  RwUInt16 unitBytes;         /* bytes of one interleave unit */
  RwUInt16 blockCount;        /* channel blocks per ring frame */
  RwUInt16 pad1e;
  RwUInt32 pad20;
  RwUInt32 offset;            /* byte offset of the sub-stream inside a ring frame */
} RwaSubStream;

/*005bf4e0*/ extern void RwaMul32By32(RwUInt32 a, RwUInt32 b, RwUInt32 *high, RwUInt32 *low);

#define U32(p, off) (*(RwUInt32 *)((char *)(p) + (off)))
#define PTR(p, off) (*(char **)((char *)(p) + (off)))

/* Copies what both the current input block and the current output block can still take, sub-stream by sub-stream and channel by
   channel, in interleave units; positions are rounded up to whole units.  After the last sub-stream the output block is closed
   (complete: write index advances) or its fill fraction stored, and the input block is freed under the ring lock (consumed) or
   its consumed fraction stored.  Repeats until an output block is complete (returns its data) or the input block at the read
   index holds no data (returns 0).  The unit width picks the copy width: even units go as 16-bit words, odd ones as bytes
   (the 4 / 8 / 16 byte paths of the original cannot be reached after the odd / even test and are kept as written). */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *_RwaStreamSourceReinterleave(char *stream)
{
  char *source = PTR(stream, 0x118);
  RwUInt32 frameBytes = U32(source, 0x20), count = U32(source, 0x14);
  char *result = 0;
  RwUInt32 inDone, outDone;      /* set at the last sub-stream: nothing left in the input block / no room left in the output block */
  RwUInt32 scratch;

  do {
    RwaStreamBlock *in, *out;
    char *inData, *outData;
    RwUInt32 inPos, outPos, i;

    if (!(((RwaStreamBlock *)PTR(stream, 0x148))[U32(stream, 0x15c)].state & 2)) return result;
    inData = PTR(stream, 0x144) + U32(stream, 0x15c) * frameBytes;
    outData = PTR(stream, 0x168) + U32(stream, 0x174) * frameBytes;
    in = (RwaStreamBlock *)PTR(stream, 0x148) + U32(stream, 0x15c);
    out = (RwaStreamBlock *)PTR(stream, 0x16c) + U32(stream, 0x174);
    outPos = out->position;
    inPos = in->position;

    for (i = 0; i < count; i++) {
      RwaSubStream *sub = (RwaSubStream *)PTR(source, 0x18) + i;
      RwUInt8 channels = sub->format[0xd];
      RwUInt32 block = sub->blockBytes, unit = sub->unitBytes, units = unit, mode;
      RwUInt32 total, inEnd, inOff, outOff, length, inRow, outRow, inRem, outRem, skip, channel;
      char *src, *dst;

      if (!(unit & 1)) { mode = 2; units >>= 1; }
      else if (!(unit & 3)) { mode = 3; units >>= 2; }
      else if (!(unit & 7)) { mode = 4; units >>= 3; }
      else if (!(unit & 15)) { mode = 5; units >>= 4; }
      else mode = 1;

      total = sub->blockCount * block;
      RwaMul32By32(inPos, total, &inOff, &scratch);
      RwaMul32By32(outPos, total, &outOff, &scratch);
      if (in->state & 0x10) {
        RwaMul32By32(U32(stream, 0xec), total, &inEnd, &scratch);
        inEnd = (inEnd + unit - 1) / unit * unit;
      } else inEnd = total;
      inOff = (inOff + unit - 1) / unit * unit;
      outOff = (outOff + unit - 1) / unit * unit;
      length = inEnd - inOff;
      if (length > total - outOff) length = total - outOff;
      if (i == count - 1) {
        inDone = inEnd == inOff + length;
        outDone = total == outOff + length;
      }

      inRow = inOff / block;
      inRem = (RwUInt16)(inOff - inRow * block);
      outRow = (RwUInt16)(outOff / block);
      outRem = outOff - outRow * block;
      skip = (channels - 1) * block;      /* a channel's next block lies behind the other channels' blocks */
      src = inData + (RwUInt16)inRow * channels * block + sub->offset + inRem;
      dst = outData + channels * outRow * block + sub->offset + outRem;

      for (channel = channels; channel != 0; channel--) {
        char *s = src, *d = dst;
        RwUInt32 inLeft = block - inRem, outLeft = block - outRem, left = length, k;

        while ((RwUInt16)left != 0) {
          if ((RwUInt16)inLeft == 0) { inLeft = block; s += skip; }
          if ((RwUInt16)outLeft == 0) { outLeft = block; d += skip; }
          switch (mode) {
          case 1:
            for (k = 0; k < units; k++) *d++ = *s++;
            break;
          case 2:
            for (k = 0; k < units; k++) { *(RwUInt16 *)d = *(RwUInt16 *)s; s += 2; d += 2; }
            break;
          case 3:
            for (k = 0; k < units; k++) { *(RwUInt32 *)d = *(RwUInt32 *)s; s += 4; d += 4; }
            break;
          case 4:
            for (k = 0; k < units; k++) {
              ((RwUInt32 *)d)[0] = ((RwUInt32 *)s)[0]; ((RwUInt32 *)d)[1] = ((RwUInt32 *)s)[1];
              s += 8; d += 8;
            }
            break;
          case 5:
            for (k = 0; k < units; k++) {
              ((RwUInt32 *)d)[0] = ((RwUInt32 *)s)[0]; ((RwUInt32 *)d)[1] = ((RwUInt32 *)s)[1];
              ((RwUInt32 *)d)[2] = ((RwUInt32 *)s)[2]; ((RwUInt32 *)d)[3] = ((RwUInt32 *)s)[3];
              s += 16; d += 16;
            }
            break;
          }
          outLeft -= unit;
          left -= unit;
          inLeft -= unit;
        }
        if ((RwUInt16)unit > (RwUInt16)block) { src += length; dst += length; }
        else { src += block; dst += block; }
      }

      if (i == count - 1) {
        out->id[0] = in->id[0];
        out->id[1] = in->id[1];
        out->owner = U32(stream, 0xf0);
        if (outDone) {
          out->position = 0;
          out->state = 2;
          U32(stream, 0x174)++;
          result = outData;
          if (U32(stream, 0x174) == U32(stream, 0x17c)) U32(stream, 0x174) = 0;
        } else {
          RwaMul32By32(outOff + length, 0xffffffff / total, &scratch, &out->position);
          out->position++;
        }
        if (inDone) {
          RwUInt32 index, next;

          WaitForSingleObject(PTR(stream, 0x18c), 0xffffffff);
          in->position = 0;
          in->state = 1;
          index = U32(stream, 0x15c);
          next = index + 1;
          if (next == U32(stream, 0x164)) next = 0;
          U32(stream, 0x15c) = next;
          if (U32(stream, 0x154) == index) {
            U32(stream, 0x154) = next;
            U32(stream, 0x158) = next;
          }
          ReleaseSemaphore(PTR(stream, 0x18c), 1, 0);
        } else {
          RwaMul32By32(inOff + length, 0xffffffff / total, &scratch, &in->position);
          in->position++;
        }
      }
    }
  } while (result == 0);
  return result;
}
