// cluster-entries: kc_panel_render
/* Installed cluster kc_panel_render (tools/cluster_install.py from output/cluster_pilot_pg2/kc_panel_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

#define KC_PANEL_ENTRY_COUNT 4

/* one entry of the kc panel table (record stride 0x114) */
typedef struct KcPanelEntry {
  unsigned char unknown_00[0x110];
  int active;                       /* 0x110: nonzero when the entry is drawn */
} KcPanelEntry;
/*00659340*/ extern KcPanelEntry g_kc_659340[KC_PANEL_ENTRY_COUNT];

static void kc_panel_entry_render_r(void * p0);
extern void kc_panel_entry_render(void);
/* kc_panel_render: draws every active entry of the kc panel */
void kc_panel_render(void)
{
  int i;

  for (i = 0; i < KC_PANEL_ENTRY_COUNT; i++) {
    if (g_kc_659340[i].active != 0) {
      kc_panel_entry_render_r(&g_kc_659340[i]);
    }
  }
}

/* kc_panel_entry_render_r: C -> original kc_panel_entry_render (registers eax) */
__declspec(naked) void kc_panel_entry_render_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call kc_panel_entry_render
        ret
    }
}
