/* RenderWare Audio (Criterion Software) DirectSound module rows, MFL.exe 0x005cdc20 (_rwaDSOpen), 0x005d09e0 (AddRef of the
 * library's IUnknown object) and 0x005d1ea0 (the shared COM "not implemented" method).  Written from the MFL.exe disassembly
 * (capstone).  The object's create 0x005d0980 references its vtable (trapped methods): an_rwa_ds_unknown_create.c (G2, T-TCA). */
#include "mfl_tier2.h"

extern __declspec(dllimport) long __stdcall InterlockedIncrement(long *addend);

/*005beb70*/ extern void *rwa_heap_create_005beb70(int kind, int block, int count, int flags, void *heap);
/*007dc184*/ extern undefined g_data_7dc184;      /* the DirectSound module heap */

/* 1: the module heap could be created */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool _rwaDSOpen(void)
{
  return rwa_heap_create_005beb70(0x12, 8, 1, 0, &g_data_7dc184) != 0;
}

/* IUnknown::AddRef: the count at +4 */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
long __stdcall _rwaDSUnknownAddRef(char *unknown)
{
  return InterlockedIncrement((long *)(unknown + 4));
}

/* E_NOTIMPL for every three-argument COM method the library does not implement (slot 13 of its DirectShow / DirectSound
   vtables) */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
long __stdcall rwa_com_not_implemented_005d1ea0(void *self, void *arg_a, void *arg_b)
{
  return 0x80004001;
}
