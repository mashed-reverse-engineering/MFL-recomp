#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*005dfcf4*/ extern float k_f_21;
/*005dfe00*/ extern float k_f_416;
/*007d2128*/ extern char *RwEngineInstance;
/*007ef7f8*/ extern float g_menu_wave_time;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
extern double sin(double);
/* build_info_display_draw_wave_grid */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "draws the animated sine wave distorted tile grid of the build info backdrop" */

void build_info_display_draw_wave_grid(void)

{
  int cell_x;
  double cell_w;
  double amp0;
  double amp1;
  double amp2;
  float wave_time2_or_sx0;
  float sy0;
  float sx1;
  float sy1;
  float sx2;
  float sy2;
  float sx3;
  short screen_dim;
  RwReal *vtx_cursor;
  uint colour_in;
  int col;
  uint row;
  double band_y;
  double wave0;
  double wave1;
  double wave2;
  double wave_tmp;
  struct {
    int pass;
    uint colour_base;
    undefined1 pad_38[6];
    ushort colour_hi;
  } frame;
  float y2;
  float y3;
  float y0;
  float y1;
  float x0f;
  float x1f;
  float x2f;
  float x3f;
  float s1r;
  float i21f;
  
  frame.colour_base = 0xff808080;
  frame.pass = 0;
  do {
    row = 0;
    do {
      col = 0;
      do {
        cell_x = (row & 1) * 0x15 + (col * 5 + 0x2d) * 8;
        band_y = (double)(int)(row * 0x15);
        if (frame.pass != 0) {
          band_y = band_y + (double)k_f_416;
        }
        cell_w = (double)k_f_21;
        wave_time2_or_sx0 = g_menu_wave_time + g_menu_wave_time;
        wave0 = (double)sin((double)(int)(col + row) + (double)wave_time2_or_sx0);
        amp0 = (double)(4.0);
        wave1 = (double)sin((double)(int)(row + 1 + col) + (double)wave_time2_or_sx0);
        amp1 = (double)(4.0);
        wave2 = (double)sin((double)(int)(row + 2 + col) + (double)wave_time2_or_sx0);
        amp2 = (double)(4.0);
        wave_tmp = (double)sin((double)(int)(col + row * 2) + (double)wave_time2_or_sx0);
        y0 = (float)(wave_tmp + wave_tmp + band_y);
        wave_tmp = (double)sin((double)(int)(col + 1 + row * 2) + (double)wave_time2_or_sx0);
        y1 = (float)(wave_tmp + wave_tmp + band_y);
        wave_tmp = (double)sin((double)(int)(col + 2 + row * 2) + (double)wave_time2_or_sx0);
        y2 = (float)(wave_tmp + wave_tmp + (double)(float)(band_y + (double)k_f_21));
        wave_tmp = (double)sin((double)(int)(col + 3 + row * 2) + (double)wave_time2_or_sx0);
        y3 = (float)(wave_tmp + wave_tmp + (double)(float)(band_y + (double)k_f_21));
        if (frame.pass == 0) {
          if (row == 2) {
            y3 = 64.0f;
            y2 = 64.0f;
          }
        }
        else if (row == 0) {
          y1 = 416.0f;
          y0 = 416.0f;
        }
        frame.colour_hi = 0xff80;
        colour_in = frame.colour_base;
        if ((col == 0) && ((row & 1) == 0)) {
          colour_in = 0;
          frame.colour_hi = 0;
        }
        *(volatile float *)&x0f = wave0 * amp0 + (double)cell_x;
        *(volatile float *)&s1r = wave1 * amp1;
        *(volatile float *)&i21f = (double)cell_x + cell_w;
        *(volatile float *)&x1f = wave1 * amp1 + ((double)cell_x + cell_w);
        *(volatile float *)&x2f = *(volatile float *)&s1r + (double)cell_x;
        *(volatile float *)&x3f = wave2 * amp2 + *(volatile float *)&i21f;
        screen_dim = screen_width();
        wave_time2_or_sx0 = (float)(int)screen_dim * *(volatile float *)&x0f * k_f_0_0015625;
        screen_dim = screen_height();
        sy0 = (float)(int)screen_dim * y0 * k_f_0_0020833334;
        screen_dim = screen_width();
        sx1 = (float)(int)screen_dim * *(volatile float *)&x1f * k_f_0_0015625;
        screen_dim = screen_height();
        sy1 = (float)(int)screen_dim * y1 * k_f_0_0020833334;
        screen_dim = screen_width();
        sx2 = (float)(int)screen_dim * *(volatile float *)&x2f * k_f_0_0015625;
        screen_dim = screen_height();
        sy2 = (float)(int)screen_dim * y2 * k_f_0_0020833334;
        screen_dim = screen_width();
        sx3 = (float)(int)screen_dim * *(volatile float *)&x3f * k_f_0_0015625;
        screen_dim = screen_height();
        g_im2d_vertices[3].y = (float)(int)screen_dim * y3 * k_f_0_0020833334;
        g_im2d_vertices[0].emissiveColor =
             (((uint)(frame.colour_hi >> 8) << 8 | colour_in & 0xff) << 8 | colour_in >> 8 & 0xff) << 8 |
             frame.colour_hi & 0xff;
        g_im2d_vertices[1].emissiveColor = 0xff808080;
        g_im2d_vertices[3].emissiveColor = 0xff808080;
        vtx_cursor = &g_im2d_vertices[0].rhw;
        g_im2d_vertices[0].x = wave_time2_or_sx0;
        g_im2d_vertices[0].y = sy0;
        g_im2d_vertices[1].x = sx1;
        g_im2d_vertices[1].y = sy1;
        g_im2d_vertices[2].x = sx2;
        g_im2d_vertices[2].y = sy2;
        g_im2d_vertices[2].emissiveColor = g_im2d_vertices[0].emissiveColor;
        g_im2d_vertices[3].x = sx3;
        do {
          vtx_cursor[-1] = RwIm2DGetNearScreenZ();
          *vtx_cursor = 1.0f;
          vtx_cursor = vtx_cursor + 7;
        } while ((char *)vtx_cursor < (char *)&g_im2d_vertices + 0x7c);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
        RwRenderStateSet
                  (rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
        RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)0x5);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)0x6);
        RwIm2DRenderPrimitive
                  (rwPRIMTYPETRISTRIP,g_im2d_vertices,4);
        col = col + 1;
      } while (col < 7);
      row = row + 1;
    } while ((int)row < 3);
    frame.pass = frame.pass + 1;
  } while (frame.pass < 2);
  return;
}


