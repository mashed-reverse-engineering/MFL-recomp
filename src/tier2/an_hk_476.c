#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(void *p0, void *p1);
/*00499100*/ extern int fx_spawn_from_matrix_2();
/*004c4720*/ extern void vec3_lerp(void *p0, void *p1, void *p2, undefined4 p3);
/*004c4c30*/ extern int FUN_004c4c30(undefined4 p0, void *p1, void *p2);
/*004c51a0*/ extern void rwmatrix_from_three_points(void *p0, void *p1, void *p2);
/*005dd628*/ extern float k_f_0_25;
/*005ddafc*/ extern float k_f_0_05;
/* explosion_decal */

/* "inferred: traces from the explosion position down against the world and orients a decal matrix
   (fx_spawn_from_matrix_2 draws it)" */

/*
 * Places a scorch decal under an explosion (inferred).
 *   explosion_pos: float[3] world position of the explosion (x, y, z).
 *   fx_param:      passed through unchanged to fx_spawn_from_matrix_2 (inferred: effect parameter).
 * Probes a short vertical segment (y +/- 0.25) around the explosion with the current weapon's
 * trace. If the probe hits and rocket_surface_hit_test returns 0, the decal is moved 0.05 off
 * the hit normal and its matrix is built and spawned.
 * Returns 1 if the probe hit anything (decal spawned or not), 0 if it missed.
 */
undefined4 explosion_decal(undefined4 *explosion_pos,undefined4 fx_param)

{
  undefined4 weapon_rw;
  int hit;
  undefined4 *probe_top;
  float *hit_normal;
  struct {
    undefined4 top_x;
    float top_y;
    undefined4 top_z;
    undefined4 bottom_x;
    float bottom_y;
    undefined4 bottom_z;
    float decal_pos_x;
    float decal_pos_y;
    float decal_pos_z;
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    undefined1 pad_74[4];
    undefined1 matrix_scratch [40];
    undefined4 hit_extra;
    undefined1 pad_44[4];
    undefined1 decal_matrix [64];
  } frame;

  frame.bottom_x = *explosion_pos;
  frame.bottom_z = explosion_pos[2];
  frame.top_x = *explosion_pos;
  frame.top_z = explosion_pos[2];
  frame.top_y = *(float *)&explosion_pos[1] + k_f_0_25;
  frame.bottom_y = *(float *)&explosion_pos[1] - k_f_0_25;
  hit_normal = &frame.hit_normal_x;
  probe_top = &frame.top_x;
  weapon_rw = weapon_get_rw_801b14();
  hit = FUN_004c4c30(weapon_rw,probe_top,hit_normal);
  if (hit != 0) {
    /* probe hit: decide whether to place a decal */
    hit = rocket_surface_hit_test(&frame.hit_normal_x,explosion_pos);
    if (hit == 0) {
      /* impact point along the segment (inferred), then lift it 0.05 off the surface along the normal.
         hit_extra is never written in this function; it is passed uninitialised (inferred: unused flag) */
      vec3_lerp(&frame.decal_pos_x,&frame.top_x,&frame.bottom_x,frame.hit_extra);
      frame.decal_pos_x = frame.hit_normal_x * k_f_0_05 + frame.decal_pos_x;
      frame.decal_pos_y = frame.hit_normal_y * k_f_0_05 + frame.decal_pos_y;
      frame.decal_pos_z = frame.hit_normal_z * k_f_0_05 + frame.decal_pos_z;
      /* build the decal orientation matrix at the lifted position, then spawn it */
      rwmatrix_from_three_points(frame.decal_matrix,frame.matrix_scratch,&frame.decal_pos_x);
      fx_spawn_from_matrix_2(frame.decal_matrix,fx_param);
    }
    return 1;
  }
  return 0;
}
