// cluster-entries: time_pack_centiseconds
/* Installed cluster time_pack_centiseconds (tools/cluster_install.py from output/cluster_pilot6/time_pack_centiseconds.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster time_pack_centiseconds: {minutes, seconds, hundredths} -> centiseconds. */
/* was: EAX = time record, the output pointer is the one stack argument */
void time_pack_centiseconds_n(const int *time, int *centiseconds)
{
    *centiseconds = (time[0] * 60 + time[1]) * 100 + time[2];
}

/* entry thunk at the original address of time_pack_centiseconds: register inputs (eax,s0) -> natural C time_pack_centiseconds_n; ret 0 */
__declspec(naked) void time_pack_centiseconds(void)
{
    __asm {
        push eax
        push dword ptr [esp + 8]
        push eax
        call time_pack_centiseconds_n
        add esp, 8
        pop eax
        ret
    }
}
