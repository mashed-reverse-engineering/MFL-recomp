/* RenderWare Audio (Criterion Software) DirectSound module: the library's IUnknown object create, MFL.exe 0x005d0980.  Written
 * from the MFL.exe disassembly (capstone).  Its vtable 0x64bfb0 holds trapped methods: T-TCA lines in rows_tca_g2.txt. */
#include "mfl_tier2.h"

/*005bf0d0*/ extern void *rwa_malloc_005bf0d0(RwUInt32 size, RwUInt32 hint);
/*005d09d0*/ extern void RpCollisionBuildParamInitDefaults(void *words3);      /* clears three words (shared body) */
/*005d09e0*/ extern long __stdcall _rwaDSUnknownAddRef(char *unknown);
/*0064bfb0*/ extern undefined g_rwa_ds_unknown_vtbl;

/* initialises the IUnknown object at unknown, or allocates one of size bytes (12 when 0) and marks it as owned (+8 = 1);
   holds one reference.  NULL when the allocation failed. */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
char *_rwaDSUnknownCreate(char *unknown, RwUInt32 size)
{
  RwUInt32 owned = 0;

  if (!unknown) {
    if (!size)
      size = 0xc;
    unknown = rwa_malloc_005bf0d0(size, 0x30800);
    owned = 1;
    if (!unknown)
      return 0;
  }
  RpCollisionBuildParamInitDefaults(unknown);
  *(void **)unknown = &g_rwa_ds_unknown_vtbl;
  *(RwUInt32 *)(unknown + 8) = owned;
  _rwaDSUnknownAddRef(unknown);
  return unknown;
}
