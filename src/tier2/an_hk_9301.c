#include "mfl_tier2.h"
/*004b0f50*/ extern int __stdcall FUN_004b0f50(int p0, int p1);
/*004b0fb0*/ extern int __stdcall FUN_004b0fb0(int p0, int *p1, int p2);
/*005e1fec*/ extern int g_data_5e1fec[4];
/*005e203c*/ extern int g_data_5e203c[4];
/*005e204c*/ extern int g_data_5e204c[4];
/*005e2624*/ extern int g_data_5e2624[4];
/* FUN_004aebd0 */

/* "QueryInterface: known interface ids give the sub object at +0xc (+0x10 for one), else the base class" */

int __stdcall FUN_004aebd0(int param_1,int *param_2,int param_3)

{
  if ((param_2[0] == g_data_5e203c[0] && param_2[1] == g_data_5e203c[1] && param_2[2] == g_data_5e203c[2] && param_2[3] == g_data_5e203c[3]) ||
      (param_2[0] == g_data_5e204c[0] && param_2[1] == g_data_5e204c[1] && param_2[2] == g_data_5e204c[2] && param_2[3] == g_data_5e204c[3]) ||
      (param_2[0] == g_data_5e2624[0] && param_2[1] == g_data_5e2624[1] && param_2[2] == g_data_5e2624[2] && param_2[3] == g_data_5e2624[3])) {
    if (param_1 != 0) {
      return FUN_004b0f50(param_1 + 0xc,param_3);
    }
    return FUN_004b0f50(0,param_3);
  }
  if ((param_2[0] == g_data_5e1fec[0] && param_2[1] == g_data_5e1fec[1] && param_2[2] == g_data_5e1fec[2] && param_2[3] == g_data_5e1fec[3])) {
    if (param_1 != 0) {
      return FUN_004b0f50(param_1 + 0x10,param_3);
    }
    return FUN_004b0f50(0,param_3);
  }
  return FUN_004b0fb0(param_1,param_2,param_3);
}
