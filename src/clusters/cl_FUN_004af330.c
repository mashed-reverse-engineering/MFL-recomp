// cluster-entries: FUN_004af330
/* Installed cluster FUN_004af330 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004af330.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

typedef int (__fastcall *ThisCall0)(void *this_);
typedef int (__fastcall *ThisCall1)(void *this_, int edx_unused, int a);

/* Binds the stream `source` (COM object, held with AddRef in this[6]) with parameter `param`: virtual methods +0x28 (check), +0x20 (prepare), +0x24, source->Connect (vtable +0x10 with this + 0xc) and +0x30;
   any failure after the binding runs the cleanup (+0x2c, Release of this[6]).  Prepare errors are mapped to 0x8004022a.  Returns the HRESULT. */
int FUN_004af330_n(int *this_, void *source, int param)
{
  int result;
  int *com;

  result = ((ThisCall1)(*(int **)this_)[10])(this_, 0, (int)source);
  if (result < 0) {
    ((ThisCall0)(*(int **)this_)[11])(this_);
    return result;
  }
  result = ((ThisCall1)(*(int **)this_)[8])(this_, 0, param);
  if (result == 0) {
    this_[6] = (int)source;
    ((int (__stdcall *)(void *))(*(int **)source)[1])(source);
    result = ((ThisCall1)(*(int **)this_)[9])(this_, 0, param);
    if (result >= 0) {
      result = ((int (__stdcall *)(void *, void *, int))(*(int **)source)[4])(source, this_ + 3, param);
      if (result >= 0) {
        result = ((ThisCall1)(*(int **)this_)[12])(this_, 0, (int)source);
        if (result >= 0) {
          return result;
        }
        ((int (__stdcall *)(void *))(*(int **)source)[5])(source);
      }
    }
  } else if (result >= 0 || result == 0x80004005 || result == 0x80070057) {
    result = 0x8004022a;
  }
  ((ThisCall0)(*(int **)this_)[11])(this_);
  if (this_[6] != 0) {
    com = (int *)this_[6];
    ((int (__stdcall *)(void *))(*(int **)com)[2])(com);
    this_[6] = 0;
  }
  return result;
}

/* entry thunk at the original address of FUN_004af330: register inputs (ecx,s0,s1) -> natural C FUN_004af330_n; ret 8 */
__declspec(naked) void FUN_004af330(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ecx
        call FUN_004af330_n
        add esp, 12
        ret 8
    }
}
