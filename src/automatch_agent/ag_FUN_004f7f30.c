#include "mfl_tier2.h"
typedef RwBool (*RwPluginDataChunkRightsCallBack)(void *, RwInt32, RwInt32, RwUInt32);
typedef char mfl_size_rights_cb[sizeof(RwPluginDataChunkRightsCallBack) == 4 ? 1 : -1];
/*0062ec04*/ extern undefined4 g_rp_62ec04;
/*004f1e70*/ extern RwInt32 __cdecl _rwPluginRegistryAddPlgnStrmRightsCB(struct RwPluginRegistry *p0, RwUInt32 p1, RwPluginDataChunkRightsCallBack p2);
/* FUN_004f7f30: same machine code as course_zone_contains_matrix_in_zone_6a2888 (tools/propagate_dups.py), byte gate checked */
void FUN_004f7f30(undefined4 param_1,undefined4 param_2)
{
  _rwPluginRegistryAddPlgnStrmRightsCB(&g_rp_62ec04,param_1,param_2);
  return;
}
