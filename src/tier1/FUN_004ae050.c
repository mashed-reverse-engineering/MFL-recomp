#include "mfl_tier2.h"
/*004b1470*/ extern int __stdcall FUN_004b1470(int a1, int a2, int a3, int a4, int a5);
/* Offsets inside obj, and result codes (inferred from the call site: 0x102 looks like a wait timeout). */
#define OBJ_HANDLE_OFFSET (0x50)
#define OBJ_VALUE_OFFSET (0x8)
#define CALLEE_RESULT_TIMEOUT (0x102)
#define RESULT_TIMEOUT_STATUS (0x40237)
#define HRESULT_E_POINTER (0x80004003)
/* FUN_004ae050
 * Waits on the handle stored at obj + OBJ_HANDLE_OFFSET (inferred) via FUN_004b1470 and copies the value at
 * obj + OBJ_VALUE_OFFSET into *out_value. wait_arg is passed on to FUN_004b1470 unchanged (inferred meaning: wait handle or timeout).
 * Returns HRESULT_E_POINTER if out_value is NULL, RESULT_TIMEOUT_STATUS if the call timed out, else 0.
 */
undefined4 __stdcall FUN_004ae050(char *obj, undefined4 wait_arg, undefined4 *out_value)
{
  int callee_result;
  int handle;

  if (out_value == (undefined4 *)0x0) {
    return HRESULT_E_POINTER;
  }
  /* call the wait routine; the value is copied out in both branches */
  handle = *(undefined4 *)(obj + OBJ_HANDLE_OFFSET);
  callee_result = FUN_004b1470(handle,wait_arg,0,0,0);
  if (callee_result == CALLEE_RESULT_TIMEOUT) {
    *out_value = *(undefined4 *)(obj + OBJ_VALUE_OFFSET);
    return RESULT_TIMEOUT_STATUS;
  }
  *out_value = *(undefined4 *)(obj + OBJ_VALUE_OFFSET);
  return 0;
}
