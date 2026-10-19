#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0065315c*/ extern undefined4 g_trail_65315c;
/*008ccb02*/ extern undefined2 g_trail_indices_8ccb02;
/*008d8b02*/ extern char g_trail_indices_8d8b02[];
#define VERTS_PER_QUAD (4)
#define INDICES_PER_QUAD (6)
/* trail_indices_init */

/* "fills the short index table at 0x8CCB00 (4 vertices per quad)" */

/* Builds the trail index table: one quad per entry, INDICES_PER_QUAD shorts each, from
   g_trail_indices_8ccb02 up to g_trail_indices_8d8b02. Each quad's indices are relative to
   its own VERTS_PER_QUAD vertices (two triangles, inferred). Then looks up the "headlight"
   texture dict and stores its result in g_trail_65315c. Takes no parameters, returns nothing. */
void trail_indices_init(void)

{
  short base_vertex;
  short *index_ptr;
  short quad_index;

  quad_index = 0;
  index_ptr = &g_trail_indices_8ccb02;
  /* Loop over quads: write the 6 indices of quad quad_index, then advance the cursor. */
  do {
    base_vertex = quad_index * VERTS_PER_QUAD;
    *index_ptr = base_vertex + 1;
    index_ptr[-1] = base_vertex;
    index_ptr[2] = base_vertex;
    index_ptr[1] = base_vertex + 3;
    index_ptr[3] = base_vertex + 2;
    index_ptr[4] = base_vertex + 3;
    index_ptr = index_ptr + INDICES_PER_QUAD;
    quad_index = quad_index + 1;
  } while ((LONG_PTR)index_ptr < ((LONG_PTR)&g_trail_indices_8d8b02));
  g_trail_65315c = find_texture_dict_b("headlight");
  return;
}


