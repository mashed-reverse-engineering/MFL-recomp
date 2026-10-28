// cluster-entries: debris_cull_update
/* Installed cluster debris_cull_update (tools/cluster_install.py from output/cluster_pilot11/debris_cull_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* debris_cull_update */

/* "frustum test of each flagged debris atomic bounding sphere" */

/* For each of the 10 debris slots (stride 0x18 from debris+0xc) whose bit is set in the mask at debris+0xf0, frustum-tests the atomic's world bounding sphere against the current camera, stores slot[2] = outside, and calls the atomic's vtable+0x48 hook (inferred). */
void debris_cull_update_n(int debris)

{
  RwSphere *sphere;
  RwFrustumTestResult test_result;
  int index;
  int *slot;
  
  if (*(int *)(debris + 0xf4) != 0) {
    index = 0;
    slot = (int *)(debris + 0xc);
    do {
      if ((*(byte *)((index >> 3) + 0xf0 + debris) & (byte)(1 << ((byte)index & 7))) != 0) {
        sphere = RpAtomicGetWorldBoundingSphere((RpAtomic *)*slot);
        test_result = RwCameraFrustumTestSphere(RwCameraGetCurrentCamera(),sphere);
        slot[2] = (test_result == rwSPHEREOUTSIDE);
        (**(code **)(*slot + 0x48))(*slot);
      }
      index = index + 1;
      slot = slot + 6;
    } while (index < 10);
  }
  return;
}

/* entry thunk at the original address of debris_cull_update: register inputs (ebx) -> natural C debris_cull_update_n; ret 0 */
__declspec(naked) void debris_cull_update(void)
{
    __asm {
        push ebx
        call debris_cull_update_n
        add esp, 4
        ret
    }
}
