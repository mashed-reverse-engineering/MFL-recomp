#include "mfl_tier2.h"
extern char LAB_00408ad0[];
/*0042a450*/ extern int course_is_outdoor();
/*00475730*/ extern char *get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*00482070*/ extern double dot_product_3d_clamped();
/*0048e320*/ extern int rwpcar_ring_add_points_for_contact();
/*00493490*/ extern undefined __cdecl rwp_object_debris_expire_check(undefined4 p0);
/*00496850*/ extern int trigger_sphere_register(float *param_1);
/*00499dc0*/ extern int __cdecl shadow_project_to_ground_00499dc0(int p0, RwMatrix *p1, float *p2, RwMatrix *p3, float *p4);
/*0049a850*/ extern int fx_dust_add();
/*004b2b48*/ extern int _ftol();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*0058c8a0*/ extern int rwp_owner_object_get(void *p0);
/*005dd378*/ extern float k_f_0_1;
/*005dd620*/ extern float float_10_0;
/*005ddad8*/ extern undefined4 g_effects_update_5ddad8;
/*005de1d8*/ extern float k_f_0_7853982;
/*005de220*/ extern float k_f_2500;
/*0062ac74*/ extern float g_car_62ac74;
/*006f6950*/ extern undefined4 g_rw_6f6950;
/*006f6c70*/ extern undefined4 g_rw_6f6c70;
/*006f6f90*/ extern undefined g_rw_6f6f90;
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fae98*/ extern undefined g_rw_6fae98;
/*006fcdd8*/ extern undefined4 g_rw_6fcdd8;
/*006fd4a8*/ extern undefined4 g_rw_6fd4a8;
/*006fe770*/ extern undefined4 g_effects_update_6fe770;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef804*/ extern undefined4 g_tick_number;
/*00801b14*/ extern undefined4 g_rw_801b14;
/* rwp_objects_cull_and_register_triggers */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per physics object tests camera distance/frustum and registers trigger spheres, updates debris
   expiry" */

void rwp_objects_cull_and_register_triggers(void)

{
  RpAtomic *atomic;
  float to_cam_z;
  uint tick_parity;
  int owner;
  RwSphere *sphere_ptr;
  int cam_mtx_or_stamp;
  RwCamera *camera;
  RwFrustumTestResult frustum_result;
  undefined4 shadow_flag_or_count;
  uint obj_parity;
  uint obj;
  bool test_this_frame;
  double shadow_angle;
  float *trigger_ptr;
  struct {
    RwMatrix *obj_mtx;
    float *shadow_a;
    float *shadow_b;
    int shadow_enabled;
    RwSphere sphere;
    RwMatrix *atomic_ltm;
    int outdoor;
    float to_cam_x;
    float to_cam_y;
    undefined1 pad_9c[4];
    float trig_x;
    float trig_y;
    float trig_z;
    float trig_radius;
    undefined4 trig_kind;
    int trig_owner;
    uint trig_obj_index;
    code *trig_callback;
  } frame;
  
  obj = 0;
  frame.outdoor = course_is_outdoor();
  frame.shadow_enabled = 1;
  if (0 < (int)g_rw_6fa7d0) {
    frame.obj_mtx = (RwMatrix *)&g_rw_6f6f90;
    frame.shadow_a = (float *)&g_rw_6fd4a8;
    frame.shadow_b = (float *)&g_rw_6fae98;
    do {
      tick_parity = g_tick_number & 0x80000001;
      if ((int)tick_parity < 0) {
        tick_parity = (tick_parity - 1 | 0xfffffffe) + 1;
      }
      obj_parity = obj & 0x80000001;
      if ((int)obj_parity < 0) {
        obj_parity = (obj_parity - 1 | 0xfffffffe) + 1;
      }
      atomic = (RpAtomic *)(&g_rw_6fab78)[obj];
      test_this_frame = obj_parity == tick_parity;
      if (game_mode == GAME_MODE_KILL_COPTER) {
        if ((int)(&g_rw_6fa4b0)[obj] < (int)g_effects_update_5ddad8) {
          frame.shadow_enabled = frame.outdoor;
        }
        else {
          test_this_frame = true;
          frame.shadow_enabled = 0;
        }
      }
      if ((atomic != (RpAtomic *)0x0) && ((*(RwUInt8 *)((char *)atomic + 2) & 4) != 0)) {
        frame.atomic_ltm = RwFrameGetLTM(RpAtomic_object_object_parent(atomic));
        owner = rwp_owner_object_get(atomic);
        if (owner != 0) {
          sphere_ptr = RpAtomicGetWorldBoundingSphere(atomic);
          frame.sphere.center.x = (sphere_ptr->center).x;
          frame.sphere.center.y = (sphere_ptr->center).y;
          frame.sphere.center.z = (sphere_ptr->center).z;
          frame.sphere.radius = sphere_ptr->radius * float_10_0;
          cam_mtx_or_stamp = get_viewport_camera_matrix(0);
          frame.to_cam_x = frame.sphere.center.x - *(float *)(cam_mtx_or_stamp + 0x30);
          cam_mtx_or_stamp = get_viewport_camera_matrix(0);
          frame.to_cam_y = frame.sphere.center.y - *(float *)(cam_mtx_or_stamp + 0x34);
          cam_mtx_or_stamp = get_viewport_camera_matrix(0);
          to_cam_z = frame.sphere.center.z - *(float *)(cam_mtx_or_stamp + 0x38);
          if (to_cam_z * to_cam_z + frame.to_cam_x * frame.to_cam_x + frame.to_cam_y * frame.to_cam_y < k_f_2500) {
            (&g_rw_6f6950)[obj] = (&g_rw_6f6c70)[obj];
            if (test_this_frame) {
              sphere_ptr = &frame.sphere;
              camera = (RwCamera *)get_viewport_camera(0);
              frustum_result = RwCameraFrustumTestSphere(camera,sphere_ptr);
              (&g_rw_6f6c70)[obj] = frustum_result;
            }
            if (((&g_rw_6f6c70)[obj] == 2) || ((&g_rw_6f6c70)[obj] == 1)) {
              if (test_this_frame) {
                frame.trig_radius = frame.sphere.radius * k_f_0_1;
                frame.trig_y = frame.sphere.center.y;
                cam_mtx_or_stamp = (&g_rw_6fa4b0)[obj];
                frame.trig_x = frame.sphere.center.x;
                frame.trig_z = frame.sphere.center.z;
                frame.trig_callback = (code *)0x0;
                frame.trig_kind = 10;
                if ((cam_mtx_or_stamp < 150000) || (0x24a21 < cam_mtx_or_stamp)) {
                  if ((cam_mtx_or_stamp < 1000) || (0x802 < cam_mtx_or_stamp)) {
                    if (((int)g_effects_update_5ddad8 <= cam_mtx_or_stamp) &&
                       (cam_mtx_or_stamp <= (int)g_effects_update_5ddad8 + 0x32)) {
                      frame.trig_callback = (code *)&LAB_00408ad0;
                      frame.trig_kind = 0x1c;
                    }
                  }
                  else if ((*(byte *)(&g_rw_6fcdd8 + obj) & 1) == 0) {
                    frame.trig_callback = rwpcar_ring_add_points_for_contact;
                  }
                }
                else {
                  frame.trig_kind = 0x1a;
                }
                trigger_ptr = &frame.trig_x;
                frame.trig_owner = owner;
                frame.trig_obj_index = obj;
                trigger_sphere_register(trigger_ptr);
                rwp_object_debris_expire_check(obj);
              }
              if (frame.shadow_enabled != 0) {
                if ((((&g_rw_6f6950)[obj] != 2) && ((&g_rw_6f6950)[obj] != 1)) ||
                   (*(int *)(*(char **)(owner + 0x10) + 4) != 0)) {
                  shadow_flag_or_count = shadow_project_to_ground_00499dc0
                                    (g_rw_801b14,frame.atomic_ltm,frame.shadow_a,frame.obj_mtx,frame.shadow_b);
                  (&g_effects_update_6fe770)[obj] = shadow_flag_or_count;
                }
                if ((&g_effects_update_6fe770)[obj] != 0) {
                  shadow_angle = (double)dot_product_3d_clamped(&frame.obj_mtx->at,&g_car_62ac74);
                  shadow_angle = (double)FUN_004b3284((double)shadow_angle);
                  if (shadow_angle < (double)k_f_0_7853982) {
                    shadow_flag_or_count = ((int)(((1.0f - (shadow_angle * 1.2732395f)) + 1.0f) * 64.0f));
                    fx_dust_add(frame.obj_mtx,shadow_flag_or_count);
                  }
                }
              }
            }
          }
        }
      }
      obj = obj + 1;
      frame.shadow_b = frame.shadow_b + 10;
      frame.shadow_a = frame.shadow_a + 6;
      frame.obj_mtx = frame.obj_mtx + 1;
    } while ((int)obj < (int)g_rw_6fa7d0);
  }
  return;
}





