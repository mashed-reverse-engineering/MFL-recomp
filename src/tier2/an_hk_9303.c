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
  float fVar1;
  uint uVar2;
  int iVar3;
  RwSphere *pRVar4;
  int camera_matrix;
  RwCamera *camera;
  RwFrustumTestResult RVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  bool bVar9;
  double fVar10;
  float *pfVar11;
  struct {
    RwMatrix *local_cc;
    float *local_c8;
    float *local_c4;
    int local_c0;
    RwSphere local_bc;
    RwMatrix *local_ac;
    int local_a8;
    float local_a4;
    float local_a0;
    undefined1 pad_9c[4];
    float local_98;
    float local_94;
    float local_90;
    float local_8c;
    undefined4 local_88;
    int local_84;
    uint local_80;
    code *local_7c;
  } frame;
  
  uVar8 = 0;
  frame.local_a8 = course_is_outdoor();
  frame.local_c0 = 1;
  if (0 < (int)g_rw_6fa7d0) {
    frame.local_cc = (RwMatrix *)&g_rw_6f6f90;
    frame.local_c8 = (float *)&g_rw_6fd4a8;
    frame.local_c4 = (float *)&g_rw_6fae98;
    do {
      uVar2 = g_tick_number & 0x80000001;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
      }
      uVar7 = uVar8 & 0x80000001;
      if ((int)uVar7 < 0) {
        uVar7 = (uVar7 - 1 | 0xfffffffe) + 1;
      }
      atomic = (RpAtomic *)(&g_rw_6fab78)[uVar8];
      bVar9 = uVar7 == uVar2;
      if (game_mode == GAME_MODE_KILL_COPTER) {
        if ((int)(&g_rw_6fa4b0)[uVar8] < (int)g_effects_update_5ddad8) {
          frame.local_c0 = frame.local_a8;
        }
        else {
          bVar9 = true;
          frame.local_c0 = 0;
        }
      }
      if ((atomic != (RpAtomic *)0x0) && ((*(RwUInt8 *)((char *)atomic + 2) & 4) != 0)) {
        frame.local_ac = RwFrameGetLTM(RpAtomic_object_object_parent(atomic));
        iVar3 = rwp_owner_object_get(atomic);
        if (iVar3 != 0) {
          pRVar4 = RpAtomicGetWorldBoundingSphere(atomic);
          frame.local_bc.center.x = (pRVar4->center).x;
          frame.local_bc.center.y = (pRVar4->center).y;
          frame.local_bc.center.z = (pRVar4->center).z;
          frame.local_bc.radius = pRVar4->radius * float_10_0;
          camera_matrix = get_viewport_camera_matrix(0);
          frame.local_a4 = frame.local_bc.center.x - *(float *)(camera_matrix + 0x30);
          camera_matrix = get_viewport_camera_matrix(0);
          frame.local_a0 = frame.local_bc.center.y - *(float *)(camera_matrix + 0x34);
          camera_matrix = get_viewport_camera_matrix(0);
          fVar1 = frame.local_bc.center.z - *(float *)(camera_matrix + 0x38);
          if (fVar1 * fVar1 + frame.local_a4 * frame.local_a4 + frame.local_a0 * frame.local_a0 < k_f_2500) {
            (&g_rw_6f6950)[uVar8] = (&g_rw_6f6c70)[uVar8];
            if (bVar9) {
              pRVar4 = &frame.local_bc;
              camera = (RwCamera *)get_viewport_camera(0);
              RVar5 = RwCameraFrustumTestSphere(camera,pRVar4);
              (&g_rw_6f6c70)[uVar8] = RVar5;
            }
            if (((&g_rw_6f6c70)[uVar8] == 2) || ((&g_rw_6f6c70)[uVar8] == 1)) {
              if (bVar9) {
                frame.local_8c = frame.local_bc.radius * k_f_0_1;
                frame.local_94 = frame.local_bc.center.y;
                camera_matrix = (&g_rw_6fa4b0)[uVar8];
                frame.local_98 = frame.local_bc.center.x;
                frame.local_90 = frame.local_bc.center.z;
                frame.local_7c = (code *)0x0;
                frame.local_88 = 10;
                if ((camera_matrix < 150000) || (0x24a21 < camera_matrix)) {
                  if ((camera_matrix < 1000) || (0x802 < camera_matrix)) {
                    if (((int)g_effects_update_5ddad8 <= camera_matrix) &&
                       (camera_matrix <= (int)g_effects_update_5ddad8 + 0x32)) {
                      frame.local_7c = (code *)&LAB_00408ad0;
                      frame.local_88 = 0x1c;
                    }
                  }
                  else if ((*(byte *)(&g_rw_6fcdd8 + uVar8) & 1) == 0) {
                    frame.local_7c = rwpcar_ring_add_points_for_contact;
                  }
                }
                else {
                  frame.local_88 = 0x1a;
                }
                pfVar11 = &frame.local_98;
                frame.local_84 = iVar3;
                frame.local_80 = uVar8;
                trigger_sphere_register(pfVar11);
                rwp_object_debris_expire_check(uVar8);
              }
              if (frame.local_c0 != 0) {
                if ((((&g_rw_6f6950)[uVar8] != 2) && ((&g_rw_6f6950)[uVar8] != 1)) ||
                   (*(int *)(*(char **)(iVar3 + 0x10) + 4) != 0)) {
                  uVar6 = shadow_project_to_ground_00499dc0
                                    (g_rw_801b14,frame.local_ac,frame.local_c8,frame.local_cc,frame.local_c4);
                  (&g_effects_update_6fe770)[uVar8] = uVar6;
                }
                if ((&g_effects_update_6fe770)[uVar8] != 0) {
                  fVar10 = (double)dot_product_3d_clamped(&frame.local_cc->at,&g_car_62ac74);
                  fVar10 = (double)FUN_004b3284((double)fVar10);
                  if (fVar10 < (double)k_f_0_7853982) {
                    uVar6 = ((int)(((1.0f - (fVar10 * 1.2732395f)) + 1.0f) * 64.0f));
                    fx_dust_add(frame.local_cc,uVar6);
                  }
                }
              }
            }
          }
        }
      }
      uVar8 = uVar8 + 1;
      frame.local_c4 = frame.local_c4 + 10;
      frame.local_c8 = frame.local_c8 + 6;
      frame.local_cc = frame.local_cc + 1;
    } while ((int)uVar8 < (int)g_rw_6fa7d0);
  }
  return;
}





