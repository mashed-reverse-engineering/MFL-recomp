// cluster-entries: line_of_sight_between
/* Installed cluster line_of_sight_between (tools/cluster_install.py from output/cluster_pilot9/line_of_sight_between.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(undefined4 p0, undefined4 p1);
/*00482070*/ extern double dot_product_3d_clamped();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005ddaec*/ extern float k_f_57_29578;
/*005ddaf0*/ extern float k_f_1_0en5;
/* line_of_sight_between */

/* WARNING: Unknown calling convention */
/* "inferred: ray test between two points with FUN_00467ea0 / FUN_004682c0" */

/* Returns 1 when `target_pos` is visible from the shooter (world_intersect_line finds no hit on the segment in either direction, or the hit is one for which rocket_surface_hit_test returns nonzero; meaning of that test is inferred),
   within max_range (flat distance) and inside the max_angle_deg cone around the shooter forward vector (+0x30). */
undefined4 line_of_sight_between_n(int shooter, float *target_pos, float max_range, float max_angle_deg)

{
  float *shooter_pos;
  int shooter_node;
  undefined4 world;
  int hit;
  float10 angle_rad;
  RwReal distance;
  float *seg;
  undefined1 *hit_buf;
  struct {
    RwV3d flat_dir;
    float seg_x0;
    float seg_y0;
    float seg_z0;
    float seg_x1;
    float seg_y1;
    float seg_z1;
    undefined1 hit_info [64];
  } frame;
  
  shooter_node = *(int *)(*(char **)(shooter + 0x10) + 4);
  shooter_pos = (float *)(shooter_node + 0x40);
  frame.seg_x0 = *shooter_pos;
  frame.seg_y0 = *(float *)(shooter_node + 0x44);
  frame.seg_z0 = *(float *)(shooter_node + 0x48);
  frame.seg_x1 = *target_pos;
  frame.seg_y1 = target_pos[1];
  frame.seg_z1 = target_pos[2];
  hit_buf = frame.hit_info;
  seg = &frame.seg_x0;
  world = weapon_get_rw_801b14();
  hit = world_intersect_line(world,seg,hit_buf);
  if ((hit == 1) && (hit = rocket_surface_hit_test(frame.hit_info,shooter_pos), hit == 0)) {
    return 0;
  }
  frame.seg_x0 = *target_pos;
  frame.seg_y0 = target_pos[1];
  frame.seg_z0 = target_pos[2];
  frame.seg_x1 = *shooter_pos;
  frame.seg_y1 = *(float *)(shooter_node + 0x44);
  frame.seg_z1 = *(float *)(shooter_node + 0x48);
  hit_buf = frame.hit_info;
  seg = &frame.seg_x0;
  world = weapon_get_rw_801b14();
  hit = world_intersect_line(world,seg,hit_buf);
  if ((hit == 1) && (hit = rocket_surface_hit_test(frame.hit_info,shooter_pos), hit == 0)) {
    return 0;
  }
  frame.flat_dir.x = *target_pos - *shooter_pos;
  frame.flat_dir.z = target_pos[2] - *(float *)(shooter_node + 0x48);
  frame.flat_dir.y = 0.0;
  distance = RwV3dLength(&frame.flat_dir);
  if (k_f_1_0en5 < frame.flat_dir.z * frame.flat_dir.z + frame.flat_dir.x * frame.flat_dir.x + frame.flat_dir.y * frame.flat_dir.y) {
    RwV3dNormalize(&frame.flat_dir,&frame.flat_dir);
  }
  angle_rad = (float10)dot_product_3d_clamped(&frame.flat_dir,shooter_node + 0x30);
  angle_rad = (float10)FUN_004b3284((double)angle_rad);
  if (((float10)k_f_57_29578 * angle_rad < (float10)max_angle_deg !=
       ((float10)k_f_57_29578 * angle_rad == (float10)max_angle_deg)) &&
     (distance <= max_range)) {
    return 1;
  }
  return 0;
}

/* entry thunk at the original address of line_of_sight_between: register inputs (eax,edi,s0,s1) -> natural C line_of_sight_between_n; ret 0 */
__declspec(naked) void line_of_sight_between(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push edi
        push eax
        call line_of_sight_between_n
        add esp, 16
        ret
    }
}
