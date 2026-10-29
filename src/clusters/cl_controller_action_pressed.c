// cluster-entries: controller_action_pressed
/* Installed cluster controller_action_pressed (tools/cluster_install.py from output/cluster_pilot_kb/controller_action_pressed.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*005dd9ac*/ extern float k_f_255;
/*005e14fc*/ extern float k_f_n255;
/*007711c4*/ extern undefined1 g_app_7711c4[]; /* joystick button bit sets: 4 bytes per joystick */
/*0077120c*/ extern float g_app_77120c[];      /* joystick axes: x, y per joystick */
/*0077122c*/ extern undefined1 g_app_77122c[]; /* keyboard key state bit set */
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/* controller_action_pressed */

#define CONTROLLER_DEVICE_GAMEPAD 1
#define CONTROLLER_DEVICE_KEYBOARD 2
#define ACTION_LEFT 9
#define ACTION_RIGHT 10
#define ACTION_UP 11
#define ACTION_DOWN 12

/* Tests one action binding of a controller: a gamepad button or axis direction, or a keyboard scancode.
   Returns 0 / 0xff for digital inputs and the axis deflection scaled to 0..255 for left / right
   (the callers only look at the low byte). */
unsigned char controller_action_pressed(int controller_index, int action)

{
  ControllerCfg *cfg;
  uint binding;
  float axis_x;
  float axis_y;
  uint joystick;

  cfg = g_controller_cfg + controller_index;
  if (cfg->device == CONTROLLER_DEVICE_GAMEPAD) {
    binding = cfg->bindings[action];
    joystick = cfg->joystick;
    axis_x = g_app_77120c[joystick * 2];
    axis_y = g_app_77120c[joystick * 2 + 1];
    switch(action) {
    case ACTION_LEFT:
      if (axis_x < k_f_0) {
        return (int)(axis_x * k_f_n255);
      }
      break;
    case ACTION_RIGHT:
      if (axis_x > k_f_0) {
        return (int)(axis_x * k_f_255);
      }
      break;
    case ACTION_UP:
      if (axis_y > k_f_0) {
        return 0xff;
      }
      break;
    case ACTION_DOWN:
      if (axis_y < k_f_0) {
        return 0xff;
      }
      break;
    default:
      return -(uint)((g_app_7711c4[joystick * 4 + (binding >> 3)] & (1 << (binding & 7))) != 0) & 0xff;
    }
  }
  else if (cfg->device == CONTROLLER_DEVICE_KEYBOARD) {
    binding = cfg->bindings[action];
    return -(uint)(((1 << (binding & 7)) & g_app_77122c[binding >> 3]) != 0) & 0xff;
  }
  return 0;
}

