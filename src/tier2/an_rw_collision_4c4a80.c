#include "mfl_tier2.h"
/* The game's collision queries over RenderWare (RpCollision): a line or a sphere against the world's triangles (or against one atomic's
   triangles), either collecting up to N hits or keeping only the closest one.  Every hit is copied into a flat record because the
   RenderWare callbacks only hand out pointers that are valid during the call.  0x4c4a80..0x4c5150. */
enum RpIntersectionType {
  RP_INTERSECT_LINE = 1,
  RP_INTERSECT_SPHERE = 3
};
/* MflRpIntersection: the line (2 points) or the sphere (center + radius) and the type tag; 7 words. */
struct MflRpIntersection {
  float shape[6];
  int type;
};
/* MflRpCollisionTriangle as handed to the callbacks. */
struct MflRpCollisionTriangle {
  RwV3d normal;
  RwV3d point;
  int index;
  RwV3d *vertices[3];
};
/* One recorded hit, 16 words. */
struct CollisionHit {
  RwV3d normal;
  int index;
  RwV3d vertex[3];
  void *owner;            /* world sector (world queries) or the atomic's geometry (atomic query) */
  float distance;
  void *source;           /* the world / atomic that was queried */
};
/* What the collecting callbacks share through the `data` pointer of RenderWare. */
struct HitCollector {
  struct CollisionHit *hits;      /* may be NULL: only count */
  int *count;
  int limit;                      /* stop after this many */
  void *source;
};
struct ClosestHitCollector {
  struct CollisionHit *hit;
  int *count;
  float *bestDistance;
  void *source;
};
/* Sector triangles collected by material colour: 12 words each. */
struct ColorTriangleHit {
  RwV3d vertex[3];
  void *sector;
  unsigned short vertIndex[3];
  unsigned short matIndex;
};
struct ColorCollector {
  unsigned int *color;
  struct ColorTriangleHit *hits;
  int *count;
  int limit;
};
struct SectorTriangle {
  unsigned short vertIndex[3];
  unsigned short matIndex;
};
#define SECTOR_TRIANGLES(s) (*(struct SectorTriangle **)((char *)(s) + 4))
typedef void *(*RpIntersectionTriangleCallBack)(struct MflRpIntersection *isect, void *sector, struct MflRpCollisionTriangle *tri, float distance, void *data);
/*00549290*/ extern int RpCollisionWorldForAllIntersections(void *world, struct MflRpIntersection *isect, void *callback, void *data);
/*0054cbe0*/ extern int RpAtomicForAllIntersections(void *atomic, struct MflRpIntersection *isect, void *callback, void *data);
/* already C elsewhere: the RenderWare callbacks of the queries below */
/*004c4a80*/ extern void *collision_cb_collect_triangles(struct MflRpIntersection *isect, void *sector, struct MflRpCollisionTriangle *tri, float distance, struct HitCollector *collector);
/*004c4cc0*/ extern void *collision_cb_closest_triangle(struct MflRpIntersection *isect, void *sector, struct MflRpCollisionTriangle *tri, float distance, struct ClosestHitCollector *collector);
/*004c4e70*/ extern void *collision_cb_closest_atomic_triangle(struct MflRpIntersection *isect, struct MflRpCollisionTriangle *tri, float distance, struct ClosestHitCollector *collector);
/*004c4fd0*/ extern struct RpWorldSector *world_sector_cb_collect_by_color(struct RpWorldSector *sector, struct ColorCollector *collector);
/*004f56f0*/ extern struct RpWorld *RpWorldSectorGetWorld(struct RpWorldSector *sector);
/*004f5e10*/ extern struct RpWorld *RpWorldForAllWorldSectors(struct RpWorld *world, RpWorldSectorCallBack callback, void *data);

/* Collects up to `limit` triangle hits of the intersection with the world; returns how many were hit. */
/* TIER2: unvalidated 2026-10-31 authority=static */
int world_collect_triangle_hits(void *world, struct MflRpIntersection *isect, struct CollisionHit *hits, int limit)
{
  int count = 0;
  struct HitCollector collector;

  collector.hits = hits;
  collector.count = &count;
  collector.limit = limit;
  collector.source = world;
  RpCollisionWorldForAllIntersections(world, isect, collision_cb_collect_triangles, &collector);
  return count;
}

/* Line (6 floats) against the world, collecting up to `limit` hits. */
/* TIER2: unvalidated 2026-10-31 authority=static */
int world_collect_line_hits(void *world, const float *line, struct CollisionHit *hits, int limit)
{
  struct MflRpIntersection isect;
  int i;

  isect.type = RP_INTERSECT_LINE;
  for (i = 0; i < 6; i++) {
    isect.shape[i] = line[i];
  }
  return world_collect_triangle_hits(world, &isect, hits, limit);
}

/* Sphere (center, radius) against the world, collecting up to `limit` hits. */
/* TIER2: unvalidated 2026-10-31 authority=static */
int world_collect_sphere_hits(void *world, const float *sphere, struct CollisionHit *hits, int limit)
{
  struct MflRpIntersection isect;

  isect.shape[0] = sphere[0];
  isect.shape[1] = sphere[1];
  isect.shape[2] = sphere[2];
  isect.shape[3] = sphere[3];
  isect.type = RP_INTERSECT_SPHERE;
  return world_collect_triangle_hits(world, &isect, hits, limit);
}

/* Line against the world, first hit only. */
/* TIER2: unvalidated 2026-10-31 authority=static */
int world_line_first_hit(void *world, const float *line, struct CollisionHit *hit)
{
  struct MflRpIntersection isect;
  int i;

  isect.type = RP_INTERSECT_LINE;
  for (i = 0; i < 6; i++) {
    isect.shape[i] = line[i];
  }
  return world_collect_triangle_hits(world, &isect, hit, 1);
}

/* Sphere against the world, first hit only. */
/* TIER2: unvalidated 2026-10-31 authority=static */
int world_sphere_first_hit(void *world, const float *sphere, struct CollisionHit *hit)
{
  struct MflRpIntersection isect;

  isect.shape[0] = sphere[0];
  isect.shape[1] = sphere[1];
  isect.shape[2] = sphere[2];
  isect.shape[3] = sphere[3];
  isect.type = RP_INTERSECT_SPHERE;
  return world_collect_triangle_hits(world, &isect, hit, 1);
}

/* Closest triangle hit of the intersection with the world; returns the number of triangles that were hit. */
/* TIER2: unvalidated 2026-10-31 authority=static */
int world_closest_triangle_hit(void *world, struct MflRpIntersection *isect, struct CollisionHit *hit)
{
  int count = 0;
  float bestDistance;
  struct ClosestHitCollector collector;

  collector.hit = hit;
  collector.count = &count;
  collector.bestDistance = &bestDistance;
  collector.source = world;
  RpCollisionWorldForAllIntersections(world, isect, collision_cb_closest_triangle, &collector);
  return count;
}

/* Line against the world, closest hit. */
/* TIER2: unvalidated 2026-10-31 authority=static */
void world_line_closest_hit(void *world, const float *line, struct CollisionHit *hit)
{
  struct MflRpIntersection isect;
  int i;

  isect.type = RP_INTERSECT_LINE;
  for (i = 0; i < 6; i++) {
    isect.shape[i] = line[i];
  }
  world_closest_triangle_hit(world, &isect, hit);
}

/* Sphere against the world, closest hit. */
/* TIER2: unvalidated 2026-10-31 authority=static */
void world_sphere_closest_hit(void *world, const float *sphere, struct CollisionHit *hit)
{
  struct MflRpIntersection isect;

  isect.shape[0] = sphere[0];
  isect.shape[1] = sphere[1];
  isect.shape[2] = sphere[2];
  isect.shape[3] = sphere[3];
  isect.type = RP_INTERSECT_SPHERE;
  world_closest_triangle_hit(world, &isect, hit);
}

/* Closest triangle hit of the intersection with one atomic. */
/* TIER2: unvalidated 2026-10-31 authority=static */
int atomic_closest_triangle_hit(void *atomic, struct MflRpIntersection *isect, struct CollisionHit *hit)
{
  int count = 0;
  float bestDistance;
  struct ClosestHitCollector collector;

  collector.hit = hit;
  collector.count = &count;
  collector.bestDistance = &bestDistance;
  collector.source = atomic;
  RpAtomicForAllIntersections(atomic, isect, collision_cb_closest_atomic_triangle, &collector);
  return count;
}

/* Line against one atomic, closest hit. */
/* TIER2: unvalidated 2026-10-31 authority=static */
void atomic_line_closest_hit(void *atomic, const float *line, struct CollisionHit *hit)
{
  struct MflRpIntersection isect;
  int i;

  isect.type = RP_INTERSECT_LINE;
  for (i = 0; i < 6; i++) {
    isect.shape[i] = line[i];
  }
  atomic_closest_triangle_hit(atomic, &isect, hit);
}

/* Collects up to `limit` world triangles whose material colour equals *color; returns the count. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int world_collect_triangles_by_color(struct RpWorld *world, unsigned int *color, struct ColorTriangleHit *hits, int limit)
{
  int count = 0;
  struct ColorCollector collector;

  collector.color = color;
  collector.hits = hits;
  collector.count = &count;
  collector.limit = limit;
  RpWorldForAllWorldSectors(world, (RpWorldSectorCallBack)world_sector_cb_collect_by_color, &collector);
  return count;
}
