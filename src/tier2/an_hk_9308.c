#include "mfl_tier2.h"
/*004aebd0*/ extern int __stdcall FUN_004aebd0(int *p0, int *p1, int p2);
/*005e1fdc*/ extern int g_data_5e1fdc[4];
/*005e209c*/ extern int g_video_5e209c[4];
/* FUN_004ac4f0 */

/* "QueryInterface of the outer object: two interface ids forward to vtable slot 9 (+0x24), the rest to FUN_004aebd0" */

int __stdcall FUN_004ac4f0(int *param_1,int *param_2,int param_3)

{
  if ((param_2[0] == g_video_5e209c[0] && param_2[1] == g_video_5e209c[1] && param_2[2] == g_video_5e209c[2] && param_2[3] == g_video_5e209c[3]) ||
      (param_2[0] == g_data_5e1fdc[0] && param_2[1] == g_data_5e1fdc[1] && param_2[2] == g_data_5e1fdc[2] && param_2[3] == g_data_5e1fdc[3])) {
    return (*(int (__fastcall **)(int *, int, int *, int))(*param_1 + 0x24))(param_1,*param_1,param_2,param_3);
  }
  return FUN_004aebd0(param_1,param_2,param_3);
}
