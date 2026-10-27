#include "mfl_tier2.h"
/*0089de04*/ extern undefined4 g_app_89de04; /* offset added to the object pointer to reach its plugin data (base name unknown) */


/* material_plugin_clear_12 */

/* "clears 3 dwords of the plugin data" */

/* Zeroes the 12 dwords (48 bytes) of plugin data at object + g_app_89de04 and returns 1
   (the quoted note above says 3 dwords; the code clears 12). */
undefined4 material_plugin_clear_12(char *object)
{
  undefined4 *plugin_data;
  
  plugin_data = (undefined4 *)(object + g_app_89de04);
  *plugin_data = 0;
  plugin_data[1] = 0;
  plugin_data[2] = 0;
  plugin_data[3] = 0;
  plugin_data[4] = 0;
  plugin_data[6] = 0;
  plugin_data[8] = 0;
  plugin_data[10] = 0;
  plugin_data[5] = 0;
  plugin_data[7] = 0;
  plugin_data[9] = 0;
  plugin_data[0xb] = 0;
  return 1;
}
