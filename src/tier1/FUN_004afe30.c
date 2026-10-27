#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* FUN_004afe30 */
/* Sets the flag byte at +0x95 of obj under its critical section (the counterpart FUN_004afe60 clears it). */
undefined4 __stdcall FUN_004afe30(char *obj)
{
  int lpCriticalSection;
  
  lpCriticalSection = *(int *)(obj + 0x14);
  EnterCriticalSection(lpCriticalSection);
  *(undefined1 *)(obj + 0x95) = 1;
  LeaveCriticalSection(lpCriticalSection);
  return 0;
}
