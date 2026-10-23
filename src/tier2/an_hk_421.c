#include "mfl_tier2.h"
/*0041b9d0*/ extern int hudpanel_slot_init_player();
/*00603e68*/ extern char *g_hudpanel_slot_603e68;
/*00658450*/ extern undefined g_hudpanel_658450;
/*00658538*/ extern undefined g_hudpanel_658538;
/*00658548*/ extern char *g_hudpanel_slot_658548;
/*0065856c*/ extern undefined4 g_hudpanel_65856c;
/*0065858c*/ extern char *g_hudpanel_slot_65858c;
/*006585a0*/ extern undefined4 g_hudpanel_6585a0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
static int hudpanel_slot_clear_r(int reg_eax);
extern void hudpanel_slot_clear(void);
__declspec(naked) int hudpanel_slot_clear_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_slot_clear
        ret
    }
}

static int hudpanel_slot_reset_r(undefined4 * reg_eax);
extern void hudpanel_slot_reset(void);
extern char g_hudpanel_6584f8[];
__declspec(naked) int hudpanel_slot_reset_r(undefined4 * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_slot_reset
        ret
    }
}

#define HUDPANEL_SLOT_SIZE (0x6c)
#define HUDPANEL_SLOT_DWORDS (0x1b)
#define HUDPANEL_STATE_ACTIVE (2)
#define HUDPANEL_ENTRY_SIZE (0x54)
#define HUDPANEL_ENTRY_PANEL_OFFSET (0x50)

/* hudpanel_slot_init_player_b */

/* "second variant of hudpanel_slot_init_player" */

/* Sets up the HUD panel of player slot slot_index.
   activate == 0: only resets the slot's panel and returns.
   otherwise: clears the slot's panel, copies the team colour and controller of
   g_player_slots[slot_index] into the slot record, marks the slot active, and
   claims the first free entry of the g_hudpanel_658450 table for the panel
   (table meaning inferred). */

void hudpanel_slot_init_player_b(int slot_index,int activate)

{
  undefined4 team_colour;
  int controller;
  undefined *entry;
  int slot_offset;
  undefined *panel;

  slot_offset = slot_index * HUDPANEL_SLOT_SIZE;
  panel = &g_hudpanel_658538 + slot_offset;
  if (activate == 0) {
    hudpanel_slot_reset_r(panel);
    return;
  }
  hudpanel_slot_reset_r(panel);
  hudpanel_slot_clear_r((&g_hudpanel_6585a0)[slot_index * HUDPANEL_SLOT_DWORDS]);
  /* copy the team colour and controller of the player into the slot record */
  team_colour = ((undefined4 *)&g_hudpanel_slot_603e68)[g_player_slots[slot_index].team];
  controller = g_player_slots[slot_index].controller;
  (&g_hudpanel_65856c)[slot_index * HUDPANEL_SLOT_DWORDS] = HUDPANEL_STATE_ACTIVE;
  *(undefined4 *)((char *)&g_hudpanel_slot_658548 + slot_offset) = team_colour;
  *(int *)((char *)&g_hudpanel_slot_65858c + slot_offset) = controller;
  /* find the first free entry (field at +HUDPANEL_ENTRY_PANEL_OFFSET is 0) and store the panel there;
     (inferred) */
  entry = &g_hudpanel_658450;
  do {
    if (*(int *)(entry + HUDPANEL_ENTRY_PANEL_OFFSET) == 0) {
      *(undefined **)(entry + HUDPANEL_ENTRY_PANEL_OFFSET) = panel;
      return;
    }
    entry = entry + HUDPANEL_ENTRY_SIZE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_hudpanel_6584f8));
  return;
}


