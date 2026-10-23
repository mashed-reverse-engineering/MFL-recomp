// cluster-entries: menu_find_texture_by_slot
/* Installed cluster menu_find_texture_by_slot (tools/cluster_install.py from output/cluster_pilot6/menu_find_texture_by_slot.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster menu_find_texture_by_slot: texture of a menu slot (0, 1, 2 depending on the event type, 3) looked up by name; 0 for the other slots. */
extern char g_menu_find_5decac[];
extern char g_menu_find_5deca4[];
extern char g_frontend_5debd0[];
extern int g_event_type;
extern int find_texture_a(const char *name);
extern int event_type_uses_standard_menu(void);

/* was: EAX = slot */
int menu_find_texture_by_slot_n(int slot)
{
    if (slot == 0) {
        return find_texture_a(g_menu_find_5decac);
    }
    if (slot != 1) {
        if (slot == 2) {
            if (event_type_uses_standard_menu() == 0) {
                return 0;
            }
            if (g_event_type == EVENT_QUICK_RACE_11 || g_event_type == EVENT_TIME_TRIAL_0 || g_event_type == EVENT_QUICK_RACE_12 || g_event_type == EVENT_TIME_TRIAL_1 || g_event_type == EVENT_QUICK_RACE_13 || g_event_type == EVENT_TIME_TRIAL_2) {
                return 0;
            }
            return find_texture_a(g_menu_find_5deca4);
        }
        if (slot != 3) {
            return 0;
        }
    }
    return find_texture_a(g_frontend_5debd0);
}

/* entry thunk at the original address of menu_find_texture_by_slot: register inputs (eax) -> natural C menu_find_texture_by_slot_n; ret 0 */
__declspec(naked) void menu_find_texture_by_slot(void)
{
    __asm {
        push eax
        call menu_find_texture_by_slot_n
        add esp, 4
        ret
    }
}
