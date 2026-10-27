// cluster-entries: d3d9_pipeline_create_a_cb_4a6860
/* Installed cluster d3d9_pipeline_create_a_cb_4a6860 (tools/cluster_install.py from output/cluster_pilot_md/d3d9_pipeline_create_a_cb_4a6860.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a6d30*/ extern void __cdecl matrix_transpose_row_copy(void *dst, RwMatrix *src);
/*004db470*/ extern RwBool RwD3D9SetTransform(RwUInt32 p0, void *p1);
/*004fae50*/ extern void _rwD3D9EnableClippingIfNeeded(void *p0, RwUInt32 p1);
/*004fc623*/ extern void __stdcall thunk_FUN_004fc616(void *dst, void *src);   /* tail jump into D3DX: ret 8 */
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/* d3d9_pipeline_create_a_cb_4a6860 */

#define D3DTS_WORLD 0x100

/* "function pointer pushed/stored by d3d9_pipeline_create_a (PUSH 0x4a6860): render callback of the pipeline: sets the transposed (and
   converted in place) world matrix of the object as D3D transform, enables clipping when requested and dispatches the instance render" */
static int d3d9_instance_render_dispatch_r(int p0, undefined4 p1, undefined4 p2);
extern void d3d9_instance_render_dispatch(void);
void d3d9_pipeline_create_a_cb_4a6860(undefined4 instance_arg, void *object, unsigned char clip_flag, undefined4 render_arg)
{
  RwMatrix transposed;
  RwMatrix *ltm;

  ltm = RwFrameGetLTM(*(RwFrame **)((char *)object + 4));
  matrix_transpose_row_copy(&transposed, ltm);
  thunk_FUN_004fc616(&transposed, &transposed);
  RwD3D9SetTransform(D3DTS_WORLD, &transposed);
  _rwD3D9EnableClippingIfNeeded(object, clip_flag);
  ltm = RwFrameGetLTM(*(RwFrame **)((char *)object + 4));
  d3d9_instance_render_dispatch_r(instance_arg, render_arg, (undefined4)ltm);
}

/* d3d9_instance_render_dispatch_r: C -> original d3d9_instance_render_dispatch (registers eax,s0,s1) */
__declspec(naked) int d3d9_instance_render_dispatch_r(int p0, undefined4 p1, undefined4 p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call d3d9_instance_render_dispatch
        add esp, 8
        ret
    }
}
