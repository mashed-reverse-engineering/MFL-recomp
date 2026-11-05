/* rwa_soft_dsp_interface_destroy_005da9e0 (0x005da9e0): software mixer: destroy the soft dsp interface definition */
#include "mfl_tier2.h"

/*005bd9e0*/ extern int FUN_005bd9e0(void *def);      /* RwaObjInterfaceDefDestroy */
/*007dc4e4*/ extern undefined g_rwa_soft_dsp_int_def;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_dsp_interface_destroy_005da9e0(void)
{
  return FUN_005bd9e0(&g_rwa_soft_dsp_int_def);
}
