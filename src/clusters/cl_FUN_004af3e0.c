// cluster-entries: FUN_004af3e0
/* Installed cluster FUN_004af3e0 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004af3e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004b2a30*/ extern void __stdcall FUN_004b2a30(void *item);

/* Walks the COM enumerator `enumerator` (Reset = vtable +0x14, Next = +0xc): for each item passing the optional `filter` (FUN_004b2880) tries FUN_004af330(this, source, item) and releases the item (FUN_004b2a30);
   returns 0 on the first success, else the first error that is not one of the "expected" ones (0x80004005, 0x80070057, 0x8004022a), else 0x80040207. */
static int FUN_004af330_r(void * p0, int p1, int p2);
extern void FUN_004af330(void);
static int FUN_004b2880_r(void * p0, int p1);
extern void FUN_004b2880(void);
int FUN_004af3e0_n(void *this_, int source, int filter, int *enumerator)
{
  void *item;
  int fetched;
  int first_error;
  int result;
  int next;

  result = ((int (__stdcall *)(void *))(*(int **)enumerator)[5])(enumerator);
  if (result < 0) {
    return result;
  }
  item = 0;
  fetched = 0;
  first_error = 0;
  next = ((int (__stdcall *)(void *, int, void **, int *))(*(int **)enumerator)[3])(enumerator, 1, &item, &fetched);
  while (next == 0) {
    if (filter != 0 && FUN_004b2880_r(item, filter) == 0) {
      result = 0x80040207;
    } else {
      result = FUN_004af330_r(this_, source, (int)item);
      if (result < 0 && first_error >= 0 && result != 0x80004005 && result != 0x80070057 && result != 0x8004022a) {
        first_error = result;
      }
    }
    FUN_004b2a30(item);
    if (result == 0) {
      return 0;
    }
    next = ((int (__stdcall *)(void *, int, void **, int *))(*(int **)enumerator)[3])(enumerator, 1, &item, &fetched);
  }
  if (first_error == 0) {
    first_error = 0x80040207;
  }
  return first_error;
}

/* FUN_004af330_r: C -> original FUN_004af330 (registers ecx,s0,s1) */
__declspec(naked) int FUN_004af330_r(void * p0, int p1, int p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 12]
        call FUN_004af330
        ret
    }
}

/* FUN_004b2880_r: C -> original FUN_004b2880 (registers ecx,s0) */
__declspec(naked) int FUN_004b2880_r(void * p0, int p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 8]
        call FUN_004b2880
        ret
    }
}

/* entry thunk at the original address of FUN_004af3e0: register inputs (ecx,s0,s1,s2) -> natural C FUN_004af3e0_n; ret 12 */
__declspec(naked) void FUN_004af3e0(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        call FUN_004af3e0_n
        add esp, 16
        ret 12
    }
}
