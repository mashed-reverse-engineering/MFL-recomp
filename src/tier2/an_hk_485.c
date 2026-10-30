#include "mfl_tier2.h"
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/* layout of *camera_frame in hud_marker_get_distance, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct hud_marker_get_distance_camera_frame_layout {
    char pad_0x0[64];
    float f_0x40;
    float f_0x44;
    float f_0x48;
} hud_marker_get_distance_camera_frame_layout;

/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/* hud_marker_get_distance */

/* "(index, out_dist, out_dist_sq): distance of the marker frame to the stored point" */
/*
 * Measures the distance from the render camera of vehicle `vehicle_index` to the
 * vehicle's marker point (world_sphere centre).
 * out_dist:    receives the straight-line distance, may be NULL.
 * out_dist_sq: receives the squared distance, may be NULL.
 * Returns 1 when the vehicle has a render camera (distances written), 0 otherwise.
 */

undefined4 __cdecl hud_marker_get_distance(int vehicle_index,RwReal *out_dist,float *out_dist_sq)

{
  hud_marker_get_distance_camera_frame_layout *camera_frame;
  RwReal dist;
  float sq;
  RwV3d delta;

  /* no render camera: the marker was never rendered this frame */
  if (g_vehicle_table[vehicle_index].render_camera != 0) {
    camera_frame = (hud_marker_get_distance_camera_frame_layout *)(*(char **)(g_vehicle_table[vehicle_index].render_camera + 4));
    /* vector from camera position to marker centre (camera position offsets inferred) */
    delta.x = g_vehicle_table[vehicle_index].world_sphere[0] - camera_frame->f_0x40;
    delta.y = g_vehicle_table[vehicle_index].world_sphere[1] - camera_frame->f_0x44;
    delta.z = g_vehicle_table[vehicle_index].world_sphere[2] - camera_frame->f_0x48;
    /* squared distance, only when the caller asked for it */
    if (out_dist_sq != (float *)0x0) {
      sq = delta.y * delta.y;
      sq = sq + delta.x * delta.x;
      *out_dist_sq = sq + delta.z * delta.z;
    }
    /* length, only when the caller asked for it */
    if (out_dist != (RwReal *)0x0) {
      dist = RwV3dLength(&delta);
      *out_dist = dist;
    }
    return 1;
  }
  return 0;
}


