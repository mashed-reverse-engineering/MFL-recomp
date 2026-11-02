/* rwp_body_init (0x0056b650): initialise a body of a world from a description and optional start matrix */
#include "mfl_tier2.h"
#include <math.h>

/* Float4 rows of the per body tables. */
typedef struct MflRow4 {
  float f[4];
} MflRow4;
/* A 32 byte per body record: two float3 (the second at 0x10). */
typedef struct MflRow8 {
  float a[3];
  float pad0c;
  float b[3];
  float pad1c;
} MflRow8;
/* A 48 byte per body record: mass (at 0xc) and the second mass-like value (at 0x2c) are the entries this function touches. */
typedef struct MflRow12 {
  float f[12];
} MflRow12;
/* The tables of a physics world, indexed by body. */
typedef struct MflBodyTables {
  float *mats;   /* 0x00: 16 floats per body */
  unsigned char pad04[4];
  MflRow8 *spin;   /* 0x08 */
  unsigned char pad0c[4];
  MflRow4 *quats;   /* 0x10 */
  unsigned char pad14[4];
  int *flags;   /* 0x18 */
  unsigned char pad1c[4];
  MflRow12 *mass;   /* 0x20 */
  unsigned char pad24[4];
  MflRow12 *inv_mass;   /* 0x28 */
  unsigned char pad2c[4];
  MflRow8 *vel;   /* 0x30 */
  unsigned char pad34[0x1c];
  MflRow4 *offset;   /* 0x50 */
} MflBodyTables;
typedef struct MflBodyOwner {
  unsigned char pad00[0x10];
  MflBodyTables *tables;   /* 0x10 */
} MflBodyOwner;
/* A body handle: the owning world, its index and the body's local offset (three words at 8) with the two damping floats at 0x14 / 0x18. */
typedef struct MflMat16 {
  int w[16];
} MflMat16;
typedef struct MflBodyHandle {
  MflBodyOwner *owner;
  int index;   /* 0x4 */
  int local[3];   /* 0x8 */
  float damp_lin;   /* 0x14 */
  float damp_ang;   /* 0x18 */
  float inv_mass_scaled;   /* 0x1c */
} MflBodyHandle;
/* The body description (only the fields used). */
typedef struct MflBodyDesc {
  unsigned char pad00[4];
  int local[3];   /* 0x4 */
  float mass;   /* 0x10 */
  float inertia[7];   /* 0x14: principal values (3) and quaternion (4) for 56ba70 */
  float mass2;   /* 0x30 */
  float damp_lin;   /* 0x34 */
  float damp_ang;   /* 0x38 */
  int offset[3];   /* 0x3c */
  unsigned int flags;   /* 0x48 */
} MflBodyDesc;

extern int rwp_body_store_world_inertia(MflBodyHandle *b, float *inertia);
extern RwMatrix *RwMatrixTranslate(RwMatrix *m, void *v, int op);
extern void RtQuatConvertFromMatrix(MflRow4 *q, float *m);

#define TAB (b->owner->tables)
#define IDX (b->index)

/* Initialises the body b of the world `owner` from the description desc (0 = unit body) and the optional start matrix m: mass and inverse
   root mass (1 / sqrt(mass), mass2 for the second value unless the description supplies the inertia tensor by itself), the damping values and
   the local offsets from the description, the start matrix (m or the identity) with its quaternion, the matrix translated by the local offset,
   the scaled inverse mass of the body and zeroed velocities.  Returns b. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
MflBodyHandle *rwp_body_init(MflBodyHandle *b, MflBodyOwner *owner, MflBodyDesc *desc, float *m)
{
  double inv;

  b->owner = owner;
  if (desc) {
    TAB->flags[IDX] = desc->flags;
    TAB->mass[IDX].f[3] = desc->mass;
    inv = sqrt(desc->mass);
    TAB->inv_mass[IDX].f[3] = 1.0f / inv;
    if (!(desc->flags & 2)) {
      TAB->mass[IDX].f[11] = desc->mass2;
      inv = sqrt(desc->mass2);
      TAB->inv_mass[IDX].f[11] = 1.0f / inv;
    } else {
      rwp_body_store_world_inertia(b, desc->inertia);
    }
    b->damp_lin = desc->damp_lin;
    b->damp_ang = desc->damp_ang;
    TAB->offset[IDX].f[0] = *(float *)&desc->offset[0];
    TAB->offset[IDX].f[1] = *(float *)&desc->offset[1];
    TAB->offset[IDX].f[2] = *(float *)&desc->offset[2];
    b->local[0] = desc->local[0];
    b->local[1] = desc->local[1];
    b->local[2] = desc->local[2];
  } else {
    inv = sqrt(1.0);
    TAB->flags[IDX] = 0;
    TAB->mass[IDX].f[3] = 1.0f;
    inv = 1.0f / inv;
    TAB->inv_mass[IDX].f[3] = inv;
    TAB->mass[IDX].f[11] = 1.0f;
    TAB->inv_mass[IDX].f[11] = inv;
    b->damp_lin = 0.01f;
    b->damp_ang = 0.01f;
    TAB->offset[IDX].f[0] = 0.0f;
    TAB->offset[IDX].f[1] = 0.0f;
    TAB->offset[IDX].f[2] = 0.0f;
    b->local[0] = 0;
    b->local[1] = 0;
    b->local[2] = 0;
  }
  if (m) {
    *(MflMat16 *)(TAB->mats + 16 * IDX) = *(MflMat16 *)m;
    RtQuatConvertFromMatrix(&TAB->quats[IDX], TAB->mats + 16 * IDX);
  } else {
    TAB->mats[16 * IDX + 10] = 1.0f;
    TAB->mats[16 * IDX + 5] = 1.0f;
    TAB->mats[16 * IDX + 0] = 1.0f;
    TAB->mats[16 * IDX + 4] = 0.0f;
    TAB->mats[16 * IDX + 2] = 0.0f;
    TAB->mats[16 * IDX + 1] = 0.0f;
    TAB->mats[16 * IDX + 9] = 0.0f;
    TAB->mats[16 * IDX + 8] = 0.0f;
    TAB->mats[16 * IDX + 6] = 0.0f;
    TAB->mats[16 * IDX + 14] = 0.0f;
    TAB->mats[16 * IDX + 13] = 0.0f;
    TAB->mats[16 * IDX + 12] = 0.0f;
    *(unsigned int *)&TAB->mats[16 * IDX + 3] |= 0x20003;
    TAB->quats[IDX].f[0] = 0.0f;
    TAB->quats[IDX].f[1] = 0.0f;
    TAB->quats[IDX].f[2] = 0.0f;
    TAB->quats[IDX].f[3] = 1.0f;
  }
  RwMatrixTranslate((RwMatrix *)(TAB->mats + 16 * IDX), b->local, 1);
  b->inv_mass_scaled = 0.001f / TAB->mass[IDX].f[3];
  TAB->vel[IDX].a[0] = 0.0f;
  TAB->vel[IDX].a[1] = 0.0f;
  TAB->vel[IDX].a[2] = 0.0f;
  TAB->vel[IDX].b[0] = 0.0f;
  TAB->vel[IDX].b[1] = 0.0f;
  TAB->vel[IDX].b[2] = 0.0f;
  TAB->spin[IDX].a[0] = 0.0f;
  TAB->spin[IDX].a[1] = 0.0f;
  TAB->spin[IDX].a[2] = 0.0f;
  TAB->spin[IDX].b[0] = 0.0f;
  TAB->spin[IDX].b[1] = 0.0f;
  TAB->spin[IDX].b[2] = 0.0f;
  return b;
}
