#include "mfl_tier2.h"
/*00422320*/ extern int vehicle_get_matrix_local();
/*00437f50*/ extern int screen_shake_set();
/*004681f0*/ extern int __cdecl rocket_hit_spawn_particle(void *p0, void *p1, undefined4 p2, void *p3);
/*00487670*/ extern int effect_sprite_spawn();
/*00492be0*/ extern int __cdecl rwp_object_query_ray_hit(undefined4 p0, void *p1, void *p2, void *p3, void *p4);
/*0049cb60*/ extern int sphere_vs_box_test(float *p1, float *p2);
/*0049ff00*/ extern undefined __cdecl particle_emit_textured(float *p0, undefined4 *p1, int p2, int p3, struct RwTexture *p4, float p5, float *p6);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd370*/ extern float k_f_0_3;
/*005dd628*/ extern float k_f_0_25;
/*005de1ac*/ extern float k_f_0_125;
/*005df77c*/ extern undefined g_rocket_hit_5df77c;
#define ROCKET_TRIGGER_TYPE (11) /* trigger record kind registered by weapon_rocket_update (see header comment) */
#define ROCKET_HIT_PARTICLE_COUNT (10)
static int rocket_explode_r(int reg_eax);
extern void rocket_explode(void);
__declspec(naked) int rocket_explode_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rocket_explode
        ret
    }
}

/* rocket_hit_callback */

/* "trigger sphere callback (type 0xB, radius 0.05) registered by weapon_rocket_update every hit"
    */

void rocket_hit_callback(float *trigger,float *other)

{
  RocketRecord *rocket;
  int target_type;
  int scratch_i;
  RwMatrix *rocket_ltm;
  undefined1 *car_matrix_buf;
  struct {
    undefined1 colour_byte0;
    undefined1 colour_byte1;
    undefined1 colour_byte2;
    undefined1 colour_byte3;
    float ray_end_x;
    float ray_end_y;
    float ray_end_z;
    float ray_start_x;
    float ray_start_y;
    float ray_start_z;
    undefined4 seg_a_x;
    float seg_a_y;
    undefined4 seg_a_z;
    undefined4 seg_b_x;
    float seg_b_y;
    undefined4 seg_b_z;
    undefined1 hit_normal [12];
    undefined1 car_matrix [48];
    undefined4 car_pos_x;
    float car_pos_y;
    undefined4 car_pos_z;
    undefined1 pad_44[4];
    undefined1 particle_matrix [64];
  } hit;
  
  if (*(int *)&trigger[4] != ROCKET_TRIGGER_TYPE) {
    return;
  }
  rocket = *(RocketRecord **)&trigger[5];
  target_type = *(int *)&other[4];
  if (target_type == 7) {
    return;
  }
  /* ignore hits from collider type 7, skip until the rocket is older than 0.3 s (k_f_0_3); type 10 = ray test along the velocity, 21 = sphere-vs-box, 18 ignored (inferred) */
  if (rocket->field_50 == 0) {
    return;
  }
  if (rocket->age < k_f_0_3) {
    return;
  }
  if (target_type == 10) {
    hit.ray_start_x = rocket->pos.x;
    hit.ray_start_y = rocket->pos.y;
    hit.ray_start_z = rocket->pos.z;
    hit.ray_end_x = rocket->vel.x + hit.ray_start_x;
    hit.ray_end_y = rocket->vel.y + hit.ray_start_y;
    hit.ray_end_z = rocket->vel.z + hit.ray_start_z;
    scratch_i = rwp_object_query_ray_hit(*(int *)&other[6],&hit.ray_start_x,&hit.ray_end_x,&hit.seg_a_x,hit.hit_normal);
  }
  else {
    if (target_type == 18) {
      return;
    }
    if (target_type != 21) goto LAB_00461442;
    scratch_i = sphere_vs_box_test((float *)(*(int *)&other[5] + 0x68),trigger);
  }
  if (scratch_i == 0) {
    return;
  }
LAB_00461442:
  target_type = *(int *)&other[4];
  /* car hit (types 0xff00..0xff07): spawn hit particle + sprites + screen shake unless it is the rocket's own car (field_3c) */
  if ((CAR_ID_BASE - 1 < target_type) && (target_type < CAR_ID_BASE + 8)) {
    scratch_i = target_type - CAR_ID_BASE;
    if (scratch_i == rocket->field_3c) {
      return;
    }
    car_matrix_buf = hit.car_matrix;
    vehicle_get_matrix_local((*(int *)((char *)other + 16) + 0xffff0100), hit.car_matrix);
    hit.seg_a_x = hit.car_pos_x;
    hit.seg_a_y = hit.car_pos_y + k_f_0_125;
    hit.seg_b_x = hit.car_pos_x;
    hit.seg_a_z = hit.car_pos_z;
    hit.seg_b_y = hit.car_pos_y - k_f_0_25;
    hit.seg_b_z = hit.car_pos_z;
    scratch_i = rocket_hit_spawn_particle(hit.particle_matrix,&hit.seg_a_x,scratch_i,car_matrix_buf);
    if (scratch_i != 0) {
      hit.colour_byte0 = 0xff;
      hit.colour_byte1 = 0x80;
      hit.colour_byte2 = 0;
      hit.colour_byte3 = 0x40;
      effect_sprite_spawn(0,hit.particle_matrix,&hit.colour_byte0,0x40800000,0x40800000);
      effect_sprite_spawn(1,hit.particle_matrix,&hit.colour_byte0,0x40000000,0x40000000);
      screen_shake_set(10);
    }
  }
  /* move the rocket back 0.5 along its LTM 'at' axis from the contact point, explode it and emit ROCKET_HIT_PARTICLE_COUNT particles */
  rocket_ltm = RwFrameGetLTM(*(RwFrame **)(rocket->frame_holder + 4));
  hit.ray_start_x = (rocket_ltm->at).x;
  hit.ray_start_y = (rocket_ltm->at).y;
  hit.ray_end_y = other[1];
  hit.ray_start_z = -(rocket_ltm->at).z;
  hit.ray_end_z = other[2];
  hit.ray_end_x = -hit.ray_start_x * k_f_0_5 + *other;
  rocket->pos.x = hit.ray_end_x;
  hit.ray_end_y = -hit.ray_start_y * k_f_0_5 + hit.ray_end_y;
  rocket->pos.y = hit.ray_end_y;
  hit.ray_end_z = hit.ray_start_z * k_f_0_5 + hit.ray_end_z;
  rocket->pos.z = hit.ray_end_z;
  rocket_explode_r((int)rocket);
  scratch_i = ROCKET_HIT_PARTICLE_COUNT;
  do {
    particle_emit_textured
              (&rocket->pos.x,(undefined4 *)&g_rocket_hit_5df77c,0,0,(RwTexture *)0x0
               ,1.0,(float *)0x0);
    scratch_i = scratch_i + -1;
  } while (scratch_i != 0);
  return;
}


