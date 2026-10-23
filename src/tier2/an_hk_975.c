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

undefined4 __stdcall FUN_004acca0(char *param_1)

{
  undefined4 uVar1;
  LPCRITICAL_SECTION lpCriticalSection;
  LPCRITICAL_SECTION lpCriticalSection_00;
  struct {
    void *pvStack_c;
    undefined1 *puStack_8;
    int local_4;
  } frame;
  
  frame.local_4 = 0xffffffff;
  frame.puStack_8 = (undefined1 *)&LAB_005db5b0;
  frame.pvStack_c = ExceptionList;
  lpCriticalSection_00 = (LPCRITICAL_SECTION)(*(int *)(param_1 + 0xcc) + 0x7c);
  ExceptionList = &frame.pvStack_c;
  EnterCriticalSection(lpCriticalSection_00);
  lpCriticalSection = (LPCRITICAL_SECTION)(*(int *)(param_1 + 0xcc) + 0x94);
  frame.local_4 = 0;
  EnterCriticalSection(lpCriticalSection);
  (*(byte *)((char *)&frame.local_4 + 0)) = 1;
  FUN_004afe30(param_1);
  call_thiscall_0_r(*(void **)(param_1 + 0xcc),*(void **)(**(int **)(param_1 + 0xcc) + OBJ_VTBL_LOCK_A));
  frame.local_4 = (uint)((uint)frame.local_4 >> 8 & 0xffffff) << 8;
  LeaveCriticalSection(lpCriticalSection);
  uVar1 = call_thiscall_0_r(*(void **)(param_1 + 0xcc),*(void **)(**(int **)(param_1 + 0xcc) + OBJ_VTBL_LOCK_B));
  LeaveCriticalSection(lpCriticalSection_00);
  ExceptionList = frame.pvStack_c;
  return uVar1;
}


