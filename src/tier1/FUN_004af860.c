#include "mfl_tier2.h"
/* layout of *self in FUN_004af860, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct FUN_004af860_param_1_layout {
    char pad_0x0[8];
    int f_0x8;
    char pad_0xc[4];
    int f_0x10;
    char pad_0x14[8];
    int f_0x1c;
} FUN_004af860_param_1_layout;

/*004b10c0*/ extern int __stdcall FUN_004b10c0(int a1, int a2, int a3);
/* FUN_004af860 */
undefined4 __stdcall FUN_004af860(FUN_004af860_param_1_layout *self, int *out)
{
  int sub_object;

  if (out == (int *)0x0) {
    return 0x80004003;
  }
  *out = (self->f_0x1c != 0) ? self->f_0x1c + 0xc : 0;
  sub_object = self->f_0x1c;
  if (sub_object != 0) {
    (*(void (__stdcall **)(int))(*(int *)(sub_object + 0xc) + 4))(sub_object + 0xc);
  }
  if (self->f_0x8 != 0) {
    FUN_004b10c0((int)(out + 2), self->f_0x8, 0x80);
  }
  else {
    *(undefined2 *)(out + 2) = 0;
  }
  out[1] = self->f_0x10;
  return 0;
}
