// cluster-entries: weapon_find_targets_in_cone
/* Installed cluster weapon_find_targets_in_cone (tools/cluster_install.py from output/cluster_pilot_md/weapon_find_targets_in_cone.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char s_333_Lap_frames_005df368[];
/*00408a00*/ extern int scene_get_fx_651aa0();
/*00408a10*/ extern int __cdecl fx_object_get_sphere(undefined4 p0, undefined4 p1);
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*004220f0*/ extern int vehicle_get_position_158();
/*00422280*/ extern int vehicle_part_frame();
/*004222e0*/ extern int vehicle_get_matrix_ltm_b();
/*004223a0*/ extern int __cdecl vehicle_part_set_effect(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004223e0*/ extern int vehicle_get_field_18c();
/*0042a210*/ extern undefined __cdecl course_active_object_get_info(undefined4 p0, undefined4 p1);
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*00482070*/ extern double dot_product_3d_clamped();
/*0048e440*/ extern int rwpcar_get_rw_6fa7d0();
/*0048e6d0*/ extern int rwp_object_get_handle();
/*004b3284*/ extern double acos(double p0);
/*004c42f0*/ extern void rwframe_look_at(undefined4 p0, undefined4 p1, undefined4 p2);
/*004b3284*/ extern double crt_acos_body(double p0);   /* acos */
/*004c4720*/ extern void vec3_lerp(void *p0, void *p1, void *p2, int p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*00569110*/ extern RwBool __cdecl RtLineSphereIntersectionTest(void * p0, void * p1, void * p2);
/*0056c140*/ extern int __cdecl FUN_0056c140(int a1, int a2, int a3);
/*0056c3b0*/ extern int __cdecl FUN_0056c3b0(int a1, int a2, int a3, int a4, int a5);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd330*/ extern float float_1_0;
/*005dd374*/ extern float k_f_0_2;
/*005ddaec*/ extern float k_f_57_29578;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddc28*/ extern float k_f_0;
/*005de490*/ extern float k_f_0_02;
/*005df83c*/ extern float k_f_1_0en6;
/*006a9424*/ extern undefined4 g_weapon_6a9424;
/*006a9428*/ extern undefined4 g_weapon_6a9428;
/*006a942c*/ extern undefined4 g_weapon_6a942c;
/*006a9434*/ extern undefined g_weapon_find_6a9434;
/*006a9438*/ extern undefined g_weapon_find_6a9438;
/*006a943c*/ extern undefined g_weapon_find_6a943c;
/*006a9444*/ extern undefined g_weapon_6a9444;
/*006a9448*/ extern undefined g_weapon_find_6a9448;
/*006a944c*/ extern undefined g_weapon_find_6a944c;
/*006a9454*/ extern undefined1 g_weapon_6a9454;
/*006a9458*/ extern undefined4 g_weapon_6a9458;
/*006a945c*/ extern undefined g_weapon_find_6a945c;
/*006a9460*/ extern undefined g_weapon_find_6a9460;
/*006a9464*/ extern undefined g_weapon_find_6a9464;
/*006a9468*/ extern undefined g_weapon_find_6a9468;
/*006a946c*/ extern undefined g_weapon_find_6a946c;
/*006a9470*/ extern undefined g_weapon_find_6a9470;
/*006a9474*/ extern undefined4 g_weapon_6a9474;
/* weapon_find_targets_in_cone */

/* "(slot, pos, range, cone angle, dir): collects cars and fx objects inside the cone for homing /
   aiming" */

/* Target list: one 7-float record per other car (3) and per fx object (g_fx_object_count, at most 8: the
   object table at 0x651260 has 0xec byte entries up to the count at 0x651aa0).  The original frame only has
   room for 6 records (the hit record of world_intersect_line overlays records 4 and 5); more would run over its
   return address, so this port keeps room for all of them. */
#define TARGET_MAX_RECORDS (3 + 8)
#define TARGET_ARRAY_FLOATS (7 * TARGET_MAX_RECORDS)
#define HIT_RECORD (frame_obj2.target_recs + 28)
void weapon_find_targets_in_cone(int shooter_slot,float *origin,float range,float cone_deg,undefined4 *aim_dir_arg)
{
  undefined4 obj_pos_b;
  RwFrame *frame;
  int object_count;   /* number of rwp objects scanned in the final pass */
  int best_target;   /* index of the nearest record */
  int tmp_i;
  int owner_obj;
  undefined4 rw_world;
  int slot_used;
  float *rec_ptr;
  int cand_idx;
  int best_idx;
  int slot_off;
  int *rec_int_ptr;
  RwMatrix *frame_matrix_src;
  RwReal *car_matrix_dst;
  float10 angle_deg;
  RwReal dist;
  float *rec_aux;
  struct {
    RwV3d delta_vec;
    int target_count;
    int best_record;
    float ray_start_x;
    float ray_start_y;
    float ray_start_z;
    float ray_end_x;
    float ray_end_y;
    float ray_end_z;
    RwV3d target_pos;
    float target_radius;
    float best_dist;
    RwReal dist_to_target;
  } frame_obj;
  RwV3d best_obj_pos;
  struct {
    float obj_x;
    float obj_y;
    float obj_z;
    undefined1 pad_b4[4];
    float target_recs [TARGET_ARRAY_FLOATS];   /* target records (7 floats each); the hit record of world_intersect_line overlays it from float 28 on, as in the original frame */
  } frame_obj2;
  /* declared after the frame structs: the compiler then keeps them below the hit record, which must end at the return address */
  float seg_len, fcopy, best_d;
  double fscale;   /* product of two floats: exact in a double, the original keeps it unrounded in the x87 register */
  double dd;
  
  /* per slot aim record in the g_weapon_* table (0x58 bytes): target pos, distance, lock flag */
  slot_off = shooter_slot * 0x58;
  rec_ptr = frame_obj2.target_recs;
  for (slot_used = 0x1c; slot_used != 0; slot_used = slot_used + -1) {
    *rec_ptr = 0.0;
    rec_ptr = rec_ptr + 1;
  }
  *(undefined4 *)((char *)&g_weapon_6a9454 + slot_off) = *aim_dir_arg;
  *(float *)&(&g_weapon_6a9424)[shooter_slot * 0x16] = cone_deg;
  *(float *)((char *)&g_weapon_find_6a9438 + slot_off) = range;
  *(undefined4 *)((char *)&g_weapon_find_6a9434 + slot_off) = 0;
  (&g_weapon_6a942c)[shooter_slot * 0x16] = 0xffffffff;
  frame_obj.best_record = -1;
  *(float *)((char *)&g_weapon_find_6a9468 + slot_off) = *origin;
  *(float *)((char *)&g_weapon_find_6a946c + slot_off) = origin[1];
  *(float *)((char *)&g_weapon_find_6a9470 + slot_off) = origin[2];
  frame_obj.best_dist = 1000.0;
  frame_obj.target_count = 0;
  cand_idx = 4;
  vehicle_get_matrix_ltm_b(shooter_slot,(&g_weapon_6a9458)[shooter_slot * 0x16]);
  RwMatrixUpdate((&g_weapon_6a9458)[shooter_slot * 0x16]);
  rec_ptr = frame_obj2.target_recs + 3;
  do {
    cand_idx = (cand_idx + -1);
    if ((shooter_slot != cand_idx) && (slot_used = race_setup_slot_used(cand_idx), slot_used != 0)) {
      vehicle_get_position_158(cand_idx, &frame_obj.target_pos);
      frame_obj.delta_vec.x = frame_obj.target_pos.x - *origin;
      frame_obj.delta_vec.y = 0.0;
      frame_obj.delta_vec.z = frame_obj.target_pos.z - origin[2];
      frame_obj.dist_to_target = RwV3dLength(&frame_obj.delta_vec);
      if (k_f_1_0en5 <
          frame_obj.delta_vec.z * frame_obj.delta_vec.z + frame_obj.delta_vec.x * frame_obj.delta_vec.x + frame_obj.delta_vec.y * frame_obj.delta_vec.y) {
        RwV3dNormalize(&frame_obj.delta_vec,&frame_obj.delta_vec);
      }
      angle_deg = (float10)dot_product_3d_clamped
                                  (&frame_obj.delta_vec,(&g_weapon_6a9458)[shooter_slot * 0x16] + 0x20);
      angle_deg = (float10)crt_acos_body((double)angle_deg);
      angle_deg = angle_deg * (float10)k_f_57_29578;
      if ((angle_deg <= (float10)cone_deg) &&
         (frame_obj.dist_to_target <= range)) {
        rec_ptr[3] = (float)angle_deg;
        rec_ptr[-3] = frame_obj.target_pos.x;
        rec_ptr[-2] = frame_obj.target_pos.y;
        rec_ptr[2] = frame_obj.dist_to_target;
        frame_obj.target_count = (frame_obj.target_count + 1);
        *(int *)rec_ptr = cand_idx;
        *(int *)&rec_ptr[1] = -1;
        rec_ptr[-1] = frame_obj.target_pos.z;
        rec_ptr = rec_ptr + 7;
      }
    }
  } while (cand_idx != 0);
  /* pass 2: fx objects (spheres) inside the cone */
  slot_used = scene_get_fx_651aa0();
  if (slot_used != 0) {
    rec_int_ptr = (int *)frame_obj2.target_recs + 4 + frame_obj.target_count * 7 + -1;
    do {
      slot_used = slot_used + -1;
      tmp_i = fx_object_get_sphere(slot_used,&frame_obj.target_pos);
      if (tmp_i != 0) {
        frame_obj.delta_vec.x = frame_obj.target_pos.x - *origin;
        frame_obj.delta_vec.y = 0.0;
        frame_obj.delta_vec.z = frame_obj.target_pos.z - origin[2];
        frame_obj.dist_to_target = RwV3dLength(&frame_obj.delta_vec);
        if (k_f_1_0en5 <
            frame_obj.delta_vec.z * frame_obj.delta_vec.z + frame_obj.delta_vec.x * frame_obj.delta_vec.x + frame_obj.delta_vec.y * frame_obj.delta_vec.y) {
          RwV3dNormalize(&frame_obj.delta_vec,&frame_obj.delta_vec);
        }
        angle_deg = (float10)dot_product_3d_clamped
                                    (&frame_obj.delta_vec,(&g_weapon_6a9458)[shooter_slot * 0x16] + 0x20);
        angle_deg = (float10)crt_acos_body((double)angle_deg);
        angle_deg = angle_deg * (float10)k_f_57_29578;
        if ((angle_deg <= (float10)cone_deg) &&
           (frame_obj.dist_to_target <= range)) {
          *(float *)&rec_int_ptr[3] = (float)angle_deg;
          *(float *)&rec_int_ptr[-3] = frame_obj.target_pos.x;
          *(float *)&rec_int_ptr[-2] = frame_obj.target_pos.y;
          *(float *)&rec_int_ptr[2] = frame_obj.dist_to_target;
          frame_obj.target_count = (frame_obj.target_count + 1);
          *rec_int_ptr = -1;
          rec_int_ptr[1] = slot_used;
          *(float *)&rec_int_ptr[-1] = frame_obj.target_pos.z;
          rec_int_ptr = rec_int_ptr + 7;
        }
      }
    } while (slot_used != 0);
  }
  /* no car / fx target: aim straight ahead (range along the car's forward vector), clipped by the world */
  if (frame_obj.target_count == 0) {
    slot_used = (&g_weapon_6a9458)[shooter_slot * 0x16];
    *(undefined4 *)((char *)&g_weapon_find_6a943c + slot_off) = *(undefined4 *)((char *)&g_weapon_find_6a9438 + slot_off);
    (&g_weapon_6a9428)[shooter_slot * 0x16] = 0xffffffff;
    frame_obj.delta_vec.x = *(float *)(slot_used + 0x20);
    frame_obj.delta_vec.y = *(float *)(slot_used + 0x24);
    frame_obj.delta_vec.z = *(float *)(slot_used + 0x28);
    frame_obj.target_pos.z = frame_obj.delta_vec.z * range;
    frame_obj.ray_start_x = frame_obj.delta_vec.x * range + *origin;
    frame_obj.ray_end_y = range * k_f_0 + origin[1];
    rec_ptr = frame_obj2.target_recs;
    frame_obj.ray_start_z = frame_obj.target_pos.z + origin[2];
    rec_aux = &frame_obj.ray_start_x;
    frame_obj.ray_start_y = frame_obj.ray_end_y + float_1_0;
    frame_obj.ray_end_y = frame_obj.ray_end_y - float_1_0;
    frame_obj.ray_end_x = frame_obj.ray_start_x;
    frame_obj.ray_end_z = frame_obj.ray_start_z;
    rw_world = weapon_get_rw_801b14();
    slot_used = world_intersect_line(rw_world,rec_aux,rec_ptr);
    if (slot_used == 0) {
      slot_used = (&g_weapon_6a9458)[shooter_slot * 0x16];
      frame_obj.delta_vec.x = *(float *)(slot_used + 0x20);
      frame_obj.delta_vec.y = *(float *)(slot_used + 0x24);
      frame_obj.delta_vec.z = *(float *)(slot_used + 0x28);
      frame_obj.target_pos.z = frame_obj.delta_vec.z * range;
      *(float *)((char *)&g_weapon_find_6a945c + slot_off) = frame_obj.delta_vec.x * range + *origin;
      *(float *)((char *)&g_weapon_find_6a9460 + slot_off) = frame_obj.delta_vec.y * range + origin[1];
      fcopy = origin[2];
      *(undefined4 *)((char *)&g_weapon_find_6a9434 + slot_off) = 0;
      *(float *)((char *)&g_weapon_find_6a9464 + slot_off) = frame_obj.target_pos.z + fcopy;
    }
    else {
      vec3_lerp(&frame_obj.delta_vec,&frame_obj.ray_start_x,&frame_obj.ray_end_x,*(int *)&frame_obj2.target_recs[14]);
      frame_obj.delta_vec.x = frame_obj2.target_recs[0] * k_f_0_02 + frame_obj.delta_vec.x;
      *(undefined4 *)((char *)&g_weapon_find_6a9434 + slot_off) = 1;
      *(RwReal *)((char *)&g_weapon_find_6a945c + slot_off) = frame_obj.delta_vec.x;
      frame_obj.delta_vec.y = frame_obj2.target_recs[1] * k_f_0_02 + frame_obj.delta_vec.y;
      *(RwReal *)((char *)&g_weapon_find_6a9460 + slot_off) = frame_obj.delta_vec.y;
      frame_obj.delta_vec.z = frame_obj2.target_recs[2] * k_f_0_02 + frame_obj.delta_vec.z;
      *(RwReal *)((char *)&g_weapon_find_6a9464 + slot_off) = frame_obj.delta_vec.z;
    }
    *(undefined4 *)((char *)&g_weapon_6a9444 + slot_off) = *(undefined4 *)((char *)&g_weapon_find_6a945c + slot_off);
    *(undefined4 *)((char *)&g_weapon_find_6a9448 + slot_off) = *(undefined4 *)((char *)&g_weapon_find_6a9460 + slot_off);
    *(undefined4 *)((char *)&g_weapon_find_6a944c + slot_off) = *(undefined4 *)((char *)&g_weapon_find_6a9464 + slot_off);
  }
  else {
    rec_ptr = frame_obj2.target_recs + 6 + frame_obj.target_count * 7;
    cand_idx = frame_obj.target_count;
    do {
      rec_aux = rec_ptr + -7;
      rec_ptr = rec_ptr + -7;
      cand_idx = (cand_idx + -1);
      if (*rec_aux < frame_obj.best_dist) {
        frame_obj.best_dist = *rec_ptr;
        frame_obj.best_record = cand_idx;
      }
      best_target = frame_obj.best_record;
    } while (cand_idx != 0);
    slot_used = -1;
    if (frame_obj.best_record != -1) {
      tmp_i = ((int *)frame_obj2.target_recs + 4)[frame_obj.best_record * 7 + -1];
      frame_obj.target_count = -1;
      frame_obj.best_dist = cand_idx;
      if (tmp_i == -1) {
        owner_obj = frame_obj.best_record * 7;
        frame_obj.best_record = cand_idx;
        if (((int *)frame_obj2.target_recs + 4)[owner_obj] != -1) {
          course_active_object_get_info(((int *)frame_obj2.target_recs + 4)[owner_obj],&frame_obj.target_pos);
        }
      }
      else {
        frame_obj.best_record = cand_idx;
        vehicle_get_position_158(tmp_i, &frame_obj.target_pos);
        frame_obj.target_count = vehicle_get_field_18c(tmp_i);
        *(int *)&frame_obj.best_record = 1;
        slot_used = tmp_i;
      }
      frame_obj.ray_start_x = *origin;
      frame_obj.ray_start_y = origin[1];
      frame_obj.ray_start_z = origin[2];
      frame_obj.ray_end_y = frame_obj.target_radius * k_f_0_2 + frame_obj.target_pos.y;
      frame_obj.ray_end_z = frame_obj.target_pos.z;
      frame_obj.ray_end_x = frame_obj.target_pos.x;
      if ((frame_obj.best_record == 0) || (owner_obj = vehicle_part_frame(slot_used, frame_obj.target_count), owner_obj == 0)) {
        RtLineSphereIntersectionTest(&frame_obj.ray_start_x,&frame_obj.target_pos, &frame_obj.best_dist);
      }
      else {
        slot_used = vehicle_part_set_effect(slot_used,frame_obj.target_count,&frame_obj.ray_start_x,HIT_RECORD);
        if (slot_used != 0) {
          best_obj_pos.x = frame_obj.ray_end_x - frame_obj.ray_start_x;
          best_obj_pos.y = frame_obj.ray_end_y - frame_obj.ray_start_y;
          best_obj_pos.z = frame_obj.ray_end_z - frame_obj.ray_start_z;
          seg_len = RwV3dLength(&best_obj_pos);
          RwV3dNormalize(&best_obj_pos,&best_obj_pos);
          fscale = HIT_RECORD[14] * seg_len;
          frame_obj.delta_vec.x = best_obj_pos.x * fscale + frame_obj.ray_start_x;
          frame_obj2.target_recs[best_target * 7] = frame_obj.delta_vec.x;
          frame_obj.delta_vec.y = best_obj_pos.y * fscale + frame_obj.ray_start_y;
          frame_obj2.target_recs[best_target * 7 + 1] = frame_obj.delta_vec.y;
          frame_obj.delta_vec.z = best_obj_pos.z * fscale + frame_obj.ray_start_z;
          frame_obj2.target_recs[best_target * 7 + 2] = frame_obj.delta_vec.z;
        }
      }
      *(float *)((char *)&g_weapon_6a9444 + slot_off) = frame_obj2.target_recs[best_target * 7];
      fcopy = frame_obj2.target_recs[best_target * 7 + 2];
      *(float *)((char *)&g_weapon_find_6a9448 + slot_off) = frame_obj2.target_recs[best_target * 7 + 1];
      *(float *)((char *)&g_weapon_find_6a944c + slot_off) = fcopy;
      *(float *)((char *)&g_weapon_find_6a945c + slot_off) = frame_obj2.target_recs[best_target * 7];
      fcopy = frame_obj2.target_recs[best_target * 7 + 2];
      *(float *)((char *)&g_weapon_find_6a9460 + slot_off) = frame_obj2.target_recs[best_target * 7 + 1];
      *(float *)((char *)&g_weapon_find_6a9464 + slot_off) = fcopy;
      slot_used = ((int *)frame_obj2.target_recs + 4)[best_target * 7 + 1];
      (&g_weapon_6a9428)[shooter_slot * 0x16] = tmp_i;
      *(int *)((char *)&g_weapon_find_6a943c + slot_off) = slot_used;
      *(undefined4 *)((char *)&g_weapon_find_6a9434 + slot_off) = 1;
    }
  }
  frame_obj.ray_start_x = *(float *)((char *)&g_weapon_find_6a9468 + slot_off);
  frame_obj.ray_start_y = *(float *)((char *)&g_weapon_find_6a946c + slot_off);
  frame_obj.ray_start_z = *(float *)((char *)&g_weapon_find_6a9470 + slot_off);
  frame_obj.ray_end_x = *(float *)((char *)&g_weapon_find_6a945c + slot_off);
  frame_obj.ray_end_y = *(float *)((char *)&g_weapon_find_6a9460 + slot_off);
  frame_obj.ray_end_z = *(float *)((char *)&g_weapon_find_6a9464 + slot_off);
  rec_aux = HIT_RECORD;
  rec_ptr = &frame_obj.ray_start_x;
  rw_world = weapon_get_rw_801b14();
  slot_used = world_intersect_line(rw_world,rec_ptr,rec_aux);
  if (slot_used != 0) {
    frame_obj.target_pos.x = frame_obj.ray_end_x - frame_obj.ray_start_x;
    frame_obj.target_pos.y = frame_obj.ray_end_y - frame_obj.ray_start_y;
    frame_obj.target_pos.z = frame_obj.ray_end_z - frame_obj.ray_start_z;
    frame_obj.best_dist = RwV3dLength(&frame_obj.target_pos);
    vec3_lerp(&frame_obj.delta_vec,&frame_obj.ray_start_x,&frame_obj.ray_end_x,*(int *)&HIT_RECORD[14]);
    *(undefined4 *)((char *)&g_weapon_find_6a9434 + slot_off) = 1;
    (&g_weapon_6a9428)[shooter_slot * 0x16] = 0xffffffff;
    frame_obj.delta_vec.x = HIT_RECORD[0] * k_f_0_02 + frame_obj.delta_vec.x;
    *(RwReal *)((char *)&g_weapon_find_6a945c + slot_off) = frame_obj.delta_vec.x;
    frame_obj.delta_vec.y = HIT_RECORD[1] * k_f_0_02 + frame_obj.delta_vec.y;
    *(RwReal *)((char *)&g_weapon_find_6a9460 + slot_off) = frame_obj.delta_vec.y;
    frame_obj.delta_vec.z = HIT_RECORD[2] * k_f_0_02 + frame_obj.delta_vec.z;
    *(RwReal *)((char *)&g_weapon_find_6a9464 + slot_off) = frame_obj.delta_vec.z;
    *(RwReal *)((char *)&g_weapon_6a9444 + slot_off) = frame_obj.delta_vec.x;
    *(RwReal *)((char *)&g_weapon_find_6a9448 + slot_off) = frame_obj.delta_vec.y;
    *(float *)((char *)&g_weapon_find_6a943c + slot_off) = frame_obj.best_dist * HIT_RECORD[14];
    *(RwReal *)((char *)&g_weapon_find_6a944c + slot_off) = frame_obj.delta_vec.z;
  }
  /* final pass: rwp objects inside range and the aiming cone, nearest hit point wins */
  slot_used = rwpcar_get_rw_6fa7d0();
  frame_obj.best_dist = range * range;
  frame_obj.target_pos.x = frame_obj.ray_end_x - frame_obj.ray_start_x;
  best_idx = -1;
  best_d = 100000.0f;
  best_obj_pos.x = 0.0;
  frame_obj.target_pos.y = frame_obj.ray_end_y - frame_obj.ray_start_y;
  best_obj_pos.y = 0.0;
  best_obj_pos.z = 0.0;
  frame_obj.target_pos.z = frame_obj.ray_end_z - frame_obj.ray_start_z;
  object_count = slot_used;
  RwV3dNormalize(&frame_obj.target_pos,&frame_obj.target_pos);
  tmp_i = 0;
  if (0 < slot_used) {
    do {
      slot_used = rwp_object_get_handle(tmp_i);
      owner_obj = rwp_owner_object_get(slot_used);
      if ((((*(byte *)(slot_used + 2) & 4) != 0) && (*(int *)(owner_obj + 0x24) != 0)) &&
         ((*(uint *)(*(int *)(*(int *)(owner_obj + 0x24) + 100) +
                    (uint)(*(ushort *)(owner_obj + 0x20) >> 5) * 4) &
          1 << ((byte)*(ushort *)(owner_obj + 0x20) & 0x1f)) != 0)) {
        rw_world = **(undefined4 **)(owner_obj + 0x10);
        obj_pos_b = (*(undefined4 **)(owner_obj + 0x10))[2];
        FUN_0056c140(rw_world,obj_pos_b, &frame_obj2.obj_x);
        frame_obj.delta_vec.x = frame_obj2.obj_x - *(float *)((char *)&g_weapon_find_6a9468 + slot_off);
        frame_obj.delta_vec.y = frame_obj2.obj_y - *(float *)((char *)&g_weapon_find_6a946c + slot_off);
        frame_obj.delta_vec.z = frame_obj2.obj_z - *(float *)((char *)&g_weapon_find_6a9470 + slot_off);
        if (((frame_obj.delta_vec.z * frame_obj.delta_vec.z + frame_obj.delta_vec.x * frame_obj.delta_vec.x + frame_obj.delta_vec.y * frame_obj.delta_vec.y <
              frame_obj.best_dist) &&
            (RwV3dNormalize(&frame_obj.delta_vec,&frame_obj.delta_vec),
            *(float *)&s_333_Lap_frames_005df368 <
            frame_obj.delta_vec.z * frame_obj.target_pos.z + frame_obj.delta_vec.x * frame_obj.target_pos.x + frame_obj.delta_vec.y * frame_obj.target_pos.y)) &&
           ((slot_used = FUN_0056c3b0(rw_world,obj_pos_b,&frame_obj.ray_start_x,&frame_obj.ray_end_x, frame_obj2.target_recs), slot_used != 0 &&
            (dd = ((double)frame_obj2.target_recs[2] - frame_obj.ray_start_y) * ((double)frame_obj2.target_recs[2] - frame_obj.ray_start_y) +
                     ((double)frame_obj2.target_recs[1] - frame_obj.ray_start_x) * ((double)frame_obj2.target_recs[1] - frame_obj.ray_start_x) +
                     ((double)frame_obj2.target_recs[3] - frame_obj.ray_start_z) * ((double)frame_obj2.target_recs[3] - frame_obj.ray_start_z), dd < best_d)))) {
          best_obj_pos.x = frame_obj2.target_recs[1];
          best_obj_pos.y = frame_obj2.target_recs[2];
          best_obj_pos.z = frame_obj2.target_recs[3];
          best_d = (float)dd;
          best_idx = tmp_i;
        }
      }
      cand_idx = best_idx;
      tmp_i = tmp_i + 1;
    } while (tmp_i < object_count);
    if ((best_idx != -1) && (k_f_1_0en6 < best_d)) {
      dist = _rwSqrt(best_d);
      *(RwReal *)((char *)&g_weapon_find_6a943c + slot_off) = dist;
      *(RwReal *)((char *)&g_weapon_find_6a945c + slot_off) = best_obj_pos.x;
      *(RwReal *)((char *)&g_weapon_find_6a9460 + slot_off) = best_obj_pos.y;
      *(RwReal *)((char *)&g_weapon_find_6a9464 + slot_off) = best_obj_pos.z;
      *(RwReal *)((char *)&g_weapon_6a9444 + slot_off) = best_obj_pos.x;
      *(RwReal *)((char *)&g_weapon_find_6a9448 + slot_off) = best_obj_pos.y;
      *(undefined4 *)((char *)&g_weapon_find_6a9434 + slot_off) = 1;
      *(RwReal *)((char *)&g_weapon_find_6a944c + slot_off) = best_obj_pos.z;
      (&g_weapon_6a9428)[shooter_slot * 0x16] = 0xffffffff;
      (&g_weapon_6a942c)[shooter_slot * 0x16] = cand_idx;
    }
  }
  frame = *(RwFrame **)((&g_weapon_6a9474)[shooter_slot * 0x16] + 4);
  RwFrameSetIdentity(frame);
  RwFrameTranslate(frame,(RwV3d *)((char *)&g_weapon_find_6a9468 + slot_off),rwCOMBINEREPLACE);
  rwframe_look_at(frame,&g_weapon_find_6a945c + slot_off,(&g_weapon_6a9458)[shooter_slot * 0x16] + 0x10);
  frame_matrix_src = &RwFrame_modelling(frame);
  car_matrix_dst = (RwReal *)(&g_weapon_6a9458)[shooter_slot * 0x16];
  for (slot_used = 0x10; slot_used != 0; slot_used = slot_used + -1) {
    *car_matrix_dst = (frame_matrix_src->right).x;
    frame_matrix_src = (RwMatrix *)&(frame_matrix_src->right).y;
    car_matrix_dst = car_matrix_dst + 1;
  }
  return;
}

