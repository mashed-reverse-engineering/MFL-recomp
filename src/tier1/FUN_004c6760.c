#include "mfl_tier2.h"
/*0091f088*/ extern undefined4 g_data_91f088;
/*0091f08c*/ extern undefined4 g_data_91f08c;
/*0091f090*/ extern undefined4 g_data_91f090;
/*0093f100*/ extern undefined4 g_data_93f100;
/*0093f104*/ extern undefined4 g_data_93f104;
/*0093f108*/ extern undefined4 g_data_93f108;
/* FUN_004c6760
 * Clears six global 32-bit words (g_data_91f088, g_data_91f08c, g_data_91f090,
 * g_data_93f100, g_data_93f104, g_data_93f108) to 0. Takes no parameters and
 * returns nothing. Meaning of the globals not known (inferred: reset of
 * per-session state). */
void FUN_004c6760(void)
{
  g_data_91f088 = 0;
  g_data_91f08c = 0;
  g_data_91f090 = 0;
  g_data_93f100 = 0;
  g_data_93f104 = 0;
  g_data_93f108 = 0;
  return;
}
