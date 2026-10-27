#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();
/*0076be30*/ extern undefined g_particles_76be30;  /* particle system 1 */
/*0076be70*/ extern undefined g_particles_76be70;  /* particle system 2 */
/*0076bfec*/ extern undefined4 g_load_76bfec;      /* first slot of the object table, 0xa4-byte slots (10 object pointers each, used from +0) */
/*0076dee0*/ extern undefined4 g_particles_76dee0; /* per-slot active flags, 4 bytes each */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0076c50c*/ extern char g_fx_systems_76c50c[];


/* fx_systems_render_all: renders the particle systems and the active object slots with z write off. */

/* "z write off, renders the systems, then the slots at 0x76DEE0" */

void fx_systems_render_all(void)
{
  int *slot_ptr;       /* current 0xa4-byte slot of the object table */
  int *flag_ptr;       /* current active flag */
  int *object_ptr;     /* current object pointer within the slot */
  int i;

  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  particles_render(&g_particles_76be30);
  particles_render(&g_particles_76be70);
  flag_ptr = &g_particles_76dee0;
  slot_ptr = &g_load_76bfec;
  do {
    if (*flag_ptr != 0) {
      /* active slot: call virtual method at +0x48 of its 10 objects (method name unknown) */
      i = 10;
      object_ptr = slot_ptr;
      do {
        (**(code **)(*object_ptr + 0x48))(*object_ptr);
        object_ptr = object_ptr + 1;
        i = i + -1;
      } while (i != 0);
    }
    slot_ptr = slot_ptr + 0x29;   /* 0x29 dwords = 0xa4 bytes per slot */
    flag_ptr = flag_ptr + 1;
  } while ((int)slot_ptr < ((int)&g_fx_systems_76c50c));
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}
