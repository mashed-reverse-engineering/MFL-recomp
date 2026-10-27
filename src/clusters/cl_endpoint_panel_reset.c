// cluster-entries: endpoint_panel_reset
/* Installed cluster endpoint_panel_reset (tools/cluster_install.py from output/cluster_pilot_zx/endpoint_panel_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006597b8*/ extern undefined g_endpoint_6597b8[0x1000];
/*006597c4*/ extern undefined g_endpoint_panel_6597c4;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
#define ENDPOINT_RECORD_SIZE (0x160) /* bytes per endpoint panel record (inferred) */
#define ENDPOINT_RECORDS_END (0x2c0) /* end of the 2 records used: 2 * ENDPOINT_RECORD_SIZE */
#define ENDPOINT_HEADER_SIZE (0x10) /* 4 ints at the start of each record, cleared on reset */
#define ENDPOINT_COUNT_INDEX (3) /* int index of the record's entry count (inferred) */

/* endpoint_panel_reset */

/*
 * Resets the endpoint panel records (inferred). Clears the header of each record,
 * appends the index of every active player slot (controller and state not -1) to the
 * record chosen by that slot's state (inferred: state = record index), then runs
 * endpoint_panel_build_team_mask on each record in turn.
 * Takes no parameters and returns nothing.
 */

static int endpoint_panel_build_team_mask_r(void * p0);
extern void endpoint_panel_build_team_mask(void);
void endpoint_panel_reset(void)

{
  int returned_record;
  int *record;
  undefined *cursor;

  /* clear the header of each endpoint record */
  cursor = g_endpoint_6597b8;
  do {
    mem_zero(cursor,ENDPOINT_HEADER_SIZE);
    cursor = cursor + ENDPOINT_RECORD_SIZE;
  } while ((int)cursor < (int)g_endpoint_6597b8 + ENDPOINT_RECORDS_END);
  /* append each active player slot's index to the record for its state (one block per slot) */
  if ((g_player_slots[0].controller != -1) && (g_player_slots[0].state != -1)) {
    record = (int *)((char *)g_endpoint_6597b8 + g_player_slots[0].state * ENDPOINT_RECORD_SIZE);
    record[record[ENDPOINT_COUNT_INDEX]] = 0;
    record[ENDPOINT_COUNT_INDEX] = record[ENDPOINT_COUNT_INDEX] + 1;
  }
  if ((g_player_slots[1].controller != -1) && (g_player_slots[1].state != -1)) {
    record = (int *)((char *)g_endpoint_6597b8 + g_player_slots[1].state * ENDPOINT_RECORD_SIZE);
    record[record[ENDPOINT_COUNT_INDEX]] = 1;
    record[ENDPOINT_COUNT_INDEX] = record[ENDPOINT_COUNT_INDEX] + 1;
  }
  if ((g_player_slots[2].controller != -1) && (g_player_slots[2].state != -1)) {
    record = (int *)((char *)g_endpoint_6597b8 + g_player_slots[2].state * ENDPOINT_RECORD_SIZE);
    record[record[ENDPOINT_COUNT_INDEX]] = 2;
    record[ENDPOINT_COUNT_INDEX] = record[ENDPOINT_COUNT_INDEX] + 1;
  }
  if ((g_player_slots[3].controller != -1) && (g_player_slots[3].state != -1)) {
    record = (int *)((char *)g_endpoint_6597b8 + g_player_slots[3].state * ENDPOINT_RECORD_SIZE);
    record[record[ENDPOINT_COUNT_INDEX]] = 3;
    record[ENDPOINT_COUNT_INDEX] = record[ENDPOINT_COUNT_INDEX] + 1;
  }
  /* build the team mask for each record; the helper hands back the record it was given (inferred) */
  cursor = g_endpoint_6597b8;
  do {
    returned_record = endpoint_panel_build_team_mask_r((void *)cursor);
    cursor = (undefined *)(returned_record + ENDPOINT_RECORD_SIZE);
  } while ((int)cursor < (int)g_endpoint_6597b8 + ENDPOINT_RECORDS_END);
  return;
}

/* endpoint_panel_build_team_mask_r: C -> original endpoint_panel_build_team_mask (registers eax) */
__declspec(naked) int endpoint_panel_build_team_mask_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call endpoint_panel_build_team_mask
        ret
    }
}
