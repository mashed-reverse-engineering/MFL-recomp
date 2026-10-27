#include "mfl_tier2.h"
/*0045e440*/ extern int weapon_pickups_clear();
/* weapon_mortar_reset_all */
/* Clears all weapon pickups by calling weapon_pickups_clear(). Takes no parameters, returns nothing.
 * The name suggests a mortar-specific reset (inferred), but the body does no mortar-specific work. */
void weapon_mortar_reset_all(void)
{
  weapon_pickups_clear();
  return;
}
