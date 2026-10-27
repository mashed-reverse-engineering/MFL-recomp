// cluster-entries: round_scores_reset
/* Installed cluster round_scores_reset (tools/cluster_install.py from output/cluster_pilot_kd/round_scores_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008d8b60*/ extern undefined4 g_round_8d8b60;
/*008d8b64*/ extern undefined4 g_round_8d8b64;
/*008d8b68*/ extern undefined4 g_round_8d8b68;
/*008d8b6c*/ extern undefined4 g_round_8d8b6c;
/*008d8b80*/ extern undefined4 g_round_8d8b80;
/*008d8b84*/ extern undefined4 g_round_8d8b84;
/*008d8b88*/ extern undefined4 g_round_8d8b88;
/*008d8b8c*/ extern undefined4 g_round_8d8b8c;
/* round_scores_reset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sets the three per player change tables at 0x8D8B60 to -1000 (0xFFFFFC18)" */

void round_scores_reset(void)

{
  g_round_8d8b60 = 0xfffffc18;
  g_round_8d8b64 = 0xfffffc18;
  g_round_8d8b68 = 0xfffffc18;
  g_round_8d8b80 = 0xfffffc18;
  g_round_8d8b84 = 0xfffffc18;
  g_round_8d8b88 = 0xfffffc18;
  g_round_8d8b6c = 0xfffffc18;
  g_round_8d8b8c = 0xfffffc18;
  return;
}

