// cluster-entries: effect_trails_render_all
/* Installed cluster effect_trails_render_all (tools/cluster_install.py from output/cluster_pilot_kc/effect_trails_render_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*005ddc28*/ extern float k_f_0;
/*00632a58*/ extern undefined8 ones;
/*006c5e18*/ extern undefined4 *g_gfx_6c5e18;
/*006c60d0*/ extern undefined4 g_gfx_6c60d0[0x1000];
/*006f1e20*/ extern undefined1 g_gfx_6f1e20;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* effect_trails_render_all */

/* "renders active trails with Im3D strips after a frustum test, frees outside ones" */

static void effect_slot_free_r(int p0);
extern void effect_slot_free(void);
void effect_trails_render_all(void)

{
  RwFrustumTestResult frustum_result;
  void *pvVar2;
  int reg_esi;
  int *piVar3;
  struct {
    void *src_blend;
    void *dst_blend;
  } frame;
  
  RwRenderStateGet(rwRENDERSTATESRCBLEND,&frame.src_blend);
  RwRenderStateGet(rwRENDERSTATEDESTBLEND,&frame.dst_blend);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)0x6);
  RwRenderStateSet
            (rwRENDERSTATETEXTURERASTER,(void *)*g_gfx_6c5e18);
  reg_esi = 0;
  piVar3 = g_gfx_6c60d0;
  do {
    if ((((&g_gfx_6f1e20)[reg_esi >> 3] & (byte)(1 << ((byte)reg_esi & 7))) != 0) && (*piVar3 != 0))
    {
      frustum_result = RwCameraFrustumTestSphere(RwCameraGetCurrentCamera(),(RwSphere *)(piVar3 + -4));
      if ((frustum_result == rwSPHEREOUTSIDE) && (piVar3[1] != 0)) {
        effect_slot_free_r(reg_esi);
      }
      else if ((*(float *)&piVar3[-1] != k_f_0) && (*piVar3 != 1)) {
        pvVar2 = RwIm3DTransform((RwIm3DVertex *)(piVar3 + -0xac),*piVar3 * 2,(RwMatrix *)0x0,1);
        if (pvVar2 != (void *)0x0) {
          RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
          RwIm3DEnd();
        }
      }
    }
    piVar3 = piVar3 + 0xb0;
    reg_esi = reg_esi + 1;
  } while ((int)piVar3 < (int)&g_gfx_6f1e20 + 0x2b0);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,frame.src_blend);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,frame.dst_blend);
  return;
}

/* effect_slot_free_r: C -> original effect_slot_free (registers esi) */
__declspec(naked) void effect_slot_free_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call effect_slot_free
        pop esi
        ret
    }
}
