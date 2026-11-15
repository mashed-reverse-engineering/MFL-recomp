// cluster-entries: _RwaWaveConvGCNADPCMEncodeHeader
/* Installed cluster _RwaWaveConvGCNADPCMEncodeHeader (tools/cluster_install.py from output/cluster_pilot_auw9/_RwaWaveConvGCNADPCMEncodeHeader.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) wave conversion: MFL.exe 0x005c5d20 writes the 0x60-byte GameCube DSP ADPCM header into a
   wave format's misc data block (register function: wave format in eax, data size in ecx).  Big-endian fields: sample count,
   nibble count, the 16 decoder coefficients (the table 0x64a9c0 x 2048), the initial history pair.  Only caller: the VAG -> GCN ADPCM
   converter 0x005c7280 (wave converter table 0x64aad8).  Written from the MFL.exe disassembly (tools/disasm_va.py). */
#include "mfl_tier2.h"

/*005bcd50*/ extern void _rwaWaveFormatFreeMiscData(void *format);
/*005bf0d0*/ extern void *rwa_malloc_005bf0d0(RwUInt32 size, RwUInt32 hint);
/*005be600*/ extern RwInt32 RwaUUIDCompare(const void *a, const void *b);

/*005f7914*/ extern undefined g_data_5f7914;      /* VAG wave format uuid */
/*005f7924*/ extern undefined g_data_5f7924;      /* PCM wave format uuid */
/*0064a9c0*/ extern RwReal g_data_64a9c0[16];    /* ADPCM decoder coefficients */

typedef struct RwaGCNWaveFormat {
  char pad00[0x10];
  RwUInt32 *misc;        /* +0x10 misc data block */
  RwUInt32 miscSize;     /* +0x14 */
} RwaGCNWaveFormat;

/* RenderWare's own inline-assembly real -> integer conversion (truncating fistp, 0x80000000 for NaN / out of range), inlined by
   the exe; see recomp/cheat_allow.txt (RwFastRealToUInt32_r; same copy as an_h_patchengine.c). */
static RwUInt32 RwFastRealToUInt32_r(RwReal x)
{
#if defined(NOASM)
  return (x >= -2147483648.0f && x < 2147483648.0f) ? (RwUInt32)(RwInt32)x : 0x80000000u;
#else
  RwUInt16 savemode;
  RwUInt16 workmode;
  RwInt32 res;

  __asm
  {
    fnstcw savemode
    fld dword ptr [x]
    mov ax, savemode
    or ah, 0ch
    mov workmode, ax
    fldcw workmode
    fistp dword ptr [res]
    fldcw savemode
  }
  return (RwUInt32)res;
#endif
}

#define SWAP32(v) (((((v) & 0xff0000) | ((v) >> 16)) >> 8) | ((((v) & 0xff00) | ((v) << 16)) << 8))
#define SWAP16(v) ((RwUInt16)((((v) >> 8) & 0xff) | (((v) & 0xff) << 8)))

/* header size 0x60, or 0 when the misc block cannot be allocated */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 _RwaWaveConvGCNADPCMEncodeHeader_n(RwaGCNWaveFormat *wave, RwUInt32 size, RwUInt16 *history, const char *format)
{
  RwUInt32 *hdr, samples = 0, nibbles;
  RwUInt16 *coef;
  RwUInt32 i;

  if (wave->miscSize < 0x60)
    _rwaWaveFormatFreeMiscData(wave);
  if (!wave->misc) {
    wave->misc = rwa_malloc_005bf0d0(0x60, 0x30806);
    if (!wave->misc)
      return 0;
    wave->miscSize = 0x60;
  }
  hdr = wave->misc;
  for (i = 0; i < 0x18; i++)
    hdr[i] = 0;
  if (!RwaUUIDCompare(*(void **)(format + 4), &g_data_5f7914))
    samples = size * 7 >> 2;
  else if (!RwaUUIDCompare(*(void **)(format + 4), &g_data_5f7924)) {
    if (*(RwUInt8 *)(format + 0xc) == 16)
      samples = size >> 1;
    else if (*(RwUInt8 *)(format + 0xc) == 8)
      samples = size;
  }
  hdr[0] = SWAP32(samples);
  nibbles = (samples / 14 * 16 + samples % 14 + 1) & ~1u;
  hdr[1] = SWAP32(nibbles);
  coef = (RwUInt16 *)((char *)hdr + 0x1c);
  for (i = 0; i < 16; i++) {
    RwUInt32 c = RwFastRealToUInt32_r(g_data_64a9c0[i] * 2048.0f);
    coef[i] = SWAP16(c);
  }
  *(RwUInt16 *)((char *)hdr + 0x40) = SWAP16(history[0]);
  *(RwUInt16 *)((char *)hdr + 0x42) = SWAP16(history[1]);
  return 0x60;
}

/* entry thunk at the original address of _RwaWaveConvGCNADPCMEncodeHeader: register inputs (eax,ecx,s0,s1) -> natural C _RwaWaveConvGCNADPCMEncodeHeader_n; ret 0 */
__declspec(naked) void _RwaWaveConvGCNADPCMEncodeHeader(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ecx
        push eax
        call _RwaWaveConvGCNADPCMEncodeHeader_n
        add esp, 16
        ret
    }
}
