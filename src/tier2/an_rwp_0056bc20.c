/* rwp_body_add_force_at_point (0x0056bc20): apply a force at a world point (linear and angular accumulators) */
#include "mfl_tier2.h"

/* Per body accumulator: linear and angular parts (two float3 padded to 4 floats). */
typedef struct MflBodyAcc {
  float lin[3];
  float pad0c;
  float ang[3];
  float pad1c;
} MflBodyAcc;
/* The body tables of the world: 64 byte matrices at 0 and the accumulators at 0x30. */
typedef struct MflBodyTables {
  float *mats;   /* 0x0: 16 floats per body, the position at 12..14 */
  unsigned char pad04[0x2c];
  MflBodyAcc *acc;   /* 0x30 */
} MflBodyTables;
typedef struct MflBodyOwner {
  unsigned char pad00[0x10];
  MflBodyTables *tables;   /* 0x10 */
} MflBodyOwner;
/* A body handle: the owner and the body index. */
typedef struct MflBodyRef {
  MflBodyOwner *owner;
  int index;   /* 0x4 */
} MflBodyRef;

/* Applies the force f at the world point p to the body: the force is added to the body's linear accumulator and the torque (p - position) x f to
   its angular accumulator. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_body_add_force_at_point(MflBodyRef *b, float *f, float *p)
{
  float *pos;
  float t[2];
  double rx, ry, rz, tz;

  b->owner->tables->acc[b->index].lin[0] = b->owner->tables->acc[b->index].lin[0] + f[0];
  b->owner->tables->acc[b->index].lin[1] = b->owner->tables->acc[b->index].lin[1] + f[1];
  b->owner->tables->acc[b->index].lin[2] = b->owner->tables->acc[b->index].lin[2] + f[2];
  pos = b->owner->tables->mats + b->index * 16;
  rx = p[0] - pos[12];
  ry = p[1] - pos[13];
  rz = p[2] - pos[14];
  t[0] = f[2] * ry - f[1] * rz;
  t[1] = rz * f[0] - f[2] * rx;
  tz = rx * f[1] - (float)ry * f[0];
  b->owner->tables->acc[b->index].ang[0] = b->owner->tables->acc[b->index].ang[0] + t[0];
  b->owner->tables->acc[b->index].ang[1] = b->owner->tables->acc[b->index].ang[1] + t[1];
  b->owner->tables->acc[b->index].ang[2] = b->owner->tables->acc[b->index].ang[2] + tz;
}
