#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*00475760*/ extern int aux_camera_get_position();
/*0047a470*/ extern int car_set_wheel_contact();
/*004810a0*/ extern undefined __cdecl atomic_set_material_alpha(int p0, void *p1);
/*004810d0*/ extern int debug_box_draw_with_culling_a();
/*00481350*/ extern int debug_box_draw_with_culling_b();
/*004b38f7*/ extern int finite();
/*004d18d0*/ extern struct RwBBox * RwBBoxCalculate(struct RwBBox *p0, struct RwV3d *p1, RwInt32 p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004dbfc0*/ extern struct RwStream * RwStreamWrite(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *p0, RpMaterialCallBack p1, void *p2);
/*00544e80*/ extern int RpRandom();
/*00555ff0*/ extern struct RwV3d * RpSplineFindPosition(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, struct RwV3d *p4);
/*005625f0*/ extern RwBool __cdecl RpMaterialUVAnimExists(struct RpMaterial *p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd61c*/ extern float k_f_0_001;
/*005dd624*/ extern float k_f_0_0020833334;
/*005de498*/ extern float k_f_4_656613en10;
/*005de7c0*/ extern float k_f_0_0015625;
/*005df83c*/ extern float k_f_1_0en6;
/*006c3da0*/ extern undefined4 g_gfx_6c3da0;
/*006c3ea0*/ extern undefined4 g_gfx_6c3ea0;
/*006c3eac*/ extern undefined4 g_gfx_6c3eac;
/*006c3f30*/ extern undefined4 g_car_6c3f30;
/*006c3f34*/ extern undefined4 g_car_6c3f34;
/*006c3f38*/ extern undefined4 g_gfx_6c3f38;
/*006c3f3c*/ extern undefined4 g_gfx_6c3f3c;
/*006c3f40*/ extern undefined4 g_car_6c3f40;
/*006c3f44*/ extern undefined4 g_app_boot_6c3f44;
/*006c3f48*/ extern undefined4 g_gfx_6c3f48;
/*006c3f4c*/ extern undefined4 g_gfx_6c3f4c;
/*006c3f50*/ extern undefined4 g_app_boot_6c3f50;
/*006c3f54*/ extern undefined4 g_app_boot_6c3f54;
/*006c3f58*/ extern undefined4 g_app_boot_6c3f58;
/*006c3f5c*/ extern undefined4 g_app_boot_6c3f5c;
/*006c3f60*/ extern undefined4 g_app_boot_6c3f60;
/*006c3f64*/ extern undefined4 g_app_boot_6c3f64;
/*006c3f68*/ extern undefined4 g_app_boot_6c3f68;
/*006c3f6c*/ extern undefined4 g_app_boot_6c3f6c;
/*006c3f70*/ extern undefined4 g_app_boot_6c3f70;
/*006c3f74*/ extern undefined4 g_app_boot_6c3f74;
/*006c3f78*/ extern undefined4 g_app_boot_6c3f78;
/*006c3f7c*/ extern undefined4 g_app_boot_6c3f7c;
/*006c3f80*/ extern undefined4 g_app_boot_6c3f80;
/*006c3f84*/ extern undefined4 g_app_boot_6c3f84;
/*006c3f88*/ extern undefined4 g_app_boot_6c3f88;
/*006c3f8c*/ extern undefined4 g_app_boot_6c3f8c;
/*006c3f90*/ extern undefined4 g_app_boot_6c3f90;
/*006c3f94*/ extern undefined4 g_app_boot_6c3f94;
/*006c3f98*/ extern undefined4 g_app_boot_6c3f98;
/*006c3f9c*/ extern undefined4 g_app_boot_6c3f9c;
/*006c3fa0*/ extern undefined4 g_app_boot_6c3fa0;
/*006c3fa4*/ extern undefined4 g_app_boot_6c3fa4;
/*006c3fa8*/ extern undefined4 g_app_boot_6c3fa8;
/*006c3fac*/ extern undefined4 g_app_boot_6c3fac;
/*006c3fb0*/ extern undefined4 g_app_boot_6c3fb0;
/*006c3fb4*/ extern undefined4 g_app_boot_6c3fb4;
/*006c3fb8*/ extern undefined4 g_app_boot_6c3fb8;
/*006c3fbc*/ extern undefined4 g_app_boot_6c3fbc;
/*006c3fc0*/ extern undefined4 g_app_boot_6c3fc0;
/*006c3fc4*/ extern undefined4 g_app_boot_6c3fc4;
/*006c3fc8*/ extern undefined4 g_app_boot_6c3fc8;
/*006c3fcc*/ extern undefined4 g_app_boot_6c3fcc;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00843e80*/ extern undefined4 g_course_843e80;
/*0089ddfc*/ extern undefined4 g_gfx_89ddfc;
/*0089de04*/ extern undefined4 g_app_89de04;
/*0089de08*/ extern undefined4 g_gfx_89de08;
/*0089de0c*/ extern undefined4 g_gfx_89de0c;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];

#define CULL_MODE_NONE ((void *)0x1)
#define SRC_BLEND_SRC_ALPHA ((void *)0x5)
#define DEST_BLEND_INV_SRC_ALPHA ((void *)0x6)
#define NUM_TRIANGLE_VERTICES (3)
#define IM2D_VERTEX_FLOATS (7) /* sizeof(RwIm2DVertex) / 4 */

/* gfx_draw_triangle_2d */

/* "screen space triangle in 640x480 coordinates with one colour (Im2D tristrip)" */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/*
 * Draws one filled triangle over the screen, as an Im2D triangle strip with a single colour.
 * ref_x0..ref_y2: the three corners in 640x480 reference coordinates; they are scaled to the
 *                 current screen_width() / screen_height() (inferred).
 * argb:           colour as 0xAARRGGBB (inferred).
 * Returns nothing. Leaves cull mode and blend state changed (inferred).
 */
/* TIER2: validated 2026-10-19 authority=diffemu */
void gfx_draw_triangle_2d(float ref_x0,float ref_y0,float ref_x1,float ref_y1,float ref_x2,float ref_y2, uint argb)
{
  float screen_x0;
  float screen_y0;
  float screen_x1;
  float screen_y1;
  float screen_x2;
  float screen_y2;
  RwGlobalsRelease *engine;
  short screen_size;
  RwReal *vert_z;

  RwRenderStateSet(rwRENDERSTATECULLMODE,CULL_MODE_NONE);
  screen_size = screen_width();
  screen_x0 = (float)(int)screen_size * ref_x0;
  screen_x0 = screen_x0 * k_f_0_0015625;
  screen_size = screen_height();
  screen_y0 = (float)(int)screen_size * ref_y0;
  screen_y0 = screen_y0 * k_f_0_0020833334;
  screen_size = screen_width();
  screen_x1 = (float)(int)screen_size * ref_x1;
  screen_x1 = screen_x1 * k_f_0_0015625;
  screen_size = screen_height();
  screen_y1 = (float)(int)screen_size * ref_y1;
  screen_y1 = screen_y1 * k_f_0_0020833334;
  screen_size = screen_width();
  screen_x2 = (float)(int)screen_size * ref_x2;
  screen_x2 = screen_x2 * k_f_0_0015625;
  screen_size = screen_height();
  engine = RwEngineInstance;
  screen_y2 = (float)(int)screen_size * ref_y2;
  g_im2d_vertices[2].y = screen_y2 * k_f_0_0020833334;
  vert_z = &g_im2d_vertices[0].z;
  g_im2d_vertices[0].x = screen_x0;
  g_im2d_vertices[0].y = screen_y0;
  g_im2d_vertices[1].x = screen_x1;
  g_im2d_vertices[1].y = screen_y1;
  g_im2d_vertices[2].x = screen_x2;
  /* per vertex 0..2: set colour, z (from the engine's near z) and rhw = 1.0 (inferred);
     the colour word is repacked from 0xAARRGGBB to the byte order Im2D expects (inferred) */
  do {
    *(RwUInt32 *)(vert_z + 2) = ((((uint)(((argb >> 16) & 0xffff) >> 8) << 8 | argb & 0xff) << 8 |
                         argb >> 8 & 0xff) << 8 | ((argb >> 16) & 0xffff) & 0xff);
    *vert_z = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    vert_z[1] = 1.0;
    vert_z = vert_z + IM2D_VERTEX_FLOATS;
  } while (vert_z < &g_im2d_vertices[3].z);
  /* no texture, alpha-blended (src alpha, inv src alpha), then draw the strip (inferred) */
  (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,SRC_BLEND_SRC_ALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,DEST_BLEND_INV_SRC_ALPHA);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,NUM_TRIANGLE_VERTICES);
  return;
}
