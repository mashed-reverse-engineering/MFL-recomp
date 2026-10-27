#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall LoadStringA(int, unsigned int, char *, int);
extern __declspec(dllimport) int __stdcall SetWindowTextA(int, int);
/*007e7700*/ extern undefined4 g_app_7e7700;
/* dialog_item_set_text */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "LoadString + SetWindowText for dialog controls" */
void dialog_item_set_text(int control,unsigned int string_id)
{
  char text [256];
  
  LoadStringA(g_app_7e7700,string_id,text,0x100);
  SetWindowTextA(control,(int)text);
  return;
}
