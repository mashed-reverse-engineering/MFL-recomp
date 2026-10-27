#include "mfl_tier2.h"
/*004f8ba0*/ extern struct RpGeometry * RpGeometryLock(struct RpGeometry *p0, RwInt32 p1);
/*004f8bd0*/ extern struct RpGeometry * RpGeometryUnlock(struct RpGeometry *p0);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd634*/ extern float k_f_2;
/*006a9584*/ extern float g_weapon_6a9584;
extern double cos(double);
/* material_uv_scroll_update */
#define ATOMIC_GEOMETRY_OFFSET (0x18) /* offset of the RpGeometry pointer in the atomic (inferred) */
#define GEOMETRY_LOCK_TEXCOORDS (16) /* rpGEOMETRYLOCKTEXCOORDS, texcoord set 1 */

/* "scrolls the u coordinates of the geometry by cos(timer) * 0.01" */
/*
 * Per-frame update of the u texture coordinates of an atomic's geometry (inferred from the body).
 * Advances the global timer g_weapon_6a9584 by k_f_0_01, locks the first texcoord set, adds
 * cos(timer) * k_f_0_01 to the u of every vertex in turn, and when a vertex's new u goes above
 * float_1_0 it subtracts k_f_2 from the u of all vertices (wraps the scroll back).
 * Parameter: atomic (inferred), the object whose geometry is at ATOMIC_GEOMETRY_OFFSET.
 * Returns: the same atomic.
 */

int material_uv_scroll_update(int atomic)

{
  RpGeometry *geometry;
  int vertex_count;
  RwTexCoords *uv_coords;
  double scroll_step;
  int wrap_index;
  int wrap_next_index;
  int vertex_index;
  double cos_timer;
  double new_u;

  g_weapon_6a9584 = g_weapon_6a9584 + k_f_0_01;
  geometry = *(RpGeometry **)(atomic + ATOMIC_GEOMETRY_OFFSET);
  vertex_count = RpGeometry_numVertices(geometry);
  RpGeometryLock(geometry,GEOMETRY_LOCK_TEXCOORDS);
  uv_coords = RpGeometry_texCoords(geometry)[0];
  vertex_index = 0;
  if (0 < vertex_count) {
    /* per-vertex scroll: u += cos(timer) * step; wrap check uses the unstored new_u */
    cos_timer = (double)cos((double)g_weapon_6a9584);
    scroll_step = (double)k_f_0_01;
    do {
      new_u = cos_timer * scroll_step + (double)uv_coords[vertex_index].u;
      uv_coords[vertex_index].u = (float)new_u;
      if ((double)float_1_0 < new_u) {
        /* wrap: subtract k_f_2 from every vertex's u, not just this one (inferred from the loop bounds) */
        wrap_index = 0;
        do {
          wrap_next_index = wrap_index + 1;
          uv_coords[wrap_index].u = uv_coords[wrap_index].u - k_f_2;
          wrap_index = wrap_next_index;
        } while (wrap_next_index < vertex_count);
      }
      vertex_index = vertex_index + 1;
    } while (vertex_index < vertex_count);
  }
  RpGeometryUnlock(geometry);
  return atomic;
}


