#include "mfl_tier2.h"
/*004c3d80*/ extern int texdict_load_file(char * p0, int p1, int p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*00664414*/ extern undefined4 g_load_664414;
/*008c88ac*/ extern undefined4 g_game_8c88ac;
/*008c8b2c*/ extern char g_load_copters_8c8b2c[];
#define RECORD_WORDS (0x14)
#define SLOT_NONE (0xffffffff)
/* load_copters_txd */
/*  Loads the Copters texture dictionary into g_load_664414, clears the current texture
 *  dictionary, then resets the table at g_game_8c88ac: each RECORD_WORDS-word record is zeroed
 *  except its last word, which is set to SLOT_NONE (inferred: "no entry" marker).
 *  Takes no parameters, returns nothing.
 */
void load_copters_txd(void)
{
  int words_left;
  undefined4 *record_end;
  undefined4 *word;
  
  g_load_664414 = texdict_load_file("ToastArt/common/perm/Copters/Copters.txd",0,0);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  record_end = &g_game_8c88ac;
  /* Walk the records; record_end is the last word of the current one. */
  do {
    word = record_end + -(RECORD_WORDS - 1);
    for (words_left = RECORD_WORDS; words_left != 0; words_left = words_left + -1) {
      *word = 0;
      word = word + 1;
    }
    *record_end = SLOT_NONE;
    record_end = record_end + RECORD_WORDS;
  } while ((LONG_PTR)record_end < ((LONG_PTR)&g_load_copters_8c8b2c));
  return;
}
