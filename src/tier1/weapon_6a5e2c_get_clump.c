#include "mfl_tier2.h"
/*004f6bf0*/ extern int rpclump_clone(undefined4 p0);
/*006a5e2c*/ extern undefined4 g_weapon_6a5e2c;
/* weapon_6a5e2c_get_clump
 * Returns the clump (inferred) of the weapon in g_weapon_6a5e2c, or 0 when the global is unset.
 * No parameters.
 */
/* "rpclump_clone of g_weapon_6a5e2c when set" */
undefined4 weapon_6a5e2c_get_clump(void)
{
  undefined4 clump;

  /* global set: look up its clump (inferred) */
  if (g_weapon_6a5e2c != 0) {
    clump = rpclump_clone(g_weapon_6a5e2c);
    return clump;
  }
  return 0;
}
