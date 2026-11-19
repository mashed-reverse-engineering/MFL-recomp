#include "mfl_tier2.h"
/*008c598c*/ extern undefined4 g_car_8c598c;
/*008c5a00*/ extern float g_car_8c5a00;
void __cdecl camera_apply_matrix(char *param_1)

{
  *(float *)(param_1 + 0x58) = g_car_8c5a00;
  camera_update_view_window(param_1);
  *(RwMatrix *)(*(char **)(*(char **)(param_1 + 0x84) + 4) + 0x10) = *(RwMatrix *)&g_car_8c598c;
}
