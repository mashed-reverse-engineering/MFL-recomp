// cluster-entries: kc_panel_destroy
/* Installed cluster kc_panel_destroy (tools/cluster_install.py from output/cluster_pilot_pg2/kc_panel_destroy.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00485240*/ extern void __cdecl RwTexDictionaryDestroy_004d5ac0(void *dictionary);

#define KC_PANEL_ENTRY_COUNT 4

/* one entry of the kc panel table (record stride 0x114) */
typedef struct KcPanelEntry {
  unsigned char unknown_00[0x114];
} KcPanelEntry;
/*00659340*/ extern KcPanelEntry g_kc_659340[KC_PANEL_ENTRY_COUNT];
/*006592f0*/ extern void *g_load_6592f0;      /* texture dictionary of the kc panel */

static void kc_panel_entry_destroy_r(void * p0);
extern void kc_panel_entry_destroy(void);
/* kc_panel_destroy: destroys every entry of the kc panel, then its texture dictionary */
void kc_panel_destroy(void)
{
  int i;

  for (i = 0; i < KC_PANEL_ENTRY_COUNT; i++) {
    kc_panel_entry_destroy_r(&g_kc_659340[i]);
  }
  RwTexDictionaryDestroy_004d5ac0(g_load_6592f0);
}

/* kc_panel_entry_destroy_r: C -> original kc_panel_entry_destroy (registers eax) */
__declspec(naked) void kc_panel_entry_destroy_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call kc_panel_entry_destroy
        ret
    }
}
