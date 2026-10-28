// cluster-entries: gap_4c4fd0
/* Installed cluster gap_4c4fd0 (tools/cluster_install.py from output/cluster_pilot_kg/gap_4c4fd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4c4fd0 */

/* world_collect_triangles callback, one world sector: copy every triangle whose material colour equals the wanted colour into the output
   buffer (12 floats per triangle: three vertices, the sector, then the six index bytes) and stop (return NULL) when the wanted count is reached.
   query = { RwRGBA *colour, float *out (may be NULL: count only), int *count, int max } */
RpWorldSector * gap_4c4fd0(RpWorldSector *sector, undefined4 *query)

{
  RpTriangle *tri;
  RwRGBA *colour;
  int max;
  int *count;
  float *out;
  int n;
  RpWorld *world;
  int i;
  float *rec;

  colour = (RwRGBA *)*query;
  max = query[3];
  count = (int *)query[2];
  out = (float *)query[1];
  n = *count;
  world = RpWorldSectorGetWorld(sector);
  i = 0;
  if (RpWorldSector_numTriangles(sector) != 0) {
    rec = out + n * 12;
    do {
      tri = RpWorldSector_triangles(sector) + i;
      if (*(unsigned int *)&RpMaterial_color(RpWorld_matList_materials(world)[(uint)tri->matIndex + (uint)RpWorldSector_matListWindowBase(sector)]) ==
          *(unsigned int *)colour) {
        if (out != 0) {
          rec[0] = RpWorldSector_vertices(sector)[tri->vertIndex[0]].x;
          rec[1] = RpWorldSector_vertices(sector)[tri->vertIndex[0]].y;
          rec[2] = RpWorldSector_vertices(sector)[tri->vertIndex[0]].z;
          rec[3] = RpWorldSector_vertices(sector)[tri->vertIndex[1]].x;
          rec[4] = RpWorldSector_vertices(sector)[tri->vertIndex[1]].y;
          rec[5] = RpWorldSector_vertices(sector)[tri->vertIndex[1]].z;
          rec[6] = RpWorldSector_vertices(sector)[tri->vertIndex[2]].x;
          rec[7] = RpWorldSector_vertices(sector)[tri->vertIndex[2]].y;
          rec[8] = RpWorldSector_vertices(sector)[tri->vertIndex[2]].z;
          *(RpWorldSector **)&rec[9] = sector;
          *(unsigned int *)&rec[10] = *(unsigned int *)tri->vertIndex;
          *(unsigned int *)&rec[11] = *(unsigned int *)(tri->vertIndex + 2);
          rec = rec + 12;
        }
        n = *count;
        *count = n + 1;
        if (n + 1 == max) {
          return (RpWorldSector *)0x0;
        }
      }
      i = i + 1;
    } while (i < (int)(uint)RpWorldSector_numTriangles(sector));
  }
  return sector;
}

