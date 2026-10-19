#include "mfl_tier2.h"
/*00422320*/ extern int vehicle_get_matrix_local();
/*00437f50*/ extern int screen_shake_set();
/*004681f0*/ extern int __cdecl rocket_hit_spawn_particle(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00487670*/ extern int effect_sprite_spawn();
/*00492be0*/ extern int __cdecl rwp_object_query_ray_hit(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*0049cb60*/ extern int sphere_vs_box_test(float *p1, float *p2);
/*0049ff00*/ extern undefined __cdecl particle_emit_textured(float *p0, undefined4 *p1, int p2, int p3, struct RwTexture *p4, float p5, float *p6);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd370*/ extern float k_f_0_3;
/*005dd628*/ extern float k_f_0_25;
/*005de1ac*/ extern float k_f_0_125;
/*005df77c*/ extern undefined g_rocket_hit_5df77c;
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

/* "trigger sphere callback (type 0xB, radius 0.05) registered by weapon_rocket_update every frame"
    */

void rocket_hit_callback(float *param_1,float *param_2)

{
  char *rocket;
  int target_type;
  int iVar2;
  RwMatrix *pRVar3;
  undefined1 *puVar4;
  struct {
    undefined1 uStack_c0;
    undefined1 uStack_bf;
    undefined1 uStack_be;
    undefined1 uStack_bd;
    float fStack_bc;
    float fStack_b8;
    float fStack_b4;
    float fStack_b0;
    float fStack_ac;
    float fStack_a8;
    undefined4 uStack_a4;
    float fStack_a0;
    undefined4 uStack_9c;
    undefined4 uStack_98;
    float fStack_94;
    undefined4 uStack_90;
    undefined1 auStack_8c [12];
    undefined1 auStack_80 [48];
    undefined4 uStack_50;
    float fStack_4c;
    undefined4 uStack_48;
    undefined1 pad_44[4];
    undefined1 auStack_40 [64];
  } frame;
  
  if (*(int *)&param_1[4] != 11) {
    return;
  }
  rocket = *(char **)&param_1[5];
  target_type = *(int *)&param_2[4];
  if (target_type == 7) {
    return;
  }
  if (*(int *)((int)rocket + 0x50) == 0) {
    return;
  }
  if (*(float *)((int)rocket + 0x18) < k_f_0_3) {
    return;
  }
  if (target_type == 10) {
    frame.fStack_b0 = *(float *)((int)rocket + 0xc);
    frame.fStack_ac = *(float *)((int)rocket + 0x10);
    frame.fStack_a8 = *(float *)((int)rocket + 0x14);
    frame.fStack_bc = *(float *)((int)rocket + 0x1c) + frame.fStack_b0;
    frame.fStack_b8 = *(float *)((int)rocket + 0x20) + frame.fStack_ac;
    frame.fStack_b4 = *(float *)((int)rocket + 0x24) + frame.fStack_a8;
    iVar2 = rwp_object_query_ray_hit(*(int *)&param_2[6],(undefined4)&frame.fStack_b0,(undefined4)&frame.fStack_bc,(undefined4)&frame.uStack_a4,(undefined4)frame.auStack_8c);
  }
  else {
    if (target_type == 18) {
      return;
    }
    if (target_type != 21) goto LAB_00461442;
    iVar2 = sphere_vs_box_test((float *)(*(int *)&param_2[5] + 0x68),param_1);
  }
  if (iVar2 == 0) {
    return;
  }
LAB_00461442:
  target_type = *(int *)&param_2[4];
  if ((0xfeff < target_type) && (target_type < 0xff08)) {
    iVar2 = target_type + -0xff00;
    if (iVar2 == *(int *)((int)rocket + 0x3c)) {
      return;
    }
    puVar4 = frame.auStack_80;
    vehicle_get_matrix_local((*(int *)((char *)param_2 + 16) + 0xffff0100), frame.auStack_80);
    frame.uStack_a4 = frame.uStack_50;
    frame.fStack_a0 = frame.fStack_4c + k_f_0_125;
    frame.uStack_98 = frame.uStack_50;
    frame.uStack_9c = frame.uStack_48;
    frame.fStack_94 = frame.fStack_4c - k_f_0_25;
    frame.uStack_90 = frame.uStack_48;
    iVar2 = rocket_hit_spawn_particle((undefined4)frame.auStack_40,(undefined4)&frame.uStack_a4,iVar2,(undefined4)puVar4);
    if (iVar2 != 0) {
      frame.uStack_c0 = 0xff;
      frame.uStack_bf = 0x80;
      frame.uStack_be = 0;
      frame.uStack_bd = 0x40;
      effect_sprite_spawn(0,frame.auStack_40,&frame.uStack_c0,0x40800000,0x40800000);
      effect_sprite_spawn(1,frame.auStack_40,&frame.uStack_c0,0x40000000,0x40000000);
      screen_shake_set(10);
    }
  }
  pRVar3 = RwFrameGetLTM(*(RwFrame **)(*(int *)((int)rocket + 0x5c) + 4));
  frame.fStack_b0 = (pRVar3->at).x;
  frame.fStack_ac = (pRVar3->at).y;
  frame.fStack_b8 = param_2[1];
  frame.fStack_a8 = -(pRVar3->at).z;
  frame.fStack_b4 = param_2[2];
  frame.fStack_bc = -frame.fStack_b0 * k_f_0_5 + *param_2;
  *(float *)((int)rocket + 0xc) = frame.fStack_bc;
  frame.fStack_b8 = -frame.fStack_ac * k_f_0_5 + frame.fStack_b8;
  *(float *)((int)rocket + 0x10) = frame.fStack_b8;
  frame.fStack_b4 = frame.fStack_a8 * k_f_0_5 + frame.fStack_b4;
  *(float *)((int)rocket + 0x14) = frame.fStack_b4;
  rocket_explode_r((int)rocket);
  iVar2 = 10;
  do {
    particle_emit_textured
              ((float *)((int)rocket + 0xc),(undefined4 *)&g_rocket_hit_5df77c,0,0,(RwTexture *)0x0
               ,1.0,(float *)0x0);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}


