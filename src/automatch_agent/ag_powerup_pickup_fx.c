#include "mfl_tier2.h"
/*00464330*/ extern char *__cdecl powerup_icon_texture(undefined4 p0);
/*0049ff00*/ extern undefined __cdecl particle_emit_textured(float *floats_a, undefined4 *words, int p2, int p3, struct RwTexture *texture, float value, float *floats_b);


/* powerup_pickup_fx */

/* spawns 10 icon particles of the powerup type at the pick-up position */

void powerup_pickup_fx(char *powerup)

{
  RwTexture *icon_texture;
  int particles_left;
  float fVar2;
  float *pfVar3;
  undefined1 color_rgba [4];
  
  color_rgba[0] = 0xff;
  color_rgba[1] = 0;
  color_rgba[2] = 0;
  color_rgba[3] = 0xff;
  particles_left = 10;
  do {
    pfVar3 = (float *)0x0;
    fVar2 = 1.0;
    particle_emit_textured((float *)(powerup + 0x38),(undefined4 *)color_rgba,0,0,
                           (RwTexture *)powerup_icon_texture(*(undefined4 *)(powerup + 0x24)),fVar2,pfVar3);
    particles_left = particles_left + -1;
  } while (particles_left != 0);
  return;
}
