#include "mfl_tier2.h"
/*00771050*/ extern void (*g_data_771050)(void);
/*0077104c*/ extern void (*g_app_boot_77104c)(void);

void render_pipelines_create_two_cb_4a6a60(void)
{
  if (g_data_771050 != 0) {
    g_data_771050();
    return;
  }
  g_app_boot_77104c();
}
