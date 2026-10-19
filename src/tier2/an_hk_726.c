#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall MessageBoxA(int, int, int, int);
/*004b3b74*/ extern int _vsnprintf();

/* size of the message buffer in chars; vsnprintf is capped at it */
#define MESSAGE_BUFFER_SIZE (0x3ff)
/* MessageBoxA uType: error icon, OK button (WinUser.h MB_ICONERROR) */
#define MB_ICONERROR (0x10)

/* app_error_message_box */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "vsnprintf into a 0x3FF buffer and MessageBoxA titled 'MASHED'" (was: "vsprintf ...") */

/* Shows a printf-style error message in an error message box titled "MASHED".
   format_string: printf format string.
   first_vararg: address of the first variadic argument; the rest follow it in memory (va_list style), so it is not a value.
   Returns nothing. */

void app_error_message_box(undefined4 format_string, undefined4 first_vararg)

{
  CHAR message_buffer [1023];
  undefined1 buffer_terminator;

  _vsnprintf(message_buffer,MESSAGE_BUFFER_SIZE,format_string,&first_vararg);
  /* last byte of the buffer (inferred): extra NUL, vsnprintf already terminates within the first 1023 bytes */
  buffer_terminator = 0;
  MessageBoxA(0,(int)message_buffer,(int)"MASHED",MB_ICONERROR);
  return;
}


