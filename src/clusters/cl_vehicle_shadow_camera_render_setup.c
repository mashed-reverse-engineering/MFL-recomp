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

/* "places the shadow camera below the car, sets the shadow render target and renders the car into
   it" */

void vehicle_shadow_camera_render_setup_n(VehicleRecord *vehicle_record)

{
  float shadow_scale_scaled;
  int part_frame;
  uint shadow_camera;
  RwFrame *frame;
  RpWorld *rpworld;
  int light_or_status;
  undefined4 collision_bsp;
  bool is_mode_2;
  float10 shadow_scale;
  RwCamera *camera_to_toggle;
  struct {
    RwRGBA clear_colour;
    RwV3d camera_pos;
    uint tint_0;
    uint tint_1;
    uint tint_2;
    uint tint_3;
  } frame_obj;
  
  part_frame = vehicle_record->part_frames[vehicle_record->field_28c];
  if ((g_particles_659d70 != 0) && (part_frame != 0)) {
    frame_obj.camera_pos.y = vehicle_record->world_sphere[1];
    frame_obj.camera_pos.z = vehicle_record->world_sphere[2];
    frame_obj.clear_colour.red = '\0';
    frame_obj.clear_colour.green = '\0';
    frame_obj.clear_colour.blue = '\0';
    frame_obj.clear_colour.alpha = '\0';
    light_or_status = *(int *)(g_particles_659d70 + 4);
    is_mode_2 = vehicle_record->field_298 == 2;
    shadow_camera = (&g_load_65a9b0)[is_mode_2];
    /* Shadow camera/texture pair chosen by field_298 == 2 (is_mode_2); camera sits at the vehicle's world sphere centre offset by -4 * the light object's direction (+0x30..+0x38), then is oriented by FUN_004c42f0 (inferred). */
    vehicle_record->shadow_camera = shadow_camera;
    vehicle_record->shadow_texture = (&g_game_659d74)[is_mode_2];
    frame = *(RwFrame **)(shadow_camera + 4);
    vehicle_record->shadow_frame = (uint)frame;
    frame_obj.camera_pos.x = *(float *)(light_or_status + 0x30) * k_f_n4 + vehicle_record->world_sphere[0];
    frame_obj.camera_pos.y = *(float *)(light_or_status + 0x34) * k_f_n4 + frame_obj.camera_pos.y;
    frame_obj.camera_pos.z = *(float *)(light_or_status + 0x38) * k_f_n4 + frame_obj.camera_pos.z;
    RwFrameTranslate(frame,&frame_obj.camera_pos,rwCOMBINEREPLACE);
    FUN_004c42f0(vehicle_record->shadow_frame,vehicle_record->world_sphere,&g_car_62ac74);
    RwFrameOrthoNormalize((RwFrame *)vehicle_record->shadow_frame);
    frame_obj.tint_0 = vehicle_record->shadow_texture;
    frame_obj.tint_1 = frame_obj.tint_0;
    frame_obj.tint_2 = frame_obj.tint_0;
    frame_obj.tint_3 = frame_obj.tint_0;
    vehicle_render_state_set_colour(vehicle_record->index,&frame_obj.tint_0);
    vehicle_render_state_set_6614e8(vehicle_record->index,vehicle_record->shadow_camera);
    RwCameraClear((RwCamera *)vehicle_record->shadow_camera,&frame_obj.clear_colour,1);
    camera_to_toggle = (RwCamera *)vehicle_record->shadow_camera;
    rpworld = (RpWorld *)course_get_rpworld();
    RpWorldAddCamera(rpworld,camera_to_toggle);
    light_or_status = RwCameraBeginUpdate(vehicle_record->shadow_camera);
    if (light_or_status != 0) {
      /* Render the part frame into the shadow camera with Z test/write off (draw via the frame's virtual call at +0x48), then restore Z state. */
      shadow_scale = (float10)course_get_shadow_scale();
      collision_bsp =
           course_get_collision_bsp
                     ((shadow_scale_scaled = (float)(shadow_scale * (float10)vehicle_record->light_factor *
                             (float10)vehicle_record->size_factor), *(int *)&shadow_scale_scaled));
      vehicle_tyre_marks_collect_triangles
                (vehicle_record->index,*(int *)(vehicle_record->frame + 4) + 0x10,collision_bsp);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
      (**(code **)(part_frame + 0x48))(part_frame);
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
      RwCameraEndUpdate(vehicle_record->shadow_camera);
    }
    camera_to_toggle = (RwCamera *)vehicle_record->shadow_camera;
    rpworld = (RpWorld *)course_get_rpworld();
    RpWorldRemoveCamera(rpworld,camera_to_toggle);
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
