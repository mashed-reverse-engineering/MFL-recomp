#include "mfl_tier2.h"
/*004b4244*/ extern int __cdecl _fputs(char *p0, FILE *p1);
/*00770f74*/ extern undefined4 g_log_770f74;


/* log_print */

/* debug log (string only) */

void log_print(char *_Str)
{
  if (g_log_770f74 != (FILE *)0x0) {
    _fputs(_Str,g_log_770f74);
  }
  return;
}
