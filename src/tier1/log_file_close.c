#include "mfl_tier2.h"
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*00770f74*/ extern undefined4 g_log_770f74;


/* log_file_close */

/* "fclose(g_log_770f74)" */

void log_file_close(void)
{
  if (g_log_770f74 != (FILE *)0x0) {
    _fclose(g_log_770f74);
    g_log_770f74 = (FILE *)0x0;
  }
  return;
}
