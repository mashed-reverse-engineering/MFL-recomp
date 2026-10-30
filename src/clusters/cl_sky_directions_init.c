// cluster-entries: sky_directions_init
/* Installed cluster sky_directions_init (tools/cluster_install.py from output/cluster_pilot_te5/sky_directions_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double __cdecl random_range(float low, float high);
/*004d5290*/ extern RwReal __cdecl RwV3dNormalize(RwV3d *out, const RwV3d *in);

#define SKY_DIRECTION_COUNT 256
#define PTANK_BLEND_SRC_OFFSET 0xa4
#define PTANK_BLEND_DST_OFFSET 0xa8
#define PTANK_FLAGS_OFFSET 0x40
#define PTANK_FLAG_SKY 0x10000000
#define PTANK_BLEND_SRC 5
#define PTANK_BLEND_DST 2

/* one random sky direction (16 bytes, the last word is unused) */
typedef struct SkyDirection {
  RwV3d direction;
  float unused;
} SkyDirection;
/*0069f6e0*/ extern SkyDirection g_sky_drift[SKY_DIRECTION_COUNT];
/*006a06e0*/ extern char *g_sky_atomic;
/*007da6bc*/ extern int g_ptank_plugin_offset;

/* sky_directions_init: fills the table of 256 random sky directions (random unit vector, each component then scaled by its own random factor 0.5 .. 1) and sets up the sky
   atomic's particle tank (blend factors 5 / 2, sky flag); the tail of course_sky_atomic_create */
void sky_directions_init(void)
{
  int i;
  SkyDirection *sky;
  char *tank;

  for (i = 0; i < SKY_DIRECTION_COUNT; i++) {
    sky = &g_sky_drift[i];
    sky->direction.x = (float)random_range(-1.0f, 1.0f);
    sky->direction.y = (float)random_range(-1.0f, 1.0f);
    sky->direction.z = (float)random_range(-1.0f, 1.0f);
    RwV3dNormalize(&sky->direction, &sky->direction);
    sky->direction.x = (float)random_range(0.5f, 1.0f) * sky->direction.x;
    sky->direction.y = (float)random_range(0.5f, 1.0f) * sky->direction.y;
    sky->direction.z = (float)random_range(0.5f, 1.0f) * sky->direction.z;
  }
  tank = *(char **)(g_sky_atomic + g_ptank_plugin_offset);
  *(int *)(tank + PTANK_BLEND_SRC_OFFSET) = PTANK_BLEND_SRC;
  *(int *)(tank + PTANK_BLEND_DST_OFFSET) = PTANK_BLEND_DST;
  *(unsigned int *)(tank + PTANK_FLAGS_OFFSET) |= PTANK_FLAG_SKY;
}

