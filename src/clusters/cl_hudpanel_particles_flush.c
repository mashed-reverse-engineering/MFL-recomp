// cluster-entries: hudpanel_particles_flush
/* Installed cluster hudpanel_particles_flush (tools/cluster_install.py from output/cluster_pilot_zy/hudpanel_particles_flush.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00658a28*/ extern float g_particles_658a28[64];
/*00658728*/ extern char g_hudpanel_particles_658728[];
/*00603ed0*/ extern unsigned int g_hudpanel_particles_603ed0;
/*006586e8*/ extern char g_load_6586e8[];
/*007ef7f4*/ extern float g_tick_seconds;
/*005dda24*/ extern float k_f_0_75;
/*005de280*/ extern float k_f_1_3333334;
/*005de27c*/ extern float k_f_1_5707964;
/*005dd330*/ extern float float_1_0;
/*004867f0*/ extern void particles_set_position(char *position);
/*00486840*/ extern void particles_set_color(unsigned int *rgba);
/*00486880*/ extern void gfx_set_gfx_6c50c8(float angle);
/*00486b50*/ extern void particles_add(char *system);
/*00486c40*/ extern void particles_flush(char *system);

/* Emits one HUD particle for each of the 64 slots whose age (g_particles_658a28[i]) is <= 0.75 -- position slot i (12 bytes), colour g_hudpanel_particles_603ed0 with the alpha faded by the age, rotation age * pi/2 * 1.3333 -- then ages the slot by the tick time and flushes the batch. */
void hudpanel_particles_flush(void)
{
  float *age = g_particles_658a28;
  char *position = g_hudpanel_particles_658728;
  unsigned int colour;
  float scaled;

  do {
    if (*age <= k_f_0_75) {
      scaled = *age * k_f_1_3333334;
      colour = g_hudpanel_particles_603ed0;
      colour = (colour & 0xffffff) | ((unsigned int)(int)((float_1_0 - scaled) * (int)(g_hudpanel_particles_603ed0 >> 0x18)) << 0x18);
      particles_set_position(position);
      particles_set_color(&colour);
      gfx_set_gfx_6c50c8(scaled * k_f_1_5707964);
      particles_add(g_load_6586e8);
      *age = g_tick_seconds + *age;
    }
    position = position + 12;
    age = age + 1;
  } while ((float *)position < g_particles_658a28);
  particles_flush(g_load_6586e8);
}

