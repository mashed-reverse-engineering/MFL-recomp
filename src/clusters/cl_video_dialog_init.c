// cluster-entries: video_dialog_init
/* Installed cluster video_dialog_init (tools/cluster_install.py from output/cluster_pilot_zy/video_dialog_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d2fd0*/ extern int RwEngineGetCurrentSubSystem(void);
/*004d3090*/ extern int RwEngineGetCurrentVideoMode(void);
/*004d2f70*/ extern int RwEngineGetNumSubSystems(void);
/*004d3000*/ extern int RwEngineSetSubSystem(int index);
/*004d2fa0*/ extern void *RwEngineGetSubSystemInfo(void *info, int index);
/*004d3030*/ extern int RwEngineGetNumVideoModes(void);
/*004d3060*/ extern void *RwEngineGetVideoModeInfo(void *info, int index);
/*004d30c0*/ extern int RwEngineSetVideoMode(int index);
/*004b45eb*/ extern void *_malloc(unsigned int size);
/*007714fc*/ extern int g_video_subsystem_index;
/*007712f0*/ extern int g_video_7712f0;
/*00771500*/ extern int g_app_771500;
/*007712ec*/ extern char *g_video_7712ec;
/*007712e8*/ extern int *g_video_mode_counts;
/*007714f8*/ extern char **g_video_mode_lists;

#define SUBSYSTEM_INFO_SIZE (0x50)
#define VIDEO_MODE_INFO_SIZE (0x18)

/* Enumerates the RenderWare sub systems (video cards) and their video modes into the tables g_video_7712ec (RwSubSystemInfo[]), g_video_mode_counts and g_video_mode_lists (RwVideoMode[] per sub system),
   then restores the current sub system and video mode. */
void video_dialog_init(void)
{
  int subsystem;
  int mode;

  g_video_subsystem_index = RwEngineGetCurrentSubSystem();
  g_video_7712f0 = RwEngineGetCurrentVideoMode();
  g_app_771500 = RwEngineGetNumSubSystems();
  g_video_7712ec = (char *)_malloc(g_app_771500 * SUBSYSTEM_INFO_SIZE);
  g_video_mode_counts = (int *)_malloc(g_app_771500 * 4);
  g_video_mode_lists = (char **)_malloc(g_app_771500 * 4);
  for (subsystem = 0; subsystem < g_app_771500; subsystem++) {
    RwEngineSetSubSystem(subsystem);
    RwEngineGetSubSystemInfo(g_video_7712ec + subsystem * SUBSYSTEM_INFO_SIZE, subsystem);
    g_video_mode_counts[subsystem] = RwEngineGetNumVideoModes();
    g_video_mode_lists[subsystem] = (char *)_malloc(g_video_mode_counts[subsystem] * VIDEO_MODE_INFO_SIZE);
    for (mode = 0; mode < g_video_mode_counts[subsystem]; mode++) {
      RwEngineGetVideoModeInfo(g_video_mode_lists[subsystem] + mode * VIDEO_MODE_INFO_SIZE, mode);
    }
  }
  RwEngineSetSubSystem(g_video_subsystem_index);
  RwEngineSetVideoMode(g_video_7712f0);
}

