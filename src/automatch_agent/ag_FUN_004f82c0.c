#include "mfl_tier2.h"
typedef RwBool (*RwPluginDataChunkAlwaysCallBack)(void *, RwInt32, RwInt32);
typedef char mfl_size_always_cb[sizeof(RwPluginDataChunkAlwaysCallBack) == 4 ? 1 : -1];
/*0062ec44*/ extern undefined4 g_rp_62ec44;
/*004f1e40*/ extern RwInt32 __cdecl _rwPluginRegistryAddPlgnStrmlwysCB(struct RwPluginRegistry *p0, RwUInt32 p1, RwPluginDataChunkAlwaysCallBack p2);
/* FUN_004f82c0: same machine code as course_zone_contains_matrix_in_zone_6a2888 (tools/propagate_dups.py), byte gate checked */
void FUN_004f82c0(undefined4 param_1,undefined4 param_2)
{
  _rwPluginRegistryAddPlgnStrmlwysCB(&g_rp_62ec44,param_1,param_2);
  return;
}
