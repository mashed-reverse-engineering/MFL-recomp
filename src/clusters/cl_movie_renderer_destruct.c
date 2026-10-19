// cluster-entries: movie_renderer_destruct
/* Installed cluster movie_renderer_destruct (tools/cluster_install.py from output/cluster_pilot6/movie_renderer_destruct.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster movie_renderer_destruct: restores the vtables of the movie renderer (base class set) and calls the base destructor. */
extern char PTR_LAB_005e0ec0[];
extern char PTR_FUN_005e0e80[];
extern char PTR_LAB_005e238c[];
extern char PTR_LAB_005e2368[];
extern char PTR_LAB_005e2354[];

static void FUN_004aead0_r(void * p0);
extern void FUN_004aead0(void);
/* was: ECX = this */
void movie_renderer_destruct_n(void **self)
{
    self[0] = PTR_LAB_005e0ec0;
    self[3] = PTR_FUN_005e0e80;
    self[4] = PTR_LAB_005e238c;
    self[0x38] = PTR_LAB_005e2368;
    self[0x39] = PTR_LAB_005e2354;
    FUN_004aead0_r(self);
}

/* FUN_004aead0_r: C -> original FUN_004aead0 (registers ecx) */
__declspec(naked) void FUN_004aead0_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004aead0
        ret
    }
}

/* entry thunk at the original address of movie_renderer_destruct: register inputs (ecx) -> natural C movie_renderer_destruct_n; ret 0 */
__declspec(naked) void movie_renderer_destruct(void)
{
    __asm {
        push eax
        push ecx
        push edx
        push ecx
        call movie_renderer_destruct_n
        add esp, 4
        pop edx
        pop ecx
        pop eax
        ret
    }
}
