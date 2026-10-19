#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*0047a470*/ extern int car_set_wheel_contact();
/*0047b520*/ extern int car_current_matrix();
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*0060d9c4*/ extern undefined g_car_60d9c4;
/*008d8b30*/ extern undefined4 player_count;

#define MATRIX_ELEMENT_COUNT (0x10)
/* zone object offsets (inferred from the bounds test and the flag write below) */
#define ZONE_MATRIX_OFFSET (0x94)
#define ZONE_X_MAX_OFFSET (0x14)
#define ZONE_X_MIN_OFFSET (0x20)
#define ZONE_Z_MAX_OFFSET (0x1c)
#define ZONE_Z_MIN_OFFSET (0x28)
#define ZONE_HAS_CAR_OFFSET (0x48)

/* course_zone_players_transform_update */

/* "per player: inverts the zone matrix and transforms the car matrix into zone space"
 * Purpose (inferred): for every player, express the car's world matrix relative to the zone's
 * matrix, then set the zone's has-car flag (ZONE_HAS_CAR_OFFSET = 1) if the car's position lies
 * strictly inside the zone's x/z bounds.
 * zone: zone object (char * because no struct is known for it). Returns nothing. */

void course_zone_players_transform_update(char *zone)

{
  int num_players;
  int remaining;
  int player_index;
  RwReal *src;
  RwMatrix *dst;
  RwReal *car_matrix;
  RwMatrix car_in_zone;
  RwMatrix inv_zone_matrix;

  num_players = get_player_count();
  player_index = 0;
  /* loop over all players */
  if (0 < num_players) {
    do {
      car_current_matrix(&car_matrix,player_index);
      RwMatrixInvert(&inv_zone_matrix,*(RwMatrix **)(zone + ZONE_MATRIX_OFFSET));
      src = car_matrix;
      dst = &car_in_zone;
      /* copy the car matrix element by element into car_in_zone (inferred: 4x4 = 16 floats) */
      for (remaining = MATRIX_ELEMENT_COUNT; remaining != 0; remaining = remaining + -1) {
        (dst->right).x = *src;
        src = src + 1;
        dst = (RwMatrix *)&(dst->right).y;
      }
      /* car_in_zone = car_in_zone * inv_zone_matrix (post-concatenate) */
      RwMatrixTransform(&car_in_zone,&inv_zone_matrix,rwCOMBINEPOSTCONCAT);
      car_set_wheel_contact(&g_car_60d9c4,player_index,0,0);
      /* car inside the zone's x/z box (strict bounds): flag the zone (inferred) */
      if ((((car_in_zone.pos.x < *(float *)(zone + ZONE_X_MAX_OFFSET)) &&
           (*(float *)(zone + ZONE_X_MIN_OFFSET) < car_in_zone.pos.x)) &&
          (car_in_zone.pos.z < *(float *)(zone + ZONE_Z_MAX_OFFSET))) &&
         (*(float *)(zone + ZONE_Z_MIN_OFFSET) < car_in_zone.pos.z)) {
        *(undefined4 *)(zone + ZONE_HAS_CAR_OFFSET) = 1;
      }
      player_index = player_index + 1;
    } while (player_index < num_players);
  }
  return;
}
