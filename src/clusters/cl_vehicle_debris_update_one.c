// cluster-entries: vehicle_debris_update_one
/* Installed cluster vehicle_debris_update_one (tools/cluster_install.py from output/cluster_pilot10/vehicle_debris_update_one.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00423fa0*/ extern int __cdecl car_damage_get_front(undefined4 p0, undefined4 p1);
/*00423fc0*/ extern int __cdecl car_damage_get_back(undefined4 p0, undefined4 p1);
/*00424e50*/ extern int vehicle_smoke_emitters_spawn();
/*00472610*/ extern undefined __cdecl sound_sfx_play_at_car(undefined4 p0, undefined4 p1, undefined4 p2);
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*0065c8f0*/ extern undefined4 g_game_65c8f0;
/* vehicle_debris_update_one */

/* WARNING: Unknown calling convention */
/* "spawns smoke and an sfx for damaged front/back or updates the flying debris pieces" */

static int debris_body_remove_if_in_world_r(int p0);
extern void debris_body_remove_if_in_world(void);
void vehicle_debris_update_one_n(int in_ebx)

{
  byte *pbVar1;
  byte bVar2;
  char *iVar3;
  byte bVar4;
  int iVar5;
  int *piVar6;
  float10 fVar7;
  struct {
    RwV3d local_54;
    undefined4 local_48;
    undefined4 local_44;
    undefined4 local_40;
    undefined4 local_3c;
    undefined4 local_38;
    undefined4 local_34;
    float local_30;
    float local_2c;
    float local_28;
    undefined4 local_24;
    undefined4 local_20;
    undefined4 local_1c;
    undefined4 local_18;
    undefined1 pad_14[4];
    undefined4 local_10;
    undefined1 pad_c[4];
    undefined4 local_8;
    undefined1 pad_4[1];
    undefined1 local_3;
  } frame;
  
  if (*(int *)(in_ebx + 0xf4) == 0) {
    if (*(int *)(in_ebx + 0xf8) == 0) {
      iVar5 = car_damage_get_front(*(undefined4 *)(*(char **)(in_ebx + 0xfc) + 0x204),0);
      if (iVar5 != 0) {
        iVar5 = in_ebx;
        vehicle_smoke_emitters_spawn(iVar5);
        sound_sfx_play_at_car(0x21,*(undefined4 *)(*(char **)(in_ebx + 0xfc) + 0x204),iVar5);
        return;
      }
    }
    else if (*(int *)(in_ebx + 0xf8) == 1) {
      iVar5 = car_damage_get_back(*(undefined4 *)(*(char **)(in_ebx + 0xfc) + 0x204),0);
      if (iVar5 != 0) {
        iVar5 = in_ebx;
        vehicle_smoke_emitters_spawn(iVar5);
        sound_sfx_play_at_car(0x21,*(undefined4 *)(*(char **)(in_ebx + 0xfc) + 0x204),iVar5);
      }
    }
  }
  else if (g_game_65c8f0 != 0) {
    iVar5 = 0;
    piVar6 = (int *)(in_ebx + 0xc);
    do {
      bVar4 = (byte)iVar5 & 7;
      pbVar1 = (byte *)((iVar5 >> 3) + 0xf0 + in_ebx);
      bVar2 = *pbVar1;
      if ((bVar2 & (byte)(1 << bVar4)) != 0) {
        if (piVar6[2] == 0) {
          iVar3 = *(char **)(*piVar6 + 4);
          frame.local_48 = *(undefined4 *)(iVar3 + 0x40);
          frame.local_44 = *(undefined4 *)(iVar3 + 0x44);
          frame.local_40 = *(undefined4 *)(iVar3 + 0x48);
          frame.local_3 = 6;
          frame.local_18 = 0x40000000;
          frame.local_3c = 0x80404040;
          frame.local_8 = 0x1120;
          frame.local_38 = 0x3e800000;
          frame.local_34 = 0x3e800000;
          frame.local_10 = 0x3f800000;
          fVar7 = (float10)rp_random_float_range(0xbe800000,0x3e800000);
          frame.local_30 = (float)fVar7;
          fVar7 = (float10)rp_random_float_range(0xbe800000,0x3e800000);
          frame.local_2c = (float)fVar7;
          fVar7 = (float10)rp_random_float_range(0xbe800000,0x3e800000);
          frame.local_28 = (float)fVar7;
          frame.local_24 = 0;
          frame.local_20 = 0;
          frame.local_1c = 0;
          fx_trail_particle_spawn_interpolated(&frame.local_48,0,0,0);
        }
        else {
          frame.local_54.x = 0.0;
          frame.local_54.y = 0.0;
          frame.local_54.z = 0.0;
          *pbVar1 = ~('\x01' << bVar4) & bVar2;
          debris_body_remove_if_in_world_r(piVar6 + -3);
          RwFrameTranslate(*(RwFrame **)(*piVar6 + 4),&frame.local_54,rwCOMBINEREPLACE);
        }
      }
      iVar5 = iVar5 + 1;
      piVar6 = piVar6 + 6;
    } while (iVar5 < 10);
    return;
  }
  return;
}

/* debris_body_remove_if_in_world_r: C -> original debris_body_remove_if_in_world (registers eax) */
__declspec(naked) int debris_body_remove_if_in_world_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call debris_body_remove_if_in_world
        ret
    }
}

/* entry thunk at the original address of vehicle_debris_update_one: register inputs (ebx) -> natural C vehicle_debris_update_one_n; ret 0 */
__declspec(naked) void vehicle_debris_update_one(void)
{
    __asm {
        push ebx
        call vehicle_debris_update_one_n
        add esp, 4
        ret
    }
}
