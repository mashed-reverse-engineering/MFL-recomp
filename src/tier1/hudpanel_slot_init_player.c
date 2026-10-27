#include "mfl_tier2.h"
/*0041c740*/ extern int hudpanel_slot_init_player_b();
/* Initialises one player's slot in the HUD panel. Forwards both arguments unchanged to
 * hudpanel_slot_init_player_b, which does the work (inferred from the forwarding only).
 * hud_panel: the HUD panel being set up (inferred from the name).
 * player_slot: the player slot index to initialise (inferred from the name). */
void hudpanel_slot_init_player(int hud_panel, int player_slot)
{
  hudpanel_slot_init_player_b(hud_panel, player_slot);
}
