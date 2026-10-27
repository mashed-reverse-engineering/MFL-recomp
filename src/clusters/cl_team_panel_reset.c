// cluster-entries: team_panel_reset
/* Installed cluster team_panel_reset (tools/cluster_install.py from output/cluster_pilot_zx/team_panel_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*00658fd8*/ extern undefined g_team_658fd8[0x1000];
/*00658fe4*/ extern undefined g_team_panel_658fe4;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];

#define TEAM_ENTRY_STRIDE (0x16c)
#define TEAM_ENTRY_CLEAR_BYTES (0x10)
#define TEAM_ENTRY_LOOP_END (0x2d8) /* 2 * TEAM_ENTRY_STRIDE: the loops cover entries 0 and 1 */
#define PLAYER_SLOT_NO_CONTROLLER (-1)
#define PLAYER_SLOT_STATE_UNUSED (-1)


/* team_panel_reset */

/* "clears 0x10 bytes per entry (0x658FD8, stride 0x16C)"
 * Rebuilds the team panel table g_team_658fd8 (TEAM_ENTRY_STRIDE bytes per entry):
 * 1. zeroes the first TEAM_ENTRY_CLEAR_BYTES bytes of each entry (words [0]..[3]; [3] is the player count);
 * 2. appends each player slot index (0..3) to the entry picked by that slot's state
 *    (inferred: state is the team/panel index) and bumps that entry's count;
 * 3. runs team_panel_build_team_mask over every entry.
 * Takes no parameters, returns nothing.
 */

static int team_panel_build_team_mask_r(void * p0);
extern void team_panel_build_team_mask(void);
void team_panel_reset(void)

{
  int built_entry;
  TeamPanelEntry *team_entry;
  undefined *entry;

  /* clear the header of entries 0 and 1 */
  entry = g_team_658fd8;
  do {
    mem_zero(entry,TEAM_ENTRY_CLEAR_BYTES);
    entry = entry + TEAM_ENTRY_STRIDE;
  } while ((int)entry < (int)g_team_658fd8 + TEAM_ENTRY_LOOP_END);
  /* slot 0: append index 0 to the entry for its state */
  if ((g_player_slots[0].controller != PLAYER_SLOT_NO_CONTROLLER) && (g_player_slots[0].state != PLAYER_SLOT_STATE_UNUSED)) {
    team_entry = (TeamPanelEntry *)((char *)g_team_658fd8 + g_player_slots[0].state * TEAM_ENTRY_STRIDE);
    team_entry->player[team_entry->player_count] = 0;
    team_entry->player_count = team_entry->player_count + 1;
  }
  /* slot 1: append index 1 */
  if ((g_player_slots[1].controller != PLAYER_SLOT_NO_CONTROLLER) && (g_player_slots[1].state != PLAYER_SLOT_STATE_UNUSED)) {
    team_entry = (TeamPanelEntry *)((char *)g_team_658fd8 + g_player_slots[1].state * TEAM_ENTRY_STRIDE);
    team_entry->player[team_entry->player_count] = 1;
    team_entry->player_count = team_entry->player_count + 1;
  }
  /* slot 2: append index 2 */
  if ((g_player_slots[2].controller != PLAYER_SLOT_NO_CONTROLLER) && (g_player_slots[2].state != PLAYER_SLOT_STATE_UNUSED)) {
    team_entry = (TeamPanelEntry *)((char *)g_team_658fd8 + g_player_slots[2].state * TEAM_ENTRY_STRIDE);
    team_entry->player[team_entry->player_count] = 2;
    team_entry->player_count = team_entry->player_count + 1;
  }
  /* slot 3: append index 3 */
  if ((g_player_slots[3].controller != PLAYER_SLOT_NO_CONTROLLER) && (g_player_slots[3].state != PLAYER_SLOT_STATE_UNUSED)) {
    team_entry = (TeamPanelEntry *)((char *)g_team_658fd8 + g_player_slots[3].state * TEAM_ENTRY_STRIDE);
    team_entry->player[team_entry->player_count] = 3;
    team_entry->player_count = team_entry->player_count + 1;
  }
  /* build the team mask for entries 0 and 1 (the builder returns the entry it was given, inferred) */
  entry = g_team_658fd8;
  do {
    built_entry = team_panel_build_team_mask_r((void *)entry);
    entry = (undefined *)(built_entry + TEAM_ENTRY_STRIDE);
  } while ((int)entry < (int)g_team_658fd8 + TEAM_ENTRY_LOOP_END);
  return;
}

/* team_panel_build_team_mask_r: C -> original team_panel_build_team_mask (registers eax) */
__declspec(naked) int team_panel_build_team_mask_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call team_panel_build_team_mask
        ret
    }
}
