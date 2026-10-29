#include "mfl_tier2.h"
/*00403450*/ extern void __cdecl game_path_to_8_3_upper(char *dst, const char *path);
/*004a5530*/ extern void __cdecl log_print(const char *text);
/*004a5550*/ extern void log_printf(const char *format, ...);
/*004c66f0*/ extern int __cdecl piz_open_archive_bool(const char *name_8_3);

/* compile: /GS */
/* Opens the piz archive `path` (through its upper case 8.3 name) and logs whether that worked. */
/* TIER2: unvalidated 2026-10-29 authority=diffemu */
void game_open_piz(const char *path)
{
  char name_8_3[64];

  game_path_to_8_3_upper(name_8_3, path);
  log_printf("Opening piz file %s\n", name_8_3);
  log_print(piz_open_archive_bool(name_8_3) ? "\tOK\n" : "\tFAILED\n");
}
