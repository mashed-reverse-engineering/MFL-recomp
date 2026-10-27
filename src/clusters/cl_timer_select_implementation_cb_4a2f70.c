// cluster-entries: timer_select_implementation_cb_4a2f70
/* Installed cluster timer_select_implementation_cb_4a2f70 (tools/cluster_install.py from output/cluster_pilot_k/timer_select_implementation_cb_4a2f70.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetACP(void);
extern __declspec(dllimport) int __stdcall GetThreadLocale(void);
extern __declspec(dllimport) int __stdcall GetLocaleInfoA(int, int, char *, int);
/* timer_select_implementation_cb_4a2f70 */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* function pointer pushed/stored by timer_select_implementation (PUSH EAX) */

unsigned int timer_select_implementation_cb_4a2f70(void)

{
  unsigned long Locale;
  int iVar1;
  char *pcVar2;
  unsigned int UVar3;
  struct {
    char cStack_c;
    char acStack_b [7];
  } frame;
  
  UVar3 = 0;
  Locale = GetThreadLocale();
  iVar1 = GetLocaleInfoA(Locale,0x1004,&frame.cStack_c,7);
  if ((iVar1 != 0) && (pcVar2 = &frame.cStack_c, frame.cStack_c != '\0')) {
    do {
      pcVar2 = pcVar2 + 1;
      UVar3 = frame.cStack_c + -0x30 + UVar3 * 10;
      frame.cStack_c = *pcVar2;
    } while (frame.cStack_c != '\0');
    if (UVar3 != 0) {
      return UVar3;
    }
  }
  UVar3 = GetACP();
  return UVar3;
}

