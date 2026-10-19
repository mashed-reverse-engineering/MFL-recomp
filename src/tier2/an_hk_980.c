#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DialogBoxParamA(int, int, int, int, int);
extern char LAB_004a7c40[];
/*004a9380*/ extern int app_get_app_7e7704();
/*004a9390*/ extern int app_get_app_7e7700();
/*007711a0*/ extern undefined4 g_controller_dialog_7711a0;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
static undefined4 controller_cfg_save_r(int reg_eax);
extern void controller_cfg_save(void);
__declspec(naked) undefined4 controller_cfg_save_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call controller_cfg_save
        ret
    }
}

/* controller_dialog_close */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "controller_cfg_save on OK" */

typedef struct { uint d[0x80]; } ControllerCfgWords;      /* the first 0x200 bytes of a controller config */

int __cdecl controller_dialog_close(int controller_index)

{
  int hwnd;
  int instance;
  int dialog_result;
  ControllerCfg *cfg;
  union {
    char gs_buf[0x200];      /* char array: /GS places the cookie after it */
    uint words[0x80];
  } backup;      /* copy of the controller config */
  
  if (g_controller_cfg[controller_index].device == 0) {
    return 1;
  }
  g_controller_dialog_7711a0 = controller_index;
  cfg = g_controller_cfg + controller_index;
  *(ControllerCfgWords *)&backup = *(ControllerCfgWords *)cfg;
  hwnd = app_get_app_7e7704();
  instance = app_get_app_7e7700();
  dialog_result = DialogBoxParamA(instance,0x68,hwnd,(int)&LAB_004a7c40,0);
  if (dialog_result == 1) {
    controller_cfg_save_r(controller_index);
  }
  else if (dialog_result == 0) {
    *(ControllerCfgWords *)cfg = *(ControllerCfgWords *)&backup;
  }
  return dialog_result;
}
