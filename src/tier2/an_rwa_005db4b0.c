/* rwa_soft_dsp2_interface_register_005db4b0 (0x005db4b0): software mixer interface (definition 0x7dc68c), set up the soft dsp2 interface definition once */
#include "mfl_tier2.h"

/*005bd960*/ extern int FUN_005bd960(void *def);      /* RwaObjInterfaceDefCreate: 0 = already registered / failed */
/*005bd930*/ extern void FUN_005bd930(void *def, const void *uuid, int flags);      /* assign the interface uuid */
/*005bd810*/ extern void FUN_005bd810(void *def, const void *inputs, int num_inputs, const void *outputs, int num_outputs);      /* RwaObjInterfaceDefSetup */

/*007dc68c*/ extern undefined g_rwa_soft_dsp2_int_def;
/*005f8ffc*/ extern undefined g_rwa_soft_dsp2_int_uuid;
/*0064c9c8*/ extern undefined g_rwa_soft_dsp2_int_inputs;      /* 6 parameter definitions */
/*0064ca40*/ extern undefined g_rwa_soft_dsp2_int_outputs;     /* 5 parameter definitions */

/* Returns the definition, or NULL when it could not be created. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_dsp2_interface_register_005db4b0(void)
{
  if (!FUN_005bd960(&g_rwa_soft_dsp2_int_def))
    return 0;
  FUN_005bd930(&g_rwa_soft_dsp2_int_def, &g_rwa_soft_dsp2_int_uuid, 0);
  FUN_005bd810(&g_rwa_soft_dsp2_int_def, &g_rwa_soft_dsp2_int_inputs, 6, &g_rwa_soft_dsp2_int_outputs, 5);
  return &g_rwa_soft_dsp2_int_def;
}
