/* RenderWare D3D9 driver: the three x87 control word routines of _rwcseg (0x5dc404..0x5dc450).  Direct3D 9 sets the FPU to single
   precision on device creation unless FPU_PRESERVE is passed, so the driver does the same itself: D3D9System saves the control word and
   sets 24 bit precision with every exception masked, rwD3D9CameraBeginUpdate sets it again each frame, D3D9DeviceSystemStop restores the
   saved word.  Written from the disassembly of MFL.exe.  The bind tables (an_hk_612_d3d9sys.inc / an_hk_612_d3d9dev2.inc) still name the
   entries FUN_005dc404 / FUN_005dc42b / FUN_005dc432, which resolve to the entry jmps at those addresses.
   Both words are dwords: fnstcw writes the low half only, the original copies and masks the whole dword. */
#include "mfl_tier2.h"

/*0094a000*/ extern RwUInt32 rwD3D9FpuControlWord;        /* the control word the driver runs with (low half) */
/*0094a004*/ extern RwUInt32 rwD3D9FpuSavedControlWord;   /* the application's control word, saved at start-up (low half) */

#define FPU_PRECISION_MASK 0x300     /* PC bits; 00 = 24 bit (single) precision */
#define FPU_EXCEPTION_MASKS 0x3f     /* IM DM ZM OM UM PM: every exception masked */

/* The lane's one x87 helper (inline asm; recomp/cheat_allow.txt mfl_x87_cw_r): returns the control word in force (fwait + fnstcw), then
   loads *load (fldcw) when load is not NULL.
   Port note (plan W8 Q4): ARM has no precision-control bits, so the PC / exception-mask part has no equivalent there; the rounding-control
   bits (0xc00) do (FPCR RMode) and must be mapped, not dropped, by any caller that relies on them.  The NOASM branch only tracks the value. */
static RwUInt16 mfl_x87_cw_r(const RwUInt16 *load)
{
  RwUInt16 cw;
#if defined(NOASM)
  static RwUInt16 soft_cw = 0x27f;
  cw = soft_cw;
  if (load != NULL) soft_cw = *load;
#else
  __asm
  {
    fstcw cw
    mov eax, load
    test eax, eax
    jz done
    fldcw word ptr [eax]
  done:
  }
#endif
  return cw;
}

/* 0x5dc404: saves the control word in rwD3D9FpuSavedControlWord and switches to single precision, exceptions masked. */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void rwD3D9FpuSaveAndSetSingle(void)
{
  *(RwUInt16 *)&rwD3D9FpuControlWord = mfl_x87_cw_r(NULL);
  rwD3D9FpuSavedControlWord = rwD3D9FpuControlWord;
  rwD3D9FpuControlWord = (rwD3D9FpuControlWord & ~FPU_PRECISION_MASK) | FPU_EXCEPTION_MASKS;
  mfl_x87_cw_r((RwUInt16 *)&rwD3D9FpuControlWord);
}

/* 0x5dc42b: restores the saved control word. */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void rwD3D9FpuRestore(void)
{
  mfl_x87_cw_r((RwUInt16 *)&rwD3D9FpuSavedControlWord);
}

/* 0x5dc432: switches to single precision, exceptions masked (without saving). */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void rwD3D9FpuSetSingle(void)
{
  *(RwUInt16 *)&rwD3D9FpuControlWord = mfl_x87_cw_r(NULL);
  rwD3D9FpuControlWord = (rwD3D9FpuControlWord & ~FPU_PRECISION_MASK) | FPU_EXCEPTION_MASKS;
  mfl_x87_cw_r((RwUInt16 *)&rwD3D9FpuControlWord);
}
