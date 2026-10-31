#include "mfl_tier2.h"

extern int g_rwp_trilist_sector_plugin_offset;
extern void *RpWorldSectorGetWorld(void *sector);
extern int RpCollisionWorldForAllIntersections(void *world, void *isect, void *callback, void *data);
extern void rwp_pipeline_00573d70(void *tri, void *incircle);
extern int LAB_00573ab0();   /* triangle intersection callback (not a function of its own in the symbol table) */

/* A world sector: triangle list (3 vertex indices and a material, 8 bytes each) at 4, vertices (12 bytes) at 8, triangle count at 0x84. */
typedef struct MflSector {
  int pad00;
  unsigned short (*tris)[4];   /* 0x4 */
  float (*verts)[3];   /* 0x8 */
  unsigned char pad0c[0x78];
  unsigned short tri_count;   /* 0x84 */
} MflSector;

/* A triangle record as built by rwp_pipeline_00573d70 (normal, margin) and the incircle record it fills. */
typedef struct MflTriangle {
  int index;
  int plugin_data;   /* 0x4: first word of the sector's trilist plugin data */
  MflSector *sector;   /* 0x8 */
  float *v[3];   /* 0xc */
  float margin;   /* 0x18 */
  float n[3];   /* 0x1c */
} MflTriangle;
typedef struct MflIncircle {
  float c[3];
  float radius;
  int pad10[2];
  int kind;   /* 0x18 */
} MflIncircle;

/* For every triangle of a sector: builds the triangle record (rwp_pipeline_00573d70 gives its normal and incircle) and runs the
   collision world query with the incircle, handing the callback LAB_00573ab0 the triangle.  Returns the sector. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
MflSector *rwp_pipeline_00573a00(MflSector *sector)
{
  MflIncircle ic;
  MflTriangle tri;
  void *world;
  unsigned int i;

  tri.plugin_data = **(int **)((char *)sector + g_rwp_trilist_sector_plugin_offset);
  tri.sector = sector;
  world = RpWorldSectorGetWorld(sector);
  for (i = 0; i < sector->tri_count; i++) {
    tri.index = i;
    tri.v[0] = sector->verts[sector->tris[i][0]];
    tri.v[1] = sector->verts[sector->tris[i][1]];
    tri.v[2] = sector->verts[sector->tris[i][2]];
    rwp_pipeline_00573d70(&tri, &ic);
    RpCollisionWorldForAllIntersections(world, &ic, (void *)LAB_00573ab0, &tri);
  }
  return sector;
}
