#include "mfl_tier2.h"
/*004846b0*/ extern int alloc_zeroed();
/*00486930*/ extern int ptank_set_size();
/*00486980*/ extern undefined __cdecl particle_system_set_tint(void *p0, undefined4 p1);
/*004869e0*/ extern undefined __cdecl particle_system_set_matrix_table(void *p0, undefined4 p1);
/*00486a30*/ extern int particle_system_set_value_c0();
/*00486a60*/ extern int ptank_set_colour();
/*00486ab0*/ extern undefined __cdecl particle_system_set_colour_table(void *p0, undefined4 p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern int RpPTankAtomicCreate();
/*0062949c*/ extern undefined4 g_particle_system_62949c;
/*006294a0*/ extern undefined4 g_particle_system_6294a0;
/*006c5148*/ extern undefined4 g_gfx_6c5148;
/*006c514c*/ extern undefined4 g_gfx_6c514c;
/*006c5150*/ extern undefined4 g_particle_system_6c5150;
/*006c5154*/ extern undefined4 g_particle_system_6c5154;
/*006c5158*/ extern undefined4 g_particle_system_6c5158;
/*006c515c*/ extern undefined4 g_particle_system_6c515c;
/*006c5160*/ extern undefined4 g_particle_system_6c5160;
/*006c5168*/ extern undefined4 g_particle_system_6c5168;
/*006c516c*/ extern undefined4 g_particle_system_6c516c;
/*006c5170*/ extern undefined4 g_particle_system_6c5170;
/*006c5178*/ extern undefined4 g_particle_system_6c5178;
/*006c517c*/ extern undefined4 g_particle_system_6c517c;
/*006c5180*/ extern undefined4 g_particle_system_6c5180;
/*007da6bc*/ extern char *g_ptank_plugin_offset;

#define PS_HEADER_SIZE (0x40)
#define PTANK_DEFAULT_FLAGS (0x807)
#define PTANK_ATOMIC_BASE_FLAGS (0x10000000)
#define PTANK_FLAG_TINT (0x100)
#define PTANK_FEATURE_MATRIX_TABLE (0x200)
#define PTANK_FLAG_VALUE_C0 (0x400)
#define PTANK_FLAG_COLOUR (0x800)
#define PTANK_FEATURE_COLOUR_TABLE (0x1000)
/* particle_system_setup_ptank */

/* "RpPTankAtomicCreate with all blend / flag setters (26 callers)" */

/*
 * Sets up a particle-tank particle system in the caller-supplied block `particle_system`
 * (64 bytes, zeroed here). `flags` selects features (0 means PTANK_DEFAULT_FLAGS);
 * `particle_count` sizes the per-particle arrays; `texture` is applied to the atomic's material.
 * Creates the RpAtomic + frame, resets the global render state, and stores results in
 * particle_system[1] (atomic), [2] (count), [4..10] (optional arrays). Returns nothing.
 * (inferred: meanings of the individual flag bits, from the callees they gate)
 */
void particle_system_setup_ptank(uint *particle_system,uint flags,uint particle_count,RwTexture *texture)

{
  uint *plugin_flags;
  RwFrame *frame;
  uint result;
  
  /* Build the atomic create flags from the feature flags (inferred). */
  result = PTANK_ATOMIC_BASE_FLAGS;
  if (flags == 0) {
    flags = PTANK_DEFAULT_FLAGS;
  }
  if ((flags & 1) != 0) {
    result = 0x10000001;
  }
  if ((flags & 2) != 0) {
    result = result | 4;
  }
  if ((flags & 4) != 0) {
    result = result | 2;
  }
  if ((flags & 8) != 0) {
    result = result | 8;
  }
  if ((flags & 0x10) != 0) {
    result = result | 0x20;
  }
  if ((flags & 0x20) != 0) {
    result = result | 0x80;
  }
  if ((flags & 0x40) != 0) {
    result = result | 0x100;
  }
  if ((flags & 0x200) != 0) {
    result = result | 0x8000;
  }
  if ((flags & 0x400) != 0) {
    result = result | 0x20000;
  }
  if ((flags & 0x800) != 0) {
    result = result | 0x80000;
  }
  if ((flags & 0x1000) != 0) {
    result = result | 0x100000;
  }
  /* Zero the header, create the atomic, and reset the global render-state defaults. */
  mem_zero(particle_system,PS_HEADER_SIZE);
  result = RpPTankAtomicCreate(particle_count,result,0);
  particle_system[1] = result;
  g_particle_system_6c5154 = g_particle_system_6c5154 | 0x20003;
  g_particle_system_6c5170 = FLOAT_ONE_BITS;
  g_particle_system_6c515c = FLOAT_ONE_BITS;
  g_gfx_6c5148 = FLOAT_ONE_BITS;
  g_particle_system_6c5158 = 0;
  g_particle_system_6c5150 = 0;
  g_gfx_6c514c = 0;
  g_particle_system_6c516c = 0;
  g_particle_system_6c5168 = 0;
  g_particle_system_6c5160 = 0;
  g_particle_system_6c5180 = 0;
  g_particle_system_6c517c = 0;
  g_particle_system_6c5178 = 0;
  *particle_system = flags;
  /* Apply the optional setters gated by the feature flags, each with a zero argument. */
  if ((char)flags < '\0') {
    ptank_set_size(particle_system,0);
  }
  if ((flags & PTANK_FLAG_TINT) != 0) {
    particle_system_set_tint(particle_system,0);
  }
  if ((flags & PTANK_FEATURE_MATRIX_TABLE) != 0) {
    particle_system_set_matrix_table(particle_system,0);
  }
  if ((flags & PTANK_FLAG_VALUE_C0) != 0) {
    particle_system_set_value_c0(particle_system,0);
  }
  if ((flags & PTANK_FLAG_COLOUR) != 0) {
    ptank_set_colour(particle_system,0);
  }
  if ((flags & PTANK_FEATURE_COLOUR_TABLE) != 0) {
    particle_system_set_colour_table(particle_system,0);
  }
  RpMaterialSetTexture((RpMaterial *)**(undefined4 **)(*(char **)(particle_system[1] + 0x18) + 0x20),texture);
  *(undefined4 *)(*(char **)(particle_system[1] + g_ptank_plugin_offset) + 0xac) = 1;
  *(undefined4 *)(*(char **)(particle_system[1] + g_ptank_plugin_offset) + 0xa4) = g_particle_system_62949c;
  *(undefined4 *)(*(char **)(particle_system[1] + g_ptank_plugin_offset) + 0xa8) = g_particle_system_6294a0;
  plugin_flags = (uint *)(*(char **)(particle_system[1] + g_ptank_plugin_offset) + 0x40);
  *plugin_flags = *plugin_flags | 0x10000000;
  plugin_flags = (uint *)(*(char **)(particle_system[1] + g_ptank_plugin_offset) + 0x40);
  *plugin_flags = *plugin_flags | 0x800000;
  *(undefined4 *)(*(char **)(particle_system[1] + g_ptank_plugin_offset) + 4) = 0;
  frame = RwFrameCreate();
  RpAtomicSetFrame((RpAtomic *)particle_system[1],frame);
  particle_system[2] = particle_count;
  /* Allocate the optional per-particle arrays selected by the low flag bits (sizes inferred). */
  if ((flags & 1) != 0) {
    result = alloc_zeroed(particle_count,0xc);
    particle_system[4] = result;
  }
  if ((flags & 2) != 0) {
    result = alloc_zeroed(particle_count,8);
    particle_system[5] = result;
  }
  if ((flags & 4) != 0) {
    result = alloc_zeroed(particle_count,4);
    particle_system[6] = result;
  }
  if ((flags & 8) != 0) {
    result = alloc_zeroed(particle_count,0x40);
    particle_system[7] = result;
  }
  if ((flags & 0x10) != 0) {
    result = alloc_zeroed(particle_count,4);
    particle_system[8] = result;
  }
  if ((flags & 0x20) != 0) {
    result = alloc_zeroed(particle_count * 2,8);
    particle_system[9] = result;
  }
  if ((flags & 0x40) != 0) {
    result = alloc_zeroed(particle_count * 4,8);
    particle_system[10] = result;
  }
  particle_system[0xb] = 0;
  particle_system[0xc] = 0;
  return;
}


