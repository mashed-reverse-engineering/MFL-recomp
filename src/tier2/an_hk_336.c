#include "mfl_tier2.h"
/*004a8420*/ extern int debug_printf();
/*004b322b*/ extern __declspec(noreturn) void __cdecl exit(int);
#define EXIT_STATUS_FAILURE (0xffffffff)
/* line_rain_error */

/* Fatal error handler for the LineRain subsystem: prints the error code and message
   through debug_printf, then exits the process with EXIT_STATUS_FAILURE (-1).
   message: NUL-terminated text to print (%s). error_code: numeric code to print (%d).
   Does not return. */

void __cdecl line_rain_error(undefined4 message,undefined4 error_code)

{
  debug_printf("LineRain error [%d] : %s\n",error_code,message);
                    /* WARNING: Subroutine does not return */
  exit(EXIT_STATUS_FAILURE);
}


