#include "mfl_tier2.h"
/*004810a0*/ extern undefined __cdecl atomic_set_material_alpha(void *p0, void *p1);
/*004810d0*/ extern int debug_box_draw_with_culling_a();
/*00481350*/ extern int debug_box_draw_with_culling_b();
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*005dd340*/ extern float k_f_0_5;
/*006c3da0*/ extern undefined4 g_gfx_6c3da0;
/*006c3ea0*/ extern undefined4 g_gfx_6c3ea0;
/*006c3eac*/ extern undefined4 g_gfx_6c3eac;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00843e80*/ extern undefined4 g_course_843e80;
/*0089de08*/ extern int (__cdecl *g_gfx_89de08)();
/* layout of *camera_frame in atomic_render_with_culling, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct atomic_render_with_culling_camera_frame_layout {
    char pad_0x0[64];
    float f_0x40;
    float f_0x44;
    float f_0x48;
} atomic_render_with_culling_camera_frame_layout;

/*0089de0c*/ extern undefined4 g_gfx_89de0c;

/* Per-atomic culling block, at byte offset g_gfx_89de0c from the atomic (inferred). */
#define CULL_DATA_MATERIAL_OFFSET (0x4) /* non-null: atomic has a material to set alpha on (inferred) */
#define CULL_DATA_BOX_A_OFFSET (0x8) /* first box argument for the debug box draw */
#define CULL_DATA_FLAGS_OFFSET (0x20)
#define CULL_DATA_BOX_B_OFFSET (0x28) /* second box argument for the debug box draw */
#define CULL_FLAG_QUEUED (0x1) /* atomic is pushed to the deferred list instead of rendered now */
#define CULL_FLAG_DEBUG_BOXES (0x2) /* draw the debug boxes */
#define CULL_RADIUS_SQ_UNSET_BITS (0xbf800000) /* float -1.0f: cached squared radius not computed yet */
#define CULL_RADIUS_SCALE_SQ (1.5542475e-05) /* cached squared radius = (radius^2) / this (inferred) */
#define CAMERA_FRAME_PTR_OFFSET (0x4)
#define CAMERA_DIST_SCALE_OFFSET (0x68) /* float factor applied to the squared camera distance (inferred) */
#define INTERPOLATOR_FLAG_DIRTY_SPHERE (0x2) /* rpAtomicResyncInterpolatedSphere is needed first (inferred) */

/* atomic_render_with_culling */

/* "atomic render callback with the bounding sphere test and the plugin flags" */

/*
 * Render callback for one atomic, with distance culling (inferred from the flow).
 * atomic: the RpAtomic being rendered. Returns atomic.
 * Course 1 always renders. Otherwise: flagged atomics draw debug boxes; a queued atomic is appended
 * to g_gfx_6c3da0 and not rendered now; the cached squared radius is computed on first use; an atomic
 * whose scaled squared camera distance is not below that radius is culled (not rendered). A visible atomic
 * with a material gets its alpha set from the distance and is counted in g_gfx_6c3ea0 (inferred: rendered count).
 * The real render callback g_gfx_89de08 runs last.
 */
RpAtomic * atomic_render_with_culling(RpAtomic *atomic)

{
  RwUInt8 *cull_data;
  void *camera;
  atomic_render_with_culling_camera_frame_layout *camera_frame;
  double fT;
  double dx;
  double dy;
  double dz;
  RwSphere *world_sphere;
  RwReal radius_scale;
  struct {
    float dist_sq;
    float radius_sq;
    float half_radius_sq;
  } alpha_args;

  camera = RwCameraGetCurrentCamera();
  world_sphere = RpAtomicGetWorldBoundingSphere(atomic);
  cull_data = &RpAtomic_object_object_type(atomic) + g_gfx_89de0c;
  /* Course 1 skips culling and always renders (inferred). */
  if (g_course_843e80 != 1) {
    /* Debug boxes for flagged atomics. */
    if ((*(uint *)(cull_data + CULL_DATA_FLAGS_OFFSET) & CULL_FLAG_DEBUG_BOXES) != 0) {
      debug_box_draw_with_culling_a(atomic,cull_data + CULL_DATA_BOX_A_OFFSET,cull_data + CULL_DATA_BOX_B_OFFSET);
      debug_box_draw_with_culling_b(atomic,cull_data + CULL_DATA_BOX_A_OFFSET,cull_data + CULL_DATA_BOX_B_OFFSET);
    }
    /* Queue for deferred rendering and return without rendering now. */
    if ((*(uint *)(cull_data + CULL_DATA_FLAGS_OFFSET) & CULL_FLAG_QUEUED) != 0) {
      (&g_gfx_6c3da0)[g_gfx_6c3eac] = (undefined4)atomic;
      g_gfx_6c3eac = g_gfx_6c3eac + 1;
      return atomic;
    }
    /* First use: cache the squared radius (radius^2 / CULL_RADIUS_SCALE_SQ). */
    if (*(uint *)cull_data == CULL_RADIUS_SQ_UNSET_BITS) {
      if ((RpAtomic_interpolator_flags(atomic) & INTERPOLATOR_FLAG_DIRTY_SPHERE) != 0) {
        _rpAtomicResyncInterpolatedSphere(atomic);
      }
      radius_scale = _rwSqrt(CULL_RADIUS_SCALE_SQ);
      fT = (double)(RpAtomic_boundingSphere(atomic)).radius / (double)radius_scale;
      *(float *)cull_data = (float)(fT * fT);
    }
    /* Camera position (camera frame, offsets 0x40..0x48 inferred) minus the sphere centre. */
    camera_frame = (atomic_render_with_culling_camera_frame_layout *)(*(char **)((char *)camera + CAMERA_FRAME_PTR_OFFSET));
    dx =(double)camera_frame->f_0x40 - (world_sphere->center).x;
    dy = (double)camera_frame->f_0x44 - (world_sphere->center).y;
    dz = (double)camera_frame->f_0x48 - (world_sphere->center).z;
    fT = (dy * dy + dx * dx + dz * dz) * *(float *)((char *)camera + CAMERA_DIST_SCALE_OFFSET);
    alpha_args.dist_sq = (float)fT;
    /* Scaled squared distance not below the cached radius: culled. */
    if (!(fT < *(float *)cull_data)) {
      return atomic;
    }
    /* Alpha from distance, passed as {dist_sq, radius_sq, half_radius_sq} (fade range inferred). */
    if (*(void **)(cull_data + CULL_DATA_MATERIAL_OFFSET) != (void *)0x0) {
      alpha_args.half_radius_sq = *(float *)cull_data * k_f_0_5;
      alpha_args.radius_sq = *(float *)cull_data;
      atomic_set_material_alpha(atomic,&alpha_args.dist_sq);
    }
    g_gfx_6c3ea0 = g_gfx_6c3ea0 + 1;
  }
  (*g_gfx_89de08)(atomic);
  return atomic;
}


