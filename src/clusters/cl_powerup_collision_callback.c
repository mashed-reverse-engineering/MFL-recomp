// cluster-entries: powerup_collision_callback
/* Installed cluster powerup_collision_callback (tools/cluster_install.py from output/cluster_pilot_kg/powerup_collision_callback.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00406cf0*/ extern int fx_objects_all_exploded(void);
/*004089e0*/ extern int __cdecl fx_object_get_rwp_id(int index);
/*0040fdc0*/ extern int race_get_phase(void);
/*004645c0*/ extern void __cdecl powerup_pickup_fx(int *powerup);
/*00467ed0*/ extern void __cdecl player_equip_weapon_by_index(int car, int weapon);
/*004682a0*/ extern int __cdecl player_has_weapon(int car);
/*004726a0*/ extern void __cdecl sfx_play_at(int sound, int *position);
/*00480060*/ extern void __cdecl hud_label_add_world_text(int text, int *position, int *colour_entry, void *colour, float height, float scale);
/*004980c0*/ extern void __cdecl fx_trail_particle_spawn_interpolated(void *particle, int a, int b, int c);
/*004c45e0*/ extern double FUN_004c45e0(float lo, float hi);
/*005dd324*/ extern float k_f_1_7;
/*005de200*/ extern float k_f_0_85;
/*005df838*/ extern float k_f_1;
/*0060da68*/ extern int g_powerup_collision_60da68[];
/*0060dac0*/ extern int g_powerup_60dac0[];
/*007ef7b8*/ extern int game_mode;
/* powerup_collision_callback */

#define POWERUP_KIND_PICKUP 7       /* trigger kind of a powerup sphere (trigger_obj + 0x10) */
#define POWERUP_STATE_ACTIVE 1
#define POWERUP_STATE_TAKEN 2
#define RACE_PHASE_RUNNING 6
#define GAME_MODE_5 5
#define PICKUP_SOUND 0x1c
#define PICKUP_SPARKS 5

/* the push velocity of a powerup record that a car bumps into (+0x44, behind spawn_position) */
#define POWERUP_VELOCITY(p) ((int *)((char *)(p) + 0x44))

typedef struct TrailParticle {
  int pos[3];
  int colour;
  float size[2];
  float velocity[3];
  float gravity[3];
  float scale;
  int _gap_34;
  float life;
  int _gap_3c;
  int flags;
  char _gap_44;
  unsigned char kind;
  char _gap_46[2];
} TrailParticle;

/* callback of the powerup trigger spheres: a car (collider id 0xff00 + n) touching an active powerup takes it: the weapon is equipped, a label flies up and sparks are spawned;
   during the race a car that already has a weapon only pushes the powerup away */
static double powerup_collision_vector_to_car_r(int p0, float * p1, int * p2);
extern void powerup_collision_vector_to_car(void);
void powerup_collision_callback(int trigger_obj, int collider_obj)

{
  struct {
    float push[3];
    TrailParticle particle;
  } frame;
  Powerup *powerup;
  int collider;
  int car;
  int spark;

  if (*(int *)(trigger_obj + 0x10) == POWERUP_KIND_PICKUP) {
    powerup = *(Powerup **)(trigger_obj + 0x14);
    if (powerup->state == POWERUP_STATE_ACTIVE) {
      if ((game_mode == GAME_MODE_5) && (fx_objects_all_exploded() != 0) && (*(int *)(collider_obj + 0x10) == 10) &&
          (*(int *)(collider_obj + 0x10) == fx_object_get_rwp_id(0))) {
        powerup->state = POWERUP_STATE_TAKEN;
        powerup->respawn_timer = powerup->respawn_time - k_f_1;
        return;
      }
      collider = *(int *)(collider_obj + 0x10);
      if ((collider > 0xfeff) && (collider < 0xff08)) {
        car = collider - CAR_ID_BASE;
        if (race_get_phase() != RACE_PHASE_RUNNING) {
          powerup->state = POWERUP_STATE_TAKEN;
          powerup->respawn_timer = powerup->respawn_time - k_f_1;
          return;
        }
        if (player_has_weapon(car) != 0) {
          powerup_collision_vector_to_car_r(car, frame.push, (int *)powerup);
          POWERUP_VELOCITY(powerup)[0] = 0;
          POWERUP_VELOCITY(powerup)[1] = 0x3d23d70a;
          POWERUP_VELOCITY(powerup)[2] = 0;
          return;
        }
        powerup->state = POWERUP_STATE_TAKEN;
        powerup->respawn_timer = powerup->respawn_time;
        powerup_pickup_fx((int *)powerup);
        player_equip_weapon_by_index(car, powerup->type);
        sfx_play_at(PICKUP_SOUND, ((int *)powerup->spawn_position));
        ((unsigned char *)&trigger_obj)[0] = 0xff;
        ((unsigned char *)&trigger_obj)[1] = 0xff;
        ((unsigned char *)&trigger_obj)[2] = 0;
        ((unsigned char *)&trigger_obj)[3] = 0;
        hud_label_add_world_text(g_powerup_collision_60da68[powerup->type], ((int *)powerup->spawn_position), &g_powerup_60dac0[powerup->type], &trigger_obj, 32.0f, 2.0f);
        spark = PICKUP_SPARKS;
        do {
          frame.particle.pos[0] = ((int *)powerup->spawn_position)[0];
          frame.particle.pos[1] = ((int *)powerup->spawn_position)[1];
          frame.particle.pos[2] = ((int *)powerup->spawn_position)[2];
          spark = spark - 1;
          frame.particle.colour = g_powerup_60dac0[powerup->type];
          frame.particle.life = 1.3f;
          frame.particle.scale = 1.0f;
          frame.particle.gravity[0] = 0.0f;
          frame.particle.gravity[1] = -3.0f;
          frame.particle.gravity[2] = 0.0f;
          frame.particle.flags = 0x1102;
          if (spark % 2 == 0) {
            frame.particle.flags = 0x1902;
          }
          frame.particle.size[0] = 2.0f;
          frame.particle.size[1] = 2.0f;
          frame.particle.kind = 3;
          frame.particle.velocity[0] = (float)(FUN_004c45e0(-1.0f, 1.0f) * k_f_0_85);
          frame.particle.velocity[1] = (float)(k_f_1_7 - FUN_004c45e0(0.0f, 1.0f));
          frame.particle.velocity[2] = (float)(FUN_004c45e0(-1.0f, 1.0f) * k_f_0_85);
          fx_trail_particle_spawn_interpolated(&frame.particle, 0, 0, 0);
        } while (spark != 0);
      }
    }
  }
  return;
}

/* powerup_collision_vector_to_car_r: C -> original powerup_collision_vector_to_car (registers eax,esi,s0) */
__declspec(naked) double powerup_collision_vector_to_car_r(int p0, float * p1, int * p2)
{
    __asm {
        push esi
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 12]
        mov esi, dword ptr [esp + 16]
        call powerup_collision_vector_to_car
        add esp, 4
        pop esi
        ret
    }
}
