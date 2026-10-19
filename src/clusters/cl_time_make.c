// cluster-entries: time_make
/* Installed cluster time_make (tools/cluster_install.py from output/cluster_pilot6/time_make.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster time_make: fills a {minutes, seconds, hundredths} record from minutes, seconds and a fraction of a second. */
extern float k_f_100;

/* was: ESI = time record, EAX = minutes, ECX = seconds, the fraction is the one stack argument */
void time_make_n(int *time, int minutes, int seconds, float fraction)
{
    time[0] = minutes;
    time[1] = seconds;
    time[2] = (int)(fraction * k_f_100);
}

/* entry thunk at the original address of time_make: register inputs (esi,eax,ecx,s0) -> natural C time_make_n; ret 0 */
__declspec(naked) void time_make(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        push eax
        push esi
        call time_make_n
        add esp, 16
        ret
    }
}
