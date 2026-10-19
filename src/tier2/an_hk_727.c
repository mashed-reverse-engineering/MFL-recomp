#include "mfl_tier2.h"
/*004b4244*/ extern int __cdecl _fputs(char *p0, FILE *p1);
/*004b42b5*/ extern int vsprintf();
/*00770f74*/ extern undefined4 g_log_770f74;
#define LOG_BUFFER_SIZE (512)
/* log_printf */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* debug log (format and arguments)
 * Writes a printf-style debug message to the log file g_log_770f74.
 * format: printf format string. The values for it follow on the stack
 * and are read by vsprintf through &first_arg (first_arg is the first of them).
 * Does nothing when g_log_770f74 is NULL (logging off). Returns nothing.
 * Caveat: the 512-byte buffer is not length-checked; vsprintf writes
 * unbounded, so a message over 511 characters overflows it. */

void log_printf(undefined4 format, undefined4 first_arg)

{
  char line_buffer[LOG_BUFFER_SIZE];

  /* log file open: format the message and write it out */
  if (g_log_770f74 != 0) {
    vsprintf(line_buffer,format,&first_arg);
    _fputs((char *)line_buffer,(FILE *)g_log_770f74);
  }
  return;
}


