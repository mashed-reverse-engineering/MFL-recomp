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

/* Return the ANSI code page of the thread locale (LOCALE_IDEFAULTANSICODE 0x1004 parsed as decimal), falling back to GetACP() (inferred). */
unsigned int timer_select_implementation_cb_4a2f70(void)

{
  unsigned long locale_id;
  int got_info;
  char *digit_ptr;
  unsigned int codepage;
  struct {
    char digit_char;
    char digits_rest [7];
  } frame;
  
  codepage = 0;
  locale_id = GetThreadLocale();
  got_info = GetLocaleInfoA(locale_id,0x1004,&frame.digit_char,7);
  if ((got_info != 0) && (digit_ptr = &frame.digit_char, frame.digit_char != '\0')) {
    do {
      digit_ptr = digit_ptr + 1;
      codepage = frame.digit_char + -0x30 + codepage * 10;
      frame.digit_char = *digit_ptr;
    } while (frame.digit_char != '\0');
    if (codepage != 0) {
      return codepage;
    }
  }
  codepage = GetACP();
  return codepage;
}

