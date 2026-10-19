#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetVersionExA(struct OsVersionInfo *);
extern __declspec(dllimport) int __stdcall InterlockedExchange(long *, long);
extern void (*PTR_timer_select_implementation_0062ad54)(void);
struct OsVersionInfo { uint dwOSVersionInfoSize; uint dwMajorVersion; uint dwMinorVersion; uint dwBuildNumber; uint dwPlatformId; char szCSDVersion[128]; };
extern char LAB_004a2f70[];
extern char LAB_004a2ff0[];
/* timer_select_implementation */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "picks a timer function by OS version and stores it (InterlockedExchange)" */

void timer_select_implementation(void)

{
  char *Value;
  struct OsVersionInfo local_98;
  
  local_98.dwOSVersionInfoSize = 0x94;
  GetVersionExA(&local_98);
  if ((local_98.dwPlatformId != 2) || (Value = LAB_004a2ff0, local_98.dwMajorVersion < 5)) {
    Value = LAB_004a2f70;
  }
  InterlockedExchange((long *)&PTR_timer_select_implementation_0062ad54,(long)Value);
  PTR_timer_select_implementation_0062ad54();
  return;
}


