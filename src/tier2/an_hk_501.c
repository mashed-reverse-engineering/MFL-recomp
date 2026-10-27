#include "mfl_tier2.h"
/*00482070*/ extern double dot_product_3d_clamped();
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
/* hud_player_marker_position_calc_with_facing */

/* "as hud_player_marker_position_calc but also computes the facing dot product" */

/* (inferred) Fills one 0x2c-byte marker vertex: position = matrix_a.pos + lerp_t * (matrix_a.pos - matrix_b.pos), size from the distance, colour from color_rgb,
   alpha from the squared dot product of the two cars' at vectors; marker_count (max 0x7f) is incremented. alpha_scale_bits carries a float through a char* parameter. */
void __cdecl
hud_player_marker_position_calc_with_facing
          (float *marker,char *matrix_a,char *matrix_b,char *alpha_scale_bits,float lerp_t,float extra_value,
          undefined1 *color_rgb,int *marker_count,char *unused_9,char *unused_10,
          char *unused_11)

{
  float base_z;
  volatile float zprod;      /* rounds the z product to float like the original's stack temp (it is stored, then reloaded for the sum) */
  double fdot;      /* the original keeps the callee's x87 result unrounded */
  undefined1 alpha;
  RwReal distance;
  struct {
    RwV3d a_to_b;
    undefined4 b_at;
    undefined4 b_at_y;
    undefined4 b_at_z;
    undefined4 a_at;
    undefined4 a_at_y;
    undefined4 a_at_z;
  } frame;
  
  if (*marker_count < 0x7f) {
    frame.a_to_b.x = *(float *)(matrix_a + 0x30) - *(float *)(matrix_b + 0x30);
    frame.a_to_b.y = *(float *)(matrix_a + 0x34) - *(float *)(matrix_b + 0x34);
    frame.a_to_b.z = *(float *)(matrix_a + 0x38) - *(float *)(matrix_b + 0x38);
    distance = RwV3dLength(&frame.a_to_b);
    frame.b_at = *(undefined4 *)(matrix_b + 0x20);
    frame.b_at_y = *(undefined4 *)(matrix_b + 0x24);
    frame.b_at_z = *(undefined4 *)(matrix_b + 0x28);
    frame.a_at = *(undefined4 *)(matrix_a + 0x20);
    frame.a_at_y = *(undefined4 *)(matrix_a + 0x24);
    frame.a_at_z = *(undefined4 *)(matrix_a + 0x28);
    fdot = dot_product_3d_clamped(&frame.a_at,&frame.b_at);
    if (fdot > 0.0) {      /* original: fcom, test ah,0x41, jne; fstp st(0); fld 0.0 */
      fdot = 0.0;
    }
    zprod = frame.a_to_b.z * lerp_t;
    *marker = frame.a_to_b.x * lerp_t + *(float *)(matrix_a + 0x30);
    marker[1] = frame.a_to_b.y * lerp_t + *(float *)(matrix_a + 0x34);
    base_z = *(float *)(matrix_a + 0x38);
    marker[5] = 1.0;
    marker[7] = 1.0;
    marker[2] = zprod + base_z;
    marker[6] = 0.0;
    marker[8] = 0.0;
    marker[3] = distance * k_f_0_025 + k_f_0_05;
    *(undefined1 *)(marker + 9) = *color_rgb;
    *(undefined1 *)((char *)marker + 0x25) = color_rgb[1];
    *(undefined1 *)((char *)marker + 0x26) = color_rgb[2];
    alpha = (int)(fdot * fdot * 255.0f * *(float *)&alpha_scale_bits);
    *(undefined1 *)((char *)marker + 0x27) = alpha;
    marker[10] = extra_value;
    *marker_count = *marker_count + 1;
  }
  return;
}


