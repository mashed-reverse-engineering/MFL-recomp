#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
/* layout of *param_1 in FUN_004afe60, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct FUN_004afe60_param_1_layout {
    char pad_0x0[20];
    int f_0x14;
    undefined1 f_0x18;
    char pad_0x19[124];
    undefined1 f_0x95;
} FUN_004afe60_param_1_layout;

extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* FUN_004afe60 */
undefined4 __stdcall FUN_004afe60(FUN_004afe60_param_1_layout *param_1)
{
  int lpCriticalSection;
  
  lpCriticalSection = param_1->f_0x14;
  EnterCriticalSection(lpCriticalSection);
  param_1->f_0x95 = 0;
  param_1->f_0x18 = 0;
  LeaveCriticalSection(lpCriticalSection);
  return 0;
}
