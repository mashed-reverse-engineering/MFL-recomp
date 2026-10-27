// cluster-entries: d3d9_pipeline_create_b_cb_4a6720
/* Installed cluster d3d9_pipeline_create_b_cb_4a6720 (tools/cluster_install.py from output/cluster_pilot_kb/d3d9_pipeline_create_b_cb_4a6720.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004db470*/ extern RwBool RwD3D9SetTransform(RwUInt32 p0, void *p1);
/*004fae50*/ extern void _rwD3D9EnableClippingIfNeeded(void *p0, RwUInt32 p1);
/* d3d9_pipeline_create_b_cb_4a6720 */

/* function pointer pushed/stored by d3d9_pipeline_create_b (PUSH 0x4a6720) */

static int d3d9_instance_render_dispatch_r(int p0, undefined4 p1, undefined4 p2);
extern void d3d9_instance_render_dispatch(void);
void d3d9_pipeline_create_b_cb_4a6720(undefined4 param_1, void *param_2, byte param_3, undefined4 param_4)

{
  struct {
    undefined4 uStack_80;
    undefined4 uStack_7c;
    undefined4 uStack_78;
    undefined4 uStack_74;
    undefined4 uStack_70;
    undefined4 uStack_6c;
    undefined4 uStack_68;
    undefined4 uStack_64;
    undefined4 uStack_60;
    undefined4 uStack_5c;
    undefined4 uStack_58;
    undefined4 uStack_54;
    undefined4 uStack_50;
    undefined4 uStack_4c;
    undefined4 uStack_48;
    undefined4 uStack_44;
    undefined4 uStack_40;
    undefined4 uStack_3c;
    undefined4 uStack_38;
    uint uStack_34;
    undefined4 uStack_30;
    undefined4 uStack_2c;
    undefined4 uStack_28;
    undefined1 pad_24[4];
    undefined4 uStack_20;
    undefined4 uStack_1c;
    undefined4 uStack_18;
    undefined1 pad_14[4];
    undefined4 uStack_10;
    undefined4 uStack_c;
    undefined4 uStack_8;
  } frame;
  
  frame.uStack_34 = frame.uStack_34 | 0x20003;
  frame.uStack_18 = 0x3f800000;
  frame.uStack_2c = 0x3f800000;
  frame.uStack_40 = 0x3f800000;
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
  frame.uStack_44 = 0x3f800000;
  frame.uStack_58 = 0x3f800000;
  frame.uStack_6c = 0x3f800000;
  frame.uStack_80 = 0x3f800000;
  RwD3D9SetTransform(0x100,&frame.uStack_80);
  _rwD3D9EnableClippingIfNeeded(param_2,(uint)param_3);
  d3d9_instance_render_dispatch_r(param_1,param_4,&frame.uStack_40);
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
