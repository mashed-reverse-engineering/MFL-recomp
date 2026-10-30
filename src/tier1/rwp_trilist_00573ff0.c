/* compile: /G5 */
#include "mfl_tier2.h"
extern char g_rwp_trilist_shape_ops[];
extern void rwp_trilist_callback_005744b0();
typedef struct RwpTriShape { char pad[0x40]; int a; int b; void (*callback)(); char pad2[0x14]; } RwpTriShape;
extern char *rwp_capsule_00575b20(int size, int align, int tag);
extern void rwp_sphere_0056c950(void *s, void *ops);
/* Creates a trilist shape (0x60 bytes) with its two parameters and callback. (Rwp library) */
RwpTriShape *rwp_trilist_00573ff0(int a, int b)
{
  RwpTriShape *s = (RwpTriShape *)rwp_capsule_00575b20(0x60, 0x10, 0x30900);
  rwp_sphere_0056c950(s, g_rwp_trilist_shape_ops);
  s->a = a;
  s->b = b;
  s->callback = rwp_trilist_callback_005744b0;
  return s;
}
