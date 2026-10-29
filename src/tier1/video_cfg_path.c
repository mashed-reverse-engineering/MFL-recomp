#include "mfl_tier2.h"
extern char s_videocfg_bin_005e1520[];
/*007712d8*/ extern undefined4 g_video_cfg_7712d8;
/*007712dc*/ extern undefined4 g_video_cfg_7712dc;
/*007712e0*/ extern undefined4 g_video_cfg_7712e0;
/*007712e4*/ extern undefined g_video_cfg_7712e4;

/* offsets inside the "videocfg.bin" block (inferred: second word, third word, the byte after them) */
#define VIDEO_CFG_OFS_WORD1 (4)
#define VIDEO_CFG_OFS_WORD2 (8)
#define VIDEO_CFG_OFS_BYTE (0xc)

/* video_cfg_path */

/* Copies fields of the "videocfg.bin" block into the g_video_cfg_7712d8..7712e4 globals
   (inferred: words at offsets 0, 4, 8 and a byte at offset 12). Returns &g_video_cfg_7712d8. */

/* TIER2: validated 2026-10-18 authority=diffemu */
undefined * video_cfg_path(void)
{
  g_video_cfg_7712d8 = (*(uint *)((char *)&s_videocfg_bin_005e1520 + 0));
  g_video_cfg_7712dc = (*(uint *)((char *)&s_videocfg_bin_005e1520 + VIDEO_CFG_OFS_WORD1));
  g_video_cfg_7712e0 = (*(uint *)((char *)&s_videocfg_bin_005e1520 + VIDEO_CFG_OFS_WORD2));
  g_video_cfg_7712e4 = s_videocfg_bin_005e1520[VIDEO_CFG_OFS_BYTE];
  return &g_video_cfg_7712d8;
}
