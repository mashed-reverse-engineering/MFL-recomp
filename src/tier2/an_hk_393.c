#include "mfl_tier2.h"
/*004a5550*/ extern int log_printf();
/*004a8580*/ extern int video_cfg_path();
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*004b4531*/ extern void * FUN_004b4531(undefined4 p0, undefined4 p1);
/*004b4b12*/ extern size_t __cdecl _fwrite(void *p0, size_t p1, size_t p2, FILE *p3);
/*005e14cc*/ extern undefined g_controller_5e14cc;
/*007712f8*/ extern struct VideoCfg g_video_cfg;
#define VIDEO_CFG_SIZE (0x200)
/* video_cfg_save */

/* fwrite VIDEO_CFG_SIZE bytes from g_video_cfg */

/* Saves the video configuration (g_video_cfg) to the video cfg file.
 * Takes no parameters.
 * Returns 1 on success, 0 if the file could not be opened (inferred). */
undefined4 video_cfg_save(void)

{
  undefined4 cfg_path;
  FILE *file;

  /* resolve the cfg file path and log it */
  cfg_path = video_cfg_path();
  log_printf("Saving video cfg to %s\n",cfg_path);
  /* open the file for writing; the second argument is the open mode (inferred) */
  file = (FILE *)FUN_004b4531(cfg_path,&g_controller_5e14cc);
  if (file != (FILE *)0x0) {
    /* write the whole config struct, then close the file */
    _fwrite(&g_video_cfg,1,VIDEO_CFG_SIZE,file);
    _fclose(file);
    return 1;
  }
  return 0;
}


