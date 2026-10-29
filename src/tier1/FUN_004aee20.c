#include "mfl_tier2.h"
/*004b10c0*/ extern int __stdcall FUN_004b10c0(void *a1, int a2, int a3);
typedef int (__stdcall *ComMethod)(int *self);
#define COM_VTBL_ADDREF (0x4)
#define COM_VTBL_ADDREF (0x4)
/* FUN_004aee20 */

undefined4 __stdcall FUN_004aee20(char *this_ptr, undefined2 *out_name)

{
  int *unused_ptr;
  
  if (out_name == (undefined2 *)0x0) {
    return 0x80004003;
  }
  if (*(int *)(this_ptr + 0x30) != 0) {
    FUN_004b10c0(out_name,*(int *)(this_ptr + 0x30),0x80);
  } else {
    *out_name = 0;
  }
  *(undefined4 *)(out_name + 0x80) = *(undefined4 *)(this_ptr + 0x34);
  unused_ptr = *(int **)(this_ptr + 0x34);
  if (unused_ptr != (int *)0x0) {
    (*(ComMethod *)(*unused_ptr + COM_VTBL_ADDREF))(unused_ptr);
  }
  return 0;
}
