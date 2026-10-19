#include "mfl_tier2.h"
/*0056bf10*/ extern float FUN_0056bf10(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void FUN_0056cac0(undefined4 p0, undefined4 p1);
/*0056cb10*/ extern void FUN_0056cb10(undefined4 p0, undefined4 p1);
/*0058cb90*/ extern int FUN_0058cb90(void);
/*0058cbe0*/ extern void FUN_0058cbe0(undefined4 p0);
static int debris_model_clear_flags_r(int reg_eax, int stk0);
extern void debris_model_clear_flags(void);
__declspec(naked) int debris_model_clear_flags_r(int reg_eax, int stk0)
{
    __asm {
        push dword ptr [esp + 8]
        mov eax, dword ptr [esp + 8]
        call debris_model_clear_flags
        add esp, 4
        ret
    }
}

/* debris_body_configure */

/* "physics body setup: FUN_0056cb10/cac0/ca70; the body (puVar1) goes to debris_model_clear_flags in eax, param_1 on the stack" */

undefined4 debris_body_configure(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
          undefined4 param_5)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = (undefined4 *)FUN_0058cb90();
  uVar2 = *puVar1;
  FUN_0056cb10(uVar2,param_2);
  FUN_0056cac0(uVar2,param_5);
  FUN_0056ca70(uVar2,param_4);
  FUN_0056bf10((undefined4)puVar1,param_3,0);
  uVar2 = debris_model_clear_flags_r((int)puVar1, param_1);
  FUN_0058cbe0((undefined4)puVar1);
  return uVar2;
}


