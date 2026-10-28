// cluster-entries: hudpanel_marker_ray_cast
/* Installed cluster hudpanel_marker_ray_cast (tools/cluster_install.py from output/cluster_pilot10/hudpanel_marker_ray_cast.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c4720*/ extern void FUN_004c4720(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void rwmatrix_from_three_points(undefined4 p0, undefined4 p1, undefined4 p2);
/*005ddafc*/ extern float k_f_0_05;
/* hudpanel_marker_ray_cast */

/* "casts a ray from the marker and stores the hit point matrix (surface id check against 0xFEFEFF)"
    */

/* (inferred names) ray = ray_start (start xyz, end xyz at +0xc) in world; frame.hit_* is the world_intersect_line hit record (normal first, then triangle index, mesh, fraction, world); the marker matrix
   is built at the hit point lifted 0.05 along the normal and copied (16 floats) to out_matrix when it is not NULL. Returns 1 on a hit that is not the 0xfefeff surface. */
undefined4 hudpanel_marker_ray_cast_n(undefined4 *out_matrix, int ray_start, undefined4 world)

{
  int hit_or_count;
  undefined4 *src_cursor;
  struct {
    float hit_pos_x;
    float hit_pos_y;
    float hit_pos_z;
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    int hit_tri_index;
    undefined1 hit_unused [36];
    int hit_mesh;
    undefined4 hit_fraction;
    int hit_world;
    undefined4 out_matrix_buf [16];
  } frame;
  
  hit_or_count = world_intersect_line(world,ray_start,&frame.hit_normal_x);
  if ((hit_or_count != 0) &&
     (*(int *)(*(int *)(*(int *)(frame.hit_world + 0x10) +
                       ((uint)*(ushort *)(*(char **)(frame.hit_mesh + 4) + 6 + frame.hit_tri_index * 8) +
                       (uint)*(ushort *)(frame.hit_mesh + 0x80)) * 4) + 4) != -0xfefeff)) {
    FUN_004c4720(&frame.hit_pos_x,ray_start,ray_start + 0xc,frame.hit_fraction);
    frame.hit_pos_x = frame.hit_normal_x * k_f_0_05 + frame.hit_pos_x;
    frame.hit_pos_y = frame.hit_normal_y * k_f_0_05 + frame.hit_pos_y;
    frame.hit_pos_z = frame.hit_normal_z * k_f_0_05 + frame.hit_pos_z;
    rwmatrix_from_three_points(frame.out_matrix_buf,frame.hit_unused,&frame.hit_pos_x);
    if (out_matrix != (undefined4 *)0x0) {
      src_cursor = frame.out_matrix_buf;
      for (hit_or_count = 0x10; hit_or_count != 0; hit_or_count = hit_or_count + -1) {
        *out_matrix = *src_cursor;
        src_cursor = src_cursor + 1;
        out_matrix = out_matrix + 1;
      }
    }
    return 1;
  }
  return 0;
}

/* entry thunk at the original address of hudpanel_marker_ray_cast: register inputs (eax,edx,ecx) -> natural C hudpanel_marker_ray_cast_n; ret 0 */
__declspec(naked) void hudpanel_marker_ray_cast(void)
{
    __asm {
        push ecx
        push edx
        push eax
        call hudpanel_marker_ray_cast_n
        add esp, 12
        ret
    }
}
