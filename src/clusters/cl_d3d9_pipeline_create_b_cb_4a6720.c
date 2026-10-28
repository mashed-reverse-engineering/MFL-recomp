// cluster-entries: d3d9_pipeline_create_b_cb_4a6720
/* Installed cluster d3d9_pipeline_create_b_cb_4a6720 (tools/cluster_install.py from output/cluster_pilot_kb/d3d9_pipeline_create_b_cb_4a6720.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004db470*/ extern RwBool RwD3D9SetTransform(RwUInt32 p0, void *p1);
/*004fae50*/ extern void _rwD3D9EnableClippingIfNeeded(void *p0, RwUInt32 p1);
/* d3d9_pipeline_create_b_cb_4a6720 */

/* function pointer pushed/stored by d3d9_pipeline_create_b (PUSH 0x4a6720) */

static int d3d9_instance_render_dispatch_r(int p0, undefined4 p1, undefined4 p2);
extern void d3d9_instance_render_dispatch(void);
#define D3DTS_WORLD (0x100)
/* Render callback: sets the D3D world transform to identity (4x4 world_m* block), builds an identity RwMatrix (ident_*, flags = identity|orthonormal) and calls d3d9_instance_render_dispatch with it. Parameter roles are inferred from the callees. */
void d3d9_pipeline_create_b_cb_4a6720(undefined4 instance_info, void *clip_arg, byte clip_flags, undefined4 dispatch_flags)

{
  struct {
    undefined4 world_m00;
    undefined4 uStack_7c;
    undefined4 uStack_78;
    undefined4 uStack_74;
    undefined4 uStack_70;
    undefined4 world_m11;
    undefined4 uStack_68;
    undefined4 uStack_64;
    undefined4 uStack_60;
    undefined4 uStack_5c;
    undefined4 world_m22;
    undefined4 uStack_54;
    undefined4 uStack_50;
    undefined4 uStack_4c;
    undefined4 uStack_48;
    undefined4 world_m33;
    undefined4 ident_right_x;
    undefined4 uStack_3c;
    undefined4 uStack_38;
    uint ident_flags;
    undefined4 uStack_30;
    undefined4 ident_up_y;
    undefined4 uStack_28;
    undefined1 pad_24[4];
    undefined4 uStack_20;
    undefined4 uStack_1c;
    undefined4 ident_at_z;
    undefined1 pad_14[4];
    undefined4 uStack_10;
    undefined4 uStack_c;
    undefined4 uStack_8;
  } frame;
  
  frame.ident_flags = frame.ident_flags | (rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL);
  frame.ident_at_z = 0x3f800000;
  frame.ident_up_y = 0x3f800000;
  frame.ident_right_x = 0x3f800000;
  frame.uStack_30 = 0;
  frame.uStack_38 = 0;
  frame.uStack_3c = 0;
  frame.uStack_1c = 0;
  frame.uStack_20 = 0;
  frame.uStack_28 = 0;
  frame.uStack_8 = 0;
  frame.uStack_c = 0;
  frame.uStack_10 = 0;
  frame.uStack_48 = 0;
  frame.uStack_4c = 0;
  frame.uStack_50 = 0;
  frame.uStack_54 = 0;
  frame.uStack_5c = 0;
  frame.uStack_60 = 0;
  frame.uStack_64 = 0;
  frame.uStack_68 = 0;
  frame.uStack_70 = 0;
  frame.uStack_74 = 0;
  frame.uStack_78 = 0;
  frame.uStack_7c = 0;
  frame.world_m33 = 0x3f800000;
  frame.world_m22 = 0x3f800000;
  frame.world_m11 = 0x3f800000;
  frame.world_m00 = 0x3f800000;
  RwD3D9SetTransform(D3DTS_WORLD,&frame.world_m00);
  _rwD3D9EnableClippingIfNeeded(clip_arg,(uint)clip_flags);
  d3d9_instance_render_dispatch_r(instance_info,dispatch_flags,&frame.ident_right_x);
  return;
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
