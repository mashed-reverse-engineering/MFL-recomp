#include "mfl_tier2.h"
/*0065315c*/ extern undefined4 *g_trail_65315c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00843ea0*/ extern undefined4 g_collision_triangles;
/*008ccb00*/ extern undefined2 g_trail_8ccb00;
/*008d8b00*/ extern undefined4 g_trail_8d8b00;
#define VERTS_PER_TRAIL_QUAD (4)
#define INDICES_PER_TRAIL_QUAD (6)
/* trail_particles_render_setup */

/* "render states and texture for the trail particles (0x65315C)" */

/* TIER2: validated 2026-10-11 authority=diffemu */
/* Draws the trail particle quads: sets the render states and the trail texture, saves the
   cull mode and fog enable, submits g_trail_8d8b00 quads (4 verts, 6 indices each) as an
   indexed tri list, then restores the saved states. Does nothing when g_trail_8d8b00 is 0.
   Takes no parameters, returns nothing. The trail texture is g_trail_65315c (NULL = no texture). */
/* TIER2: unvalidated 2026-10-19 authority=shadow */
void trail_particles_render_setup(void)
{
  void *saved_cull_mode;
  void *saved_fog_enable;

  /* skip when there are no trail quads to draw (count inferred) */
  if (g_trail_8d8b00 != 0) {
    /* bind the trail texture raster, or NULL when the texture is not loaded */
    if (g_trail_65315c == (undefined4 *)0x0) {
      RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
    }
    else {
      RwRenderStateSet
                (rwRENDERSTATETEXTURERASTER,*(void **)g_trail_65315c);
    }
    /* blended, depth-tested, depth-write-off, no fog: particles are drawn alpha-blended and do not occlude */
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)rwFILTERLINEAR);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)true);
    /* save cull mode and fog enable so the restore at the end puts back what the caller had */
    RwRenderStateGet(rwRENDERSTATECULLMODE,&saved_cull_mode);
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
    RwRenderStateGet(rwRENDERSTATEFOGENABLE,&saved_fog_enable);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)true);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDONE);
    /* draw the quads as an indexed triangle list */
    RwIm2DRenderIndexedPrimitive
              (rwPRIMTYPETRILIST,(RwIm2DVertex *)&g_collision_triangles,g_trail_8d8b00 * VERTS_PER_TRAIL_QUAD,
               &g_trail_8ccb00,g_trail_8d8b00 * INDICES_PER_TRAIL_QUAD);
    /* restore the states changed above */
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)true);
    RwRenderStateSet(rwRENDERSTATECULLMODE,saved_cull_mode);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,saved_fog_enable);
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  }
  return;
}
