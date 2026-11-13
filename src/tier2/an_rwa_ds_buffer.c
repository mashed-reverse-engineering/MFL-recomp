/* RenderWare Audio (Criterion Software) DirectSound buffer helpers, MFL.exe 0x005cd3e0-0x005cd550: play state and the
 * lock / copy / unlock transfers into and out of a (circular) sound buffer.  Written from the MFL.exe disassembly (objdump);
 * names from the symbol table.  The buffer object's first word is its IDirectSoundBuffer. */
#include "mfl_tier2.h"

typedef struct DSBuffer DSBuffer;
struct DSBuffer { const struct DSBufferVtbl *vtbl; };
struct DSBufferVtbl {      /* IDirectSoundBuffer: only the slots used here are typed */
  void *slot0_8[9];
  RwInt32 (__stdcall *GetStatus)(DSBuffer *self, RwUInt32 *status);                                           /* +0x24 */
  void *slot10;
  RwInt32 (__stdcall *Lock)(DSBuffer *self, RwUInt32 offset, RwUInt32 bytes, void **part1, RwUInt32 *bytes1,
                            void **part2, RwUInt32 *bytes2, RwUInt32 flags);                                  /* +0x2c */
  void *slot12_18[7];
  RwInt32 (__stdcall *Unlock)(DSBuffer *self, void *part1, RwUInt32 bytes1, void *part2, RwUInt32 bytes2);    /* +0x4c */
};

/* 1 while the buffer plays (DSBSTATUS_PLAYING), 0 when it does not or the status call fails. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwUInt32 _rwaDSBufferIsPlaying(DSBuffer **buffer)
{
  RwUInt32 status = 0;

  if ((*buffer)->vtbl->GetStatus(*buffer, &status) < 0) return 0;
  return status & 1;
}

/* Copies `bytes` from `data` into the buffer at `offset` (two parts when the range wraps).  1: ok, 0: lock or unlock failed. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool _rwaDSBufferUpload(DSBuffer **buffer, RwUInt32 offset, const char *data, RwUInt32 bytes)
{
  void *part1, *part2;
  RwUInt32 bytes1, bytes2;

  if ((*buffer)->vtbl->Lock(*buffer, offset, bytes, &part1, &bytes1, &part2, &bytes2, 0) < 0) return 0;
  memcpy(part1, data, bytes1);
  if (bytes2) memcpy(part2, data + bytes1, bytes2);
  return (*buffer)->vtbl->Unlock(*buffer, part1, bytes1, part2, bytes2) >= 0;
}

/* Copies `bytes` at `offset` out of the buffer into `data`.  1: ok, 0: lock or unlock failed. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool _rwaDSBufferDownload(DSBuffer **buffer, RwUInt32 offset, char *data, RwUInt32 bytes)
{
  void *part1, *part2;
  RwUInt32 bytes1, bytes2;

  if ((*buffer)->vtbl->Lock(*buffer, offset, bytes, &part1, &bytes1, &part2, &bytes2, 0) < 0) return 0;
  memcpy(data, part1, bytes1);
  if (bytes2) memcpy(data + bytes1, part2, bytes2);
  return (*buffer)->vtbl->Unlock(*buffer, part1, bytes1, part2, bytes2) >= 0;
}
