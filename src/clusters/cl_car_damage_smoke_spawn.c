// cluster-entries: car_damage_smoke_spawn
/* Installed cluster car_damage_smoke_spawn (tools/cluster_install.py from output/cluster_pilot_pg2/car_damage_smoke_spawn.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double __cdecl random_range(float low, float high);
/*00482010*/ extern int __cdecl random_int_range(int low, int high);

#define SMOKE_PUFF_COUNT 128

/* one puff of the damage smoke ring (stride 0x2c) */
typedef struct SmokePuff {
  float width;                  /* 0x00 */
  float height;                 /* 0x04 */
  float position[3];            /* 0x08 */
  float angle;                  /* 0x14 */
  float angle_step;             /* 0x18 */
  int age;                      /* 0x1c */
  float fade_scale;             /* 0x20 */
  unsigned char colour[4];      /* 0x24: r g b a */
  int car;                      /* 0x28 */
} SmokePuff;
/*0065aac8*/ extern SmokePuff g_car_damage_65aac8[SMOKE_PUFF_COUNT];
/*0065a9d8*/ extern unsigned char g_car_damage_65a9d8[];       /* one bit per puff (the bit of the NEXT ring position is set) */
/*0065c0c8*/ extern int g_car_damage_65c0c8;                   /* next ring position */
/*005dd374*/ extern float k_f_0_2;
/*005dd370*/ extern float k_f_0_3;

#define SMOKE_ALPHA_JITTER 15

/* car_damage_smoke_spawn: adds one smoke puff at `position` for `car` to the ring of 128 puffs: random size, spin and fade, a colour from
   the car's damage (car_damage_smoke_choose) darkened by a random factor, alpha jittered by +-15 */
static void car_damage_smoke_choose_r(int p0, unsigned char * p1);
extern void car_damage_smoke_choose(void);
void car_damage_smoke_spawn(float *position, int car)
{
  SmokePuff *puff = &g_car_damage_65aac8[g_car_damage_65c0c8];
  double darken;
  int next;

  puff->age = 0;
  puff->position[0] = position[0];
  puff->position[1] = position[1];
  puff->position[2] = position[2];
  puff->car = car;
  darken = random_range(0.3f, 1.16f);
  car_damage_smoke_choose_r(car, puff->colour);
  puff->colour[0] = (unsigned char)(puff->colour[0] * darken);
  puff->colour[1] = (unsigned char)(puff->colour[1] * darken);
  puff->colour[2] = (unsigned char)(puff->colour[2] * darken);
  puff->colour[3] += (unsigned char)random_int_range(-SMOKE_ALPHA_JITTER, SMOKE_ALPHA_JITTER);
  puff->width = (float)(random_range(0.8f, 1.2f) * k_f_0_2);
  puff->height = (float)(random_range(0.8f, 1.2f) * k_f_0_3);
  puff->fade_scale = (float)random_range(1.1f, 1.5f);
  puff->angle_step = (float)random_range(-0.01f, 0.01f);
  puff->angle = (float)random_range(-3.1415927f, 3.1415927f);
  next = g_car_damage_65c0c8 + 1;
  g_car_damage_65c0c8 = next;
  g_car_damage_65a9d8[next >> 3] |= (unsigned char)(1 << (next & 7));
  if (next >= SMOKE_PUFF_COUNT) {
    g_car_damage_65c0c8 = 0;
  }
}

/* car_damage_smoke_choose_r: C -> original car_damage_smoke_choose (registers eax,ecx) */
__declspec(naked) void car_damage_smoke_choose_r(int p0, unsigned char * p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call car_damage_smoke_choose
        ret
    }
}
