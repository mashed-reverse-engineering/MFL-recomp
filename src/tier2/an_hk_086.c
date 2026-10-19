#include "mfl_tier2.h"
/*0047fa80*/ extern int __cdecl rt2d_stream_find_chunk(void *p0);
/*004c3d80*/ extern int texdict_load_file(char * p0, int p1, int p2);
/*005649f0*/ extern int Rt2dBrushCreate();
/*00567780*/ extern void FUN_00567780(undefined4 p0);
/*005dfdc0*/ extern undefined g_load_ingame_5dfdc0;
/*006c2ed8*/ extern undefined4 *g_text_state;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* g_text_state is a block of TEXT_STATE_SIZE bytes: slot 0 = user data, then the txd, the chunk and the brush (word indices) */
#define TEXT_STATE_SIZE (0x20)
#define TEXT_STATE_TXD (1)
#define TEXT_STATE_CHUNK (2)
#define TEXT_STATE_BRUSH (3)

/* load_ingame_font */

/* InGameFont.txd */

/*
 * Loads the in-game font: allocates g_text_state, loads InGameFont.txd, finds its chunk and creates the brush.
 * user_data is stored in g_text_state[0] (inferred: owner value for the font).
 * Returns 1 on success, 0 if any step failed (the state block is kept, partly filled).
 */
undefined4 load_ingame_font(undefined4 user_data)

{
  undefined4 *state;
  int result;

  /* allocate the state block */
  g_text_state = RwMalloc(TEXT_STATE_SIZE,0);
  /* load the texture dictionary */
  result = texdict_load_file("InGameFont.txd",0,0);
  g_text_state[TEXT_STATE_TXD] = result;
  if (result != 0) {
    FUN_00567780(result);
    /* find the font chunk inside the dictionary */
    result = rt2d_stream_find_chunk(&g_load_ingame_5dfdc0);
    g_text_state[TEXT_STATE_CHUNK] = result;
    if (result != 0) {
      /* create the 2D brush used to draw the font */
      result = Rt2dBrushCreate();
      state = g_text_state;
      g_text_state[TEXT_STATE_BRUSH] = result;
      if (result != 0) {
        *state = user_data;
        return 1;
      }
    }
  }
  return 0;
}


