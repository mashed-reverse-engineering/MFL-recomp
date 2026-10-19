#include "mfl_tier2.h"
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004a5550*/ extern int log_printf();
/*004a8580*/ extern int video_cfg_path();
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*004b4531*/ extern int FUN_004b4531(undefined4 p0, undefined4 p1);
/*004b49bf*/ extern size_t __cdecl _fread(void *p0, size_t p1, size_t p2, FILE *p3);
/*005e0450*/ extern undefined g_piz_5e0450;
/*007712f8*/ extern struct VideoCfg g_video_cfg;
#define VIDEO_CFG_FILE_SIZE (0x200)
/* video_cfg_load
   Loads the video config file from the path returned by video_cfg_path() and reads
   VIDEO_CFG_FILE_SIZE bytes of it into g_video_cfg.
   No parameters. Returns 1 if the file was opened and read, 0 if it could not be opened
   (the failure is logged). */

undefined4 video_cfg_load(void)

{
  undefined4 cfg_path;
  FILE *cfg_file;

  cfg_path = video_cfg_path();
  log_printf("Loading video cfg from %s\n",cfg_path);
  /* open the file (FUN_004b4531 is the open call, inferred; g_piz_5e0450 is its mode string) */
  cfg_file = (FILE *)FUN_004b4531(cfg_path,&g_piz_5e0450);
  if (cfg_file != (FILE *)0x0) {
    /* read the whole config struct, then close the file */
    _fread(&g_video_cfg,1,VIDEO_CFG_FILE_SIZE,cfg_file);
    _fclose(cfg_file);
    return 1;
  }
  /* open failed */
  log_print("\tFAILED\n");
  return 0;
}


