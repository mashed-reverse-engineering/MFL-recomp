#include "mfl_tier2.h"
/*004220c0*/ extern int vehicle_get_position_148();
/*0042a440*/ extern char *course_get_directional_light();
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*00607868*/ extern undefined4 g_vehicle_load_607868;
/*006605a0*/ extern undefined g_game_6605a0;
/*006642f0*/ extern undefined4 g_game_6642f0;
/* per-vehicle render state: size in bytes, and float bit patterns written into it */
#define VEHICLE_RENDER_STATE_SIZE (0xf54)
#define FLOAT_HALF_BITS (0x3f000000)
#define FLOAT_NEG_1_8_BITS (0xbfe66666)
#define FLOAT_NEG_0_85_BITS (0xbf59999a)
/* vehicle_render_state_init */

/* "clears the 0xF54 render state and sets the initial fields (0.5 scales, colour from param)" */

/*
 * Clears the render state of one vehicle slot and sets its initial values.
 *   vehicle_index: slot number; the state lives at vehicle_index * VEHICLE_RENDER_STATE_SIZE
 *                  inside the global block starting at g_game_6605a0.
 *   init_vec4:     4 words copied to the first four state fields (inferred: a vector + w).
 *   colour:        stored into the state field at g_game_6614e8 (colour from param).
 * Returns nothing. Also caches the directional light pointer in g_game_6642f0 on first use.
 */
typedef struct { undefined4 d[4]; } Dw4;
void __cdecl vehicle_render_state_init(int vehicle_index,undefined4 *init_vec4,undefined4 colour)
{
  undefined4 saved_607868;
  char *state_offset;
  char *rec;
  bool need_directional_light;
  struct {
    undefined1 position [12];
    undefined4 position_w;
  } frame;

  /* byte offset of this vehicle's render state; the same variable is reused below for the light pointer */
  rec = (char *)&g_game_6605a0 + vehicle_index * VEHICLE_RENDER_STATE_SIZE;
  /* zero the whole state, then store the index and the initial fields */
  mem_zero(rec,VEHICLE_RENDER_STATE_SIZE);
  *(int *)(rec + 0xf3c) = vehicle_index;
  saved_607868 = g_vehicle_load_607868;
  *(Dw4 *)(rec + 0xf00) = *(Dw4 *)init_vec4;
  *(undefined4 *)(rec + 0xf2c) = FLOAT_HALF_BITS;
  *(undefined4 *)(rec + 0xf34) = FLOAT_HALF_BITS;
  *(undefined4 *)(rec + 0xf38) = saved_607868;
  *(undefined4 *)(rec + 0xf44) = 0;
  *(undefined4 *)(rec + 0xf20) = FLOAT_NEG_1_8_BITS;
  *(undefined4 *)(rec + 0xf24) = FLOAT_ONE_BITS;
  *(undefined4 *)(rec + 0xf28) = FLOAT_NEG_0_85_BITS;
  *(undefined4 *)(rec + 0xf30) = 0;
  *(undefined4 *)(rec + 0xf48) = colour;
  /* fetch the vehicle position; its 4th word goes into the state (meaning unknown) */
  vehicle_get_position_148(vehicle_index,frame.position);
  need_directional_light = g_game_6642f0 == 0;
  *(undefined4 *)(rec + 0xf50) = frame.position_w;
  /* first call only: look up the course's directional light and cache its field at +4 */
  if (need_directional_light) {
    state_offset = course_get_directional_light();
    if (state_offset != 0) {
      g_game_6642f0 = *(int *)(state_offset + 4);
    }
  }
  return;
}
