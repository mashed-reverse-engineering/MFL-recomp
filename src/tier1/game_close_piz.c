#include "mfl_tier2.h"
/*004a5550*/ extern int log_printf();
/*004c6710*/ extern int piz_close_archive_004c6920();
/*004c6740*/ extern void FUN_004c6740(void *p0);
/* game_close_piz */
/* compile: /GS */
#define PIZ_FILENAME_BUFFER_SIZE (16556)
/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Closes the currently open piz archive. Takes no parameters and returns nothing.
   FUN_004c6740 fills a local buffer with the archive's file name (inferred), which is logged
   before piz_close_archive_004c6920 closes the archive. */
void game_close_piz(void)
{
  undefined1 piz_filename [PIZ_FILENAME_BUFFER_SIZE];

  FUN_004c6740(piz_filename);
  log_printf("Closing piz file %s\n",piz_filename);
  piz_close_archive_004c6920();
  return;
}
