// cluster-entries: effect_trails_render_all
/* Installed cluster effect_trails_render_all (tools/cluster_install.py from output/cluster_pilot_kc/effect_trails_render_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*005e64a4*/ extern float k_f_0;
/*00632a58*/ extern undefined8 ones;
/*006c5e18*/ extern undefined4 *g_gfx_6c5e18;
/*006c60d0*/ extern undefined4 g_gfx_6c60d0[0x1000];
/*006f1e20*/ extern undefined1 g_gfx_6f1e20;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define TRAIL_SLOT_STRIDE_DWORDS (0xb0) /* ints per trail slot */
/* effect_trails_render_all */

/* Draws every active trail slot as an Im3D triangle strip (blend src ZERO, dst INVSRCALPHA, trail texture); a slot outside the camera frustum that has a flag in word 1 is freed instead */

static void effect_slot_free_r(int p0);
extern void effect_slot_free(void);
void effect_trails_render_all(void)

{
  RwFrustumTestResult frustum_result;
  void *im3d_vertices;
  int slot_index;
  int *slot_ptr;
  struct {
    void *src_blend;
    void *dst_blend;
  } frame;
  
  RwRenderStateGet(rwRENDERSTATESRCBLEND,&frame.src_blend);
  RwRenderStateGet(rwRENDERSTATEDESTBLEND,&frame.dst_blend);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDZERO);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  RwRenderStateSet
            (rwRENDERSTATETEXTURERASTER,(void *)*g_gfx_6c5e18);
  slot_index = 0;
  slot_ptr = g_gfx_6c60d0;
  do {
    if ((((&g_gfx_6f1e20)[slot_index >> 3] & (byte)(1 << ((byte)slot_index & 7))) != 0) && (*slot_ptr != 0))
    {
      frustum_result = RwCameraFrustumTestSphere(RwCameraGetCurrentCamera(),(RwSphere *)(slot_ptr + -4));
      if ((frustum_result == rwSPHEREOUTSIDE) && (slot_ptr[1] != 0)) {
        effect_slot_free_r(slot_index);
      }
      else if ((*(float *)&slot_ptr[-1] != k_f_0) && (*slot_ptr != 1)) {
        im3d_vertices = RwIm3DTransform((RwIm3DVertex *)(slot_ptr + -0xac),*slot_ptr * 2,(RwMatrix *)0x0,1);
        if (im3d_vertices != (void *)0x0) {
          RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
          RwIm3DEnd();
        }
      }
    }
    slot_ptr = slot_ptr + TRAIL_SLOT_STRIDE_DWORDS;
    slot_index = slot_index + 1;
  } while ((int)slot_ptr < (int)&g_gfx_6f1e20 + 0x2b0);
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
