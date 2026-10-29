#include "mfl_tier2.h"
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*00560a60*/ extern RwBool RtFSManagerRegister(struct RtFileSystem *p0);
/*005616f0*/ extern char *FUN_005616f0(undefined4 p0, void *p1, void *p2);
/*005dd5a2*/ extern undefined g_game_5dd5a2;
/*006ad084*/ extern undefined4 g_game_6ad084;
/* Creates the root RtFileSystem (via FUN_005616f0; the returned object is stored in g_game_6ad084) and registers it with the RenderWare file system manager. */
/*
 * game_register_root_filesystem
 * Creates the root RtFileSystem (via FUN_005616f0; the returned object is stored
 * in g_game_6ad084) and registers it with the RenderWare file system manager.
 * Takes no parameters, returns nothing; prints success or failure to stdout.
 * (inferred: FUN_005616f0 is a create-file-system call; the 5 and the two
 * g_game_5dd5a2 addresses are its arguments, meaning unknown)
 */
void game_register_root_filesystem(void)
{
  RwBool registered;

  /* Create the root file system object and keep it in the global. */
  g_game_6ad084 = (RtFileSystem *)FUN_005616f0(5,&g_game_5dd5a2,&g_game_5dd5a2);
  /* Hand it to the FS manager; nonzero return means success. */
  registered = RtFSManagerRegister(g_game_6ad084);
  if (registered != 0) {
    FID_conflict__wprintf("RtFSManagerRegister successful\n");
    return;
  }
  FID_conflict__wprintf("RtFSManagerRegister failed\n");
  return;
}
