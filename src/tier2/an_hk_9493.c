#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049c260*/ extern undefined __cdecl fx_slot_table_clear(undefined4 p0);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern int RpPTankAtomicCreate();
/*0075cda8*/ extern undefined g_particles_75cda8;
/*0075cdcc*/ extern undefined g_particles_75cdcc;
/*0075cddc*/ extern RwTexture *g_particles_75cddc;
/*0075cdec*/ extern undefined g_particles_75cdec;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/* fx_snow_ptank_create */

/* "creates the 0x200 snow/rain ptank atomic of system n with its bounding sphere and texture" */

void fx_snow_ptank_create(int param_1,RwTexture *param_2)

{
  uint *puVar1;
  RwTexture *texture;
  RwFrame *frame;
  void *pvVar2;
  int iVar3;
  int iVar4;
  struct {
    undefined1 *puStack_30;
    int iStack_2c;
    undefined4 local_28;
    undefined4 local_24;
    undefined4 local_20;
    undefined4 local_1c;
    undefined4 local_18;
    undefined4 local_14;
    undefined4 local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame_obj;
  
  frame = RwFrameCreate();
  frame_obj.local_10 = 0;
  frame_obj.local_c = 0;
  frame_obj.local_8 = 0x3f800000;
  frame_obj.local_4 = 0x3f800000;
  frame_obj.local_28 = 0x3dcccccd;
  frame_obj.local_24 = 0x3dcccccd;
  if (*(int *)((char *)&g_particles_75cdec + param_1 * 4) == 0) {
    pvVar2 = RwMalloc(0x4000,0);
    *(void **)((char *)&g_particles_75cdec + param_1 * 4) = pvVar2;
  }
  if (*(int *)((char *)&g_particles_75cdcc + param_1 * 4) == 0) {
    frame_obj.local_20 = 0;
    frame_obj.local_1c = 0;
    frame_obj.local_18 = 0;
    frame_obj.local_14 = 0x447a0000;
    g_particles_75cddc = param_2;
    iVar3 = RpPTankAtomicCreate(0x200,0x80007,0);
    iVar4 = g_ptank_plugin_offset;
    *(int *)((char *)&g_particles_75cdcc + param_1 * 4) = iVar3;
    *(undefined4 *)(*(int *)(iVar3 + iVar4) + 0xa4) = 5;
    *(undefined4 *)
     (*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4)) + 0xa8) = 6;
    puVar1 = (uint *)(*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4))
                     + 0x40);
    *puVar1 = *puVar1 | 0x10000000;
    puVar1 = (uint *)(*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4))
                     + 0x40);
    *puVar1 = *puVar1 | 0x800000;
    *(undefined4 *)
     (*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4)) + 4) = 0x200;
    RpAtomicSetFrame(*(RpAtomic **)((char *)&g_particles_75cdcc + param_1 * 4),frame);
    iVar4 = *(int *)(*(int *)(*(int *)((char *)&g_particles_75cdcc + param_1 * 4) + 0x18) + 0x5c);
    *(undefined4 *)(iVar4 + 4) = frame_obj.local_20;
    *(undefined4 *)(iVar4 + 8) = frame_obj.local_1c;
    *(undefined4 *)(iVar4 + 0xc) = frame_obj.local_18;
    *(undefined4 *)(iVar4 + 0x10) = frame_obj.local_14;
    *(undefined4 *)
     (*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4)) + 0xac) = 1;
    iVar4 = *(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4));
    *(undefined4 *)(iVar4 + 0xd8) = frame_obj.local_10;
    *(undefined4 *)(iVar4 + 0xdc) = frame_obj.local_c;
    *(undefined4 *)(iVar4 + 0xe0) = frame_obj.local_8;
    *(undefined4 *)(iVar4 + 0xe4) = frame_obj.local_4;
    *(uint *)(*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4)) + 0x40)
         = *(uint *)(*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4)) +
                    0x40) | 0x80000;
    iVar4 = *(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4));
    *(undefined4 *)(iVar4 + 0xb8) = frame_obj.local_28;
    *(undefined4 *)(iVar4 + 0xbc) = frame_obj.local_24;
    puVar1 = (uint *)(*(int *)(g_ptank_plugin_offset + *(int *)((char *)&g_particles_75cdcc + param_1 * 4))
                     + 0x40);
    *puVar1 = *puVar1 | 0x4000;
    texture = g_particles_75cddc;
    if (g_particles_75cddc != (RwTexture *)0x0) {
      *(undefined1 *)&RwTexture_filterAddressing(g_particles_75cddc) = 2;
      RpMaterialSetTexture
                ((RpMaterial *)
                 **(undefined4 **)
                   (*(int *)(*(int *)((char *)&g_particles_75cdcc + param_1 * 4) + 0x18) + 0x20),texture);
    }
    RpPTankAtomicLock_00545d10
              (*(undefined4 *)((char *)&g_particles_75cdcc + param_1 * 4),&frame_obj.puStack_30,2,0x40000000);
    iVar4 = 0x200;
    do {
      *frame_obj.puStack_30 = 0xff;
      frame_obj.puStack_30[1] = 0xff;
      frame_obj.puStack_30[2] = 0xff;
      frame_obj.puStack_30[3] = 0xff;
      frame_obj.puStack_30 = frame_obj.puStack_30 + frame_obj.iStack_2c;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  *(undefined4 *)((char *)&g_particles_75cda8 + param_1 * 4) = 0;
  fx_slot_table_clear(param_1);
  return;
}


