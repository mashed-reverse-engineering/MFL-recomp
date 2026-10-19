// cluster-entries: FUN_004acbb0
/* Installed cluster FUN_004acbb0 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004acbb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e2228*/ extern int PTR_LAB_005e2228[];
/*005e21e0*/ extern int PTR_FUN_005e21e0[];
/*005e21c8*/ extern int PTR_LAB_005e21c8[];
/*005e21a4*/ extern int PTR_LAB_005e21a4[];

static void FUN_004b0d90_r(void * p0, int p1, int p2, int p3, int p4, int p5);
extern void FUN_004b0d90(void);
/* Constructor: initialises the base part (FUN_004b0d90 with the owner `owner` and its sub object at +0x7c), stores the owner at +0xd8 and installs the vtable pointers (+0, +0xc, +0x10, +0x98). Returns this. */
void *FUN_004acbb0_n(int *this_, int owner, int arg1, int arg2)
{
  FUN_004b0d90_r(this_, 0, owner, owner + 0x7c, arg1, arg2);
  this_[0x36] = owner;
  this_[0] = (int)PTR_LAB_005e2228;
  this_[3] = (int)PTR_FUN_005e21e0;
  this_[4] = (int)PTR_LAB_005e21c8;
  this_[0x26] = (int)PTR_LAB_005e21a4;
  return this_;
}

/* FUN_004b0d90_r: C -> original FUN_004b0d90 (registers ecx,s0,s1,s2,s3,s4) */
__declspec(naked) void FUN_004b0d90_r(void * p0, int p1, int p2, int p3, int p4, int p5)
{
    __asm {
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        mov ecx, dword ptr [esp + 24]
        call FUN_004b0d90
        ret
    }
}

/* entry thunk at the original address of FUN_004acbb0: register inputs (ecx,s0,s1,s2) -> natural C FUN_004acbb0_n; ret 12 */
__declspec(naked) void FUN_004acbb0(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        call FUN_004acbb0_n
        add esp, 16
        ret 12
    }
}
