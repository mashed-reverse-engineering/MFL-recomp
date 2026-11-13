/* Rwp physics library, triangle-list shape: MFL.exe 0x00573ff0 (create) and 0x00574400 (a do-nothing method of its ops table).
 * Written from the MFL.exe disassembly (objdump).
 *
 * The create stores the default per-triangle callback 0x5744b0.  That callback is an int3 trap row (tools/delink/rw_trap.txt,
 * S3 2026-11-06): the only caller of the create, rwp_world_create_once (0x4913a0), replaces it at once through 0x56ca60.  The
 * address is written as a constant so no installed C names the trapped body (trapcheck); the standalone link keeps original
 * code at its original address, so the word stored is the same as the original's. */
#include "mfl_tier2.h"

#define RWP_MEM_HINT 0x30900                     /* allocation hint used by all Rwp objects */
#define RWP_TRILIST_DEFAULT_CALLBACK 0x005744b0  /* rwp_trilist_callback_005744b0, trap row: replaced by every caller */

typedef struct RwpTrilistShape {
  unsigned char base[0x40];   /* common shape header (rwp_shape_reset_identity) */
  void *triangles;            /* 0x40 */
  void *user;                 /* 0x44 */
  void *callback;             /* 0x48: per-triangle callback */
} RwpTrilistShape;

/*00575b20*/ extern void *rwp_capsule_00575b20(int size, int align, unsigned int hint);      /* aligned Rwp allocation */
/*0056c950*/ extern void rwp_shape_reset_identity(void *shape, const void *ops);
/*005f6800*/ extern const unsigned char g_rwp_trilist_shape_ops[];

/* Allocates a 0x60-byte, 16-byte aligned shape, gives it the triangle-list ops table and stores the two arguments.
   No out-of-memory check (the original has none either). */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwpTrilistShape *rwp_trilist_create_00573ff0(void *triangles, void *user)
{
  RwpTrilistShape *shape = (RwpTrilistShape *)rwp_capsule_00575b20(0x60, 0x10, RWP_MEM_HINT);

  rwp_shape_reset_identity(shape, g_rwp_trilist_shape_ops);
  shape->triangles = triangles;
  shape->user = user;
  shape->callback = (void *)RWP_TRILIST_DEFAULT_CALLBACK;
  return shape;
}
