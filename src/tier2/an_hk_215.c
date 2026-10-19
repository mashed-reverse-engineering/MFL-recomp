#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*00417b70*/ extern int __cdecl ai_find_leader_a(undefined4 p0);
/*0044c180*/ extern float player_distance_get();
/*005ddc28*/ extern float k_f_0;
#define RACE_SLOT_COUNT (4)
#define NO_LEADER_SLOT (-1)
/* ai_find_leader_b */

/* "variant of ai_find_leader_a" */

/*
 * Leader check, variant of ai_find_leader_a (inferred from the name).
 * Returns 1 if player_index's distance equals k_f_0 (inferred: 0.0) and the
 * last race slot of type 1 (inferred: the leader slot) is at a distance
 * in [min_distance, max_distance] from player_index, otherwise 0.
 */
undefined4 __cdecl ai_find_leader_b(undefined4 player_index,float min_distance,float max_distance)

{
  int slot_type;
  int slot_index;
  int leader_slot;
  float distance;

  if (player_distance_get(player_index) == k_f_0) {
    leader_slot = NO_LEADER_SLOT;
    slot_index = 0;
    do {
      slot_type = race_setup_get_slot_type(slot_index);
      if (slot_type == 1) {
        leader_slot = slot_index;
      }
      slot_index = slot_index + 1;
    } while (slot_index < RACE_SLOT_COUNT);
    if (leader_slot != NO_LEADER_SLOT) {
      distance = player_distance_get(leader_slot);
      if (distance >= min_distance && distance <= max_distance) {
        return 1;
      }
    }
  }
  return 0;
}


