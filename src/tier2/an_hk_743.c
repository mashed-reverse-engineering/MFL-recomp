#include "mfl_tier2.h"
/*0041be50*/ extern int hudpanel_particles_flush(void);
/*0041d050*/ extern int hudpanel_markers_update_all_slots();
/*00486c40*/ extern int particles_flush();
/*00658450*/ extern undefined g_hudpanel_658450[0x1000];
/*006584f8*/ extern undefined g_hudpanel_6584f8;
/*00658538*/ extern undefined g_hudpanel_658538[0x1000];
static int hudpanel_cursor_update_r(float * reg_esi);
extern void hudpanel_cursor_update(void);
__declspec(naked) int hudpanel_cursor_update_r(float * reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call hudpanel_cursor_update
        pop esi
        ret
    }
}

static int hudpanel_marker_place_r(void *reg_eax);
extern void hudpanel_marker_place(void);
extern char g_load_6586e8[];
__declspec(naked) int hudpanel_marker_place_r(void *reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_marker_place
        ret
    }
}

#define MARKER_SLOT_STRIDE (0x6c)
#define MARKER_SLOT_ACTIVE_OFFSET (0x34)
#define MARKER_ENTRY_STRIDE (0x54)

/* hudpanel_markers_update_active
 * Per-frame HUD marker update. Does nothing unless menu_markers_enabled_for_event()
 * returns nonzero. Takes no parameters and returns nothing.
 * (was: "for each active 0x6C byte slot from 0x658538 when ui_fn_4353e0 is set")
 */

void hudpanel_markers_update_active(void)

{
  int markers_enabled;
  undefined4 particles_flush_result;
  undefined *entry;

  markers_enabled = menu_markers_enabled_for_event();
  if (markers_enabled != 0) {
    /* Cursor pass: walk the slot table in g_hudpanel_658538 up to g_load_6586e8,
       and update each slot whose active field is nonzero (inferred). */
    entry = g_hudpanel_658538;
    do {
      if (*(int *)(entry + MARKER_SLOT_ACTIVE_OFFSET) != 0) {
        hudpanel_cursor_update_r((float *)entry);
      }
      entry = entry + MARKER_SLOT_STRIDE;
    } while ((char *)entry < ((char *)&g_load_6586e8));
    /* Refresh all slots, then flush the particle queues. */
    hudpanel_markers_update_all_slots();
    particles_flush_result = particles_flush(&g_hudpanel_6584f8);
    hudpanel_particles_flush();
    /* Marker pass: place each entry in the table at g_hudpanel_658450, up to g_hudpanel_6584f8. */
    entry = g_hudpanel_658450;
    do {
      hudpanel_marker_place_r((void *)entry);
      entry = entry + MARKER_ENTRY_STRIDE;
    } while ((char *)entry < ((char *)&g_hudpanel_6584f8));
  }
  return;
}


