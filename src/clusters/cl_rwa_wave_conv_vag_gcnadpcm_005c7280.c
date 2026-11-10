// cluster-entries: rwa_wave_conv_vag_gcnadpcm_005c7280
/* Installed cluster rwa_wave_conv_vag_gcnadpcm_005c7280 (tools/cluster_install.py from output/cluster_pilot_a4/rwa_wave_conv_vag_gcnadpcm_005c7280.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio wave conversion (MFL.exe 0x005c7280, rwa.dll unit 249490): _RwaWaveConvVAGGCNADPCM (console format, no register
   input; a cluster only because it calls the register function _RwaWaveConvGCNADPCMEncodeHeader 0x005c5d20 (eax, ecx), which stays
   original and is called through the generated adapter).  Reports the header size, and with a destination buffer clears the data part,
   writes the header and converts the samples.  Returns the converted size (channels x size), 0 when the header fails.
   Written from the MFL.exe disassembly (tools/disasm_va.py). */
#include "mfl_tier2.h"

void *memset(void *dst, int value, size_t size);
#pragma intrinsic(memset)      /* the original clears inline (rep stosd / stosb) */

/*005c6990*/ extern void rwa_unit249490_005c6990(const RwUInt8 *src, RwUInt32 size, RwUInt8 *dst);

static RwInt32 _RwaWaveConvGCNADPCMEncodeHeader_r(void * p0, RwUInt32 p1, RwUInt16 * p2, const char * p3);
extern void _RwaWaveConvGCNADPCMEncodeHeader(void);
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwUInt32 rwa_wave_conv_vag_gcnadpcm_005c7280(const char *format, const RwUInt8 *src, RwUInt32 headerSize, void *ctx, RwUInt8 *dst, RwUInt32 *outHeaderSize)
{
  RwUInt32 size = *(RwUInt32 *)(format + 8), total;
  RwUInt32 local;
  RwUInt16 state[2];

  if (!outHeaderSize)
    outHeaderSize = &local;
  *outHeaderSize = headerSize;
  total = *(RwUInt8 *)(format + 0xd) * size;
  if (!dst)
    return total;
  state[0] = 0;
  state[1] = 0;
  memset(dst + headerSize, 0, size);
  if (!_RwaWaveConvGCNADPCMEncodeHeader_r(ctx, size - headerSize, state, format))
    return 0;
  rwa_unit249490_005c6990(src + headerSize, size - headerSize, dst + headerSize);
  return total;
}

/* _RwaWaveConvGCNADPCMEncodeHeader_r: C -> original _RwaWaveConvGCNADPCMEncodeHeader (registers eax,ecx,s0,s1) */
__declspec(naked) RwInt32 _RwaWaveConvGCNADPCMEncodeHeader_r(void * p0, RwUInt32 p1, RwUInt16 * p2, const char * p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 16]
        call _RwaWaveConvGCNADPCMEncodeHeader
        add esp, 8
        ret
    }
}
