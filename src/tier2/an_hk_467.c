#include "mfl_tier2.h"
/*006605a0*/ extern undefined g_game_6605a0;
/*006614a0*/ extern char *g_game_6614a0;
/*006614a4*/ extern char *g_game_6614a4;
/*006614a8*/ extern char *g_game_6614a8;
/*006614ac*/ extern char *g_game_6614ac;
/*006614c0*/ extern char *g_vehicle_load_6614c0;
/*006614c4*/ extern char *g_vehicle_load_6614c4;
/*006614c8*/ extern char *g_vehicle_load_6614c8;
/*006614cc*/ extern char *g_vehicle_load_6614cc;
/*006614d0*/ extern char *g_vehicle_load_6614d0;
/*006614d4*/ extern char *g_vehicle_load_6614d4;
/*006614d8*/ extern char *g_game_6614d8;
/*006614dc*/ extern char *g_vehicle_load_6614dc;
/*006614e4*/ extern char *g_game_6614e4;
/*006614e8*/ extern undefined g_game_6614e8;
/*006614f0*/ extern char *g_game_6614f0;
/*006642f0*/ extern undefined4 g_game_6642f0;

/* per-vehicle render state: size in bytes, and float bit patterns written into it */
#define VEHICLE_RENDER_STATE_SIZE (0xf54)

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
extern void __cdecl vehicle_render_state_init(int vehicle_index,undefined4 *init_vec4,undefined4 colour);


