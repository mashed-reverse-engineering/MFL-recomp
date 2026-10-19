#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
#define MARKER_LIMIT (0x7f)
#define ALPHA_TO_BYTE (255.0f)
#define CAR_POS_X_OFFSET (0x30)
#define CAR_POS_Y_OFFSET (0x34)
#define CAR_POS_Z_OFFSET (0x38)
/* hud_player_marker_position_calc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "world position between two cars scaled by scale, distance*10, fills one marker entry (limit
   0x7f)" */
/* Fills one HUD marker entry (the float array at marker, 11 floats per entry) for the pair car_a / car_b.
   The marker is placed at car_a moved away from car_b: pos = car_a + (car_a - car_b) * scale.
   Size = min(distance * 10, k_f_100) * k_f_0_025 + k_f_0_05 (constants inferred from names).
   Layout: [0..2] position, [3] size, [5] and [7] = 1.0, [6] and [8] = 0.0, bytes 0x24..0x26 = rgb,
   byte 0x27 = alpha (alpha_bits is a float in 0..1 passed as raw bits, times ALPHA_TO_BYTE),
   [10] = extra_value. Increments *marker_count when an entry is written.
   param_9..param_11 are unused. Returns nothing. */

void __cdecl hud_player_marker_position_calc(float *marker,int car_a,int car_b,undefined4 alpha_bits,float scale,float extra_value,
          undefined1 *rgb,int *marker_count,undefined4 param_9,undefined4 param_10,
          undefined4 param_11)

{
  float marker_size;
  undefined1 alpha_byte;
  RwReal distance;
  RwV3d delta;
  float *delta_z;
  float10 scaled_x;
  float10 scaled_y;

  /* Only write a new entry while the marker list has room (MARKER_LIMIT entries). */
  if (*marker_count < MARKER_LIMIT) {
    /* Offset from car_b to car_a. */
    delta.x = *(float *)(car_a + CAR_POS_X_OFFSET) - *(float *)(car_b + CAR_POS_X_OFFSET);
    delta.y = *(float *)(car_a + CAR_POS_Y_OFFSET) - *(float *)(car_b + CAR_POS_Y_OFFSET);
    delta.z = *(float *)(car_a + CAR_POS_Z_OFFSET) - *(float *)(car_b + CAR_POS_Z_OFFSET);
    distance = RwV3dLength(&delta);
    delta_z = &delta.z;
    scaled_x = (float10)delta.x * scale;
    scaled_y = (float10)delta.y * scale;
    *delta_z = *delta_z * scale;
    /* Marker position: car_a plus the scaled offset. */
    *marker = (float)(scaled_x + *(float *)(car_a + CAR_POS_X_OFFSET));
    marker[1] = (float)(scaled_y + *(float *)(car_a + CAR_POS_Y_OFFSET));
    marker[2] = *delta_z + *(float *)(car_a + CAR_POS_Z_OFFSET);
    /* Size grows with distance, clamped at k_f_100 (inferred 100). */
    marker_size = distance * float_10_0;
    if (k_f_100 < distance * float_10_0) {
      marker_size = k_f_100;
    }
    marker_size = marker_size * k_f_0_025;
    marker[5] = 1.0;
    marker[7] = 1.0;
    marker_size = marker_size + k_f_0_05;
    marker[6] = 0.0;
    marker[3] = marker_size;
    marker[8] = 0.0;
    /* Colour bytes, copied from rgb. */
    *(undefined1 *)(marker + 9) = *rgb;
    *(undefined1 *)((int)marker + 0x25) = rgb[1];
    *(undefined1 *)((int)marker + 0x26) = rgb[2];
    /* Alpha: float bits of alpha_bits scaled to a byte. */
    alpha_byte = ((int)(*(float *)&alpha_bits * ALPHA_TO_BYTE));
    *(undefined1 *)((int)marker + 0x27) = alpha_byte;
    marker[10] = extra_value;
    *marker_count = *marker_count + 1;
  }
  return;
}


