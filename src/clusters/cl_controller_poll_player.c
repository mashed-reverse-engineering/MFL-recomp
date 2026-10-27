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

/* builds the per-player input state from the bindings */

/* Per frame: previous <- current input record, clear, read the stick of a joypad device, then the bound actions (indices of controller_action_pressed:
   0 accelerate, 1 brake, 2 fire, 3 drop, 4 menu select, 5 menu back, 6 pause, 7 menu next, 8 insult, 9..0xc left/right/up/down), derive analog x/y (device 1: stick,
   device 2: keys scaled by 1/255), swap left/right in mirror mode; for player 1 also fill the signed-byte stick state for the second pad channel. */
void controller_poll_player_n(int player)

{
  struct PlayerInput *pin;
  float axis_y_out;
  byte tmp_up;
  byte tmp_down;
  byte pressed;
  undefined1 uVar5;
  int n;
  PlayerInput *cur_in;
  PlayerInput *prev_in;
  struct {
    undefined4 axis_aux;
    ControllerCfg *cfg_or_tmp;
    float axis_x;
    float axis_y;
  } frame;
  
  cur_in = g_player_inputs + player;
  prev_in = g_player_inputs_previous + player;
  for (n = 0x13; n != 0; n = n + -1) {
    pressed = cur_in->right;
    tmp_up = cur_in->up;
    tmp_down = cur_in->down;
    prev_in->left = cur_in->left;
    prev_in->right = pressed;
    prev_in->up = tmp_up;
    prev_in->down = tmp_down;
    cur_in = (PlayerInput *)&cur_in->accelerate;
    prev_in = (PlayerInput *)&prev_in->accelerate;
  }
  frame.cfg_or_tmp = g_controller_cfg + player;
  cur_in = g_player_inputs + player;
  for (n = 0x13; n != 0; n = n + -1) {
    cur_in->left = 0;
    cur_in->right = 0;
    cur_in->up = 0;
    cur_in->down = 0;
    cur_in = (PlayerInput *)&cur_in->accelerate;
  }
  frame.axis_x = 0.0;
  frame.axis_y = 0.0;
  frame.axis_aux = 0;
  if (g_controller_cfg[player].device == 1) {
    joypad_get_axis_info(g_controller_cfg[player].joystick,0,&frame.axis_x,0,&frame.axis_aux);
  }
  pressed = controller_action_pressed(player,4);
  pin = &g_player_inputs[player];
  pin->menu_select = pressed;
  pressed = controller_action_pressed(player,5);
  pin->menu_back = pressed;
  pressed = controller_action_pressed(player,6);
  pin->pause = pressed;
  pressed = controller_action_pressed(player,7);
  pin->menu_next = pressed;
  pressed = controller_action_pressed(player,0);
  pin->accelerate = pressed;
  pressed = controller_action_pressed(player,1);
  pin->brake = pressed;
  pressed = controller_action_pressed(player,2);
  pin->fire = pressed;
  pressed = controller_action_pressed(player,3);
  pin->drop = pressed;
  pressed = controller_action_pressed(player,8);
  pin->insult = pressed;
  pin->fire_copy = pin->fire;
  pressed = controller_action_pressed(player,9);
  if (pressed != 0) {
    pin->direction_flags[0] = 0xff;
    pin->left = pressed;
  }
  pressed = controller_action_pressed(player,10);
  if (pressed != 0) {
    pin->direction_flags[1] = 0xff;
    pin->right = pressed;
  }
  pressed = controller_action_pressed(player,0xb);
  if (pressed != 0) {
    pin->direction_flags[2] = 0xff;
    pin->up = pressed;
  }
  pressed = controller_action_pressed(player,0xc);
  if (pressed != 0) {
    pin->direction_flags[3] = 0xff;
    pin->down = pressed;
  }
  if (frame.cfg_or_tmp->device == 1) {
    pin->analog_x = frame.axis_x;
    axis_y_out = -frame.axis_y;
  }
  else {
    if (frame.cfg_or_tmp->device != 2) goto LAB_004a5867;
    frame.cfg_or_tmp = (ControllerCfg *)
              ((uint)pin->down - (uint)pin->up);
    pin->analog_x =
         (float)(int)((uint)pin->right - (uint)pin->left) *
         k_f_0_003921569;
    axis_y_out = (float)(int)frame.cfg_or_tmp * k_f_0_003921569;
  }
  pin->analog_y = axis_y_out;
LAB_004a5867:
  if (g_game_state.mirror_mode != 0) {
    pressed = pin->left;
    pin->left = pin->right;
    pin->right = pressed;
  }
  if (player == 1) {
    g_controller_poll_8c7e2c = g_controller_poll_8c7e5c;
    g_controller_poll_8c7e28 = g_controller_8c7e58;
    g_controller_poll_8c7e5c = 0;
    if (g_player_inputs[1].accelerate != 0) {
      g_controller_8c7e58.axes[1] = 0;
      g_controller_8c7e58.axes[0] = 0;
      g_controller_8c7e58.axes[2] = (signed char)(int)(frame.axis_y * -127.0f);
      g_controller_8c7e58.axes[3] = (signed char)(int)(frame.axis_x * 127.0f);
    } else {
      g_controller_8c7e58.axes[1] = (signed char)(int)(frame.axis_x * 127.0f);
      g_controller_8c7e58.axes[0] = (signed char)(int)(frame.axis_y * -127.0f);
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
