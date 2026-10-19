#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CloseHandle(int);
#define OBJ_HANDLE_OFFSET (0xc)
/* FUN_005c93c0
 * Closes the Win32 HANDLE stored at OBJ_HANDLE_OFFSET inside obj.
 * obj: pointer to the owning object (inferred). Returns nothing;
 * the CloseHandle result is ignored. */

void FUN_005c93c0(char *obj)

{
  CloseHandle(*(HANDLE *)(obj + OBJ_HANDLE_OFFSET));
  return;
}


