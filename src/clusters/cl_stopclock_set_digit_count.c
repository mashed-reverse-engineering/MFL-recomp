// cluster-entries: stopclock_set_digit_count
/* Installed cluster stopclock_set_digit_count (tools/cluster_install.py from output/cluster_pilot5/stopclock_set_digit_count.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster stopclock_set_digit_count: stop clock panel digit count from the car value (stored as float at +0x4c, as int at +0x50, the float clamped to 4.0). */
typedef struct StopclockPanel {
    char unknown_00[0x4c];
    float value;           /* 0x4c */
    int digits;            /* 0x50 */
} StopclockPanel;

extern float float_1_0;
extern int ui_get_car_8c8070(void);

/* was: ESI = panel */
void stopclock_set_digit_count_n(StopclockPanel *panel)
{
    float value = (float)ui_get_car_8c8070();

    panel->value = value;
    panel->digits = (int)(value + float_1_0);
    if (panel->digits > 4) {
        panel->value = 4.0f;
    }
}

/* entry thunk at the original address of stopclock_set_digit_count: register inputs (esi) -> natural C stopclock_set_digit_count_n; ret 0 */
__declspec(naked) void stopclock_set_digit_count(void)
{
    __asm {
        push esi
        call stopclock_set_digit_count_n
        add esp, 4
        ret
    }
}
