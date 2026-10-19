// cluster-entries: debris_cull_update
/* Installed cluster debris_cull_update (tools/cluster_install.py from output/cluster_pilot11/debris_cull_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* debris_cull_update */

/* WARNING: Unknown calling convention */
/* "frustum test of each flagged debris atomic bounding sphere" */

void debris_cull_update_n(int in_ebx)

{
  RwSphere *sphere;
  RwFrustumTestResult RVar1;
  int iVar2;
  int *piVar3;
  
  if (*(int *)(in_ebx + 0xf4) != 0) {
    iVar2 = 0;
    piVar3 = (int *)(in_ebx + 0xc);
    do {
      if ((*(byte *)((iVar2 >> 3) + 0xf0 + in_ebx) & (byte)(1 << ((byte)iVar2 & 7))) != 0) {
        sphere = RpAtomicGetWorldBoundingSphere((RpAtomic *)*piVar3);
        RVar1 = RwCameraFrustumTestSphere(RwCameraGetCurrentCamera(),sphere);
        piVar3[2] = (RVar1 == rwSPHEREOUTSIDE);
        (**(code **)(*piVar3 + 0x48))(*piVar3);
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 6;
    } while (iVar2 < 10);
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
