#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define LINE_VERTEX_COUNT (2)
#define VERTEX_STRIDE_WORDS (7) /* sizeof(RwIm2DVertex) / sizeof(RwReal) */
/* ui_draw_line */

/* Draws one 2D line from (x1, y1) to (x2, y2) in the menu's 640x480 virtual screen, scaled to the
 * current screen size. Used for menu separator lines (inferred). Returns nothing.
 * Parameters: x1, y1, x2, y2 = line ends in virtual screen coordinates; colour = 4 bytes, packed as
 * [3]<<24 | [0]<<16 | [1]<<8 | [2].
 * Plain cdecl, five stack arguments (the decompiler's leading register parameter reg_edi is not
 * an argument: it shifted every real argument by one slot). */
void __cdecl ui_draw_line(float x1,float y1,float x2,float y2,undefined1 *colour)

{
  undefined1 colour_byte1;
  undefined1 colour_byte3;
  undefined1 colour_byte0;
  undefined1 colour_byte2;
  float scale_x;
  float scale_y;
  short screen_extent;
  RwReal *vertex_z;
  int vertices_left;
  RwIm2DVertex v[2];  /* both vertices live in one array: the call below passes &v[0], 2 */
  
  /* scale = screen size / 640 (x) and / 480 (y): maps virtual coordinates to pixels */
  screen_extent = screen_width();
  scale_x = (float)(int)screen_extent * k_f_0_0015625;
  screen_extent = screen_height();
  colour_byte1 = colour[1];
  colour_byte3 = colour[3];
  scale_y = (float)(int)screen_extent * k_f_0_0020833334;
  v[0].x = scale_x * x1;
  v[0].y = y1 * scale_y;
  colour_byte0 = *colour;
  colour_byte2 = colour[2];
  v[1].x = scale_x * x2;
  v[1].y = scale_y * y2;
  vertex_z = &v[0].z;
  /* for each of the two vertices: store packed colour (see above), z = zBufferNear, rhw = 1.0 */
  vertices_left = LINE_VERTEX_COUNT;
  do {
    *(undefined4 *)&vertex_z[2] = CONCAT31(CONCAT21(CONCAT11(colour_byte3,colour_byte0),colour_byte1),colour_byte2);
    *vertex_z = RwIm2DGetNearScreenZ();
    vertex_z[1] = 1.0;
    vertex_z = vertex_z + VERTEX_STRIDE_WORDS;
    vertices_left = vertices_left + -1;
  } while (vertices_left != 0);
  /* plain coloured line list: no texture, vertex alpha on, alpha blending (src alpha, inverse src alpha) */
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  RwIm2DRenderPrimitive(rwPRIMTYPELINELIST,v,LINE_VERTEX_COUNT);
  return;
}


