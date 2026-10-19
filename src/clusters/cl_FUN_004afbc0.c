// cluster-entries: FUN_004afbc0
/* Installed cluster FUN_004afbc0 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004afbc0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e201c*/ extern char g_data_5e201c[];

typedef int (__fastcall *ThisCall0)(void *this_);
typedef int (__fastcall *ThisCall1)(void *this_, int edx_unused, int a);
typedef int (__stdcall *Com1)(void *iface);
typedef int (__stdcall *Com2)(void *iface, void *a);
typedef int (__stdcall *Com3)(void *iface, void *a, void *b);

/* Reads the capabilities of the COM object `iface` into the record `self` (flags at +0x18: 1, 2, 4, 8, 0x110; sizes at +0x1c / +0x3c, pairs at +0x20 / +0x28 / +0x34 / +0x38),
   either directly or through the interface returned by QueryInterface(g_data_5e201c) (method +0x4c fills self + 0x10); then, when flag 8 is set and the base part (self - 0x98, vtable +0x20) rejects the value at +0x34,
   marks the object, calls the owner's method +0x38 and reports 0x8004022a through FUN_004aef40.  Returns an HRESULT. */
static int FUN_004aef40_r(void * p0, int p1, int p2, int p3);
extern void FUN_004aef40(void);
int FUN_004afbc0_n(char *self, int *iface)
{
  void *base = self - 0x98;
  int *sub = iface;  /* the original's out slot is the parameter slot of `iface` itself: it keeps that value when QueryInterface does not write it */
  int result;

  if (iface == 0) {
    return 0x80004003;
  }
  result = ((ThisCall0)(*(int **)base)[0xe])(base);
  if (result != 0) {
    return result;
  }
  result = ((Com3)(*(int **)iface)[0])(iface, g_data_5e201c, &sub);
  if (result < 0) {
    *(int *)(self + 0x10) = 0x30;
    *(int *)(self + 0x14) = 0;
    *(int *)(self + 0x30) = 0;
    *(int *)(self + 0x18) = 0;
    if (((Com1)(*(int **)iface)[0xf])(iface) == 0) {
      *(unsigned int *)(self + 0x18) |= 4;
    }
    if (((Com1)(*(int **)iface)[9])(iface) == 0) {
      *(unsigned int *)(self + 0x18) |= 2;
    }
    if (((Com1)(*(int **)iface)[7])(iface) == 0) {
      *(unsigned int *)(self + 0x18) |= 1;
    }
    if (((Com3)(*(int **)iface)[5])(iface, self + 0x20, self + 0x28) >= 0) {
      *(unsigned int *)(self + 0x18) |= 0x110;
    }
    if (((Com2)(*(int **)iface)[0xd])(iface, self + 0x34) == 0) {
      *(unsigned int *)(self + 0x18) |= 8;
    }
    ((Com2)(*(int **)iface)[3])(iface, self + 0x38);
    *(int *)(self + 0x1c) = ((Com1)(*(int **)iface)[0xb])(iface);
    *(int *)(self + 0x3c) = ((Com1)(*(int **)iface)[4])(iface);
  } else {
    int fill = ((Com3)(*(int **)sub)[0x13])(sub, (void *)0x30, self + 0x10);

    ((Com1)(*(int **)sub)[2])(sub);
    if (fill < 0) {
      return fill;
    }
  }
  if ((*(unsigned char *)(self + 0x18) & 8) == 0) {
    return 0;
  }
  if (((ThisCall1)(*(int **)base)[8])(base, 0, *(int *)(self + 0x34)) == 0) {
    return 0;
  }
  *(unsigned char *)(self - 0x74) = 1;
  ((Com1)(*(int **)(self - 0x8c))[0xe])(self - 0x8c);
  FUN_004aef40_r(*(void **)(self - 0x70), 3, (int)0x8004022a, 0);
  return 0x80040200;
}

/* FUN_004aef40_r: C -> original FUN_004aef40 (registers ecx,s0,s1,s2) */
__declspec(naked) int FUN_004aef40_r(void * p0, int p1, int p2, int p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 16]
        call FUN_004aef40
        ret
    }
}

/* entry thunk at the original address of FUN_004afbc0: register inputs (s0,s1) -> natural C FUN_004afbc0_n; ret 8 */
__declspec(naked) void FUN_004afbc0(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        call FUN_004afbc0_n
        add esp, 8
        ret 8
    }
}
