// cluster-entries: trigger_register_object_type8_cb_4565b0
/* Installed cluster trigger_register_object_type8_cb_4565b0 (tools/cluster_install.py from output/cluster_pilot_kb/trigger_register_object_type8_cb_4565b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
/*0040fdc0*/ extern int race_get_phase();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b520*/ extern int car_current_matrix();
/*0047b6e0*/ extern int car_get_velocity();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd378*/ extern float k_f_0_1;
/*005ddad0*/ extern float k_f_180;
/*005df2b4*/ extern float k_f_5_0en4;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/* trigger_register_object_type8_cb_4565b0 */

/* function pointer pushed/stored by trigger_register_object_type8 (MOV dword ptr [ESP +
   0x28],0x4565b0) */

void trigger_register_object_type8_cb_4565b0(int param_1, int param_2)

{
  char *iVar1;
  float fVar2;
  undefined4 uVar3;
  int iVar4;
  RwMatrix *matrixIn;
  int iVar5;
  RwReal RVar6;
  RwV3d RStack_58;
  RwV3d RStack_4c;
  RwMatrix RStack_40;
  
  iVar4 = param_2;
  iVar1 = *(char **)(param_1 + 0x14);
  iVar5 = *(int *)(param_2 + 0x10);
  if ((0xfeff < iVar5) && (iVar5 < 0xff08)) {
    iVar5 = iVar5 + -0xff00;
    car_current_matrix(&param_1,iVar5);
    matrixIn = RwFrameGetLTM(*(RwFrame **)(*(char **)(iVar1 + 8) + 4));
    RStack_58.x = *(float *)(param_1 + 0x30) - (matrixIn->pos).x;
    RStack_58.z = *(float *)(param_1 + 0x38) - (matrixIn->pos).z;
    RStack_58.y = 0.0;
    car_get_velocity(&RStack_4c,iVar5);
    RVar6 = RwV3dLength(&RStack_4c);
    fVar2 = RVar6 * k_f_0_1 + k_f_180;
    RStack_58.x = RStack_58.x * fVar2;
    RStack_58.y = RStack_58.y * fVar2;
    RStack_58.z = RStack_58.z * fVar2;
    car_add_velocity(&RStack_58,iVar5);
    RwMatrixInvert(&RStack_40,matrixIn);
    RwV3dTransformVectors(&RStack_58,&RStack_58,1,&RStack_40);
    *(float *)(iVar1 + 0x14) = RStack_58.x * k_f_5_0en4;
    *(float *)(iVar1 + 0x1c) = RStack_58.z * k_f_5_0en4;
    iVar5 = race_get_phase();
    uVar3 = g_last_frame_ticks;
    if (((iVar5 != 1) && (((iVar5 != 2 && (iVar5 != 3)) && (iVar5 != 4)))) && (iVar5 != 5)) {
      (&g_event_log_type)[g_event_log_head] = 0xb;
      (&g_event_log_time)[g_event_log_head] = uVar3;
      (&g_event_log_param)[g_event_log_head] = *(undefined4 *)(iVar4 + 0x10);
      g_event_log_args[g_event_log_head].arg0 = 1;
      g_event_log_head = g_event_log_head + 1;
      if (0xff < g_event_log_head) { /* unsigned (jb) */
        g_event_log_head = 0;
      }
    }
  }
  return;
}

