#include "mfl_tier2.h"
/*004a4720*/ extern int game_register_native_filesystems();


/* filesystem_register_native_b */

/* "game_register_native_filesystems() != 0" */

/* Registers native filesystems and returns true if successful. */
bool filesystem_register_native_b(void)
{
  int registration_result;
  
  registration_result = game_register_native_filesystems();
  return registration_result != 0;
}
