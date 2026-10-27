#include "mfl_tier2.h"
extern char PTR_g_data_746f64_005de9d0[];
/*0040d0f0*/ extern int __cdecl find_texture_dict_c(RwChar *p0);
/*
 * Returns the texture dictionary for a menu icon.
 * icon_index selects the icon: 0 = "lock", 1 = a global string (name unknown, inferred),
 * 2 = "check". Each is passed to find_texture_dict_c, whose result is returned.
 * Any other index returns 0.
 */
/* "0 lock, 1 (string), 2 check -> find_texture_dict_c" */
undefined4 menu_icon_texture_dict_lookup(int icon_index)
{
  undefined4 texture_dict;

  if (icon_index == 0) {
    texture_dict = find_texture_dict_c("lock");
    return texture_dict;
  }
  if (icon_index == 1) {
    texture_dict = find_texture_dict_c((RwChar *)&PTR_g_data_746f64_005de9d0);
    return texture_dict;
  }
  if (icon_index == 2) {
    texture_dict = find_texture_dict_c("check");
    return texture_dict;
  }
  return 0;
}
