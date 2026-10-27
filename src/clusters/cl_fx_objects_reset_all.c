// cluster-entries: fx_objects_reset_all
/* Installed cluster fx_objects_reset_all (tools/cluster_install.py from output/cluster_pilot_zx/fx_objects_reset_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00484680*/ extern int object_set_flag_4();
/*0075f828*/ extern undefined4 g_particles_75f828;
extern char g_particles_769788[];
#define POOL_COUNT_INDEX (0xcb)
#define POOL_STRIDE_WORDS (0xcc)
#define OBJECT_STRIDE_WORDS (0x14)

/* fx_objects_reset_all */

/* "for each of the pools applies the transform reset, flags each object and clears the counters" */

/* Resets every particle object pool (inferred): walks the pools laid out from g_particles_75f828 up to g_particles_769788.
   Each pool holds an object count at POOL_COUNT_INDEX. Takes no parameters and returns nothing. */
static int fx_object_apply_transform_r(int * p0);
extern void fx_object_apply_transform(void);
void fx_objects_reset_all(void)

{
  int object_count;
  undefined4 *obj;
  undefined4 *pool;

  pool = &g_particles_75f828;
  do {
    /* read this pool's object count (inferred) */
    object_count = pool[POOL_COUNT_INDEX];
    if (object_count != 0) {
      /* walk the objects from the last one back to the first (inferred): apply the transform reset, then set flag 4 */
      obj = pool + object_count * OBJECT_STRIDE_WORDS + 3;
      do {
        obj = obj + -OBJECT_STRIDE_WORDS;
        fx_object_apply_transform_r(obj);
        object_set_flag_4(*obj,1);
        object_count = object_count + -1;
      } while (object_count != 0);
    }
    *pool = 0;
    pool[1] = 0;
    pool = pool + POOL_STRIDE_WORDS;
  } while ((LONG_PTR)pool < ((LONG_PTR)&g_particles_769788));
  return;
}

/* fx_object_apply_transform_r: C -> original fx_object_apply_transform (registers esi) */
__declspec(naked) int fx_object_apply_transform_r(int * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call fx_object_apply_transform
        pop esi
        ret
    }
}
