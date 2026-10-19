#include "mfl_tier2.h"
/*005dd340*/ extern float k_f_0_5;
/*0076e898*/ extern undefined4 g_d3dx_76e898;
static int gamma_apply_via_device_r(int reg_esi, int stk0, int stk1);
extern void gamma_apply_via_device(void);
/* register-convention callee gamma_apply_via_device (0x4a4110): ESI = device, two cdecl stack arguments */
__declspec(naked) int gamma_apply_via_device_r(int reg_esi, int stk0, int stk1)
{
    __asm {
        push esi
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov esi, dword ptr [esp + 16]
        call gamma_apply_via_device
        add esp, 8
        pop esi
        ret
    }
}

/* gamma_apply_via_device_scaled: applies gamma through gamma_apply_via_device_r using the scaled value 2*x + 0.5
 * (inferred). x is the float input, arg is passed through unchanged as the last argument; returns the callee's result. */

/* "gamma_apply_via_device_r(2*x + 0.5, arg)"
 * Plain cdecl, two stack arguments (float x, int arg); the decompiler's reg_ecx parameter does not exist
 * (ecx is only the scratch slot the float is pushed through).  The float result is passed as its 4 raw bytes. */
int __cdecl gamma_apply_via_device_scaled(float x,int arg)

{
  float scaled;

  scaled = x + x + k_f_0_5;
  return gamma_apply_via_device_r(g_d3dx_76e898,*(int *)&scaled,arg);
}
