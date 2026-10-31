#include "mfl_tier2.h"
/* Small game helpers over RenderWare: random numbers from the RenderWare generator, look-at matrices, point sets,
   projection, material colour / flag changes and user data lookups.  0x4c4120..0x4c56c0. */
typedef struct RpAtomic *(*RpAtomicCallBackFn)(struct RpAtomic *atomic, void *data);
/*00544e80*/ extern int RpRandom(void);
/*004d0ff0*/ extern RwMatrix *RwFrameGetLTM(struct RwFrame *frame);
/*004d15a0*/ extern struct RwFrame *RwFrameTransform(struct RwFrame *frame, RwMatrix *matrix, enum RwOpCombineType combine);
/*004d3cd0*/ extern RwMatrix *RwMatrixMultiply(RwMatrix *out, RwMatrix *a, RwMatrix *b);
/*004d4490*/ extern RwMatrix *RwMatrixInvert(RwMatrix *out, RwMatrix *in);
/*004d3cc0*/ extern int RwMatrixUpdate(RwMatrix *matrix);
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combine);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *out, struct RwV3d *in);
/*004d5640*/ extern struct RwV3d *RwV3dTransformPoint(struct RwV3d *out, struct RwV3d *in, RwMatrix *matrix);
/*004d7770*/ extern int RwRasterGetOffset(struct RwRaster *raster, short *xOffset, short *yOffset);
/*004d5410*/ extern RwReal _rwSqrt(RwReal value);
/*004f6810*/ extern struct RpClump *RpClumpForAllAtomics(struct RpClump *clump, RpAtomicCallBack callback, void *data);
/*00543100*/ extern void RpHAnimKeyFrameInterpolate(void *out, void *prev, void *next, RwReal time, void *unused);
/*00542fe0*/ extern void __cdecl FUN_00542fe0(void *hierarchy, void *keyFrame);
/*00559a00*/ extern void *FUN_00559a00(void *atomic);
/*00559c40*/ extern void RpPatchMeshLock(void *patchMesh, int lockMode);
/*00559c50*/ extern void __cdecl RpPatchMeshUnlock(void *patchMesh);
/* already C elsewhere */
/*004c4620*/ extern void points_centroid(float *center, const float *points, int count);
/*004c5420*/ extern struct RpAtomic *atomic_cb_set_geometry_flags(struct RpAtomic *atomic, struct AtomicFlagRequest *request);
/*004c56a0*/ extern struct RpAtomic *atomic_cb_set_material_color(struct RpAtomic *atomic, const unsigned int *color);
/*004c3f90*/ extern int clump_get_atomics(struct RpClump *clump, struct RpAtomic **array);
/*004c4100*/ extern int atomic_copy_materials(struct RpAtomic *atomic, struct RpMaterial **out);
/*004c4110*/ extern int atomic_count_materials(struct RpAtomic *atomic);
/*005542c0*/ extern void FUN_005542c0(struct RpGeometry *geometry);
/*005542f0*/ extern void *FUN_005542f0(struct RpGeometry *geometry, int index);
/*005543e0*/ extern int rw_second_field(void *array);
/*005543f0*/ extern int __cdecl FUN_005543f0(void *array, int index);
extern float float_1_0;

#define RP_PATCHMESH_LOCK_PRELIT 8
#define MAX_ATOMIC_MATERIALS 16
#define MAX_CLUMP_ATOMICS 256
#define RW_ATOMIC_RENDER_BIT 4         /* rpATOMICRENDER in RwObject.flags */
#define RANDOM_UNIT (1.0f / 2147483648.0f)      /* RpRandom() & 0x7fffffff is a 31 bit value */
/* The patch mesh the game reads: pre-lit colour array at +8, vertex count at +0x30. */
struct PatchMeshView {
  char pad0[8];
  unsigned int *preLit;
  char pad1[0x24];
  int numVertices;
};
struct AtomicFlagRequest {
  unsigned int mask;
  int enable;
};

/* Paints the whole pre-lit colour array of the atomic's patch mesh with *color (opaque white when NULL). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void patchmesh_fill_prelit(void *atomic, const unsigned int *color)
{
  unsigned int white = 0xffffffff;
  struct PatchMeshView *mesh = (struct PatchMeshView *)FUN_00559a00(atomic);
  unsigned int *prelit = mesh->preLit;
  int count = mesh->numVertices;
  int i;

  RpPatchMeshLock(mesh, RP_PATCHMESH_LOCK_PRELIT);
  if (color == 0) {
    color = &white;
  }
  for (i = 0; i < count; i++) {
    prelit[i] = *color;
  }
  RpPatchMeshUnlock(mesh);
}

/* Projects `count` camera space points to the screen: out[i] = (width * x / z, height * y / z). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void camera_project_points(struct RwCamera *camera, struct RwV3d *points, float *out, int count)
{
  struct RwRaster *raster = RwCamera_frameBuffer(camera);
  float width = (float)RwRaster_width(raster);
  float height = (float)RwRaster_height(raster);
  short xOffset, yOffset;
  struct RwV3d p;
  int i;

  RwRasterGetOffset(raster, &xOffset, &yOffset);
  for (i = 0; i < count; i++) {
    float rhw;
    double acc;

    RwV3dTransformPoint(&p, &points[i], (RwMatrix *)((char *)camera + 0x20));
    rhw = float_1_0 / p.z;
    acc = width * p.x;                  /* one operation per statement keeps cl from regrouping the product */
    acc = acc * rhw;
    out[i * 2] = (float)acc;
    acc = height * p.y;
    acc = acc * rhw;
    out[i * 2 + 1] = (float)acc;
  }
}

/* Random integer in [lo, hi]. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rp_random_int_range(int lo, int hi)
{
  return RpRandom() % (hi - lo + 1) + lo;
}

/* Random float in [lo, hi). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rp_random_float_range(float lo, float hi)
{
  return (hi - lo) * (RpRandom() & 0x7fffffff) * RANDOM_UNIT + lo;
}

/* Bounding sphere of a point set: center = centroid, center[3] = radius (distance to the farthest point). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void points_bounding_sphere(float *sphere, const float *points, int count)
{
  float maxDistSq = 0.0f;
  int i;

  points_centroid(sphere, points, count);
  for (i = 0; i < count; i++) {
    const float *p = points + i * 3;
    float distSq = (sphere[2] - p[2]) * (sphere[2] - p[2]) + (sphere[0] - p[0]) * (sphere[0] - p[0]) + (sphere[1] - p[1]) * (sphere[1] - p[1]);

    if (maxDistSq < distSq) {
      maxDistSq = distSq;
    }
  }
  sphere[3] = _rwSqrt(maxDistSq);
}

/* Exact equality of two 3D vectors. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int vec3_equal(const float *a, const float *b)
{
  if (a[0] == b[0] && a[1] == b[1] && a[2] == b[2]) {
    return 1;
  }
  return 0;
}

/* Sets `out` to the pose of the animation at `time`: first key frame before 0, last after the end, else an interpolation. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void hanim_pose_at_time(void *hierarchy, char *anim, float time)
{
  char *frame = *(char **)(anim + 0x10);
  char keyFrame[0x24];
  char *prev;

  if (time <= 0.0f) {
    FUN_00542fe0(hierarchy, frame);
    return;
  }
  if (!(time >= *(float *)(anim + 0xc))) {     /* NaN time takes the interpolation branch like the original (fcomp, test ah,1) */
    do {
      prev = frame;
      frame = prev + 0x24;
    } while (*(float *)(prev + 0x28) < time);
    RpHAnimKeyFrameInterpolate(keyFrame, prev, prev + 0x24, time, 0);
    FUN_00542fe0(hierarchy, keyFrame);
    return;
  }
  FUN_00542fe0(hierarchy, frame + *(int *)(anim + 4) * 0x24 - 0x24);
}

/* Builds a rotation matrix from a triangle (3 points): right = p0 - p1, at = right x (p2 - p1), optional translation. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void matrix_from_triangle(RwMatrix *matrix, const float *tri, struct RwV3d *translation)
{
  struct RwV3d edgeA;
  struct RwV3d edgeB;
  struct RwV3d cross;

  edgeA.x = tri[0] - tri[3];
  edgeA.y = tri[1] - tri[4];
  edgeA.z = tri[2] - tri[5];
  edgeB.x = tri[6] - tri[3];
  edgeB.y = tri[7] - tri[4];
  edgeB.z = tri[8] - tri[5];
  RwV3dNormalize(&matrix->right, &edgeA);
  cross.x = edgeB.z * edgeA.y - edgeB.y * edgeA.z;
  cross.y = edgeB.x * edgeA.z - edgeB.z * edgeA.x;
  cross.z = edgeB.y * edgeA.x - edgeB.x * edgeA.y;
  RwV3dNormalize(&matrix->at, &cross);
  matrix->up.x = matrix->at.y * matrix->right.z - matrix->at.z * matrix->right.y;
  matrix->up.y = matrix->at.z * matrix->right.x - matrix->right.z * matrix->at.x;
  matrix->pos.x = 0.0f;
  matrix->pos.y = 0.0f;
  matrix->pos.z = 0.0f;
  matrix->up.z = matrix->at.x * matrix->right.y - matrix->at.y * matrix->right.x;
  RwMatrixUpdate(matrix);
  if (translation != 0) {
    RwMatrixTranslate(matrix, translation, rwCOMBINEPOSTCONCAT);
  }
}

/* Sets (on) or clears the "render" bit of an atomic. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void atomic_set_render_flag(char *atomic, int on)
{
  if (on) {
    atomic[2] |= RW_ATOMIC_RENDER_BIT;
  } else {
    atomic[2] &= ~RW_ATOMIC_RENDER_BIT;
  }
}

/* Sets the colour of every material of the atomic (opaque white when `color` is NULL). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void atomic_set_material_color(struct RpAtomic *atomic, const unsigned int *color)
{
  unsigned int white = 0xffffffff;
  struct RpMaterial *materials[MAX_ATOMIC_MATERIALS];
  int count = atomic_count_materials(atomic);
  int i;

  atomic_copy_materials(atomic, materials);
  if (color == 0) {
    color = &white;
  }
  for (i = 0; i < count; i++) {
    *(unsigned int *)&RpMaterial_color(materials[i]) = *color;
  }
}

/* Sets (on) or clears (!on) the bits `mask` of the geometry flags. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void geometry_set_flags(char *geometry, unsigned int mask, int on)
{
  unsigned int flags = *(unsigned int *)(geometry + 8);

  if (on) {
    *(unsigned int *)(geometry + 8) = flags | mask;
  } else {
    *(unsigned int *)(geometry + 8) = flags & ~mask;
  }
}

/* Same for the geometry of an atomic. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void atomic_set_geometry_flags(struct RpAtomic *atomic, unsigned int mask, int on)
{
  geometry_set_flags((char *)RpAtomic_geometry(atomic), mask, on);
}

/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void clump_set_geometry_flags(struct RpClump *clump, unsigned int mask, int on)
{
  struct AtomicFlagRequest request;

  request.mask = mask;
  request.enable = on;
  RpClumpForAllAtomics(clump, (RpAtomicCallBack)atomic_cb_set_geometry_flags, &request);
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void clump_set_material_color(struct RpClump *clump, const unsigned int *color)
{
  RpClumpForAllAtomics(clump, (RpAtomicCallBack)atomic_cb_set_material_color, (void *)color);
}
