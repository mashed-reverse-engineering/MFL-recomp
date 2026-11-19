#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff70*/ extern int race_get_last_active_player();
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd634*/ extern float k_f_2;
/*008c9984*/ extern undefined4 g_ai_last_active_player;
/*008c9988*/ extern undefined4 g_ai_catchup_class;
/*008c99a0*/ extern float g_ai_skill_score;


/* ai_bots_assign_slots */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "assigns bots to player slots for the active race setup (inferred)" */

void ai_bots_assign_slots(void)

{
  int last_active_player;
  int slot_type;
  int slot_index;
  int type1_slot;
  
  last_active_player = race_get_last_active_player();
  if (last_active_player != -1) {
    type1_slot = -1;
    g_ai_last_active_player = last_active_player;
    slot_index = 0;
    do {
      slot_type = race_setup_get_slot_type(slot_index);
      if (slot_type == 1) {
        type1_slot = slot_index;
      }
      slot_index = slot_index + 1;
    } while (slot_index < 4);
    if (type1_slot == last_active_player) {
      if (g_ai_catchup_class == 2) {
        g_ai_skill_score = g_ai_skill_score + k_f_3;
        return;
      }
      if (g_ai_catchup_class == 0) {
        g_ai_skill_score = g_ai_skill_score + k_f_2;
        return;
      }
      if (g_ai_catchup_class == 1) {
        g_ai_skill_score = g_ai_skill_score + float_1_0;
        return;
      }
    }
    else {
      if (g_ai_catchup_class == 2) {
        g_ai_skill_score = g_ai_skill_score - float_1_0;
        return;
      }
      if (g_ai_catchup_class == 0) {
        g_ai_skill_score = g_ai_skill_score - k_f_2;
        return;
      }
      if (g_ai_catchup_class == 1) {
        g_ai_skill_score = g_ai_skill_score - k_f_3;
      }
    }
  }
  return;
}
