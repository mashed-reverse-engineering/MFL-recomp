#include "mfl_tier2.h"
/*0056bf10*/ extern float FUN_0056bf10(void *p0, undefined4 p1, undefined4 p2);
/*0056ca70*/ extern void rwp_shape_set_property_c(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void rwp_shape_set_property_b(undefined4 p0, undefined4 p1);
/*0056cb10*/ extern void FUN_0056cb10(undefined4 p0, undefined4 p1);
/*0058cb90*/ extern char *rwp_body_create(void);
/*0058cbe0*/ extern void rwp_body_destroy(void *p0);
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

/* "physics body setup: FUN_0056cb10/cac0/ca70; the body (body) goes to debris_model_clear_flags in eax, model on the stack" */

undefined4 debris_body_configure(undefined4 model,undefined4 property_e,undefined4 property_d,undefined4 property_c,
          undefined4 property_b)

{
  undefined4 *body;
  undefined4 handle;
  
  body = (undefined4 *)rwp_body_create();
  handle = *body;
  FUN_0056cb10(handle,property_e);
  rwp_shape_set_property_b(handle,property_b);
  rwp_shape_set_property_c(handle,property_c);
  FUN_0056bf10(body,property_d,0);
  handle = debris_model_clear_flags_r((int)body, model);
  rwp_body_destroy(body);
  return handle;
}


