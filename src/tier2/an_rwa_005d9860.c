/* rwa_soft_dsp_start_execute_005d9860 (0x005d9860): software dsp node: object start-execute */
#include "mfl_tier2.h"

/*005bac70*/ extern void *FUN_005bac70(void *obj, int flag);      /* _rwaObjStartExecute */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_dsp_start_execute_005d9860(char *dsp)
{
  FUN_005bac70(dsp, 1);
  return dsp;
}
