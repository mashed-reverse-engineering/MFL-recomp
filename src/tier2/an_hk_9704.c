/* compile: /GS */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetDlgItem(int dialog, int id);
extern __declspec(dllimport) int __stdcall LoadBitmapA(int instance, int id);
extern __declspec(dllimport) int __stdcall LoadStringA(int instance, unsigned int id, char *buffer, int max);
extern __declspec(dllimport) int __stdcall SetWindowTextA(int window, const char *text);
extern __declspec(dllimport) int __stdcall ShowWindow(int window, int command);
extern __declspec(dllimport) int __stdcall SendMessageA(int window, unsigned int message, unsigned int wparam, int lparam);
extern char *strcat(char *dst, const char *src);
extern int sprintf(char *dst, const char *format, ...);
/*004a9390*/ extern int app_get_app_7e7700(void);
/*004a93b0*/ extern void __cdecl dialog_item_set_text(int item, unsigned int string_id);
/*0062c588*/ extern unsigned short g_controller_dialog_62c588[];   /* bitmap resource id per gamepad binding (stride 4 bytes) */
/*007711a0*/ extern int g_controller_dialog_7711a0;                /* the controller being configured */
/*00771208*/ extern unsigned char *g_controller_dialog_771208;      /* the same for the keyboard */
/*0076e874*/ extern int g_language_offset;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];

#define DEVICE_GAMEPAD 1
#define DEVICE_KEYBOARD 2
#define STM_SETIMAGE 0xf7
#define FIRST_TEXT_ID 0x83e   /* string ids are g_language_offset + these */

/* A dialog label (control id) and the offset of its text in the language string table. */
typedef struct DialogText { int control; int string; } DialogText;

/* Fills the controller setup dialog `dialog`: title "<name of the controller being configured>", the labels from the language strings (the first two
   depend on the device), one binding picture per control (13 bindings) and hides the controls that the device does not have. */
/* TIER2: unvalidated 2026-10-29 authority=diffemu */
void controller_dialog_init(int dialog)
{
  DialogText labels[12] = {
    { 0x3fa, 0x85c }, { 0x3fc, 0x866 }, { 0x402, 0x870 }, { 0x406, 0x87a }, { 0x408, 0x884 }, { 0x40a, 0x88e }, { 0x40c, 0x898 },
    { 0x40e, 0x8a2 }, { 0x415, 0x8ac }, { 0x416, 0x8b6 }, { 0x410, 2000 }, { 0x411, 0x7da }
  };
  int hidden_controls[8] = { 0x409, 0x40b, 0x40d, 0x40f, 0x408, 0x40a, 0x40c, 0x40e };
  int keyboard_hidden_controls[4] = { 0x403, 0x402, 0x41b, 0x41c };
  int binding_controls[13] = { 0x3f6, 0x3f9, 0x3fb, 0x3fd, 0x3ff, 0x401, 0x403, 0x405, 0x407, 0x409, 0x40b, 0x40d, 0x40f };
  char title[256] = "";
  char suffix[128] = "";
  int instance, i;
  struct ControllerCfg *cfg;

  instance = app_get_app_7e7700();
  cfg = &g_controller_cfg[g_controller_dialog_7711a0];
  LoadStringA(app_get_app_7e7700(), g_language_offset + FIRST_TEXT_ID, title, 256);
  sprintf(suffix, " %d  -  %s", g_controller_dialog_7711a0 + 1, cfg->name);
  strcat(title, suffix);
  SetWindowTextA(dialog, title);

  if (cfg->device == DEVICE_GAMEPAD) {
    dialog_item_set_text(GetDlgItem(dialog, 0x3f7), g_language_offset + 0x848);
    dialog_item_set_text(GetDlgItem(dialog, 0x3f8), g_language_offset + 0x852);
  } else if (cfg->device == DEVICE_KEYBOARD) {
    dialog_item_set_text(GetDlgItem(dialog, 0x3f7), g_language_offset + 0x8f2);
    dialog_item_set_text(GetDlgItem(dialog, 0x3f8), g_language_offset + 0x8fc);
  }
  for (i = 0; i < 12; i++)
    dialog_item_set_text(GetDlgItem(dialog, labels[i].control), g_language_offset + labels[i].string);

  for (i = 0; i < 13; i++) {
    int picture = GetDlgItem(dialog, binding_controls[i]);
    int bitmap = 0;
    if (cfg->device == DEVICE_GAMEPAD)
      bitmap = LoadBitmapA(instance, g_controller_dialog_62c588[cfg->bindings[i] * 2]);
    else if (cfg->device == DEVICE_KEYBOARD)
      bitmap = LoadBitmapA(instance, *(unsigned short *)(g_controller_dialog_771208 + cfg->bindings[i] * 4));
    SendMessageA(picture, STM_SETIMAGE, 0, bitmap);
  }

  for (i = 0; i < 8; i++)
    ShowWindow(GetDlgItem(dialog, hidden_controls[i]), 0);
  if (cfg->device == DEVICE_KEYBOARD)
    for (i = 0; i < 4; i++)
      ShowWindow(GetDlgItem(dialog, keyboard_hidden_controls[i]), 0);
}
