#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
extern char LAB_005db5b0[];
/*004afe30*/ extern int __stdcall FUN_004afe30(void *a1);
/*ffdff000*/ extern void *ExceptionList;
/* methods of the object at +0xcc: thiscall */
static int call_thiscall_0_r(void *self,void *method);
/* calls method(self) with the C++ this-call convention: this in ecx, no stack arguments */
__declspec(naked) int call_thiscall_0_r(void *self,void *method)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call dword ptr [esp + 8]
        ret
    }
}
#define OBJ_VTBL_LOCK_A (0x84)
#define OBJ_VTBL_LOCK_B (0x64)
/* FUN_004acca0 */

/* Enters the two critical sections of the object at obj+0xcc (+0x7c, +0x94), calls FUN_004afe30(obj), then thiscall vtable slots +0x84 and +0x64 on it and leaves them; returns the second call's result (inferred). */
undefined4 __stdcall FUN_004acca0(char *obj)

{
  undefined4 result;
  LPCRITICAL_SECTION inner_cs;
  LPCRITICAL_SECTION outer_cs;
  struct {
    void *pvStack_c;
    undefined1 *puStack_8;
    int local_4;
  } frame;
  
  frame.local_4 = 0xffffffff;
  frame.puStack_8 = (undefined1 *)&LAB_005db5b0;
  frame.pvStack_c = ExceptionList;
  outer_cs = (LPCRITICAL_SECTION)(*(int *)(obj + 0xcc) + 0x7c);
  ExceptionList = &frame.pvStack_c;
  EnterCriticalSection(outer_cs);
  inner_cs = (LPCRITICAL_SECTION)(*(int *)(obj + 0xcc) + 0x94);
  frame.local_4 = 0;
  EnterCriticalSection(inner_cs);
  (*(byte *)((char *)&frame.local_4 + 0)) = 1;
  FUN_004afe30(obj);
  call_thiscall_0_r(*(void **)(obj + 0xcc),*(void **)(**(int **)(obj + 0xcc) + OBJ_VTBL_LOCK_A));
  frame.local_4 = (uint)((uint)frame.local_4 >> 8 & 0xffffff) << 8;
  LeaveCriticalSection(inner_cs);
  result = call_thiscall_0_r(*(void **)(obj + 0xcc),*(void **)(**(int **)(obj + 0xcc) + OBJ_VTBL_LOCK_B));
  LeaveCriticalSection(outer_cs);
  ExceptionList = frame.pvStack_c;
  return result;
}


