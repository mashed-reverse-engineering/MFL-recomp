// cluster-entries: grid_position
/* Installed cluster grid_position (tools/cluster_install.py from output/cluster_pilot_j1/grid_position.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042b1c0*/ extern int course_get_record_pos();
/*0042b1d0*/ extern int course_get_record_base();
/*0042b210*/ extern int course_get_record_point();
/*00434bb0*/ extern int get_event_type();
/*00482030*/ extern double dot_product_2d_clamped();
/*004a8420*/ extern int debug_printf();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5540*/ extern RwReal RwV2dNormalize(struct RwV2d *p0, struct RwV2d *p1);
/*005dd328*/ extern float k_f_0_6;
/*005dd36c*/ extern float k_f_0_4;
/*005ddacc*/ extern float k_f_360;
/*005ddaec*/ extern float k_f_57_29578;
/*005ddb18*/ extern float float_0_8;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc10*/ extern float k_f_3_3;
/*005ddc14*/ extern float k_f_2_2;
/*005ddc18*/ extern float k_f_1_1;
/*005ddc28*/ extern float k_f_0;
/*00651ab0*/ extern undefined4 g_fx_651ab0;
/*00651ab4*/ extern undefined4 g_grid_position_651ab4;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7d4*/ extern undefined4 g_race_time_ticks;
/* Start grid position of one car of the race (computes a position and a heading from the route record; "Invalid number of cars in race" / "Invalid position").  Ghidra-style locals kept: the x87 expression order is the verified one. */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* computes a position/heading for one of the cars of the race ("Invalid number of cars in race";
   "Invalid position") */

void grid_position(undefined4 param_1,int param_2,int param_3,float *param_4,float *param_5)

{
  float fVar1;
  float fVar2;
  int event_type;
  float *pfVar3;
  float *pfVar4;
  int event_type_2;
  float10 fVar5;
  struct {
    RwV2d local_68;
    float local_60;
    float local_5c;
    float local_58;
  } frame;
  RwV3d RStack_54;
  struct {
    float local_48;
    float local_44;
    float local_40;
    float fStack_3c;
    float fStack_38;
    float fStack_34;
    float fStack_30;
    float fStack_2c;
    float fStack_28;
    float local_24;
    float local_20;
    float local_1c;
    float fStack_18;
    float fStack_14;
    float fStack_10;
    float fStack_c;
    float fStack_8;
    float fStack_4;
  } frame2;
  
  frame.local_60 = 0.0;
  frame.local_5c = 1.0;
  if ((g_race_time_ticks != g_fx_651ab0) &&
     (g_grid_position_651ab4 = g_grid_position_651ab4 + 1, param_3 <= (int)g_grid_position_651ab4)) {
    g_grid_position_651ab4 = 0;
  }
  event_type = get_event_type();
  if (event_type == 0x11) {
    g_grid_position_651ab4 = 0;
  }
  g_fx_651ab0 = g_race_time_ticks;
  if (((game_mode == 4) || (game_mode == 9)) || (game_mode == 8)) {
    g_grid_position_651ab4 = 3;
  }
  pfVar3 = (float *)course_get_record_pos(param_1);
  fVar2 = *pfVar3;
  frame2.local_44 = pfVar3[1];
  frame2.local_40 = pfVar3[2];
  frame2.local_48 = fVar2;
  pfVar3 = (float *)course_get_record_base(param_1);
  frame.local_68.x = *pfVar3;
  frame2.local_20 = pfVar3[1];
  frame.local_68.y = pfVar3[2];
  frame2.local_24 = frame.local_68.x;
  frame2.local_1c = frame.local_68.y;
  RwV2dNormalize(&frame.local_68,&frame.local_68);
  fVar5 = (float10)dot_product_2d_clamped(&frame.local_60,&frame.local_68);
  fVar5 = (float10)FUN_004b3284((double)fVar5);
  fVar5 = fVar5 * (float10)k_f_57_29578;
  if (frame.local_68.x < k_f_0) {
    fVar5 = (float10)k_f_360 - fVar5;
  }
  *param_5 = (float)fVar5;
  pfVar3 = (float *)course_get_record_point(param_1,0);
  pfVar4 = (float *)course_get_record_point(param_1,3);
  RStack_54.x = *pfVar3 - *pfVar4;
  event_type = course_get_record_point(param_1,0);
  fVar1 = *(float *)(event_type + 4);
  event_type = course_get_record_point(param_1,3);
  RStack_54.y = fVar1 - *(float *)(event_type + 4);
  event_type = course_get_record_point(param_1,0);
  fVar1 = *(float *)(event_type + 8);
  event_type = course_get_record_point(param_1,3);
  RStack_54.z = fVar1 - *(float *)(event_type + 8);
  RwV3dNormalize(&RStack_54,&RStack_54);
  frame2.fStack_3c = RStack_54.x * k_f_0_4;
  frame2.fStack_38 = RStack_54.y * k_f_0_4;
  frame2.fStack_34 = RStack_54.z * k_f_0_4;
  frame2.fStack_30 = frame2.local_24 * k_f_1_1;
  frame2.fStack_2c = frame2.local_20 * k_f_1_1;
  frame2.fStack_28 = frame2.local_1c * k_f_1_1;
  frame2.fStack_18 = frame2.local_24 * k_f_2_2;
  frame2.fStack_14 = frame2.local_20 * k_f_2_2;
  frame2.fStack_10 = frame2.local_1c * k_f_2_2;
  frame2.fStack_c = frame2.local_24 * k_f_3_3;
  frame2.fStack_8 = frame2.local_20 * k_f_3_3;
  frame2.fStack_4 = frame2.local_1c * k_f_3_3;
  if (param_2 == 100) {
    frame.local_60 = frame2.local_48 + frame2.fStack_18;
    frame.local_5c = frame2.local_44 + frame2.fStack_14;
    frame2.local_40 = frame2.local_40 + frame2.fStack_10;
LAB_0040a700:
    *param_4 = frame.local_60;
    param_4[1] = frame.local_5c;
    param_4[2] = frame2.local_40;
    return;
  }
  if (param_2 < 4) {
    RStack_54.x = RStack_54.x * float_0_8;
    RStack_54.y = RStack_54.y * float_0_8;
    fVar2 = RStack_54.z * float_0_8;
    frame.local_60 = frame2.fStack_3c * float_0_8;
    frame.local_5c = frame2.fStack_38 * float_0_8;
    RStack_54.z = frame2.fStack_34 * float_0_8;
    switch(param_2) {
    case 0:
      {
        frame.local_58 = frame2.local_40 + fVar2;
        frame.local_5c = (frame2.local_44 + RStack_54.y) - frame.local_5c;
        frame.local_60 = (frame2.local_48 + RStack_54.x) - frame.local_60;
        RStack_54.z = frame.local_58 - RStack_54.z;
      }
      break;
    case 1:
      {
        frame.local_60 = frame2.local_48 - RStack_54.x - frame.local_60;
        frame.local_58 = frame2.local_40 - fVar2;
        frame.local_5c = ((frame2.local_44 - RStack_54.y) - frame.local_5c) - frame2.fStack_2c;
        frame.local_60 = frame.local_60 - frame2.fStack_30;
        frame2.fStack_28 = frame.local_58 - RStack_54.z - frame2.fStack_28;
      }
      goto LAB_0040a68e;
    case 2:
      {
        frame.local_60 = frame2.local_48 + RStack_54.x + frame.local_60;
        frame.local_58 = frame2.local_40 + fVar2;
        frame.local_5c = (frame2.local_44 + RStack_54.y + frame.local_5c) - frame2.fStack_14;
        frame.local_60 = frame.local_60 - frame2.fStack_18;
        frame2.local_40 = frame.local_58 + RStack_54.z - frame2.fStack_10;
      }
      goto LAB_0040a700;
    case 3:
      {
        frame.local_60 = frame2.local_48 - RStack_54.x + frame.local_60;
        frame.local_58 = frame2.local_40 - fVar2;
        frame.local_5c = ((frame2.local_44 - RStack_54.y) + frame.local_5c) - frame2.fStack_8;
        frame.local_60 = frame.local_60 - frame2.fStack_c;
        RStack_54.z = frame.local_58 + RStack_54.z - frame2.fStack_4;
      }
      break;
    default:
      return;
    }
    goto LAB_0040a772;
  }
  for (event_type = param_2 + -4 + g_grid_position_651ab4; param_3 <= event_type;
      event_type = event_type - param_3) {
  }
  switch(param_3) {
  case 1:
    if (event_type == 0) {
LAB_0040a48b:
      *param_4 = fVar2;
      param_4[1] = frame2.local_44;
      param_4[2] = frame2.local_40;
      return;
    }
    break;
  case 2:
    RStack_54.x = RStack_54.x * k_f_0_6;
    RStack_54.y = RStack_54.y * k_f_0_6;
    RStack_54.z = RStack_54.z * k_f_0_6;
    if (event_type == 0) {
      frame.local_60 = frame2.local_48 + RStack_54.x;
      frame.local_5c = frame2.local_44 + RStack_54.y;
      frame2.fStack_28 = frame2.local_40 + RStack_54.z;
      goto LAB_0040a68e;
    }
    if (event_type == 1) {
      frame.local_60 = frame2.local_48 - RStack_54.x;
      frame.local_5c = frame2.local_44 - RStack_54.y;
      RStack_54.z = frame2.local_40 - RStack_54.z;
      goto LAB_0040a772;
    }
    break;
  case 3:
    RStack_54.x = RStack_54.x * k_f_0_9;
    RStack_54.y = RStack_54.y * k_f_0_9;
    RStack_54.z = RStack_54.z * k_f_0_9;
    if (event_type == 0) {
      frame.local_60 = frame2.local_48 + RStack_54.x;
      frame.local_5c = frame2.local_44 + RStack_54.y;
      RStack_54.z = frame2.local_40 + RStack_54.z;
      goto LAB_0040a772;
    }
    if (event_type == 1) goto LAB_0040a48b;
    if (event_type == 2) {
      frame.local_60 = frame2.local_48 - RStack_54.x;
      frame.local_5c = frame2.local_44 - RStack_54.y;
      frame2.local_40 = frame2.local_40 - RStack_54.z;
      goto LAB_0040a700;
    }
    break;
  case 4:
    RStack_54.x = RStack_54.x * float_0_8;
    RStack_54.y = RStack_54.y * float_0_8;
    RStack_54.z = RStack_54.z * float_0_8;
    frame2.fStack_3c = frame2.fStack_3c * float_0_8;
    frame2.fStack_38 = frame2.fStack_38 * float_0_8;
    frame2.fStack_34 = frame2.fStack_34 * float_0_8;
    event_type_2 = get_event_type();
    if (event_type_2 == 0x11) {
      if (event_type != 0) {
        if (event_type == 1) {
          {
            frame.local_58 = frame2.local_40 - RStack_54.z;
            frame.local_5c = (frame2.local_44 - RStack_54.y) - frame2.fStack_38;
            frame.local_60 = (frame2.local_48 - RStack_54.x) - frame2.fStack_3c;
            frame2.local_40 = frame.local_58 - frame2.fStack_34;
          }
          goto LAB_0040a700;
        }
        if (event_type != 2) {
          return;
        }
        frame.local_60 = ((((frame2.local_48 - RStack_54.x) - RStack_54.x) - frame2.fStack_30) - frame2.fStack_30) - frame2.fStack_30;
        frame.local_5c = ((((frame2.local_44 - RStack_54.y) - RStack_54.y) - frame2.fStack_2c) - frame2.fStack_2c) - frame2.fStack_2c;
        frame2.fStack_28 = ((((frame2.local_40 - RStack_54.z) - RStack_54.z) - frame2.fStack_28) - frame2.fStack_28) - frame2.fStack_28
        ;
        goto LAB_0040a68e;
      }
      {
        frame.local_58 = frame2.local_40 + RStack_54.z;
        frame.local_5c = frame2.local_44 + RStack_54.y + frame2.fStack_38;
        frame.local_60 = frame2.local_48 + RStack_54.x + frame2.fStack_3c;
        RStack_54.z = frame.local_58 + frame2.fStack_34;
      }
LAB_0040a772:
      *param_4 = frame.local_60;
      param_4[1] = frame.local_5c;
      param_4[2] = RStack_54.z;
      return;
    }
    switch(event_type) {
    case 0:
      {
        frame.local_58 = frame2.local_40 + RStack_54.z;
        frame.local_5c = (frame2.local_44 + RStack_54.y) - frame2.fStack_38;
        frame.local_60 = (frame2.local_48 + RStack_54.x) - frame2.fStack_3c;
        frame2.fStack_28 = frame.local_58 - frame2.fStack_34;
      }
      break;
    case 1:
      {
        frame.local_60 = frame2.local_48 - RStack_54.x - frame2.fStack_3c;
        frame.local_58 = frame2.local_40 - RStack_54.z;
        frame.local_5c = ((frame2.local_44 - RStack_54.y) - frame2.fStack_38) - frame2.fStack_2c;
        frame.local_60 = frame.local_60 - frame2.fStack_30;
        frame2.local_40 = frame.local_58 - frame2.fStack_34 - frame2.fStack_28;
      }
      goto LAB_0040a700;
    case 2:
      {
        frame.local_60 = frame2.local_48 + RStack_54.x + frame2.fStack_3c;
        frame.local_58 = frame2.local_40 + RStack_54.z;
        frame.local_5c = (frame2.local_44 + RStack_54.y + frame2.fStack_38) - frame2.fStack_14;
        frame.local_60 = frame.local_60 - frame2.fStack_18;
        RStack_54.z = frame.local_58 + frame2.fStack_34 - frame2.fStack_10;
      }
      goto LAB_0040a772;
    case 3:
      {
        frame.local_60 = frame2.local_48 - RStack_54.x + frame2.fStack_3c;
        frame.local_58 = frame2.local_40 - RStack_54.z;
        frame.local_5c = ((frame2.local_44 - RStack_54.y) + frame2.fStack_38) - frame2.fStack_8;
        frame.local_60 = frame.local_60 - frame2.fStack_c;
        frame2.fStack_28 = frame.local_58 + frame2.fStack_34 - frame2.fStack_4;
      }
      break;
    default:
      goto switchD_0040a651_default;
    }
LAB_0040a68e:
    *param_4 = frame.local_60;
    param_4[1] = frame.local_5c;
    param_4[2] = frame2.fStack_28;
    return;
  default:
    debug_printf("Invalid number of cars in race (%d)\n",param_3);
    return;
  }
switchD_0040a651_default:
  debug_printf("Invalid position (%d)\n",event_type);
  return;
}

