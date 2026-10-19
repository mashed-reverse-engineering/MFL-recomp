#include "mfl_tier2.h"
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*008c8860*/ extern undefined4 g_course_8c8860;
/*008c8894*/ extern undefined4 g_course_8c8894;
/*008c8898*/ extern undefined4 g_game_8c8898;
/*008c889c*/ extern undefined4 g_game_8c889c;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
#define OBJECT_SLOT_WORDS (0x14)
#define ATOMIC_INTERPOLATOR_FLAGS_OFFSET (0x4c)
#define ATOMIC_BOUNDING_RADIUS_OFFSET (0x28)
/* course_active_object_get_info */

/* "(index, out xyz+radius): position and bounding radius of the object atomic" */

/*
 * Writes the position (x, y, z) and bounding radius of the course object in slot
 * object_index into out_xyzr[0..3]. The object's clump is the one stored in
 * g_course_8c8860 for that slot; its first atomic supplies the radius.
 * Writes nothing when the slot's flag (g_game_8c88a4) or clump is zero (inferred).
 */
void __cdecl course_active_object_get_info(int object_index,undefined4 *out_xyzr)

{
  char *atomic_addr;
  RpAtomic *atomic;

  if (((&g_game_8c88a4)[object_index * OBJECT_SLOT_WORDS] != 0) && ((&g_course_8c8860)[object_index * OBJECT_SLOT_WORDS] != 0)) {
    *(RwV3d *)out_xyzr = *(RwV3d *)&(&g_course_8c8894)[object_index * OBJECT_SLOT_WORDS];
    atomic_addr = clump_first_atomic((&g_course_8c8860)[object_index * OBJECT_SLOT_WORDS]);
    /* If bit 2 of the atomic's interpolator flags is set, resync its interpolated
       bounding sphere before the radius is read (inferred). */
    if ((*(byte *)(atomic_addr + ATOMIC_INTERPOLATOR_FLAGS_OFFSET) & 2) != 0) {
      atomic = (RpAtomic *)clump_first_atomic((&g_course_8c8860)[object_index * OBJECT_SLOT_WORDS]);
      _rpAtomicResyncInterpolatedSphere(atomic);
    }
    /* Radius: boundingSphere.radius of the first atomic. */
    atomic_addr = clump_first_atomic((&g_course_8c8860)[object_index * OBJECT_SLOT_WORDS]);
    out_xyzr[3] = *(undefined4 *)(atomic_addr + ATOMIC_BOUNDING_RADIUS_OFFSET);
  }
  return;
}


