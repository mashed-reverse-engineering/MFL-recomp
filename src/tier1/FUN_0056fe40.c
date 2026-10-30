/* compile: /G5 */
#include "mfl_tier2.h"
extern char g_rwp_cylinder_shape_ops[];
typedef struct RwpCylinder { char pad[0x40]; int height_raw; int radius_raw; } RwpCylinder;
extern void rwp_sphere_0056c950(RwpCylinder *s, void *ops);
/* Initialises a cylinder shape with its ops table and the two raw size values. */
RwpCylinder *FUN_0056fe40(RwpCylinder *c, int radius_raw, int height_raw)
{
  rwp_sphere_0056c950(c, g_rwp_cylinder_shape_ops);
  c->radius_raw = radius_raw;
  c->height_raw = height_raw;
  return c;
}
