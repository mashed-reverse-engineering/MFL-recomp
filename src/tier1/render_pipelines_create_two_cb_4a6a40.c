#include "mfl_tier2.h"
/*00771040*/ extern void (*g_data_771040)(void);
/*00771044*/ extern void (*g_app_boot_771044)(void);

/* Render callbacks of render_pipelines_create_two: run the application's hook when one is installed, else the default one. */
void render_pipelines_create_two_cb_4a6a40(void)
{
  if (g_data_771040 != 0) {
    g_data_771040();
    return;
  }
  g_app_boot_771044();
}
