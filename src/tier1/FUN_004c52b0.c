#include "mfl_tier2.h"
/*005542c0*/ extern int FUN_005542c0(undefined4 p0);
/*005542f0*/ extern int FUN_005542f0(undefined4 p0, undefined4 p1);
/*005543e0*/ extern int rw_second_field(int * p0);
/*005543f0*/ extern int __cdecl FUN_005543f0(int a1, int a2);
#define OWNER_HANDLE_OFFSET (0x18)
/* FUN_004c52b0 */
/*
 * (inferred) Takes the handle stored at OWNER_HANDLE_OFFSET inside `owner`,
 * passes it to FUN_005542c0, then runs it through FUN_005542f0 (with
 * first_arg) and rw_second_field, and finally FUN_005543f0 (with second_arg).
 * Returns nothing. first_arg / second_arg are only named by position (inferred).
 */
void FUN_004c52b0(char *owner,undefined4 first_arg,undefined4 second_arg)
{
  undefined4 handle;

  /* read the handle the owner object carries */
  handle = *(undefined4 *)(owner + OWNER_HANDLE_OFFSET);
  FUN_005542c0(handle);
  /* (inferred) FUN_005542f0 returns the handle that the rest of the chain uses */
  handle = FUN_005542f0(handle,first_arg);
  rw_second_field(handle);
  FUN_005543f0(handle,second_arg);
  return;
}
