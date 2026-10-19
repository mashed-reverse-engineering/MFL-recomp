#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall OutputDebugStringA(int);
/*004b42b5*/ extern int vsprintf();
#define DEBUG_PRINTF_BUFFER_SIZE (512)
/* debug_printf */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "vsprintf into 512 bytes then OutputDebugStringA" */

/* Printf-style debug logging: formats `format` with the arguments that follow it and sends the
   result to the debugger via OutputDebugStringA. No return value.
   format: printf format string.
   first_arg: address of the first variadic argument (inferred: vsprintf reads the rest from here).
   Note: vsprintf is unbounded, so a message longer than DEBUG_PRINTF_BUFFER_SIZE overflows buffer. */
void debug_printf(undefined4 format, undefined4 first_arg)

{
  CHAR buffer [DEBUG_PRINTF_BUFFER_SIZE];

  vsprintf(buffer,format,&first_arg);
  OutputDebugStringA((int)buffer);
  return;
}


