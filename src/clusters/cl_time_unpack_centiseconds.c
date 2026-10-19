// cluster-entries: time_unpack_centiseconds
/* Installed cluster time_unpack_centiseconds (tools/cluster_install.py from output/cluster_pilot6/time_unpack_centiseconds.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster time_unpack_centiseconds: centiseconds -> {minutes, seconds, hundredths}. */
/* was: EAX = centiseconds, EDI = time record */
void time_unpack_centiseconds_n(int centiseconds, int *time)
{
    int minutes = centiseconds / 6000;
    int seconds = (centiseconds - minutes * 6000) / 100;

    time[0] = minutes;
    time[2] = centiseconds - (minutes * 60 + seconds) * 100;
    time[1] = seconds;
}

/* entry thunk at the original address of time_unpack_centiseconds: register inputs (eax,edi) -> natural C time_unpack_centiseconds_n; ret 0 */
__declspec(naked) void time_unpack_centiseconds(void)
{
    __asm {
        push edi
        push eax
        call time_unpack_centiseconds_n
        add esp, 8
        ret
    }
}
