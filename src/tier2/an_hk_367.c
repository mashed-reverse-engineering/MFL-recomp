#include "mfl_tier2.h"
/*007712ec*/ extern char *g_video_7712ec;
/*007712f0*/ extern undefined4 g_video_7712f0;
/*007712f8*/ extern struct VideoCfg g_video_cfg;
/*007714f8*/ extern char *g_video_mode_lists;
/*007714fc*/ extern undefined4 g_video_subsystem_index;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
static int video_mode_name_format_004a8670_r(int reg_ebx, undefined4 stk0);
extern void video_mode_name_format_004a8670(void);
extern char g_video_cfg_mode_name_0_[];
extern char g_video_cfg_subsystem_0_[];
#define VIDEO_SUBSYSTEM_NAME_SIZE (0x50) /* sizeof(g_video_cfg.subsystem) */
#define VIDEO_MODE_ENTRY_SIZE (0x18)
#define VIDEO_MODE_HEIGHT_OFFSET (4)
#define VIDEO_MODE_DEPTH_OFFSET (8)
#define VIDEO_MODE_FULLSCREEN_OFFSET (0xc)
__declspec(naked) int video_mode_name_format_004a8670_r(int reg_ebx, undefined4 stk0)
{
    __asm {
        push ebx
        push dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 12]
        call video_mode_name_format_004a8670
        add esp, 4
        pop ebx
        ret
    }
}

/* video_cfg_apply_selection */

/* copies the chosen adapter/mode (name, width, height, depth, fullscreen flag) into g_video_cfg and
   sets version 5
   Takes no parameters. Reads the globals g_video_subsystem_index (chosen adapter) and g_video_7712f0
   (chosen mode index within that adapter). */

void video_cfg_apply_selection(void)

{
  char *mode_entry_offset;

  g_video_cfg.subsystem_index = g_video_subsystem_index;
  g_video_cfg.version = 5;
  g_video_cfg.mode_index = g_video_7712f0;
  /* adapter name: table of VIDEO_SUBSYSTEM_NAME_SIZE-byte entries indexed by subsystem (inferred) */
  rwstrcpy
            ((RwChar *)&g_video_cfg_subsystem_0_,(RwChar *)(g_video_subsystem_index * VIDEO_SUBSYSTEM_NAME_SIZE + g_video_7712ec));
  video_mode_name_format_004a8670_r
            (*(int *)(g_video_mode_lists + g_video_subsystem_index * 4) + g_video_7712f0 * VIDEO_MODE_ENTRY_SIZE,
             ((int)&g_video_cfg_mode_name_0_));
  /* mode entry of the chosen mode: width, height, depth, flags as uints (inferred) */
  mode_entry_offset = g_video_7712f0 * VIDEO_MODE_ENTRY_SIZE;
  g_video_cfg.width =
       *(uint *)(mode_entry_offset + *(int *)(g_video_mode_lists + g_video_subsystem_index * 4));
  g_video_cfg.height =
       *(uint *)(mode_entry_offset + VIDEO_MODE_HEIGHT_OFFSET + *(int *)(g_video_mode_lists + g_video_subsystem_index * 4));
  g_video_cfg.depth =
       *(uint *)(mode_entry_offset + VIDEO_MODE_DEPTH_OFFSET + *(int *)(g_video_mode_lists + g_video_subsystem_index * 4));
  g_video_cfg.fullscreen =
       *(uint *)(mode_entry_offset + VIDEO_MODE_FULLSCREEN_OFFSET + *(int *)(g_video_mode_lists + g_video_subsystem_index * 4)) & 1
  ;
  return;
}


