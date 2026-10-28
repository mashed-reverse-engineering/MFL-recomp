// cluster-entries: FUN_004acf00
/* Installed cluster FUN_004acf00 (tools/cluster_install.py from output/cluster_pilot9/FUN_004acf00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall timeGetTime(void);
/* FUN_004acf00 */

/* Resets a state record to its initial values and stamps the current time (timeGetTime) at +0x158 (inferred: timer/animation state, owner unknown). */
void FUN_004acf00_n(char *state)

{
  DWORD now_ms;
  
  *(undefined4 *)(state + 0x138) = 0xfffffc18;
  *(undefined4 *)(state + 0x13c) = 0xffffffff;
  now_ms = timeGetTime();
  *(DWORD *)(state + 0x158) = now_ms;
  *(undefined4 *)(state + 0xf4) = 0;
  *(undefined4 *)(state + 0x10c) = 0xffffffff;
  *(undefined4 *)(state + 0x110) = 0;
  *(undefined4 *)(state + 0xf8) = 0;
  *(undefined4 *)(state + 0x108) = 0;
  *(undefined4 *)(state + 0xfc) = 0;
  *(undefined4 *)(state + 0x124) = 0;
  *(undefined4 *)(state + 0x120) = 0;
  *(undefined4 *)(state + 0x128) = 0;
  *(undefined4 *)(state + 300) = 0;
  *(undefined4 *)(state + 0x130) = 0;
  *(undefined4 *)(state + 0x134) = 0;
  *(undefined4 *)(state + 0x140) = 0;
  *(undefined4 *)(state + 0x144) = 0;
  *(undefined4 *)(state + 0x154) = 0;
  *(undefined4 *)(state + 0x150) = 0;
  *(undefined4 *)(state + 0x148) = 0;
  *(undefined4 *)(state + 0x14c) = 0;
  *(undefined4 *)(state + 0xe8) = 0;
  *(undefined4 *)(state + 0x100) = 0;
  *(undefined4 *)(state + 0x104) = 0xfffb6c20;
  *(undefined4 *)(state + 0xf0) = 0;
  *(undefined4 *)(state + 0x118) = 0;
  *(undefined4 *)(state + 0x11c) = 0;
  return;
}

/* entry thunk at the original address of FUN_004acf00: register inputs (ecx) -> natural C FUN_004acf00_n; ret 0 */
__declspec(naked) void FUN_004acf00(void)
{
    __asm {
        push ecx
        call FUN_004acf00_n
        add esp, 4
        ret
    }
}
