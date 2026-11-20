#include "mfl_tier2.h"
extern char PTR_g_data_656369_005de9f8[];
/*0040d130*/ extern int __cdecl find_texture_a(RwChar *p0);
#define ICON_VARIANT_NIGHT (1)
#define ICON_VARIANT_REVERSED (2)
#define ICON_VARIANT_FLIP (3)
/* menu_icon_texture_lookup */
/* "0 (string), 1 night, 2 reversed -> find_texture_a" */
/* Looks up the menu icon texture for icon_variant (inferred: menu icon style selector)
 * and passes the texture name to find_texture_a. Variant 0 passes the global string
 * PTR_g_data_656369_005de9f8, the others pass a fixed name. Unknown variants do nothing.
 * Returns find_texture_a's result (the texture; the menu draw code at 0x43d660 uses it, VOID_w16: was void).
 * Unknown variants return whatever EAX holds (the compare value), as the original does; no caller passes one. */
int __cdecl menu_icon_texture_lookup(int icon_variant)
{
  /* Variant 0: texture name comes from the global string pointer. */
  if (icon_variant == 0) {
    return find_texture_a((RwChar *)&PTR_g_data_656369_005de9f8);
  }
  if (icon_variant == ICON_VARIANT_NIGHT) {
    return find_texture_a("night");
  }
  if (icon_variant == ICON_VARIANT_REVERSED) {
    return find_texture_a("reversed");
  }
  if (icon_variant == ICON_VARIANT_FLIP) {
    return find_texture_a("flip");
  }
  return icon_variant;
}
