// cluster-entries: controller_dialog_start_key_capture_cb_4a7310
/* Installed cluster controller_dialog_start_key_capture_cb_4a7310 (tools/cluster_install.py from output/cluster_pilot_pg/controller_dialog_start_key_capture_cb_4a7310.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void * __stdcall GetDlgItem(void *dialog, int id);
extern __declspec(dllimport) int __stdcall ShowWindow(void *window, int command);
extern __declspec(dllimport) int __stdcall KillTimer(void *window, unsigned int id);
extern __declspec(dllimport) void * __stdcall LoadBitmapA(void *instance, unsigned int resource_id);
extern __declspec(dllimport) int __stdcall SendMessageA(void *window, unsigned int message, unsigned int wparam, void *lparam);
/*004a48f0*/ extern int __cdecl joypad_get_axis_info(int joystick, int axis, void *out_a, void *out_b, void *out_buttons);
/*004a5130*/ extern void joypads_release_all(void);
/*004a5250*/ extern int __cdecl keyboard_state_to_bits(unsigned char *key_bits);
/*004a9390*/ extern void * app_get_app_7e7700(void);
/*004c6600*/ extern void __cdecl FUN_004c6600(void *bits, int bit_count);      /* clears the first bit_count bits of a bit set */
/*0050b5e0*/ extern void nop_stub(void);
/*0062c588*/ extern unsigned int g_controller_dialog_62c588[];              /* pad button -> low word: bitmap resource id */
/*007711a0*/ extern int g_controller_dialog_7711a0;                          /* controller being edited */
/*00771208*/ extern unsigned int *g_controller_dialog_771208;                /* keyboard key -> low word: bitmap resource id, 0 = key not bindable */
/*0077124c*/ extern int g_controller_dialog_77124c;                          /* dialog control id of the button being rebound */
/*00771250*/ extern int g_controller_dialog_771250;                          /* binding slot being rebound */
/*00771254*/ extern unsigned int g_data_771254;                              /* capture timer ticks, bit 3 blinks the button */
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];

#define SW_HIDE 0
#define SW_SHOW 5
#define BM_SETIMAGE 0xf7
#define KEY_CAPTURE_TIMER 20000           /* timer id set by controller_dialog_start_key_capture */
#define KEY_BLINK_BIT 8                   /* the control blinks every 8 timer ticks */
#define CONTROLLER_DEVICE_PAD 1
#define CONTROLLER_DEVICE_KEYBOARD 2
#define PAD_BUTTON_COUNT 16
#define KEYBOARD_KEY_COUNT 256
/* DirectInput key codes the dialog keeps for itself (not bindable) */
#define DIK_ESCAPE 0x01
#define DIK_RETURN 0x1c
#define DIK_UP 0xc8
#define DIK_LEFT 0xcb
#define DIK_RIGHT 0xcd
#define DIK_DOWN 0xd0

/* A button / key was captured: stores it in the binding slot of the controller being edited, shows its bitmap on the
   control, stops the capture timer and makes the control visible again. */
static void key_capture_commit(void *dialog, int binding_value, unsigned int bitmap_id)
{
  void *control;

  g_controller_cfg[0].bindings[g_controller_dialog_7711a0 * (sizeof(struct ControllerCfg) / sizeof(unsigned int)) + g_controller_dialog_771250] = binding_value;
  control = GetDlgItem(dialog, g_controller_dialog_77124c);
  SendMessageA(control, BM_SETIMAGE, 0, LoadBitmapA(app_get_app_7e7700(), bitmap_id));
  KillTimer(dialog, KEY_CAPTURE_TIMER);
  ShowWindow(GetDlgItem(dialog, g_controller_dialog_77124c), SW_SHOW);
  g_data_771254 = 0;
}

/* Timer procedure of the controller dialog while a key / button is being captured (pushed by controller_dialog_start_key_capture):
   blinks the control being rebound, and polls the pad (first pressed of its 16 buttons) or the keyboard (first pressed bindable
   key) of the controller being edited; the first one found becomes the binding. A pad profile keeps three of its bindings
   mirrored into its second set (bindings 4, 5, 7 follow 0, 1, 2). */
void controller_dialog_start_key_capture_cb_4a7310_n(void *dialog, unsigned int message, unsigned int timer_id, unsigned int tick)
{
  struct ControllerCfg *cfg;
  unsigned int buttons;
  unsigned char key_bits[KEYBOARD_KEY_COUNT / 8];
  unsigned int index;
  unsigned int mirror_1;
  unsigned int mirror_2;

  g_data_771254++;
  ShowWindow(GetDlgItem(dialog, g_controller_dialog_77124c), (g_data_771254 & KEY_BLINK_BIT) ? SW_SHOW : SW_HIDE);
  joypads_release_all();
  nop_stub();
  cfg = &g_controller_cfg[g_controller_dialog_7711a0];
  if (cfg->device == CONTROLLER_DEVICE_PAD) {
    buttons = 0;
    if (joypad_get_axis_info(cfg->joystick, 0, 0, 0, &buttons) != 0) {
      for (index = 0; index < PAD_BUTTON_COUNT; index++) {
        if (((buttons >> index) & 1) != 0) {
          key_capture_commit(dialog, index, (unsigned short)g_controller_dialog_62c588[index]);
          break;
        }
      }
    }
  } else if (cfg->device == CONTROLLER_DEVICE_KEYBOARD) {
    FUN_004c6600(key_bits, KEYBOARD_KEY_COUNT);
    if (keyboard_state_to_bits(key_bits) != 0) {
      for (index = 0; index < KEYBOARD_KEY_COUNT; index++) {
        if ((key_bits[index >> 3] & (1 << (index & 7))) != 0) {
          if (g_controller_dialog_771208[index] != 0 && index != DIK_ESCAPE && index != DIK_RETURN && index != DIK_UP &&
              index != DIK_DOWN && index != DIK_LEFT && index != DIK_RIGHT) {
            key_capture_commit(dialog, index, (unsigned short)g_controller_dialog_771208[index]);
          }
          break;
        }
      }
    }
  }
  if (cfg->device == CONTROLLER_DEVICE_PAD) {
    mirror_1 = cfg->bindings[1];
    mirror_2 = cfg->bindings[2];
    cfg->bindings[4] = cfg->bindings[0];
    cfg->bindings[5] = mirror_1;
    cfg->bindings[7] = mirror_2;
  }
}

/* entry thunk at the original address of controller_dialog_start_key_capture_cb_4a7310: register inputs (s0,s1,s2,s3) -> natural C controller_dialog_start_key_capture_cb_4a7310_n; ret 16 */
__declspec(naked) void controller_dialog_start_key_capture_cb_4a7310(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        call controller_dialog_start_key_capture_cb_4a7310_n
        add esp, 16
        ret 16
    }
}
