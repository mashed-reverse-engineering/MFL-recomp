#include "mfl_tier2.h"
/*004aa200*/ extern int __cdecl FUN_004aa200(int a1, int a2);
#define OBJECT_STATE_OFFSET (0x188) /* state word passed to FUN_004aa200 (inferred) */
#define OBJECT_ATOMIC_OFFSET (0x184) /* pointer to the atomic object (inferred) */
#define ATOMIC_RENDER_VTABLE_OFFSET (0x48) /* render method slot in the atomic's vtable (inferred) */
/* object_render: draws one object. Calls FUN_004aa200(object, state) (inferred: sets up the object's state
 * before drawing), then calls the render method of the object's atomic.
 * object: the object to render (its atomic is at OBJECT_ATOMIC_OFFSET). Returns nothing. */

/* "renders the object's atomic after setting its state" */

void object_render(int object)

{
  FUN_004aa200(object,*(undefined4 *)(object + OBJECT_STATE_OFFSET));
  (**(code **)(*(char **)(object + OBJECT_ATOMIC_OFFSET) + ATOMIC_RENDER_VTABLE_OFFSET))(*(int *)(object + OBJECT_ATOMIC_OFFSET));
  return;
}


