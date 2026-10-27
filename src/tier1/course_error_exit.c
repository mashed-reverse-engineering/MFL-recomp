#include "mfl_tier2.h"
/*004a8420*/ extern int debug_printf();
/*004b322b*/ extern __declspec(noreturn) void __cdecl exit(int);
/* course_error_exit */
/* "Exiting due to course error %d" */
/* Logs the course error and terminates the process with it as the exit code.
 * error_code: the course error number, printed in the log line and passed to exit().
 * Does not return. */
void course_error_exit(undefined4 error_code)
{
  debug_printf("Exiting due to course error %d\n",error_code);
  exit(error_code);
}
