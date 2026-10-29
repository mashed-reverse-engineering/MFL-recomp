/* compile: /G5 */
#include "mfl_tier2.h"
/*00638f58*/ extern int g_inflate_trees_638f58;
/*00638f5c*/ extern int g_inflate_trees_638f5c;
/*00638f60*/ extern int g_inflate_trees_638f60;
/*00639f60*/ extern int g_inflate_trees_639f60;
/* zlib: returns the fixed literal and distance tables (bit counts and tree pointers). */
int inflate_trees_fixed(int *bl, int *bd, void **tl, void **td)
{
  *bl = g_inflate_trees_638f58;
  *bd = g_inflate_trees_638f5c;
  *tl = &g_inflate_trees_638f60;
  *td = &g_inflate_trees_639f60;
  return 0;
}
