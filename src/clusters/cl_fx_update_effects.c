// cluster-entries: fx_update_effects
/* Installed cluster fx_update_effects (tools/cluster_install.py from output/cluster_pilot_md/fx_update_effects.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004075c0*/ extern int fx_update_all();
/*00407db0*/ extern undefined __cdecl fx_arm_weapon_slots(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13);
/*00407e30*/ extern int fx_distance_check();
/*004220f0*/ extern int vehicle_get_position_158();
/*0042a3b0*/ extern int course_get_world_object();
/*00482010*/ extern int random_int_range();
/*0048e4f0*/ extern int rwp_object_set_enabled();
/*0048e650*/ extern int rwp_object_set_in_world();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*004c4460*/ extern void rwmatrix_look_at(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c48b0*/ extern void hanim_keyframes_sample_at_time(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4b40*/ extern RwMatrix * RwMatrixQueryRotate(RwMatrix *p0, struct RwV3d *p1, RwReal *p2, struct RwV3d *p3);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dd40c*/ extern float k_f_1_5;
/*005dd41c*/ extern float float_5_0;
/*005ddb50*/ extern float k_f_8;
/*005ddb54*/ extern float k_f_0_0055555557;
/*00651250*/ extern undefined g_scene_651250[0x1000];
/*0065132c*/ extern undefined4 g_fx_65132c;
/*00651aa0*/ extern undefined4 g_fx_object_count;
/*007ef7f4*/ extern float g_tick_seconds;
static float10 explosion_spawn_fx_r(void *fx_obj);
extern void explosion_spawn_fx(void);
__declspec(naked) float10 explosion_spawn_fx_r(void *fx_obj)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call explosion_spawn_fx
        ret
    }
}

static int fx_course_specific_update_r(void *fx_obj);
extern void fx_course_specific_update(void);
__declspec(naked) int fx_course_specific_update_r(void *fx_obj)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call fx_course_specific_update
        ret
    }
}

/* fx_update_effects */

/* "fx_update_all then the effect object table at 0x651250 (0x651AA0 entries)" */

void fx_update_effects(void)

{
  int *body_link;
  float tmp_f;
  int tmp_i;
  RwMatrix *obj_matrix;
  RwMatrix *mat_a;
  int next_idx;
  float *body_vec;
  FxSceneObject *fx_obj;
  RwMatrix *mat_b;
  float10 fl_tmp;
  undefined *obj_ptr_copy;
  RwMatrix *matrix;
  RwV3d *unitAxis;
  RwReal *angle;
  RwV3d *center;
  RwMatrix *matrix_00;
  RwMatrix *transform;
  undefined4 render_arg_a;
  RwV3d *pRVar11;
  undefined4 render_arg_b;
  struct {
    RwV3d pivot_vec;
    int object_index;
    RwV3d *obj_aux_ptr;
    float body_pos_x;
    float body_pos_y;
    float body_pos_z;
    float body_vel_x;
    float body_vel_y;
    float body_vel_z;
    RwV3d tmp_vec;
    float rot_angle;
    int *body_link_saved;
    undefined4 up_x;
    undefined4 up_y;
    undefined4 up_z;
    RwV3d rot_axis;
    RwMatrix matrix_d;
    undefined1 car_pos [16];
    RwMatrix matrix_c;
    RwMatrix pivot_matrix;
    RwMatrix inv_matrix;
  } frame;
  
  fx_update_all();
  /* all effects, then every effect object of the table g_scene_651250 (236 byte records): follow its rwp object, spin the child frames, drive its physics body */
  vehicle_get_position_158(0,frame.car_pos);
  frame.object_index = 0;
  if (0 < (int)g_fx_object_count) {
    fx_obj = (FxSceneObject *)g_scene_651250;
    frame.obj_aux_ptr = (RwV3d *)&g_fx_65132c;
    do {
      if (fx_obj->owner == 0) {
        return;
      }
      fx_obj->age = g_tick_seconds + fx_obj->age;
      rwp_object_set_in_world(fx_obj->rwp_object,1);
      tmp_i = rwp_owner_object_get(*(undefined4 *)&fx_obj->owner);
      rwp_world_set_bit_5c(*(undefined4 *)(tmp_i + 0x24),tmp_i,1);
      rwp_object_set_enabled(fx_obj->rwp_object,1);
      body_link = *(int **)(*(int *)(tmp_i + 0x10) + 4);
      frame.body_link_saved = body_link;
      obj_matrix = (RwMatrix *)rwp_object_get_matrix(fx_obj->rwp_object);
      mat_a = RwFrameGetLTM(*(RwFrame **)(fx_obj->frame_a + 4));
      fx_obj->offset_x = (obj_matrix->pos).x - (mat_a->pos).x;
      mat_a = RwFrameGetLTM(*(RwFrame **)(fx_obj->frame_a + 4));
      fx_obj->offset_y = (obj_matrix->pos).y - (mat_a->pos).y;
      mat_a = RwFrameGetLTM(*(RwFrame **)(fx_obj->frame_a + 4));
      fx_obj->offset_z = (obj_matrix->pos).z - (mat_a->pos).z;
      RwFrameTransform(*(RwFrame **)(fx_obj->frame_a + 4),obj_matrix,rwCOMBINEREPLACE);
      fl_tmp = (float10)explosion_spawn_fx_r((void *)fx_obj);
      fx_obj->spin_speed = (float)fl_tmp;
      if (fx_obj->use_b == 0) {
        tmp_i = *(int *)(fx_obj->spin_a + 4);
        mat_a = (RwMatrix *)(tmp_i + 0x10);
        frame.pivot_vec.x = *(float *)(tmp_i + 0x40) * k_f_n1;
        frame.pivot_vec.y = *(float *)(tmp_i + 0x44) * k_f_n1;
        frame.pivot_vec.z = *(float *)(tmp_i + 0x48) * k_f_n1;
        RwMatrixTranslate(mat_a,&frame.pivot_vec,rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(mat_a,(RwV3d *)(tmp_i + 0x20),1.0,rwCOMBINEPOSTCONCAT);
        frame.pivot_vec.x = frame.pivot_vec.x * k_f_n1;
        frame.pivot_vec.y = frame.pivot_vec.y * k_f_n1;
        frame.pivot_vec.z = frame.pivot_vec.z * k_f_n1;
        RwMatrixTranslate(mat_a,&frame.pivot_vec,rwCOMBINEPOSTCONCAT);
        tmp_i = *(int *)(fx_obj->spin_b + 4);
        mat_a = (RwMatrix *)(tmp_i + 0x10);
        frame.pivot_vec.x = *(float *)(tmp_i + 0x40) * k_f_n1;
        frame.pivot_vec.y = *(float *)(tmp_i + 0x44) * k_f_n1;
        frame.pivot_vec.z = *(float *)(tmp_i + 0x48) * k_f_n1;
        RwMatrixTranslate(mat_a,&frame.pivot_vec,rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(mat_a,(RwV3d *)(tmp_i + 0x20),1.0,rwCOMBINEPOSTCONCAT);
        frame.pivot_vec.x = frame.pivot_vec.x * k_f_n1;
        frame.pivot_vec.y = frame.pivot_vec.y * k_f_n1;
        frame.pivot_vec.z = frame.pivot_vec.z * k_f_n1;
        RwMatrixTranslate(mat_a,&frame.pivot_vec,rwCOMBINEPOSTCONCAT);
        if (body_link != (int *)0x0) {
          hanim_keyframes_sample_at_time((undefined4)&frame.pivot_matrix,*(undefined4 *)&fx_obj->cur,*(undefined4 *)&fx_obj->timer);
          pRVar11 = frame.obj_aux_ptr;
          fx_obj->vel.y = fx_obj->gravity + fx_obj->vel.y;
          RwMatrixTranslate(&frame.pivot_matrix,frame.obj_aux_ptr,rwCOMBINEPOSTCONCAT);
          pRVar11->x = pRVar11->x * fx_obj->damping;
          fx_obj->vel.y =
               (fx_obj->vel.y - fx_obj->gravity) *
               fx_obj->damping;
          fx_obj->vel.z = fx_obj->vel.z * fx_obj->damping;
          frame.body_vel_x = frame.pivot_matrix.pos.x - (obj_matrix->pos).x;
          frame.body_vel_y = frame.pivot_matrix.pos.y - (obj_matrix->pos).y;
          body_vec = (float *)(*(int *)(*(int *)(*body_link + 0x10) + 8) + 0x10 + body_link[1] * 0x20);
          frame.body_pos_x = *body_vec;
          frame.body_pos_y = body_vec[1];
          frame.body_pos_z = body_vec[2];
          frame.body_vel_z = frame.pivot_matrix.pos.z - (obj_matrix->pos).z;
          mat_a = obj_matrix;
          mat_b = &frame.matrix_c;
          for (tmp_i = 0x10; tmp_i != 0; tmp_i = tmp_i + -1) {
            (mat_b->right).x = (mat_a->right).x;
            mat_a = (RwMatrix *)&(mat_a->right).y;
            mat_b = (RwMatrix *)&(mat_b->right).y;
          }
          frame.tmp_vec.x = frame.matrix_c.pos.x * k_f_n1;
          frame.tmp_vec.y = frame.matrix_c.pos.y * k_f_n1;
          frame.tmp_vec.z = frame.matrix_c.pos.z * k_f_n1;
          RwMatrixTranslate(&frame.matrix_c,&frame.tmp_vec,rwCOMBINEPOSTCONCAT);
          tmp_i = 0x10;
          if (fx_obj->flag_a4 == 0) {
            obj_matrix = &frame.pivot_matrix;
            mat_a = &frame.matrix_d;
            for (; tmp_i != 0; tmp_i = tmp_i + -1) {
              (mat_a->right).x = (obj_matrix->right).x;
              obj_matrix = (RwMatrix *)&(obj_matrix->right).y;
              mat_a = (RwMatrix *)&(mat_a->right).y;
            }
          }
          else {
            mat_a = &frame.matrix_d;
            for (; tmp_i != 0; tmp_i = tmp_i + -1) {
              (mat_a->right).x = (obj_matrix->right).x;
              obj_matrix = (RwMatrix *)&(obj_matrix->right).y;
              mat_a = (RwMatrix *)&(mat_a->right).y;
            }
            frame.up_x = 0;
            frame.up_y = 0x3f800000;
            frame.up_z = 0;
            rwmatrix_look_at((undefined4)&frame.matrix_d,(undefined4)frame.car_pos,(undefined4)&frame.up_x);
          }
          render_arg_b = 2;
          frame.tmp_vec.x = frame.matrix_d.pos.x * k_f_n1;
          pRVar11 = &frame.tmp_vec;
          obj_matrix = &frame.matrix_d;
          frame.tmp_vec.y = frame.matrix_d.pos.y * k_f_n1;
          frame.tmp_vec.z = frame.matrix_d.pos.z * k_f_n1;
          RwMatrixTranslate(obj_matrix,pRVar11,rwCOMBINEPOSTCONCAT);
          mat_a = &frame.matrix_c;
          mat_b = &frame.inv_matrix;
          RwMatrixInvert(mat_b,mat_a);
          render_arg_a = 1;
          transform = &frame.inv_matrix;
          matrix_00 = &frame.matrix_d;
          RwMatrixTransform(matrix_00,transform,rwCOMBINEPRECONCAT);
          center = &frame.tmp_vec;
          angle = &frame.rot_angle;
          unitAxis = &frame.rot_axis;
          matrix = &frame.matrix_d;
          RwMatrixQueryRotate(matrix,unitAxis,angle,center);
          tmp_f = frame.rot_angle * k_f_0_0055555557 * k_f_1_5;
          frame.body_pos_x = frame.rot_axis.x * tmp_f + frame.body_pos_x;
          frame.body_pos_y = frame.rot_axis.y * tmp_f + frame.body_pos_y;
          frame.body_pos_z = frame.rot_axis.z * tmp_f + frame.body_pos_z;
          *(float *)&frame.body_link_saved[6] = float_1_0 /
                              ((fx_obj->spin_speed * k_f_8 + float_5_0) * g_tick_seconds);
          body_vec = (float *)(*(int *)(*(int *)(*frame.body_link_saved + 0x10) + 8) + 0x10 + frame.body_link_saved[1] * 0x20)
          ;
          *body_vec = frame.body_pos_x;
          body_vec[1] = frame.body_pos_y;
          body_vec[2] = frame.body_pos_z;
          frame.body_vel_x = frame.body_vel_x * k_f_3;
          frame.body_vel_y = frame.body_vel_y * k_f_3;
          body_vec = (float *)(*(int *)(*(int *)(*frame.body_link_saved + 0x10) + 8) + frame.body_link_saved[1] * 0x20);
          *body_vec = frame.body_vel_x;
          frame.body_vel_z = frame.body_vel_z * k_f_3;
          body_vec[1] = frame.body_vel_y;
          body_vec[2] = frame.body_vel_z;
          /* updates the body towards the transformed matrix, distance check, weapon slots, course specific update; then advance along the path (random next node when the segment time is over) */
          obj_ptr_copy = (undefined *)fx_obj;
          fx_distance_check(fx_obj);
          fx_arm_weapon_slots((undefined4)frame.object_index,(undefined4)obj_ptr_copy,(undefined4)matrix,(undefined4)unitAxis,(undefined4)angle,(undefined4)center,(undefined4)matrix_00,(undefined4)transform,
                              render_arg_a,(undefined4)mat_b,(undefined4)mat_a,(undefined4)obj_matrix,(undefined4)pRVar11,render_arg_b);
          fx_course_specific_update_r((void *)fx_obj);
          tmp_f = g_tick_seconds * fx_obj->timer_rate + fx_obj->timer;
          fx_obj->timer = tmp_f;
          if (*(float *)(fx_obj->cur + 0xc) < tmp_f) {
            tmp_i = course_get_world_object();
            next_idx = random_int_range(0,fx_obj->choice_count + -1);
            fx_obj->choice_index = next_idx;
            *(undefined4 *)&fx_obj->cur =
                 *(undefined4 *)(tmp_i + 0xc9b0 + fx_obj->choice[next_idx] * 4);
            *(undefined4 *)&fx_obj->timer = 0;
          }
          frame.object_index = frame.object_index + 1;
          fx_obj = fx_obj + 1;
          frame.obj_aux_ptr = (RwV3d *)&frame.obj_aux_ptr[0x13].z;
        }
      }
      frame.object_index = frame.object_index + 1;
      frame.obj_aux_ptr = (RwV3d *)&frame.obj_aux_ptr[0x13].z;
      fx_obj = fx_obj + 1;
    } while (frame.object_index < (int)g_fx_object_count);
  }
  return;
}

