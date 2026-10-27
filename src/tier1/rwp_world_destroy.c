#include "mfl_tier2.h"
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*0056e340*/ extern void FUN_0056e340(undefined4 p0);
/*00573f50*/ extern RwBool RtAnimAnimationDestroy(struct RtAnimAnimation *p0);
/*005e0458*/ extern undefined g_rwp_world_5e0458;
/*00701c9c*/ extern undefined4 g_rw_701c9c;
/*0088a840*/ extern undefined4 g_rwp_world_88a840;
/* rwp_world_destroy */
/* "prints 'DESTROY CALLED' then destroys the animation and the physics world" */
void rwp_world_destroy(void)
{
  if (g_rw_701c9c != 0) {
    FID_conflict__wprintf((char *)&g_rwp_world_5e0458);
    RtAnimAnimationDestroy(g_rwp_world_88a840);
    FUN_0056e340(g_rw_701c9c);
    g_rw_701c9c = 0;
  }
  return;
}
