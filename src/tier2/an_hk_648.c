#include "mfl_tier2.h"
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004b2a60*/ extern int sprintf();
/*004b43a9*/ extern int _ctime64();
/*004b43c2*/ extern int _ftime64();
/*004b4531*/ extern int FUN_004b4531(const char *p0, void *p1);
/*005e13fc*/ extern undefined g_log_open_5e13fc;
/*00770f74*/ extern undefined4 g_log_770f74;

#define LOG_TIMEB_SIZE (16)
#define LOG_LINE_SIZE (256)
/* log_open_write */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "mashed.log" */

/*
 * Opens mashed.log and writes a session header line, e.g. "Sat Oct 17 21:47:00\n\n"
 * (ctime text cut to 19 chars, so no year). The handle FUN_004b4531 returns is
 * stored in g_log_770f74 (inferred: the log file handle). The second argument is
 * the address of g_log_open_5e13fc (inferred: open mode / options block).
 * Returns 1 on success, 0 if the file could not be opened (nothing is written).
 */
undefined4 log_open_write(void)

{
  undefined4 ctime_text;
  struct {
    undefined1 local_114 [LOG_TIMEB_SIZE]; /* struct __timeb64 filled by _ftime64 */
    char local_104 [LOG_LINE_SIZE];        /* formatted line that gets logged */
  } log_entry;

  g_log_770f74 = FUN_004b4531("mashed.log",&g_log_open_5e13fc);
  /* open failed: nothing to log */
  if (g_log_770f74 == 0) {
    return 0;
  }
  /* current time -> ctime string -> "%.19s" line -> log */
  _ftime64(log_entry.local_114);
  ctime_text = _ctime64(log_entry.local_114);
  sprintf(log_entry.local_104,"%.19s\n\n",ctime_text);
  log_print(log_entry.local_104);
  return 1;
}


