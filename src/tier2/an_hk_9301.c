#include "mfl_tier2.h"
/*004b0f50*/ extern int __stdcall FUN_004b0f50(int p0, int p1);
/*004b0fb0*/ extern int __stdcall FUN_004b0fb0(int p0, int *p1, int p2);
/*005e1fec*/ extern int g_data_5e1fec[4];
/*005e203c*/ extern int g_data_5e203c[4];
/*005e204c*/ extern int g_data_5e204c[4];
/*005e2624*/ extern int g_data_5e2624[4];
/* FUN_004aebd0 */

/* "QueryInterface: known interface ids give the sub object at +0xc (+0x10 for one), else the base class" */

int __stdcall FUN_004aebd0(int this_ptr,int *iid,int out_object)

{
  if ((iid[0] == g_data_5e203c[0] && iid[1] == g_data_5e203c[1] && iid[2] == g_data_5e203c[2] && iid[3] == g_data_5e203c[3]) ||
      (iid[0] == g_data_5e204c[0] && iid[1] == g_data_5e204c[1] && iid[2] == g_data_5e204c[2] && iid[3] == g_data_5e204c[3]) ||
      (iid[0] == g_data_5e2624[0] && iid[1] == g_data_5e2624[1] && iid[2] == g_data_5e2624[2] && iid[3] == g_data_5e2624[3])) {
    if (this_ptr != 0) {
      return FUN_004b0f50(this_ptr + 0xc,out_object);
    }
    return FUN_004b0f50(0,out_object);
  }
  if ((iid[0] == g_data_5e1fec[0] && iid[1] == g_data_5e1fec[1] && iid[2] == g_data_5e1fec[2] && iid[3] == g_data_5e1fec[3])) {
    if (this_ptr != 0) {
      return FUN_004b0f50(this_ptr + 0x10,out_object);
    }
    return FUN_004b0f50(0,out_object);
  }
  return FUN_004b0fb0(this_ptr,iid,out_object);
}
