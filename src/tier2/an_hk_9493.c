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
#define SNOW_PTANK_PARTICLES (0x200) /* ptank particle count (inferred) */
#define SNOW_PTANK_BUFFER_BYTES (0x4000)

/* "creates the 0x200 snow/rain ptank atomic of system n with its bounding sphere and texture" */

void fx_snow_ptank_create(int system,RwTexture *texture_in)

{
  uint *ptank_flags;
  RwTexture *texture;
  RwFrame *frame;
  void *buffer;
  int atomic;
  int plugin_ptr;
  struct {
    undefined1 *puStack_30;
    int iStack_2c;
    undefined4 size_x;
    undefined4 size_y;
    undefined4 sphere_x;
    undefined4 sphere_y;
    undefined4 sphere_z;
    undefined4 sphere_radius;
    undefined4 uv_u0;
    undefined4 uv_v0;
    undefined4 uv_u1;
    undefined4 uv_v1;
  } frame_obj;
  
  frame = RwFrameCreate();
  frame_obj.uv_u0 = 0;
  frame_obj.uv_v0 = 0;
  frame_obj.uv_u1 = 0x3f800000;
  frame_obj.uv_v1 = 0x3f800000;
  frame_obj.size_x = 0x3dcccccd;
  frame_obj.size_y = 0x3dcccccd;
  if (((int *)&g_particles_75cdec)[system] == 0) {
    buffer = RwMalloc(SNOW_PTANK_BUFFER_BYTES,0);
    *(void **)((char *)&g_particles_75cdec + system * 4) = buffer;
  }
  if (((int *)&g_particles_75cdcc)[system] == 0) {
    frame_obj.sphere_x = 0;
    frame_obj.sphere_y = 0;
    frame_obj.sphere_z = 0;
    frame_obj.sphere_radius = 0x447a0000;
    g_particles_75cddc = texture_in;
    atomic = RpPTankAtomicCreate(SNOW_PTANK_PARTICLES,0x80007,0);
    plugin_ptr = g_ptank_plugin_offset;
    ((int *)&g_particles_75cdcc)[system] = atomic;
    *(undefined4 *)(*(char **)(atomic + plugin_ptr) + 0xa4) = 5;
    *(undefined4 *)
     (*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4)) + 0xa8) = 6;
    ptank_flags = (uint *)(*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4))
                     + 0x40);
    *ptank_flags = *ptank_flags | 0x10000000;
    ptank_flags = (uint *)(*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4))
                     + 0x40);
    *ptank_flags = *ptank_flags | 0x800000;
    *(undefined4 *)
     (*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4)) + 4) = SNOW_PTANK_PARTICLES;
    RpAtomicSetFrame(*(RpAtomic **)((char *)&g_particles_75cdcc + system * 4),frame);
    plugin_ptr = *(int *)(*(char **)(*(char **)((char *)&g_particles_75cdcc + system * 4) + 0x18) + 0x5c);
    *(undefined4 *)(plugin_ptr + 4) = frame_obj.sphere_x;
    *(undefined4 *)(plugin_ptr + 8) = frame_obj.sphere_y;
    *(undefined4 *)(plugin_ptr + 0xc) = frame_obj.sphere_z;
    *(undefined4 *)(plugin_ptr + 0x10) = frame_obj.sphere_radius;
    *(undefined4 *)
     (*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4)) + 0xac) = 1;
    plugin_ptr = *(int *)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4));
    *(undefined4 *)(plugin_ptr + 0xd8) = frame_obj.uv_u0;
    *(undefined4 *)(plugin_ptr + 0xdc) = frame_obj.uv_v0;
    *(undefined4 *)(plugin_ptr + 0xe0) = frame_obj.uv_u1;
    *(undefined4 *)(plugin_ptr + 0xe4) = frame_obj.uv_v1;
    *(uint *)(*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4)) + 0x40)
         = *(uint *)(*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4)) +
                    0x40) | 0x80000;
    plugin_ptr = *(int *)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4));
    *(undefined4 *)(plugin_ptr + 0xb8) = frame_obj.size_x;
    *(undefined4 *)(plugin_ptr + 0xbc) = frame_obj.size_y;
    ptank_flags = (uint *)(*(char **)(g_ptank_plugin_offset + *(char **)((char *)&g_particles_75cdcc + system * 4))
                     + 0x40);
    *ptank_flags = *ptank_flags | 0x4000;
    texture = g_particles_75cddc;
    if (g_particles_75cddc != (RwTexture *)0x0) {
      *(undefined1 *)&RwTexture_filterAddressing(g_particles_75cddc) = 2;
      RpMaterialSetTexture
                ((RpMaterial *)
                 **(undefined4 **)
                   (*(char **)(*(char **)((char *)&g_particles_75cdcc + system * 4) + 0x18) + 0x20),texture);
    }
    RpPTankAtomicLock_00545d10
              (((undefined4 *)&g_particles_75cdcc)[system],&frame_obj.puStack_30,2,0x40000000);
    plugin_ptr = SNOW_PTANK_PARTICLES;
    do {
      *frame_obj.puStack_30 = 0xff;
      frame_obj.puStack_30[1] = 0xff;
      frame_obj.puStack_30[2] = 0xff;
      frame_obj.puStack_30[3] = 0xff;
      frame_obj.puStack_30 = frame_obj.puStack_30 + frame_obj.iStack_2c;
      plugin_ptr = plugin_ptr + -1;
    } while (plugin_ptr != 0);
  }
  ((undefined4 *)&g_particles_75cda8)[system] = 0;
  fx_slot_table_clear(system);
  return;
}


