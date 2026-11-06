/* rwa_base_register_defs_005b66f0 (0x005b66f0): base module: register its object definition tables (3, 1 and 1 entries); 1: all registered */
#include "mfl_tier2.h"

/*005b9fa0*/ extern int _rwaObjDefRegister(void *table, int count);      /* 1: ok */
/*006491d8*/ extern undefined PTR_LAB_006491d8;
/*006491d4*/ extern undefined PTR_RwaListenerUnregister_006491d4;
/*006491e4*/ extern undefined PTR__RwaParamTypeSetupAtomics_006491e4;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_base_register_defs_005b66f0(void)
{
  if (_rwaObjDefRegister(&PTR_LAB_006491d8, 3) == 1 && _rwaObjDefRegister(&PTR_RwaListenerUnregister_006491d4, 1) == 1 &&
      _rwaObjDefRegister(&PTR__RwaParamTypeSetupAtomics_006491e4, 1) == 1)
    return 1;
  return 0;
}
