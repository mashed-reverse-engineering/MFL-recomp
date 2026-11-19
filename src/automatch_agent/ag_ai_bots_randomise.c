#include "mfl_tier2.h"
/*00481fd0*/ extern double __cdecl random_range(float p0, float p1);
/*00603950*/ extern undefined g_ai_bots_603950;
/*00603954*/ extern undefined g_ai_bots_603954;
/*00603a84*/ extern undefined g_ai_bots_603a84;
/*007ef7b8*/ extern undefined4 game_mode;
/*008c9980*/ extern float g_ai_f_8c9980;
/*005de214*/ extern float k_f_5de214;
/*008c9988*/ extern undefined4 g_ai_catchup_class;
/*008c9990*/ extern float g_car_8c9990;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008c9cc0*/ extern char g_car_8c9cc0[];

/* game modes in which every bot starts from the fixed value k_f_5de214 */
#define FIXED_START_MODE_A 4
#define FIXED_START_MODE_B 8
#define FIXED_START_MODE_C 9
#define BOT_TABLE_END ((int)&g_car_8c9cc0)   /* one past the last AiBotRecord */

/* ai_bots_randomise */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Gives each bot a random start value drawn from the range table at 0x603950. */

void ai_bots_randomise(void)

{
  int table_index;
  float start_value;
  int range_min;
  int range_max;
  AiBotRecord *bot;

  g_ai_catchup_class = 0;
  g_car_8c9990 = 0;
  bot = g_ai_bots;
  do {
    table_index = (int)g_ai_f_8c9980;
    range_min = *(int *)(&g_ai_bots_603950 + table_index * 8);
    range_max = *(int *)(&g_ai_bots_603954 + table_index * 8);
    start_value = (float)random_range((float)range_min,(float)range_max);
    if (game_mode == FIXED_START_MODE_A || game_mode == FIXED_START_MODE_C ||
        game_mode == FIXED_START_MODE_B) {
      start_value = k_f_5de214;
    }
    bot->start_random = (int)start_value;
    bot->base_value = *(int *)(&g_ai_bots_603a84 + table_index * 4);
    bot = bot + 1;
  } while ((int)bot < BOT_TABLE_END);
  return;
}
