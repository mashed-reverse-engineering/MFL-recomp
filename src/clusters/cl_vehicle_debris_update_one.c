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
#define DEBRIS_PIECE_COUNT (10) /* debris pieces per vehicle */
#define DEBRIS_PIECE_STRIDE_INTS (6) /* 0x18 bytes per piece record */
/* vehicle_debris_update_one */

/* "spawns smoke and an sfx for damaged front/back or updates the flying debris pieces" */

static int debris_body_remove_if_in_world_r(int p0);
extern void debris_body_remove_if_in_world(void);
/* debris+0xf4 == 0: car intact-ish, spawn smoke + sfx when the car (+0xfc) has front (side 0) or back (side 1) damage.
   Otherwise (while g_game_65c8f0 != 0) walk the 10 debris pieces (stride 0x18 from +0xc, live bits at +0xf0): pieces without body
   (piece_ptr[2]==0) emit a trail particle with random velocity, the others are removed from the world and their frame reset to origin. */
void vehicle_debris_update_one_n(int debris)

{
  byte *live_byte;
  byte live_bits;
  char *piece_node;
  byte bit;
  int piece;
  int *piece_ptr;
  float10 rnd;
  struct {
    RwV3d zero_pos;
    undefined4 part_pos_x;
    undefined4 part_pos_y;
    undefined4 part_pos_z;
    undefined4 part_color;
    undefined4 part_size_x;
    undefined4 part_size_y;
    float part_vel_x;
    float part_vel_y;
    float part_vel_z;
    undefined4 local_24;
    undefined4 local_20;
    undefined4 local_1c;
    undefined4 local_18;
    undefined1 pad_14[4];
    undefined4 local_10;
    undefined1 pad_c[4];
    undefined4 local_8;
    undefined1 pad_4[1];
    undefined1 part_type;
  } spawn;
  
  if (*(int *)(debris + 0xf4) == 0) {
    if (*(int *)(debris + 0xf8) == 0) {
      piece = car_damage_get_front(*(undefined4 *)(*(char **)(debris + 0xfc) + 0x204),0);
      if (piece != 0) {
        piece = debris;
        vehicle_smoke_emitters_spawn(piece);
        sound_sfx_play_at_car(0x21,*(undefined4 *)(*(char **)(debris + 0xfc) + 0x204),piece);
        return;
      }
    }
    else if (*(int *)(debris + 0xf8) == 1) {
      piece = car_damage_get_back(*(undefined4 *)(*(char **)(debris + 0xfc) + 0x204),0);
      if (piece != 0) {
        piece = debris;
        vehicle_smoke_emitters_spawn(piece);
        sound_sfx_play_at_car(0x21,*(undefined4 *)(*(char **)(debris + 0xfc) + 0x204),piece);
      }
    }
  }
  else if (g_game_65c8f0 != 0) {
    piece = 0;
    piece_ptr = (int *)(debris + 0xc);
    do {
      bit = (byte)piece & 7;
      live_byte = (byte *)((piece >> 3) + 0xf0 + debris);
      live_bits = *live_byte;
      if ((live_bits & (byte)(1 << bit)) != 0) {
        if (piece_ptr[2] == 0) {
          piece_node = *(char **)(*piece_ptr + 4);
          spawn.part_pos_x = *(undefined4 *)(piece_node + 0x40);
          spawn.part_pos_y = *(undefined4 *)(piece_node + 0x44);
          spawn.part_pos_z = *(undefined4 *)(piece_node + 0x48);
          spawn.part_type = 6;
          spawn.local_18 = 0x40000000;
          spawn.part_color = 0x80404040;
          spawn.local_8 = 0x1120;
          spawn.part_size_x = 0x3e800000;
          spawn.part_size_y = 0x3e800000;
          spawn.local_10 = 0x3f800000;
          rnd = (float10)rp_random_float_range(0xbe800000,0x3e800000);
          spawn.part_vel_x = (float)rnd;
          rnd = (float10)rp_random_float_range(0xbe800000,0x3e800000);
          spawn.part_vel_y = (float)rnd;
          rnd = (float10)rp_random_float_range(0xbe800000,0x3e800000);
          spawn.part_vel_z = (float)rnd;
          spawn.local_24 = 0;
          spawn.local_20 = 0;
          spawn.local_1c = 0;
          fx_trail_particle_spawn_interpolated(&spawn.part_pos_x,0,0,0);
        }
        else {
          spawn.zero_pos.x = 0.0;
          spawn.zero_pos.y = 0.0;
          spawn.zero_pos.z = 0.0;
          *live_byte = ~('\x01' << bit) & live_bits;
          debris_body_remove_if_in_world_r(piece_ptr + -3);
          RwFrameTranslate(*(RwFrame **)(*piece_ptr + 4),&spawn.zero_pos,rwCOMBINEREPLACE);
        }
      }
      piece = piece + 1;
      piece_ptr = piece_ptr + DEBRIS_PIECE_STRIDE_INTS;
    } while (piece < DEBRIS_PIECE_COUNT);
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
