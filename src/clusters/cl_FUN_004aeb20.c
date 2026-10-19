// cluster-entries: FUN_004aeb20
/* Installed cluster FUN_004aeb20 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004aeb20.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e23a0*/ extern int PTR_LAB_005e23a0[];
/*005e0e80*/ extern int PTR_FUN_005e0e80[];
/*005e238c*/ extern int PTR_LAB_005e238c[];
/*005e2368*/ extern int PTR_LAB_005e2368[];
/*005e2354*/ extern int PTR_LAB_005e2354[];

static void FUN_004adc90_r(void * p0, int p1, int p2, int p3, int p4);
extern void FUN_004adc90(void);
static void FUN_004acf00_r(void * p0);
extern void FUN_004acf00(void);
/* Constructor: base part FUN_004adc90 with the four arguments, installs the vtable pointers (+0, +0xc, +0x10, +0xe0, +0xe4), clears +0xec / +0x120 / +0x124, runs FUN_004acf00. Returns this. */
void *FUN_004aeb20_n(int *this_, int a, int b, int c, int d)
{
  FUN_004adc90_r(this_, a, b, c, d);
  this_[0] = (int)PTR_LAB_005e23a0;
  this_[3] = (int)PTR_FUN_005e0e80;
  this_[4] = (int)PTR_LAB_005e238c;
  this_[0x38] = (int)PTR_LAB_005e2368;
  this_[0x39] = (int)PTR_LAB_005e2354;
  this_[0x3b] = 0;
  this_[0x48] = 0;
  this_[0x49] = 0;
  FUN_004acf00_r(this_);
  return this_;
}

/* FUN_004adc90_r: C -> original FUN_004adc90 (registers ecx,s0,s1,s2,s3) */
__declspec(naked) void FUN_004adc90_r(void * p0, int p1, int p2, int p3, int p4)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov ecx, dword ptr [esp + 20]
        call FUN_004adc90
        ret
    }
}

/* FUN_004acf00_r: C -> original FUN_004acf00 (registers ecx) */
__declspec(naked) void FUN_004acf00_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004acf00
        ret
    }
}

/* entry thunk at the original address of FUN_004aeb20: register inputs (ecx,s0,s1,s2,s3) -> natural C FUN_004aeb20_n; ret 16 */
__declspec(naked) void FUN_004aeb20(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push ecx
        call FUN_004aeb20_n
        add esp, 20
        ret 16
    }
}
