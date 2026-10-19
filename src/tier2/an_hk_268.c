#include "mfl_tier2.h"
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00564ac0*/ extern RwBool __cdecl Rt2dBrushDestroy(void * p0);
/*005662e0*/ extern RwBool __cdecl Rt2dFontDestroy(void * p0);
/*00567780*/ extern void FUN_00567780(undefined4 p0);
/*006c2ed8*/ extern undefined4 g_text_state;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* byte offsets of the handles stored in g_text_state (inferred from the destroy calls below) */
#define TEXT_STATE_TEX_DICT (0x4)
#define TEXT_STATE_FONT (0x8)
#define TEXT_STATE_BRUSH (0xc)

/* rt2d_text_destroy */

/* "destroys brush, font, texture dictionary of g_text_state"
 * Tears down the 2D text system: destroys the brush, the font and the texture
 * dictionary held in g_text_state, frees the state block through the engine's
 * rwfree and clears the global pointer. No parameters. Always returns 1. */

undefined4 rt2d_text_destroy(void)

{
  /* brush, then font, then texture dictionary */
  Rt2dBrushDestroy(*(undefined4 *)((int)g_text_state + TEXT_STATE_BRUSH));
  Rt2dFontDestroy(*(undefined4 *)((int)g_text_state + TEXT_STATE_FONT));
  FUN_00567780(0);
  RwTexDictionaryDestroy_004d5ac0(*(RwTexDictionary **)((int)g_text_state + TEXT_STATE_TEX_DICT));
  RwFree(g_text_state);
  g_text_state = (void *)0x0;
  return 1;
}


