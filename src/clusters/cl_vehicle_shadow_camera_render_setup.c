// cluster-entries: vehicle_shadow_camera_render_setup
/* Installed cluster vehicle_shadow_camera_render_setup (tools/cluster_install.py from output/cluster_pilot8/vehicle_shadow_camera_render_setup.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00425fc0*/ extern int vehicle_tyre_marks_collect_triangles();
/*004265e0*/ extern int vehicle_render_state_set_colour();
/*00426610*/ extern int vehicle_render_state_set_6614e8();
/*0042a3f0*/ extern int course_get_rpworld();
/*0042a400*/ extern int course_get_collision_bsp();
extern float course_get_shadow_scale(void);
/*004c42f0*/ extern void FUN_004c42f0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d17a0*/ extern struct RwFrame * RwFrameOrthoNormalize(struct RwFrame *p0);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*005ddb5c*/ extern float k_f_n4;
/*0062ac74*/ extern float g_car_62ac74;
/*00659d70*/ extern undefined4 g_particles_659d70;
/*00659d74*/ extern undefined4 g_game_659d74;
/*0065a9b0*/ extern undefined4 g_load_65a9b0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* vehicle_shadow_camera_render_setup */

/* WARNING: Unknown calling convention */
/* "places the shadow camera below the car, sets the shadow render target and renders the car into
   it" */

void vehicle_shadow_camera_render_setup_n(VehicleRecord *vehicle_record)

{
  float fbits1;
  int iVar1;
  uint uVar2;
  RwFrame *frame;
  RpWorld *pRVar3;
  int iVar4;
  undefined4 collision_bsp;
  bool bVar5;
  float10 fVar6;
  RwCamera *pRVar7;
  struct {
    RwRGBA local_20;
    RwV3d local_1c;
    uint local_10;
    uint local_c;
    uint local_8;
    uint local_4;
  } frame_obj;
  
  iVar1 = vehicle_record->part_frames[vehicle_record->field_28c];
  if ((g_particles_659d70 != 0) && (iVar1 != 0)) {
    frame_obj.local_1c.y = vehicle_record->world_sphere[1];
    frame_obj.local_1c.z = vehicle_record->world_sphere[2];
    frame_obj.local_20.red = '\0';
    frame_obj.local_20.green = '\0';
    frame_obj.local_20.blue = '\0';
    frame_obj.local_20.alpha = '\0';
    iVar4 = *(int *)(g_particles_659d70 + 4);
    bVar5 = vehicle_record->field_298 == 2;
    uVar2 = (&g_load_65a9b0)[bVar5];
    vehicle_record->shadow_camera = uVar2;
    vehicle_record->shadow_texture = (&g_game_659d74)[bVar5];
    frame = *(RwFrame **)(uVar2 + 4);
    vehicle_record->shadow_frame = (uint)frame;
    frame_obj.local_1c.x = *(float *)(iVar4 + 0x30) * k_f_n4 + vehicle_record->world_sphere[0];
    frame_obj.local_1c.y = *(float *)(iVar4 + 0x34) * k_f_n4 + frame_obj.local_1c.y;
    frame_obj.local_1c.z = *(float *)(iVar4 + 0x38) * k_f_n4 + frame_obj.local_1c.z;
    RwFrameTranslate(frame,&frame_obj.local_1c,rwCOMBINEREPLACE);
    FUN_004c42f0(vehicle_record->shadow_frame,vehicle_record->world_sphere,&g_car_62ac74);
    RwFrameOrthoNormalize((RwFrame *)vehicle_record->shadow_frame);
    frame_obj.local_10 = vehicle_record->shadow_texture;
    frame_obj.local_c = frame_obj.local_10;
    frame_obj.local_8 = frame_obj.local_10;
    frame_obj.local_4 = frame_obj.local_10;
    vehicle_render_state_set_colour(vehicle_record->index,&frame_obj.local_10);
    vehicle_render_state_set_6614e8(vehicle_record->index,vehicle_record->shadow_camera);
    RwCameraClear((RwCamera *)vehicle_record->shadow_camera,&frame_obj.local_20,1);
    pRVar7 = (RwCamera *)vehicle_record->shadow_camera;
    pRVar3 = (RpWorld *)course_get_rpworld();
    RpWorldAddCamera(pRVar3,pRVar7);
    iVar4 = RwCameraBeginUpdate(vehicle_record->shadow_camera);
    if (iVar4 != 0) {
      fVar6 = (float10)course_get_shadow_scale();
      collision_bsp =
           course_get_collision_bsp
                     ((fbits1 = (float)(fVar6 * (float10)*(float *)((char *)vehicle_record + 0x288) *
                             (float10)vehicle_record->size_factor), *(int *)&fbits1));
      vehicle_tyre_marks_collect_triangles
                (vehicle_record->index,*(int *)(vehicle_record->frame + 4) + 0x10,collision_bsp);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
      (**(code **)(iVar1 + 0x48))(iVar1);
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
      RwCameraEndUpdate(vehicle_record->shadow_camera);
    }
    pRVar7 = (RwCamera *)vehicle_record->shadow_camera;
    pRVar3 = (RpWorld *)course_get_rpworld();
    RpWorldRemoveCamera(pRVar3,pRVar7);
  }
  return;
}

/* entry thunk at the original address of vehicle_shadow_camera_render_setup: register inputs (esi) -> natural C vehicle_shadow_camera_render_setup_n; ret 0 */
__declspec(naked) void vehicle_shadow_camera_render_setup(void)
{
    __asm {
        push esi
        call vehicle_shadow_camera_render_setup_n
        add esp, 4
        ret
    }
}
