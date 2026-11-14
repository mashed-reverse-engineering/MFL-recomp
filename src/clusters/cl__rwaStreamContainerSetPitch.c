// cluster-entries: _rwaStreamContainerSetPitch
/* Installed cluster _rwaStreamContainerSetPitch (tools/cluster_install.py from output/cluster_pilot_auw8c/_rwaStreamContainerSetPitch.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) stream container: MFL.exe 0x005c0220 sets the pitch of a stream and pushes the resulting
   sample rate to every voice of the stream (register function: container in eax, stream in ebx).  Written from the MFL.exe
   disassembly (tools/disasm_va.py). */
#include "mfl_tier2.h"

typedef struct RwaPitchLink { struct RwaPitchLink *next, *prev; } RwaPitchLink;

/*005c1f10*/ extern void RwaObjIntSetParamBool(void *obj, void *param, RwUInt32 flags, RwUInt32 value);

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

/* stream + 0x34 = the container's pitch (+0x10); for every voice of the stream's list (head at stream + 0x20, link at voice + 0x2c)
   the matching wave format of the stream's codec (+0x0c -> +0x18, 0x28-byte records from +0x14, sample rate first) times the
   pitch goes to the voice's rate parameter (+0x58) */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void _rwaStreamContainerSetPitch_n(char *container, char *stream)
{
  RwaPitchLink *head = (RwaPitchLink *)(stream + 0x20);
  RwaPitchLink *link;
  char *codec = *(char **)(stream + 0xc);
  RwUInt32 **rate;
  char *voice;
  RwReal value;

  link = head->next;      /* (read order as in the original: list head and codec before the store, the codec's table after) */
  *(RwUInt32 *)(stream + 0x34) = *(RwUInt32 *)(container + 0x10);      /* the pitch word, copied as is */
  rate = (RwUInt32 **)(*(char **)(codec + 0x18) + 0x14);
  for (; link != head; link = link->next) {
    value = (RwReal)**rate * *(RwReal *)(stream + 0x34);
    rate = (RwUInt32 **)((char *)rate + 0x28);
    voice = (char *)link - 0x2c;
    RwaObjIntSetParamBool(voice, *(void **)(voice + 0x58), 0, RwFastRealToUInt32_r(value));
  }
}

/* entry thunk at the original address of _rwaStreamContainerSetPitch: register inputs (eax,ebx) -> natural C _rwaStreamContainerSetPitch_n; ret 0 */
__declspec(naked) void _rwaStreamContainerSetPitch(void)
{
    __asm {
        push ebx
        push eax
        call _rwaStreamContainerSetPitch_n
        add esp, 8
        ret
    }
}
