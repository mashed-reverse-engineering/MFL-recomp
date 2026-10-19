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
#define QUAD_VERTEX_COUNT (4)
/* im2d_draw_quad */

/* "(?, x, y, w, h ...): draws a screen space quad through g_im2d_vertices" */

/* Draws a 2D screen-space quad (triangle strip of QUAD_VERTEX_COUNT vertices in g_im2d_vertices) with its
 * top-left corner at (x, y), size width x height, filled with the packed colour `color`.
 * `texture` points to a value whose first word is the raster to bind; NULL draws untextured.
 * param_7..param_10 are never read (inferred). Returns nothing.
 * Culling is off and vertex alpha is on, so the quad is visible from both sides and alpha blends. (inferred) */

/* TIER2: validated 2026-10-19 authority=shadow */
void __cdecl im2d_draw_quad(undefined4 *texture,float x,float y,float width,float height, uint color,char *param_7,char *param_8,char *param_9, char *param_10)
{
  RwGlobalsRelease *engine;
  RwReal *vertex_z;

  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  engine = RwEngineInstance;
  /* Corner positions: vertex 0 = (x, y), 1 = (x + width, y), 2 = (x, y + height), 3 = (x + width, y + height).
   * Texture coordinates run 0..1 across the quad. */
  g_im2d_vertices[1].x = width + x;
  g_im2d_vertices[0].x = x;
  g_im2d_vertices[0].y = y;
  g_im2d_vertices[0].u = 0.0;
  g_im2d_vertices[0].v = 0.0;
  g_im2d_vertices[0].z = RwIm2DGetNearScreenZ();
  g_im2d_vertices[2].y = height + y;
  g_im2d_vertices[1].y = y;
  g_im2d_vertices[2].x = x;
  g_im2d_vertices[0].rhw = 1.0;
  g_im2d_vertices[1].u = 1.0;
  g_im2d_vertices[1].v = 0.0;
  g_im2d_vertices[2].u = 0.0;
  g_im2d_vertices[2].v = 1.0;
  g_im2d_vertices[3].u = 1.0;
  g_im2d_vertices[3].v = 1.0;
  vertex_z = &g_im2d_vertices[0].z;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  /* For each vertex: diffuse colour (repacked byte order, inferred), z = zBufferNear, rhw = 1.0.
   * vertex_z walks the z field of each vertex (sizeof vertex = 7 floats). */
  /* For each vertex: diffuse colour (repacked byte order, inferred), z = zBufferNear, rhw = 1.0.
   * vertex_z walks the z field of each vertex (sizeof vertex = 7 floats). */
  do {
    *(RwUInt32 *)(vertex_z + 2) = ((((uint)(((color >> 16) & 0xffff) >> 8) << 8 | color & 0xff) << 8 |
                         color >> 8 & 0xff) << 8 | ((color >> 16) & 0xffff) & 0xff);
    *vertex_z = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    vertex_z[1] = 1.0;
    vertex_z = vertex_z + 7;
  } while (vertex_z < &g_im2d_vertices[QUAD_VERTEX_COUNT].z);
  /* Bind the texture raster, or clear it for an untextured quad. Filtering is only set when textured. */
  /* Bind the texture raster, or clear it for an untextured quad. Filtering is only set when textured. */
  if (texture == (undefined4 *)0x0) {
    (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  }
  else {
    (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,*(void **)texture);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)rwFILTERLINEAR);
  }
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,QUAD_VERTEX_COUNT);
  return;
}
