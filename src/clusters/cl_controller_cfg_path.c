// cluster-entries: controller_cfg_path
/* Installed cluster controller_cfg_path (tools/cluster_install.py from output/cluster_pilot6/controller_cfg_path.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster controller_cfg_path: formats the controller config file name of a player into the shared buffer and returns it. */
extern char g_controller_cfg_7711d4[];
extern const char s_contcfg_d_bin_005e1400[];
extern int sprintf(char *buffer, const char *format, ...);

/* was: EAX = player index */
char *controller_cfg_path_n(int player)
{
    sprintf(g_controller_cfg_7711d4, s_contcfg_d_bin_005e1400, player);
    return g_controller_cfg_7711d4;
}

/* entry thunk at the original address of controller_cfg_path: register inputs (eax) -> natural C controller_cfg_path_n; ret 0 */
__declspec(naked) void controller_cfg_path(void)
{
    __asm {
        push eax
        call controller_cfg_path_n
        add esp, 4
        ret
    }
}
