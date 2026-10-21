// cluster-entries: ui_draw_sprite_oriented
/* Installed cluster ui_draw_sprite_oriented (tools/cluster_install.py from output/cluster_pilot_uc3/ui_draw_sprite_oriented.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*005de7c0*/ extern float k_f_0_0015625;
/*005dd624*/ extern float k_f_0_0020833334;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005de1ac*/ extern float k_f_0_125;
/*005dfe0c*/ extern float k_f_4_8828125en4;
/*005de1b4*/ extern float k_f_0_0625;
/*005de3e8*/ extern float k_f_0_9375;

/* Draws one frame of a 640x480-unit sprite sheet entry: quad (x, y, w, h), colour 0xAARRGGBB, frame `index` of the sheet strip (texture window rows scaled by index / 2048) and an orientation `mode` (1..5: flipped / rotated windows).
   Drawn as two strips (left half, then right half of the sheet entry); `no_alpha` clears the alpha of the left strip's left vertices.  texture NULL draws untextured. */
void ui_draw_sprite_oriented(int *texture, float x, float y, float w, float h, unsigned int colour,
                             int index, int mode, int no_alpha)
{
  volatile float sx, sy, scaled_h;  /* ponytail: the original stores all four scaled values as 32-bit floats in its parameter slots */
  float half_w, bottom, right;
  volatile float v_top, v_bottom;  /* stored floats in the original frame */
  float u_left, u_right;
  volatile float step;
  int i;
  unsigned int packed;

  RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)0x1);
  sx = (float)(int)screen_width() * x * k_f_0_0015625;
  sy = (float)(int)screen_height() * y * k_f_0_0020833334;
  half_w = (float)(int)screen_width() * w * k_f_0_0015625;
  scaled_h = (float)(int)screen_height() * h * k_f_0_0020833334;
  switch (mode) {
  case 1:
    v_top = k_f_0_125 - (double)index * k_f_4_8828125en4;
    v_bottom = float_1_0 - v_top;
    u_left = v_top;
    u_right = v_bottom;
    break;
  case 2:
    v_top = 0.0625f;
    u_left = k_f_0_125 - (double)index * k_f_4_8828125en4;
    v_bottom = 0.9375f;
    u_right = float_1_0 - (double)index * k_f_4_8828125en4;
    break;
  case 3:
    v_top = 0.0625f;
    step = k_f_0_125 - (double)index * k_f_4_8828125en4;
    u_left = (float)(k_f_0_125 - step);
    v_bottom = 0.9375f;
    u_right = (float)(float_1_0 - step);
    break;
  case 4:
    step = k_f_0_125 - (double)index * k_f_4_8828125en4;
    goto shared;
  case 5:
    step = (double)index * k_f_4_8828125en4;
  shared:
    v_top = (float)(k_f_0_125 - step);
    v_bottom = (float)(float_1_0 - step);
    u_left = k_f_0_0625;
    u_right = k_f_0_9375;
    break;
  default:
    step = (double)index * k_f_4_8828125en4;  /* through the stored float: the original subtracts the 32 bit value, not the extended product */
    v_top = step;
    v_bottom = (float)(float_1_0 - step);
    u_left = v_top;
    u_right = v_bottom;
  }
  u_right = u_right * k_f_0_5;
  half_w = half_w * k_f_0_5;
  g_im2d_vertices[0].v = v_top;
  right = half_w + sx;
  g_im2d_vertices[1].v = v_top;
  bottom = scaled_h + sy;
  g_im2d_vertices[2].v = v_bottom;
  g_im2d_vertices[3].v = v_bottom;
  packed = ((((colour >> 0x18) << 8 | (colour & 0xff)) << 8 | (colour >> 8 & 0xff)) << 8 | (colour >> 0x10 & 0xff));
  g_im2d_vertices[0].x = sx;
  g_im2d_vertices[0].y = sy;
  g_im2d_vertices[0].u = u_left;
  g_im2d_vertices[1].x = right;
  g_im2d_vertices[1].y = sy;
  g_im2d_vertices[1].u = u_right;
  g_im2d_vertices[2].x = sx;
  g_im2d_vertices[2].y = bottom;
  g_im2d_vertices[2].u = g_im2d_vertices[0].u;
  g_im2d_vertices[3].x = right;
  g_im2d_vertices[3].y = bottom;
  g_im2d_vertices[3].u = u_right;
  for (i = 0; i < 4; i++) {
    g_im2d_vertices[i].emissiveColor = packed;
    g_im2d_vertices[i].z = RwIm2DGetNearScreenZ();
    g_im2d_vertices[i].rhw = 1.0f;
  }
  if (no_alpha != 0) {
    g_im2d_vertices[0].emissiveColor = ((colour & 0xff) << 8 | (colour >> 8 & 0xff)) << 8 | (colour >> 0x10 & 0xff);
    g_im2d_vertices[2].emissiveColor = g_im2d_vertices[0].emissiveColor;
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture == 0 ? (void *)0x0 : (void *)*texture);
  if (texture != 0) {
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)0x2);
  }
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)0x5);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)0x6);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, g_im2d_vertices, 4);
  /* second half: the right part of the strip (u_right .. 2 * u_right), full colour */
  g_im2d_vertices[1].u = u_right + u_right;
  g_im2d_vertices[1].v = v_top;
  g_im2d_vertices[0].v = v_top;
  g_im2d_vertices[1].x = ((half_w + half_w) - half_w) + right;
  g_im2d_vertices[2].v = v_bottom;
  g_im2d_vertices[3].v = v_bottom;
  g_im2d_vertices[0].x = right;
  g_im2d_vertices[0].y = sy;
  g_im2d_vertices[0].u = u_right;
  g_im2d_vertices[1].y = sy;
  g_im2d_vertices[2].x = right;
  g_im2d_vertices[2].y = bottom;
  g_im2d_vertices[2].u = u_right;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = bottom;
  g_im2d_vertices[3].u = g_im2d_vertices[1].u;
  for (i = 0; i < 4; i++) {
    g_im2d_vertices[i].emissiveColor = packed;
    g_im2d_vertices[i].z = RwIm2DGetNearScreenZ();
    g_im2d_vertices[i].rhw = 1.0f;
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture == 0 ? (void *)0x0 : (void *)*texture);
  if (texture != 0) {
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)0x2);
  }
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)0x5);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)0x6);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, g_im2d_vertices, 4);
}

