#include "mfl_tier2.h"
/*0042f620*/ extern int game_content_exists_prefixed();
/*004a5550*/ extern int log_printf();


/* game_content_find: finds which content prefix holds a file name; returns the prefix code (0 = not found). */

/* "Searching piz for %s": pc/ then raw then xbox/ ps2/ gamecube/ */

undefined4 game_content_find(undefined4 content_ctx,undefined4 file_name)   /* content_ctx: unknown */
{
  int exists;   /* nonzero if the file exists under the prefix */

  log_printf("Searching piz for %s \n",file_name);
  /* probe the prefix codes in this order: 4, 1, 3, 2, 5 (code meanings unknown) */
  exists = game_content_exists_prefixed(content_ctx,file_name,4);
  if (exists != 0) {
    return 4;
  }
  exists = game_content_exists_prefixed(content_ctx,file_name,1);
  if (exists != 0) {
    return 1;
  }
  exists = game_content_exists_prefixed(content_ctx,file_name,3);
  if (exists != 0) {
    return 3;
  }
  exists = game_content_exists_prefixed(content_ctx,file_name,2);
  if (exists != 0) {
    return 2;
  }
  exists = game_content_exists_prefixed(content_ctx,file_name,5);
  if (exists != 0) {
    return 5;
  }
  log_printf("Unable to find %s in piz\n",file_name);
  return 0;
}
