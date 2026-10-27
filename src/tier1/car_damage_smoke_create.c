#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*004241b0*/ extern int car_damage_smoke_reset();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c6600*/ extern void FUN_004c6600(void *p0, undefined4 p1);
/*0065a9d8*/ extern undefined1 g_car_damage_65a9d8;
/*0065aa88*/ extern undefined g_car_damage_65aa88;
/* car_damage_smoke_create */
/* "smoke texture, particles_create(0x65AA88, 0x817, 0x80), reset tables" */
/*
 * Sets up the car damage smoke effect. Takes no parameters and returns nothing.
 * Looks up the "smoke" texture, sets up the particle system at g_car_damage_65aa88
 * with it, resets the car damage smoke state, then calls FUN_004c6600 on
 * g_car_damage_65a9d8 with 0x80 (meaning of 0x80 inferred).
 */
void car_damage_smoke_create(void)
{
  RwTexture *smoke_texture;

  smoke_texture = (RwTexture *)find_texture_dict_b("smoke");
  particle_system_setup_ptank((uint *)&g_car_damage_65aa88,0x817,0x80,smoke_texture);
  car_damage_smoke_reset();
  FUN_004c6600(&g_car_damage_65a9d8,0x80);
  return;
}
