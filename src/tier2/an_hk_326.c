#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0049b780*/ extern int __cdecl particles_alloc_system(struct RwTexture *p0, int p1, int p2, uint p3);
/*0049b860*/ extern int fx_set_blend_modes();
/*00751bc4*/ extern undefined g_race_load_751bc4;
/*00752364*/ extern undefined g_race_load_752364;

#define FX_SYSTEM_COUNT (4)

/* fx_explosion_systems_init */

/*
 * Sets up the explosion particle systems: for each of the FX_SYSTEM_COUNT (4) systems,
 * loads the "exp_cloud2" texture, allocates a particle system from it and sets its
 * blend modes (src alpha / one, inferred from the arguments). Each system gets a record
 * in the g_race_load globals (handle, index, and a cleared block of 5 fields per entry).
 * Takes no parameters and returns nothing.
 */

void fx_explosion_systems_init(void)

{
  RwTexture *texture_dict;
  undefined4 particle_system;
  undefined4 *cursor;
  undefined4 *system_record;
  int system_index;
  int count;
  int iVar6;
  uint uVar7;
  
  system_index = FX_SYSTEM_COUNT;
  cursor = (undefined4 *)&g_race_load_752364;
  /* one pass per explosion system; the loop ends when system_record reaches the end of the table */
  do {
    uVar7 = 0x37;
    iVar6 = 4;
    count = 0x1c;
    system_index = system_index + -1;
    system_record = cursor + -0x7a;
    texture_dict = (RwTexture *)find_texture_dict_b("exp_cloud2");
    particle_system = particles_alloc_system(texture_dict,count,iVar6,uVar7);
    cursor[-0xf2] = particle_system;
    fx_set_blend_modes(particle_system,rwBLENDSRCALPHA,rwBLENDONE);
    cursor[-0xf3] = system_index;
    *system_record = 0;
    count = 7;
    cursor = system_record;
    /* clear 7 entries of the system record (5 fields each, walking down the table) */
    do {
      count = count + -1;
      cursor[-0x16] = 0;
      cursor[-1] = 0;
      cursor[-0xf] = 0;
      cursor[-8] = 0;
      cursor[-0x1d] = 0;
      cursor = cursor + -1;
    } while (count != 0);
    cursor = system_record;
  } while (system_record != (undefined4 *)&g_race_load_751bc4);
  return;
}


