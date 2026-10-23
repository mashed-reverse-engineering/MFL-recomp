#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*007d2128*/ extern char *RwEngineInstance;
/* video_draw_textured_quad */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "screen quad with uv rect and colour using the movie/video texture in the viewport" */

void video_draw_textured_quad(char *param_1,float *param_2,float *param_3,undefined1 *param_4,void *param_5,
               undefined4 param_6,undefined4 param_7)
{
  char *iVar1;
  undefined1 local_a0[4];
  short local_9c [2];
  short local_98 [2];
  int local_94;
  float local_90;
  float local_8c;
  float local_88;
  float local_84;
  float local_80 [4];
  RwIm2DVertex v[4];
  
  local_80[0] = 0.0;
  local_80[1] = 0.0;
  local_80[2] = 1.0;
  local_80[3] = 1.0;
  local_a0[0] = 0xff;
  local_a0[1] = 0xff;
  local_a0[2] = 0xff;
  local_a0[3] = 0xff;
  if (param_2 == (float *)0x0) {
    param_2 = local_80;
  }
  if (param_3 == (float *)0x0) {
    param_3 = local_80 + 2;
  }
  if (param_4 == (undefined1 *)0x0) {
    param_4 = local_a0;
  }
  iVar1 = *(char **)(param_1 + 0x60);
  local_90 = (float)*(int *)(iVar1 + 0xc);
  local_8c = (float)*(int *)(iVar1 + 0x10);
  RwRasterGetOffset(iVar1,local_98,local_9c);
  local_88 = (float)(int)local_98[0];
  local_94 = (int)local_9c[0];
  local_84 = (float)local_94;
  v[0].rhw = float_1_0 / *(float *)(param_1 + 0x80);
  v[0].x = local_90 * *param_2 + local_88;
  v[0].y = local_8c * param_2[1] + local_84;
  v[0].v = *(RwReal *)&param_7;
  v[0].z = RwIm2DGetNearScreenZ();
  v[0].u = 0.0;
  v[1].x = v[0].x;
  v[1].y = local_8c * param_3[1] + local_84;
  v[1].z = RwIm2DGetNearScreenZ();
  v[1].rhw = v[0].rhw;
  v[1].u = 0.0;
  v[1].v = 0.0;
  v[2].x = local_90 * *param_3 + local_88;
  v[2].y = v[1].y;
  v[2].z = RwIm2DGetNearScreenZ();
  v[2].rhw = v[0].rhw;
  v[2].u = *(RwReal *)&param_6;
  v[2].v = 0.0;
  v[3].x = v[2].x;
  v[3].y = v[0].y;
  v[3].z = RwIm2DGetNearScreenZ();
  v[3].rhw = v[0].rhw;
  v[3].v = *(RwReal *)&param_7;
  v[3].u = *(RwReal *)&param_6;
  v[0].emissiveColor = CONCAT31(CONCAT21(CONCAT11(param_4[3],*param_4),param_4[1]),param_4[2]);
  v[1].emissiveColor = v[0].emissiveColor;
  v[2].emissiveColor = v[0].emissiveColor;
  v[3].emissiveColor = v[0].emissiveColor;
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER,param_5);
  if (param_4[3] != 0xff) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  }
  RwIm2DRenderTriangle(v,4,0,1,2);
  RwIm2DRenderTriangle(v,4,2,3,0);
  if (param_4[3] != 0xff) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x0);
  }
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)0x2);
  return;
}
