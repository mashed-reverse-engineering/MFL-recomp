#include "mfl_tier2.h"

#define PTR_SIZE 4

/*004b3bca*/ extern void __cdecl _free_004b45fd(void *p0);
/*007712e8*/ extern undefined4 g_video_mode_counts;
/*007712ec*/ extern undefined4 g_video_7712ec;
/*007714f8*/ extern undefined4 g_video_mode_lists;
/*00771500*/ extern int g_app_771500;


/* video_dialog_free */

/* Frees the per-subsystem video mode tables and video dialog resources. */

void video_dialog_free(void)
{
  int subsystem_idx;
  
  subsystem_idx = 0;
  if (0 < g_app_771500) {
    /* Free mode list allocated for each subsystem */
    do {
      _free_004b45fd(*(void **)((int)g_video_mode_lists + subsystem_idx * PTR_SIZE));
      subsystem_idx = subsystem_idx + 1;
    } while (subsystem_idx < g_app_771500);
  }
  _free_004b45fd(g_video_mode_lists);
  _free_004b45fd(g_video_mode_counts);
  /* Free video dialog resource (unknown) */
  _free_004b45fd(g_video_7712ec);
  return;
}
