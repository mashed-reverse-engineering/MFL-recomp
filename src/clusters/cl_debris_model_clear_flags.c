// cluster-entries: debris_model_clear_flags
/* Installed cluster debris_model_clear_flags (tools/cluster_install.py from output/cluster_pilot6/debris_model_clear_flags.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster debris_model_clear_flags: clears the flags 0xc of the object FUN_0058c990 finds for the model and hands the object to FUN_0058c8b0; returns the object. */
extern int *FUN_0058c990(void *model);
extern void FUN_0058c8b0(int arg, int *object);

/* was: EAX = model, the second argument is the one stack argument */
int *debris_model_clear_flags_n(void *model, int arg)
{
    int *object = FUN_0058c990(model);

    object[2] = object[2] & 0xfffffff3;
    FUN_0058c8b0(arg, object);
    return object;
}

/* entry thunk at the original address of debris_model_clear_flags: register inputs (eax,s0) -> natural C debris_model_clear_flags_n; ret 0 */
__declspec(naked) void debris_model_clear_flags(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        call debris_model_clear_flags_n
        add esp, 8
        ret
    }
}
