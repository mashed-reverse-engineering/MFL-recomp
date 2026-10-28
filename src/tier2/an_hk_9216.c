#include "mfl_tier2.h"
/*00424760*/ extern undefined __cdecl car_damage_at_world_point(int car, float amount, void *trigger);
/*00434bb0*/ extern int get_event_type();
/*00437f60*/ extern int __cdecl screen_shake_add(int amount);
/*0048e3b0*/ extern int __cdecl rwp_object_set_user_data(int id, uint data);
/*0048e3d0*/ extern uint __cdecl rwp_object_get_user_data(int id);
/*00491ab0*/ extern int __cdecl car_enter_rwp_mode(int car);
/*00492b20*/ extern int __cdecl car_apply_rwp_impulse(int car, RwV3d *impulse, void *trigger);
/*004a1680*/ extern int __cdecl player_rumble(int player, int a, int b, int c);
/*004b2b48*/ extern int _ftol();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int world, void *owner, int v);
/*0056b220*/ extern void rwp_world_set_bit_60_64(int world, void *owner, int v);
/*0056b300*/ extern void FUN_0056b300(int world, void *owner);
/*0056bc20*/ extern void rwp_body_add_force_at_point(int body, RwV3d *impulse, void *trigger);
/*005dd330*/ extern float float_1_0;
/*005dd374*/ extern float k_f_0_2;
/*005ddc28*/ extern float k_f_0;
/* explosion_trigger_callback */

/* "(trigger, other): horizontal distance falloff 1 - d/radius, scaled by 1/frames; cars (id 0xFF00
   + n) get player_rumble, car_enter_rwp_mode, car_apply_rwp_impulse and
   car_damage_at_world_point(amount * falloff * entry.damage); physics objects (types 10 and 0x1A)
   get a push (doubled for 0x1A) through the RWP body" */

/* the trigger sphere record (see powerups_update) and the touching object */
typedef struct { float pos[3]; float radius; int kind; ExplosionEntry *entry; int zero; void *callback; } TrigSphere;
typedef struct { float pos[3]; int pad; int kind; char *owner; int obj_id; } TrigOther;
/* explosion_trigger_callback */

/* "(trigger, other): horizontal distance falloff 1 - d/radius, scaled by 1/frames; cars (id 0xFF00
   + n) get player_rumble, car_enter_rwp_mode, car_apply_rwp_impulse and
   car_damage_at_world_point(amount * falloff * entry.damage); physics objects (types 10 and 0x1A)
   get a push (doubled for 0x1A) through the RWP body" */

void explosion_trigger_callback(TrigSphere *trig,TrigOther *other)

{
  ExplosionEntry *entry;
  float falloff;
  int car;
  int obj_id;
  int world;
  char *owner;
  int body;
  RwReal len;
  RwV3d d;

  d.x = other->pos[0] - trig->pos[0];
  entry = trig->entry;
  d.z = other->pos[2] - trig->pos[2];
  d.y = 0.0;
  len = RwV3dLength(&d);
  if (trig->radius != k_f_0) {
    falloff = float_1_0 - len / trig->radius;
    if ((!(falloff < k_f_0)) && (!(falloff > float_1_0))) {
      falloff = (float_1_0 / (float)entry->frames) * falloff;
      if ((d.x != k_f_0) || (d.y != k_f_0) || (d.z != k_f_0)) {
        RwV3dNormalize(&d,&d);
        d.x = falloff * entry->push_horizontal * d.x;
        d.y = falloff * entry->push_horizontal * d.y;
        d.z = falloff * entry->push_horizontal * d.z;
      }
      d.y = falloff * entry->push_vertical;
      if ((other->kind >= 0xff00) && (other->kind <= 0xff07)) {
        car = other->kind - 0xff00;
        player_rumble(car,2,0xf,0x80 - (int)(falloff * -127.0f));
        car_enter_rwp_mode(car);
        car_apply_rwp_impulse(car,&d,trig);
        car_damage_at_world_point(car,falloff * entry->damage,trig);
        if ((get_event_type() == EVENT_TRAINING) && (car >= 1)) {
          screen_shake_add(10);
        }
        return;
      }
      if ((other->kind == 10) || (other->kind == 0x1a)) {
        if (other->kind == 0x1a) {
          d.x = d.x + d.x;
          d.y = d.y + d.y;
          d.z = d.z + d.z;
        }
        else {
          d.x = d.x * k_f_0_2;
          d.y = d.y * k_f_0_2;
          d.z = d.z * k_f_0_2;
        }
        owner = other->owner;
        if (owner != (char *)0x0) {
          obj_id = other->obj_id;
          world = *(int *)(owner + 0x24);
          rwp_object_set_user_data(obj_id,rwp_object_get_user_data(obj_id) | 4);
          if (world != 0) {
            FUN_0056b300(*(int *)(owner + 0x24),owner);
            rwp_world_set_bit_60_64(*(int *)(owner + 0x24),owner,1);
            rwp_world_set_bit_5c(*(int *)(owner + 0x24),owner,1);
            body = *(int *)(*(char **)(owner + 0x10) + 4);
            if (body != 0) {
              rwp_body_add_force_at_point(body,&d,trig);
            }
          }
        }
      }
    }
  }
  return;
}
