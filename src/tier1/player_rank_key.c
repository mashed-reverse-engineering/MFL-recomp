#include "mfl_tier2.h"
/*008d8d48*/ extern undefined g_player_8d8d48;
/*008d8d48*/ extern undefined g_player_8d8d48;
/* player_rank_key */

/* float used to sort the 4 players (g_rank_order) */

double player_rank_key(int player_index)
{
  return (double)*(float *)((char *)&g_player_8d8d48 + player_index * 0x30c);
}
