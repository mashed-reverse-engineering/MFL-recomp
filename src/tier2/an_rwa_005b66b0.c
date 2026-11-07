/* rwa_core_register_default_objects_005b66b0 (0x005b66b0): core (RwaCoreRegisterDefaultObjects): register the default object definition tables (3, 1 and 1 entries); 1: all registered */
#include "mfl_tier2.h"

/*005b9fa0*/ extern int _rwaObjDefRegister(void *table, int count);      /* 1: ok */
/*006491c4*/ extern undefined PTR_LAB_006491c4;
/*006491c0*/ extern undefined PTR_rwa_unit23cc10_005b9f20_006491c0;
/*006491d0*/ extern undefined PTR__rwaNULLWaveRegister_006491d0;

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_core_register_default_objects_005b66b0(void)
{
  if (_rwaObjDefRegister(&PTR_LAB_006491c4, 3) == 1 && _rwaObjDefRegister(&PTR_rwa_unit23cc10_005b9f20_006491c0, 1) == 1 &&
      _rwaObjDefRegister(&PTR__rwaNULLWaveRegister_006491d0, 1) == 1)
    return 1;
  return 0;
}
