// cluster-entries: joypads_poll_frame
/* Installed cluster joypads_poll_frame (tools/cluster_install.py from output/cluster_pilot_x2/joypads_poll_frame.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00652f64*/ extern int g_race_652f64;
/*00770fb4*/ extern int g_app_770fb4;
/*00770fb8*/ extern int g_joypads_poll_770fb8[12];
/*00770fe8*/ extern int g_app_770fe8;
/*00770fec*/ extern int g_joypads_poll_770fec;
/*00770ff0*/ extern int g_joypads_poll_770ff0;
/*00770ff4*/ extern int g_joypads_poll_770ff4;
/*00770ff8*/ extern int g_joypads_poll_770ff8;
/*00770ffc*/ extern int g_joypads_poll_770ffc;
/*00771000*/ extern int g_joypads_poll_771000;
/*00771004*/ extern int g_joypads_poll_771004;
/*00771008*/ extern int g_joypads_poll_771008;
/*0077100c*/ extern int g_joypads_poll_77100c;
/*00771010*/ extern unsigned char g_joypads_poll_771010[32];
/*008c7e58*/ extern signed char g_controller_8c7e58;
/*008c7e59*/ extern signed char g_input_8c7e59;
/*008c7e5a*/ extern signed char g_input_8c7e5a;
/*008c7e5b*/ extern signed char g_input_8c7e5b;
/*005dd620*/ extern float float_10_0;
/*004a5130*/ extern void joypads_release_all(void);
/*0050b5e0*/ extern void nop_stub(void);
/*004a6f20*/ extern void joypads_poll_axes_and_keyboard(void);
/*004c66a0*/ extern void __cdecl FUN_004c66a0(void *memory, int size);
/*004a5250*/ extern int __cdecl keyboard_state_to_bits(unsigned char *key_bits);
/*004a70c0*/ extern int __cdecl controller_present_004a70c0(int controller_index);
/*004a9490*/ extern void video_get_window_size_a(float *out_size);
/*004a94b0*/ extern void video_get_window_size_b(int *out_width, int *out_height);

#define KEY_BITS g_joypads_poll_771010
#define KEY_BIT(n) (((1 << ((n) & 7)) & KEY_BITS[(n) >> 3]) != 0)
#define BUTTON_FIRST_BIT 0x3b
#define BUTTON_COUNT 12
#define STICK_LIMIT 0x7f

static int controller_poll_player_r(int p0);
extern void controller_poll_player(void);
/* clamp v to -0x7f..0x7f */
static int clamp_stick(int v)
{
  if (v < -STICK_LIMIT) {
    return -STICK_LIMIT;
  }
  if (v > STICK_LIMIT) {
    return STICK_LIMIT;
  }
  return v;
}

/* joypads_poll_frame: per frame input poll. Clears and refreshes the keyboard bit set, polls every present controller, expands 12 button bits and the 10 direction/action
   bits into the int flags the game reads, and while a race runs turns the window size scale into the signed stick values of the mouse-like input record. */
void joypads_poll_frame(void)
{
  int controller;
  int i;
  int stick_x;
  int stick_y;
  float size[2];
  int width;
  int height;

  joypads_release_all();
  nop_stub();
  joypads_poll_axes_and_keyboard();
  FUN_004c66a0(KEY_BITS, 0x20);
  keyboard_state_to_bits(KEY_BITS);
  g_app_770fb4 = 0;
  for (controller = 0; controller < 4; controller++) {
    if (controller_present_004a70c0(controller) != 0) {
      controller_poll_player_r(controller);
      g_app_770fb4 = g_app_770fb4 + 1;
    }
  }
  for (i = 0; i < BUTTON_COUNT; i++) {
    g_joypads_poll_770fb8[i] = KEY_BIT(BUTTON_FIRST_BIT + i);
  }
  g_app_770fe8 = (KEY_BITS[1] >> 3) & 1;
  g_joypads_poll_770fec = (KEY_BITS[0] >> 2) & 1;
  g_joypads_poll_770ff0 = (KEY_BITS[0] >> 3) & 1;
  g_joypads_poll_770ff4 = (KEY_BITS[0] >> 4) & 1;
  g_joypads_poll_770ff8 = (KEY_BITS[0] >> 5) & 1;
  g_joypads_poll_771000 = KEY_BITS[0] >> 7;
  g_joypads_poll_770ffc = (KEY_BITS[0] >> 6) & 1;
  g_joypads_poll_771004 = KEY_BITS[1] & 1;
  g_joypads_poll_771008 = (KEY_BITS[1] >> 1) & 1;
  g_joypads_poll_77100c = (KEY_BITS[1] >> 2) & 1;
  if (g_race_652f64 != 0) {
    video_get_window_size_a(size);
    video_get_window_size_b(&width, &height);
    stick_x = clamp_stick((int)(size[0] * float_10_0));
    stick_y = clamp_stick((int)(size[1] * float_10_0));
    if (width != 0) {
      g_input_8c7e59 = (signed char)stick_x;
      g_controller_8c7e58 = -(signed char)stick_y;
      g_input_8c7e5a = 0;
      g_input_8c7e5b = 0;
    } else if (height != 0) {
      g_input_8c7e59 = 0;
      g_controller_8c7e58 = 0;
      g_input_8c7e5a = (signed char)stick_y;
      g_input_8c7e5b = (signed char)stick_x;
    }
  }
}

/* controller_poll_player_r: C -> original controller_poll_player (registers ebx) */
__declspec(naked) int controller_poll_player_r(int p0)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call controller_poll_player
        pop ebx
        ret
    }
}
