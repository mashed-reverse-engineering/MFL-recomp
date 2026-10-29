// cluster-entries: fx_ring_update
/* Installed cluster fx_ring_update (tools/cluster_install.py from output/cluster_pilot_md/fx_ring_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int CourseId;
extern char LAB_00499120[];
/*0042b110*/ extern int current_course_id();
/*0044a4f0*/ extern int camera_get_position();
/*0045d590*/ extern int course_zone_contains_matrix_in_zone_6a2888();
/*004726a0*/ extern int sfx_play_at();
/*00472a10*/ extern undefined __cdecl sound_sfx_update_position_volume(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*00472b10*/ extern int sound_sfx_set_volume_clamped();
/*00481fd0*/ extern double random_range();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*00486890*/ extern int gfx_set_params_6c5188();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*00496850*/ extern int trigger_sphere_register(float *param_1);
/*004b2b48*/ extern int _ftol();
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd328*/ extern float k_f_0_6;
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd370*/ extern float k_f_0_3;
/*005ddb58*/ extern float k_f_1000;
/*005e64a4*/ extern float k_f_0;
/*005de290*/ extern float float_50_0;
/*005de3a0*/ extern float k_f_1_3;
/*005de490*/ extern float k_f_0_02;
/*005df99c*/ extern float k_f_22050;
/*005e0848*/ extern float k_f_0_02_5e0848;
/*005e0850*/ extern undefined g_effects_update_5e0850;
/*005e0854*/ extern undefined g_effects_update_5e0854;
/*005e0858*/ extern undefined g_effects_update_5e0858;
/*005e085c*/ extern undefined g_effects_update_5e085c;
/*005e0950*/ extern float k_f_0_5_5e0950;
/*005e0954*/ extern float k_f_0_5_5e0954;
/*005e0958*/ extern undefined4 g_effects_update_5e0958;
/*00752180*/ extern undefined g_load_752180;
/*007521c0*/ extern undefined g_sound_7521c0;
/*00752200*/ extern undefined1 g_particles_752200;
/*00752230*/ extern undefined4 g_sound_752230;
/*00752258*/ extern undefined4 g_sound_752258[0x1000];
/*00752298*/ extern undefined4 g_particles_752298[0x1000];
/*007ef7f4*/ extern float g_tick_seconds;
/* fx_ring_update */
#define COURSE_WARZONE (33)

/* "updates the 256 entry fx marker ring filled by fx_ring_add_point: finds the 4 nearest points to
   the camera and emits particles from them" */

void fx_ring_update(void)

{
  float fbits1;
  float fbits0;
  double ddx, ddy, ddz;
  byte *live_bitmap_byte_ptr;
  byte live_bitmap_byte;
  float *attach_pos;
  bool swapped_01;
  bool nearest_dirty;
  float saved_dist_1;
  double fsz;
  float saved_dist_2;
  float saved_dist_3;
  float point_dist_sq;
  float cam_dz;
  float cam_dy;
  RwV3d *saved_pt;
  undefined1 unused_13;
  CourseId course_id;
  int uv_offset;
  RwReal RVar15;
  byte bit_in_byte;
  RwV3d *point;
  float10 jitter;
  struct {
    int slot_index;
    undefined4 color_rgba;
    undefined4 *slot_aux_ptr;
    float scratch_f;
    undefined4 *slot_matrix_ptr;
    float nearest_dist_sq [4];
    RwV3d *nearest_pts [4];
    int nearest_count;
    int emitter_flag; /* word 9 x of the point: an int flag */
    int any_attached;
    RwReal particle_pos_x;
    float particle_pos_y;
    RwReal particle_pos_z;
    float particle_size_w;
    float particle_size_h;
    float cam_pos_x;
    float cam_pos_y;
    float cam_pos_z;
    undefined4 uv_rect_0;
    undefined4 uv_rect_1;
    undefined4 uv_rect_2;
    undefined4 uv_rect_3;
    RwMatrix zone_matrix;
    RwReal trig_sphere_x;
    RwReal trig_sphere_y;
    RwReal trig_sphere_z;
    undefined4 trig_sphere_radius;
    undefined4 trig_sphere_kind;
    RwV3d *trig_sphere_owner;
    undefined4 trig_sphere_unused;
    undefined1 *trig_sphere_callback;
  } frame;
  
  /* init: empty nearest-4 list (distances start at 1000.0), camera position */
  nearest_dirty = false;
  frame.slot_index = 0;
  frame.any_attached = 0;
  frame.nearest_pts[0] = (RwV3d *)0x0;
  frame.nearest_pts[1] = (RwV3d *)0x0;
  frame.nearest_pts[2] = (RwV3d *)0x0;
  frame.nearest_pts[3] = (RwV3d *)0x0;
  frame.nearest_dist_sq[0] = 1000.0;
  frame.nearest_dist_sq[1] = 1000.0;
  frame.nearest_dist_sq[2] = 1000.0;
  frame.nearest_dist_sq[3] = 1000.0;
  frame.nearest_count = 0;
  camera_get_position(&frame.cam_pos_x);
  point = (RwV3d *)&g_sound_752230;
  frame.slot_matrix_ptr = g_sound_752258;
  frame.slot_aux_ptr = g_particles_752298;
  do {
    /* slot liveness is a bitmap (g_particles_752200); a slot record is 29 dwords */
    bit_in_byte = (byte)frame.slot_index & 7;
    live_bitmap_byte = (&g_particles_752200)[frame.slot_index >> 3];
    live_bitmap_byte_ptr = &g_particles_752200 + (frame.slot_index >> 3);
    if ((live_bitmap_byte & (byte)(1 << bit_in_byte)) != 0) {
      attach_pos = *(float **)&point[9].y;
      if (attach_pos == (float *)0x0) {
        frame.emitter_flag = *(int *)&point[9].x;
      }
      else {
        frame.emitter_flag = 1;
        frame.any_attached = 1;
        if (((*attach_pos == k_f_0) && (attach_pos[1] == k_f_0)) && (attach_pos[2] == k_f_0)) {
          *frame.slot_aux_ptr = 0;
          point[9].y = 0.0;
          *live_bitmap_byte_ptr = ~('\x01' << bit_in_byte) & live_bitmap_byte;
        }
        else {
          ddx = (double)point->x - (double)*attach_pos;
          ddy = (double)point->y - (double)attach_pos[1];
          ddz = (double)point->z - (double)attach_pos[2];
          point->x = *attach_pos;
          point->y = attach_pos[1];
          point->z = attach_pos[2];
          frame.scratch_f = (float)(ddy * ddy + ddx * ddx + ddz * ddz);
          if (k_f_0_01 < frame.scratch_f) {
            jitter = (float10)random_range(0xbca3d70a,0x3ca3d70a);
            point->x = (float)(jitter + (float10)point->x);
            jitter = (float10)random_range(0xbca3d70a,0x3ca3d70a);
            point->y = (float)(jitter + (float10)point->y);
            jitter = (float10)random_range(0xbca3d70a,0x3ca3d70a);
            point->z = (float)(jitter + (float10)point->z);
          }
          if (k_f_0_02 < frame.scratch_f) {
            jitter = (float10)random_range(0xbd23d70a,0x3d23d70a);
            point->x = (float)(jitter + (float10)point->x);
            jitter = (float10)random_range(0xbd23d70a,0x3d23d70a);
            point->y = (float)(jitter + (float10)point->y);
            jitter = (float10)random_range(0xbd23d70a,0x3d23d70a);
            point->z = (float)(jitter + (float10)point->z);
            jitter = (float10)random_range(0xbca3d70a,0x3ca3d70a);
            point[1].z = (float)(jitter + (float10)point[1].z);
          }
        }
      }
      /* WARZONE: transform the point by its zone matrix */
      course_id = current_course_id();
      if (course_id == COURSE_WARZONE) {
        frame.zone_matrix.flags = frame.zone_matrix.flags | 0x20003;
        frame.zone_matrix.at.z = 1.0;
        frame.zone_matrix.up.y = 1.0;
        frame.zone_matrix.right.x = 1.0;
        frame.zone_matrix.up.x = 0.0;
        frame.zone_matrix.right.z = 0.0;
        frame.zone_matrix.right.y = 0.0;
        frame.zone_matrix.at.y = 0.0;
        frame.zone_matrix.at.x = 0.0;
        frame.zone_matrix.up.z = 0.0;
        frame.zone_matrix.pos.z = 0.0;
        frame.zone_matrix.pos.y = 0.0;
        frame.zone_matrix.pos.x = 0.0;
        RwMatrixTranslate(&frame.zone_matrix,point,rwCOMBINEREPLACE);
        course_zone_contains_matrix_in_zone_6a2888(&frame.zone_matrix,&frame.zone_matrix);
        point->x = frame.zone_matrix.pos.x;
        point->y = frame.zone_matrix.pos.y;
        point->z = frame.zone_matrix.pos.z;
        course_zone_contains_matrix_in_zone_6a2888(frame.slot_matrix_ptr,frame.slot_matrix_ptr);
      }
      frame.scratch_f = k_f_0_02_5e0848 * g_tick_seconds + point[2].z;
      point[2].z = frame.scratch_f;
      if (float_1_0 < frame.scratch_f) {
        live_bitmap_byte = *live_bitmap_byte_ptr;
        *frame.slot_aux_ptr = 0;
        point[9].y = 0.0;
        *live_bitmap_byte_ptr = live_bitmap_byte & ~('\x01' << bit_in_byte);
      }
      if (!(k_f_0_02 > frame.scratch_f)) {
        fsz = float_1_0 - frame.scratch_f;
      }
      else {
        fsz = frame.scratch_f * float_50_0;
      }
      fsz = fsz * point[1].z;
      frame.particle_size_h = fsz * k_f_0_5_5e0954;
      frame.particle_size_w = (k_f_0_5_5e0950 * fsz) * k_f_1_3;
      /* emit the animated sprite (16 frames) for this point */
      particles_set_size(&frame.particle_size_w);
      uv_offset = *(int *)&point[1].x * 0x10;
      frame.uv_rect_0 = *(undefined4 *)((char *)&g_effects_update_5e0850 + uv_offset);
      frame.uv_rect_1 = *(undefined4 *)((char *)&g_effects_update_5e0854 + uv_offset);
      frame.uv_rect_2 = *(undefined4 *)((char *)&g_effects_update_5e0858 + uv_offset);
      frame.uv_rect_3 = *(undefined4 *)((char *)&g_effects_update_5e085c + uv_offset);
      gfx_set_params_6c5188(&frame.uv_rect_0);
      point[2].y = g_tick_seconds + point[2].y;
      *(int *)&point[1].x = (int)(point[2].y * *(float *)((char *)&point[2].y - 0x4) * 30.0f);
      if (0xf < *(uint *)&point[1].x) {
        *(int *)&point[1].x = 0;
        point[2].y = 0.0;
      }
      frame.particle_pos_x = point->x;
      if (frame.any_attached == 0) {
        frame.particle_pos_y = (float_1_0 - point[2].z) * k_f_0_3 * point[2].y + point->y;
      }
      else {
        frame.particle_pos_y = point->y;
      }
      frame.particle_pos_z = point->z;
      particles_set_position(&frame.particle_pos_x);
      frame.color_rgba = g_effects_update_5e0958;
      ((unsigned char *)&frame.color_rgba)[3] = (unsigned char)(int)((frame.any_attached != 0) ? (255.0f - (point[2].z * 210.0f)) : (164.0f - (point[2].z * 163.0f)));
      particles_set_color(&frame.color_rgba);
      particles_add(&g_load_752180);
      if (*(int *)&point[9].x != 0) {
        ((unsigned char *)&frame.color_rgba)[0] = 0x10;
        ((unsigned char *)&frame.color_rgba)[1] = 0x10;
        ((unsigned char *)&frame.color_rgba)[2] = 0x10;
        ((unsigned char *)&frame.color_rgba)[3] = (unsigned char)(int)(255.0f - (point[2].z * 255.0f));
        particles_set_color(&frame.color_rgba);
        gfx_set_fx_matrix(frame.slot_matrix_ptr);
        particles_add(&g_sound_7521c0);
      }
      /* keep the 4 points nearest to the camera, sorted, for the sfx below */
      saved_pt = frame.nearest_pts[0];
      saved_dist_3 = frame.nearest_dist_sq[3];
      saved_dist_2 = frame.nearest_dist_sq[2];
      saved_dist_1 = frame.nearest_dist_sq[1];
      if ((*(int *)&point[9].y != 0) || (*(int *)&point[9].x != 0)) {      /* attach pointer or flag */
        point_dist_sq = frame.cam_pos_x - point->x;
        cam_dy = frame.cam_pos_y - point->y;
        cam_dz = frame.cam_pos_z - point->z;
        point_dist_sq = cam_dz * cam_dz + point_dist_sq * point_dist_sq + cam_dy * cam_dy;
        if (frame.nearest_count < 4) {
          frame.nearest_dist_sq[frame.nearest_count] = point_dist_sq;
          frame.nearest_pts[frame.nearest_count] = point;
          frame.nearest_count = frame.nearest_count + 1;
          nearest_dirty = true;
        }
        else {
          if (nearest_dirty) {
            swapped_01 = frame.nearest_dist_sq[0] < frame.nearest_dist_sq[1];
            if (swapped_01) {
              frame.nearest_dist_sq[1] = frame.nearest_dist_sq[0];
              frame.nearest_pts[0] = frame.nearest_pts[1];
              frame.nearest_dist_sq[0] = saved_dist_1;
              frame.nearest_pts[1] = saved_pt;
            }
            saved_pt = frame.nearest_pts[1];
            nearest_dirty = frame.nearest_dist_sq[1] < frame.nearest_dist_sq[2];
            if (nearest_dirty) {
              frame.nearest_dist_sq[2] = frame.nearest_dist_sq[1];
              frame.nearest_pts[1] = frame.nearest_pts[2];
              frame.nearest_dist_sq[1] = saved_dist_2;
              frame.nearest_pts[2] = saved_pt;
            }
            saved_pt = frame.nearest_pts[2];
            nearest_dirty = nearest_dirty || swapped_01;
            if (frame.nearest_dist_sq[2] < frame.nearest_dist_sq[3]) {
              frame.nearest_dist_sq[3] = frame.nearest_dist_sq[2];
              frame.nearest_pts[2] = frame.nearest_pts[3];
              frame.nearest_dist_sq[2] = saved_dist_3;
              frame.nearest_pts[3] = saved_pt;
              nearest_dirty = true;
            }
          }
          if (point_dist_sq < frame.nearest_dist_sq[0]) {
            nearest_dirty = true;
            frame.nearest_dist_sq[0] = point_dist_sq;
            frame.nearest_pts[0] = point;
          }
        }
      }
      if (frame.emitter_flag != 0) {
        frame.trig_sphere_x = point->x;
        frame.trig_sphere_y = point->y;
        frame.trig_sphere_z = point->z;
        frame.trig_sphere_radius = 0x3e19999a;
        frame.trig_sphere_unused = 0;
        frame.trig_sphere_callback = (undefined1 *)&LAB_00499120;
        frame.trig_sphere_kind = 0x11;
        frame.trig_sphere_owner = point;
        trigger_sphere_register(&frame.trig_sphere_x);
      }
    }
    frame.slot_aux_ptr = frame.slot_aux_ptr + 0x1d;
    point = (RwV3d *)&point[9].z;
    frame.slot_matrix_ptr = frame.slot_matrix_ptr + 0x1d;
    frame.slot_index = frame.slot_index + 1;
  } while ((int)frame.slot_aux_ptr < (int)g_particles_752298 + 0x7400);
  /* all points done: flush both particle batches, then play one sfx per nearest point */
  particles_flush(&g_load_752180);
  particles_flush(&g_sound_7521c0);
  frame.slot_index = 0;
  do {
    sound_sfx_set_volume_clamped(0x20,frame.slot_index,0);
    if (frame.slot_index < frame.nearest_count) {
      point = frame.nearest_pts[frame.slot_index];
      sfx_play_at(0x20,point);
      sound_sfx_update_position_volume
                (0x20,(undefined4)point,(undefined4)frame.slot_index,(fbits0 = (float_1_0 - point[2].z) * k_f_0_6, *(int *)&fbits0),
                 (fbits1 = (float)frame.slot_index * k_f_1000 + k_f_22050, *(int *)&fbits1));
    }
    frame.slot_index = frame.slot_index + 1;
  } while (frame.slot_index < 4);
  return;
}

