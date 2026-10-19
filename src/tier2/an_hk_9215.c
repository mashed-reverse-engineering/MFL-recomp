#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type();
/*00437f30*/ extern int ui_get_car_69d2d8();
/*00463310*/ extern int weapon_effects_any_active();
/*00464620*/ extern int powerup_collision_callback();
/*00464b30*/ extern int powerup_random_weapon_type();
/*00464c00*/ extern int weapon_powerup_set_icon_texture();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8(float v);
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*00496850*/ extern int trigger_sphere_register(float *param_1);
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*005de1f8*/ extern float k_f_3_1415927;
/*005de9c4*/ extern float k_f_0_017453292;
/*005df830*/ extern float g_powerups_update_5df830;
/*005df834*/ extern float g_powerups_update_5df834;
/*005df838*/ extern float k_f_1;
/*0060dac0*/ extern undefined g_powerup_60dac0;
/*006a8bc0*/ extern struct Powerup g_powerups[25];
/*006a9390*/ extern undefined g_powerup_glow;
/*006a93d0*/ extern undefined4 g_powerup_count;
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);
static int powerup_move_and_spin_r(int * reg_edi);
extern void powerup_move_and_spin(void);
__declspec(naked) int powerup_move_and_spin_r(int * reg_edi)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call powerup_move_and_spin
        pop edi
        ret
    }
}

/* powerups_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per frame: glow particles for available ones; for taken ones the respawn countdown (state 2 ->
   1, random type for special slots)" */

void powerups_update(void)

{
  int i;
  int type;
  int ok;
  float10 diff;
  float d2;
  float t;
  float10 sn;
  struct Powerup *p;
  int *parent;
  float size_a[2];
  float size_b[2];
  struct {
    float x, y, z, radius;
    int kind;
    struct Powerup *owner;
    int zero;
    void *callback;
  } rec;

  i = g_powerup_count;
  if (i != 0) {
    do {
      i = i - 1;
      p = &g_powerups[i];
      type = p->type;
      if (type == 21) {
        p->state = 0;
      }
      switch (p->state) {
      case 0:
        break;
      case 2:
        if (p->respawn_timer > k_f_0) {
          diff = (float10)p->respawn_time - (float10)p->respawn_timer;
          p->respawn_timer = p->respawn_timer - g_tick_seconds;
          if (diff < k_f_1) {
            sn = sin((diff / (float10)k_f_1) * (float10)k_f_3_1415927);
            size_b[0] = (float)((float10)g_powerups_update_5df830 * sn);
            size_b[1] = (float)((float10)g_powerups_update_5df834 * sn);
            particles_set_color((char *)&g_powerup_60dac0 + type * 4);
            particles_set_position(p->spawn_position);
            particles_set_size(size_b);
            gfx_set_gfx_6c50c8(0);
            particles_add(&g_powerup_glow);
          }
        }
        else {
          ok = 1;
          if ((type == 19) && (weapon_effects_any_active() == 1)) {
            ok = 0;
          }
          if ((get_event_type() != 0x11) || (ui_get_car_69d2d8() == 0)) {
            if (ok != 0) {
              p->respawn_timer = p->respawn_time;
              p->state = 1;
              if (p->special != 0) {
                p->type = powerup_random_weapon_type();
                weapon_powerup_set_icon_texture(p);
              }
            }
          }
        }
        break;
      case 1:
        parent = *(int **)(p->clump + 4);
        d2 = (p->position[0] - p->spawn_position[0]) * (p->position[0] - p->spawn_position[0]) +
             (p->position[1] - p->spawn_position[1]) * (p->position[1] - p->spawn_position[1]) +
             (p->position[2] - p->spawn_position[2]) * (p->position[2] - p->spawn_position[2]);
        if (d2 <= k_f_0) {
          size_a[0] = g_powerups_update_5df830;
          size_a[1] = g_powerups_update_5df834;
        }
        else {
          t = float_1_0 - d2;
          if (t <= k_f_0) {
            t = k_f_0;
          }
          else if (t >= float_1_0) {
            t = float_1_0;
          }
          size_a[0] = g_powerups_update_5df830 * t;
          size_a[1] = g_powerups_update_5df834 * t;
        }
        powerup_move_and_spin_r((int *)p);
        particles_set_color((char *)&g_powerup_60dac0 + p->type * 4);
        particles_set_position((char *)parent + 0x40);
        particles_set_size(size_a);
        gfx_set_gfx_6c50c8(*(float *)(p->_gap_4) * k_f_0_017453292);
        particles_add(&g_powerup_glow);
        rec.x = *(float *)((char *)parent + 0x40);
        rec.y = *(float *)((char *)parent + 0x44);
        rec.z = *(float *)((char *)parent + 0x48);
        rec.radius = 0.5f;
        rec.kind = 7;
        rec.owner = p;
        rec.zero = 0;
        rec.callback = (void *)powerup_collision_callback;
        trigger_sphere_register(&rec.x);
        break;
      default:
        break;
      }
    } while (i != 0);
  }
  particles_flush(&g_powerup_glow);
  return;
}
