// cluster-entries: controller_poll_player
/* Installed cluster controller_poll_player (tools/cluster_install.py from output/cluster_pilot8/controller_poll_player.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a48f0*/ extern int __cdecl joypad_get_axis_info(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004a6f80*/ extern undefined __cdecl controller_action_pressed(int p0, int p1);
/*004b2b48*/ extern int _ftol();
/*005dff00*/ extern float k_f_0_003921569;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/* the stick state of player 2 as signed bytes (the force feedback / second pad channel): current and previous frame */
typedef struct StickState {
    signed char axes[4];
} StickState;
extern StickState g_controller_8c7e58;           /* current */
extern StickState g_controller_poll_8c7e28;      /* previous */
extern unsigned char g_controller_poll_8c7e5c;   /* current extra byte */
extern unsigned char g_controller_poll_8c7e2c;   /* previous extra byte */

/* controller_poll_player */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* builds the per-player input state from the bindings */

void controller_poll_player_n(int in_ebx)

{
  struct PlayerInput *p_in_ebx;
  float fVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  undefined1 uVar5;
  int iVar6;
  PlayerInput *pPVar7;
  PlayerInput *pPVar8;
  struct {
    undefined4 local_10;
    ControllerCfg *local_c;
    float local_8;
    float local_4;
  } frame;
  
  pPVar7 = g_player_inputs + in_ebx;
  pPVar8 = g_player_inputs_previous + in_ebx;
  for (iVar6 = 0x13; iVar6 != 0; iVar6 = iVar6 + -1) {
    bVar4 = pPVar7->right;
    bVar2 = pPVar7->up;
    bVar3 = pPVar7->down;
    pPVar8->left = pPVar7->left;
    pPVar8->right = bVar4;
    pPVar8->up = bVar2;
    pPVar8->down = bVar3;
    pPVar7 = (PlayerInput *)&pPVar7->accelerate;
    pPVar8 = (PlayerInput *)&pPVar8->accelerate;
  }
  frame.local_c = g_controller_cfg + in_ebx;
  pPVar7 = g_player_inputs + in_ebx;
  for (iVar6 = 0x13; iVar6 != 0; iVar6 = iVar6 + -1) {
    pPVar7->left = 0;
    pPVar7->right = 0;
    pPVar7->up = 0;
    pPVar7->down = 0;
    pPVar7 = (PlayerInput *)&pPVar7->accelerate;
  }
  frame.local_8 = 0.0;
  frame.local_4 = 0.0;
  frame.local_10 = 0;
  if (g_controller_cfg[in_ebx].device == 1) {
    joypad_get_axis_info(g_controller_cfg[in_ebx].joystick,0,&frame.local_8,0,&frame.local_10);
  }
  bVar4 = controller_action_pressed(in_ebx,4);
  p_in_ebx = &g_player_inputs[in_ebx];
  p_in_ebx->menu_select = bVar4;
  bVar4 = controller_action_pressed(in_ebx,5);
  p_in_ebx->menu_back = bVar4;
  bVar4 = controller_action_pressed(in_ebx,6);
  p_in_ebx->pause = bVar4;
  bVar4 = controller_action_pressed(in_ebx,7);
  p_in_ebx->menu_next = bVar4;
  bVar4 = controller_action_pressed(in_ebx,0);
  p_in_ebx->accelerate = bVar4;
  bVar4 = controller_action_pressed(in_ebx,1);
  p_in_ebx->brake = bVar4;
  bVar4 = controller_action_pressed(in_ebx,2);
  p_in_ebx->fire = bVar4;
  bVar4 = controller_action_pressed(in_ebx,3);
  p_in_ebx->drop = bVar4;
  bVar4 = controller_action_pressed(in_ebx,8);
  p_in_ebx->insult = bVar4;
  p_in_ebx->fire_copy = p_in_ebx->fire;
  bVar4 = controller_action_pressed(in_ebx,9);
  if (bVar4 != 0) {
    p_in_ebx->direction_flags[0] = 0xff;
    p_in_ebx->left = bVar4;
  }
  bVar4 = controller_action_pressed(in_ebx,10);
  if (bVar4 != 0) {
    p_in_ebx->direction_flags[1] = 0xff;
    p_in_ebx->right = bVar4;
  }
  bVar4 = controller_action_pressed(in_ebx,0xb);
  if (bVar4 != 0) {
    p_in_ebx->direction_flags[2] = 0xff;
    p_in_ebx->up = bVar4;
  }
  bVar4 = controller_action_pressed(in_ebx,0xc);
  if (bVar4 != 0) {
    p_in_ebx->direction_flags[3] = 0xff;
    p_in_ebx->down = bVar4;
  }
  if (frame.local_c->device == 1) {
    p_in_ebx->analog_x = frame.local_8;
    fVar1 = -frame.local_4;
  }
  else {
    if (frame.local_c->device != 2) goto LAB_004a5867;
    frame.local_c = (ControllerCfg *)
              ((uint)p_in_ebx->down - (uint)p_in_ebx->up);
    p_in_ebx->analog_x =
         (float)(int)((uint)p_in_ebx->right - (uint)p_in_ebx->left) *
         k_f_0_003921569;
    fVar1 = (float)(int)frame.local_c * k_f_0_003921569;
  }
  p_in_ebx->analog_y = fVar1;
LAB_004a5867:
  if (g_game_state.mirror_mode != 0) {
    bVar4 = p_in_ebx->left;
    p_in_ebx->left = p_in_ebx->right;
    p_in_ebx->right = bVar4;
  }
  if (in_ebx == 1) {
    g_controller_poll_8c7e2c = g_controller_poll_8c7e5c;
    g_controller_poll_8c7e28 = g_controller_8c7e58;
    g_controller_poll_8c7e5c = 0;
    if (g_player_inputs[1].accelerate != 0) {
      g_controller_8c7e58.axes[1] = 0;
      g_controller_8c7e58.axes[0] = 0;
      g_controller_8c7e58.axes[2] = (signed char)(int)(frame.local_4 * -127.0f);
      g_controller_8c7e58.axes[3] = (signed char)(int)(frame.local_8 * 127.0f);
    } else {
      g_controller_8c7e58.axes[1] = (signed char)(int)(frame.local_8 * 127.0f);
      g_controller_8c7e58.axes[0] = (signed char)(int)(frame.local_4 * -127.0f);
      g_controller_8c7e58.axes[2] = 0;
      g_controller_8c7e58.axes[3] = 0;
    }
  }
  return;
}

/* entry thunk at the original address of controller_poll_player: register inputs (ebx) -> natural C controller_poll_player_n; ret 0 */
__declspec(naked) void controller_poll_player(void)
{
    __asm {
        push ebx
        call controller_poll_player_n
        add esp, 4
        ret
    }
}
