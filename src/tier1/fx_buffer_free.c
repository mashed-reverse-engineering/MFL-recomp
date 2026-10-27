#include "mfl_tier2.h"
/*0076e3b8*/ extern undefined4 g_app_76e3b8;
/*0076e3bc*/ extern undefined4 g_app_76e3bc;
/*0076e3c0*/ extern undefined4 g_app_76e3c0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* fx_buffer_free */

/* "frees the 0x7000 byte buffer g_app_76e3bc and clears the flag" */

void fx_buffer_free(void)
{
  if (g_app_76e3c0 != 0) {
    g_app_76e3c0 = 0;
    if (g_app_76e3bc != 0) {
      RwFree(g_app_76e3bc);
      g_app_76e3bc = 0;
    }
    if (g_app_76e3b8 != 0) {
      RwFree(g_app_76e3b8);
      g_app_76e3b8 = 0;
    }
  }
  return;
}
