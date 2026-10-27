#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*006586e8*/ extern undefined g_load_6586e8;
/*0041b970*/ extern int hudpanel_reset_all();
/*0041b9c0*/ extern int hudpanel_clumps_destroy();
/*0041ba10*/ extern int hudpanel_slot_reset(undefined4 p0);
/*0041be10*/ extern int hudpanel_slots_clear();
/*0041be30*/ extern int hudpanel_values_init();
/*0041c0d0*/ extern int hudpanel_counters_clear(undefined4 p0);
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00486710*/ extern int particles_destroy();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00658300*/ extern undefined4 g_load_658300;
/*00658450*/ extern undefined g_hudpanel_658450;
/*00658490*/ extern undefined4 g_particles_658490;
/*006584f8*/ extern undefined g_hudpanel_6584f8;
/*00658538*/ extern undefined g_hudpanel_658538;
/*006586e8*/ extern undefined g_load_6586e8;
/*00658b28*/ extern undefined4 g_load_658b28;
/*00658b2c*/ extern undefined4 g_load_658b2c;
/* marker_particles_destroy */

/* "particles_destroy(0x6586E8)" */

void marker_particles_destroy(void)
{
  particles_destroy(&g_load_6586e8);
  return;
}
