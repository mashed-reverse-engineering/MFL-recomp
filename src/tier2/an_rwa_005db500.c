/* rwa_soft_dsp2_interface_destroy_005db500 (0x005db500): software output: destroy the second soft dsp interface definition */
#include "mfl_tier2.h"

/*005bd9e0*/ extern int FUN_005bd9e0(void *def);      /* RwaObjInterfaceDefDestroy */
/*007dc68c*/ extern undefined g_rwa_soft_dsp2_int_def;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_dsp2_interface_destroy_005db500(void)
{
  return FUN_005bd9e0(&g_rwa_soft_dsp2_int_def);
}
