#include "mfl_tier2.h"
/*0042bd00*/ extern int dat_get_string();
/*0047fed0*/ extern int hud_label_find_free();
/*00480020*/ extern int wstring_length_skip();
static int hud_label_set_params_r(int reg_ecx, int reg_eax, int stk0, int stk1, int stk2, int stk3, int stk4);
extern void hud_label_set_params(void);

/* default value for the first stack argument of hud_label_set_params (inferred: 0x5dc = 1500) */
#define HUD_LABEL_DEFAULT_PARAM (0x5dc)

__declspec(naked) int hud_label_set_params_r(int reg_ecx, int reg_eax, int stk0, int stk1, int stk2, int stk3, int stk4)
{
    __asm {
        push dword ptr [esp + 28]
        push dword ptr [esp + 28]
        push dword ptr [esp + 28]
        push dword ptr [esp + 28]
        push dword ptr [esp + 28]
        mov ecx, dword ptr [esp + 24]
        mov eax, dword ptr [esp + 28]
        call hud_label_set_params
        add esp, 20
        ret
    }
}

/* hud_label_add_from_dat_string
 * Adds a HUD label whose text comes from the dat file.
 * Takes a free label slot from hud_label_find_free(); if there is none, does nothing.
 * Otherwise looks up the string for dat_string_id, copies it into the slot (inferred from the
 * wstring_length_skip() call), and sets the slot's params with hud_label_set_params(): label_arg is
 * passed in ecx, HUD_LABEL_DEFAULT_PARAM is the first stack argument, extra_param_1..4 follow.
 * Returns nothing.
 * (was: "allocates a label, copies the dat string and sets its params (0x5dc)")
 */

void __cdecl hud_label_add_from_dat_string(undefined4 dat_string_id,undefined4 label_arg,undefined4 extra_param_1,undefined4 extra_param_2,
          undefined4 extra_param_3,undefined4 extra_param_4)

{
  int free_label;
  undefined4 dat_string;

  free_label = hud_label_find_free();
  /* only when a free slot exists */
  if (free_label != 0) {
    dat_string = dat_get_string(dat_string_id);
    wstring_length_skip(free_label,dat_string);
    hud_label_set_params_r(label_arg,free_label, HUD_LABEL_DEFAULT_PARAM, extra_param_1, extra_param_2, extra_param_3, extra_param_4);
  }
  return;
}


