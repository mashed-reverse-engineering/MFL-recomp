#include "mfl_tier2.h"
/* gap_5d01c0: IEnumPins::Clone of the RWA DirectShow pin enumerator (vtable 0x64be68 slot 17).  The enumerator constructor
   0x005cfc80 is an int3 trap row (tools/delink/rw_trap.txt, lane AU W7): called by its original address, not by name (trapcheck).
   This function is itself reached only through vtables that trapped AU2 code installs, so the original would trap the same way. */
#define FUN_005cfc80 ((int (__cdecl *)(int a1, int a2, int a3))0x005cfc80)

/* no context */

int __stdcall gap_5d01c0(undefined4 param_1, int *param_2)

{
  int iVar1;

  iVar1 = FUN_005cfc80(param_1,0,0);
  *param_2 = iVar1;
  return (-(uint)(iVar1 != 0) & 0x7ff8fff2) + 0x8007000e;
}


