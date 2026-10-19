#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DialogBoxParamA(int, int, int, int, int);
extern char LAB_004a7fd0[];
/*004a48e0*/ extern int app_get_app_770f64();
/*004a4980*/ extern int controller_dialog_copy_name(int index, char *name);
/*004a9380*/ extern int app_get_app_7e7704();
/*004a9390*/ extern int app_get_app_7e7700();
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0062ad88*/ extern undefined g_controller_dialog_62ad88;
/*0062b188*/ extern undefined g_controller_dialog_62b188;
/*0062b588*/ extern undefined g_controller_dialog_62b588;
/*0062b988*/ extern undefined g_controller_dialog_62b988;
/*0062bd88*/ extern undefined g_controller_dialog_62bd88;
/*0062c188*/ extern undefined g_controller_dialog_62c188;
/*0076e874*/ extern undefined4 g_language_offset;
/*00771188*/ extern undefined4 g_controller_dialog_771188;
/*0077118c*/ extern undefined4 g_controller_dialog_77118c;
/*00771190*/ extern undefined4 g_controller_dialog_771190;
/*00771194*/ extern undefined4 g_controller_dialog_771194;
/*00771198*/ extern undefined4 g_controller_dialog_771198;
/*0077119c*/ extern undefined4 g_controller_dialog_77119c;
/*007711a4*/ extern undefined1 g_controller_dialog_7711a4[0x1000];
/*007711e8*/ extern undefined g_controller_dialog_7711e8;
/*00771208*/ extern undefined4 g_controller_dialog_771208;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
static undefined4 controller_cfg_load_r(int reg_eax);
extern void controller_cfg_load(void);
__declspec(naked) undefined4 controller_cfg_load_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call controller_cfg_load
        ret
    }
}

typedef struct { uint d[0x80]; } ControllerCfgWords;      /* the first 0x200 bytes of a controller config */
extern __declspec(dllimport) int __stdcall LoadStringA(int, int, int, int);
#define CONTROLLER_DIALOG_STRING_KEYBOARD (0x82a)
#define CONTROLLER_DIALOG_STRING_NONE (0x834)
#define CONTROLLER_COUNT (4)

/* controller_dialog_run */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "controller_cfg_load + DialogBoxParam" */

void controller_dialog_run(int show_dialog,int bitmap_set)

{
  uint joystick_count;
  uint controller_index;
  int slot_index;
  int loaded;
  int same;
  uint *binding;
  char *name_a;
  char *name_b;
  ControllerCfg *slot;
  int hwnd;
  undefined4 *bitmap_tables;
  union {
    char gs_buf[0x200];      /* char array: /GS places the cookie after it */
    uint words[0x80];
  } backup;      /* copy of the controller config */
  
  /* table of the six controller bitmap sets (g_controller_dialog_771188 .. 77119c), indexed by bitmap_set */
  bitmap_tables = &g_controller_dialog_771188;
  bitmap_tables[0] = (undefined4)(&g_controller_dialog_62ad88);
  bitmap_tables[1] = (undefined4)(&g_controller_dialog_62b188);
  bitmap_tables[2] = (undefined4)(&g_controller_dialog_62b588);
  bitmap_tables[3] = (undefined4)(&g_controller_dialog_62b988);
  bitmap_tables[4] = (undefined4)(&g_controller_dialog_62bd88);
  bitmap_tables[5] = (undefined4)(&g_controller_dialog_62c188);
  g_controller_dialog_771208 = bitmap_tables[bitmap_set];
  LoadStringA(app_get_app_7e7700(),g_language_offset + CONTROLLER_DIALOG_STRING_KEYBOARD,(int)g_controller_dialog_7711a4,0x20);
  LoadStringA(app_get_app_7e7700(),g_language_offset + CONTROLLER_DIALOG_STRING_NONE,(int)&g_controller_dialog_7711e8,0x20);
  joystick_count = app_get_app_770f64();
  mem_zero(g_controller_cfg,0x800);
  controller_index = 0;
  slot = g_controller_cfg;
  do {
    slot->version = 6;
    binding = slot->bindings;
    if ((int)controller_index < (int)joystick_count) {
      slot->device = 1;
      slot->joystick = controller_index;
      controller_dialog_copy_name(controller_index,slot->name);
      binding[0] = 0;
      binding[1] = 1;
      binding[2] = 2;
      binding[3] = 3;
      binding[4] = 0;
      binding[5] = 1;
      binding[6] = 4;
      binding[7] = 2;
      binding[8] = 5;
    }
    else if (controller_index == joystick_count) {
      slot->device = 2;
      name_a = g_controller_dialog_7711a4;
      name_b = slot->name;
      do {
        *name_b = *name_a;
        name_b = name_b + 1;
        name_a = name_a + 1;
      } while (name_a[-1] != '\0');
      binding[0] = 0x1f;
      binding[1] = 0x2d;
      binding[2] = 0x1e;
      binding[3] = 0x20;
      binding[4] = 0x1c;
      binding[5] = 1;
      binding[6] = 1;
      binding[7] = 0x1e;
      binding[8] = 0x2e;
      binding[9] = 0xcb;
      binding[10] = 0xcd;
      binding[11] = 200;
      binding[12] = 0xd0;
    }
    else {
      slot->device = 0;
    }
    slot = slot + 1;
    controller_index = controller_index + 1;
  } while ((int)controller_index < CONTROLLER_COUNT);
  slot_index = 0;
  slot = g_controller_cfg;
  do {
    *(ControllerCfgWords *)&backup = *(ControllerCfgWords *)slot;
    loaded = controller_cfg_load_r(slot_index);
    if (loaded != 0) {
      /* keep the loaded config only when it is for the same device name */
      same = 0;
      if (slot->version == backup.words[0]) {
        name_a = slot->name;
        name_b = (char *)&backup.words[1];
        same = 1;
        while (*name_a == *name_b) {
          if (*name_a == '\0') break;
          name_a = name_a + 1;
          name_b = name_b + 1;
        }
        if (*name_a != *name_b) same = 0;
      }
      if (same == 0) {
        *(ControllerCfgWords *)slot = *(ControllerCfgWords *)&backup;
      }
    }
    slot_index = slot_index + 1;
    slot = slot + 1;
  } while (slot_index < CONTROLLER_COUNT);
  if (show_dialog != 0) {
    hwnd = app_get_app_7e7704();
    DialogBoxParamA(app_get_app_7e7700(),0x67,hwnd,(int)&LAB_004a7fd0,0);
  }
  return;
}
