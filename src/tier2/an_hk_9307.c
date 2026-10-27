#include "mfl_tier2.h"
/*004b1100*/ extern int __stdcall FUN_004b1100(int *p0, int *p1);
/*005e219c*/ extern int PTR_g_data_6e0049_005e219c[4];
/* FUN_004ac600 */

/* "QueryInterface of a sub object at this-0xc: asks the outer object (vtable +0x1c) and hands out the sub object at +0xc" */

int __stdcall FUN_004ac600(int this_sub,int *iid,int *out_object)

{
  int outer_result;
  int *sub_object;
  int *null_object;
  
  if (out_object == (int *)0x0) {
    return 0x80004003;
  }
  outer_result = FUN_004b1100(iid,PTR_g_data_6e0049_005e219c);
  if (outer_result == 0) {
    outer_result = (*(int (__fastcall **)(int, int, int))(*(int *)(this_sub + -0xc) + 0x1c))(this_sub + -0xc,*(int *)(this_sub + -0xc),0);
    if (outer_result != 0) {
      sub_object = (int *)(outer_result + 0xc);
      *out_object = (int)sub_object;
      (*(int (__stdcall **)(int *))(*sub_object + 4))(sub_object);
      return 0;
    }
    null_object = (int *)outer_result;
    *out_object = 0;
    (*(int (__stdcall **)(int *))(*null_object + 4))(null_object);
    return 0;
  }
  *out_object = 0;
  return 0x80040216;
}
