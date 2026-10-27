#include "mfl_tier2.h"
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*00422350*/ extern char *vehicle_part_ltm();
/*00424290*/ extern undefined __cdecl car_damage_smoke_spawn(float *position, int car);
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*00481fd0*/ extern double random_range();
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd328*/ extern float k_f_0_6;
/*005dd330*/ extern float float_1_0;
/*005dd41c*/ extern float float_5_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd340*/ extern float k_f_0_5;
/*005dd374*/ extern float k_f_0_2;
/*005ddc28*/ extern float k_f_0;
/*005de430*/ extern float k_f_6_2831855;
/*0065a9d8*/ extern undefined1 g_car_damage_65a9d8;
/*0065a9e8*/ extern undefined4 car_damage;
/*0065a9ec*/ extern undefined4 g_car_damage_65a9ec;
/*0065aa88*/ extern undefined g_car_damage_65aa88;
/*0065aae4*/ extern undefined4 g_car_damage_65aae4;
/*007ef7f4*/ extern float g_tick_seconds;
extern double cos(double);
/* layout of *part_ltm in car_damage_smoke_update, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct car_damage_smoke_update_iVar4_layout {
    char pad_0x0[48];
    float f_0x30;
    float f_0x34;
    float f_0x38;
} car_damage_smoke_update_iVar4_layout;

extern double sin(double);
/* car_damage_smoke_update */

/* "per car: emits smoke particles for the damaged front / back (inferred)" */

void car_damage_smoke_update(void)

{
  byte bVar1;
  float fVar2;
  undefined1 uVar3;
  int slot_used;
  car_damage_smoke_update_iVar4_layout *part_ltm;
  float *damage_ptr;
  int car;
  float *damage_ptr2;
  double rnd;
  float strength;
  double angle;
  double cos_offset;
  double alpha;
  struct {
    uint particle_colour;      /* particle colour */
    float offset_x;
    uint local_28;
    float smoke_x;
    float smoke_y;
    float smoke_z;
    float local_18;
    float local_14;
    float local_10;
    RwV3d velocity;
  } frame;
  
  car = 0;
  damage_ptr = (float *)&car_damage;
  damage_ptr2 = (float *)&g_car_damage_65a9ec;
  do {
    car_get_velocity_any_mode(&frame.velocity,car);
    RwV3dLength(&frame.velocity);
    strength = (*damage_ptr2 + *damage_ptr) * k_f_0_01;
    if (!(k_f_0 > strength)) {
      slot_used = race_setup_slot_used(car);
      if (slot_used != 0) {
        rnd = (double)random_range(0,0x3f800000);
        if (rnd < (double)strength * (double)k_f_0_5) {
          part_ltm = (car_damage_smoke_update_iVar4_layout *)(vehicle_part_ltm(car, 0x33));
          frame.smoke_x = part_ltm->f_0x30;
          frame.smoke_y = part_ltm->f_0x34;
          frame.smoke_z = part_ltm->f_0x38;
          /* random offset of the smoke position around the part: x +-0.07, y +-0.1, z +-0.07 */
          rnd = (double)random_range(0xbd8f5c29,0x3d8f5c29);
          frame.smoke_x = (float)(rnd + (double)frame.smoke_x);
          rnd = (double)random_range(0xbdcccccd,0x3dcccccd);
          frame.smoke_y = (float)(rnd + (double)frame.smoke_y);
          rnd = (double)random_range(0xbd8f5c29,0x3d8f5c29);
          frame.smoke_z = (float)(rnd + (double)frame.smoke_z);
          car_damage_smoke_spawn((char *)&frame.smoke_x,car);
        }
      }
    }
    damage_ptr2 = damage_ptr2 + 10;
    car = car + 1;
    damage_ptr = damage_ptr + 10;
  } while ((char *)damage_ptr2 < (char *)&g_car_damage_65aa88 + 0x4);
  car = 0;
  damage_ptr = (float *)&g_car_damage_65aae4;
  do {
    fVar2 = *damage_ptr;
    if ((!(float_1_0 > fVar2)) || (fVar2 < k_f_0)) {
      (&g_car_damage_65a9d8)[car >> 3] =
           (&g_car_damage_65a9d8)[car >> 3] & ~('\x01' << ((byte)car & 7));
    }
    else {
      frame.particle_colour = *(uint *)&damage_ptr[2];
      angle = (double)fVar2 * (double)k_f_6_2831855;
      rnd = (double)sin(angle);
      frame.offset_x = (float)(rnd * (double)(0.05000000074505806));
      cos_offset = (double)cos(angle) * (double)(0.05000000074505806);
      *damage_ptr = g_tick_seconds + *damage_ptr;
      /* alpha of the particle colour: fades in over the first 0.2 of its life and out afterwards */
      if (fVar2 < k_f_0_2) {
        alpha = (double)(int)*(byte *)((char *)damage_ptr + 0xb) * ((double)fVar2 * (double)float_5_0);
      }
      else {
        alpha = (double)(int)*(byte *)((char *)damage_ptr + 0xb) * ((double)float_1_0 - (double)fVar2);
      }
      frame.smoke_x = damage_ptr[-7] * damage_ptr[1];
      frame.particle_colour = (frame.particle_colour & 0xffffff) | ((uint)(byte)(int)alpha << 0x18);
      frame.smoke_y = damage_ptr[-6] * damage_ptr[1];
      damage_ptr[-2] = damage_ptr[-1] + damage_ptr[-2];
      frame.local_14 = fVar2 * k_f_0_6 + damage_ptr[-4];
      if (damage_ptr[-1] <= k_f_0) {
        frame.local_18 = damage_ptr[-5] - frame.offset_x;
        rnd = (double)damage_ptr[-3] - cos_offset;
      }
      else {
        frame.local_18 = frame.offset_x + damage_ptr[-5];
        rnd = cos_offset + (double)damage_ptr[-3];
      }
      frame.local_10 = (float)rnd;
      point_lerp_to_aux_camera(&frame.local_18,&frame.local_18,0x3d4ccccd);
      particles_set_position(&frame.local_18);
      particles_set_size(&frame.smoke_x);
      gfx_set_gfx_6c50c8(*(int *)&damage_ptr[-2]);
      particles_set_color(&frame.particle_colour);
      particles_add(&g_car_damage_65aa88);
    }
    damage_ptr = damage_ptr + 0xb;
    car = car + 1;
  } while ((char *)damage_ptr < (char *)&g_car_damage_65aae4 + 0x1600);
  particles_flush(&g_car_damage_65aa88);
  return;
}


