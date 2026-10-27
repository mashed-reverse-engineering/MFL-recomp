#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0041be30*/ extern int hudpanel_values_init();
/*00486930*/ extern int ptank_set_size();
/*00486a60*/ extern int ptank_set_colour();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006586e8*/ extern undefined g_load_6586e8;
#define PTANK_BUFFER_SIZE (0x440)
#define SMOKE_PTANK_TYPE (0x895)
#define SMOKE_PARTICLE_COUNT (0x40)
/*
 * load_smoke_texture: sets up the smoke particle tank g_load_6586e8.
 * Zeroes the tank buffer, attaches the "smoke" texture from the texture
 * dictionary, sets the tank size to 0.35 (inferred), sets colour 0 (inferred)
 * and then initialises the HUD panel values.
 * Takes no parameters and returns nothing.
 */
void load_smoke_texture(void)
{
  RwTexture *smoke_texture;
  struct {
    float width;
    float height;
  } smoke_size;

  smoke_size.width = 0.35f;
  smoke_size.height = 0.35f;
  mem_zero(&g_load_6586e8,PTANK_BUFFER_SIZE);
  smoke_texture = (RwTexture *)find_texture_dict_b("smoke");
  particle_system_setup_ptank((uint *)&g_load_6586e8,SMOKE_PTANK_TYPE,SMOKE_PARTICLE_COUNT,smoke_texture);
  ptank_set_size(&g_load_6586e8,&smoke_size.width);
  ptank_set_colour(&g_load_6586e8,0);
  hudpanel_values_init();
  return;
}
