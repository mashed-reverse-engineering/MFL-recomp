#include "mfl_tier2.h"
/*0071abf4*/ extern char *g_particles_71abf4;


/* Walks the list at g_particles_71abf4+4 for an entry whose key (+8) equals the argument and returns that entry's value (+0xC), or -1 if none. List node: [0] = entry ptr, [2] = next. */
/* Walks the list at g_particles_71abf4+4 for an entry whose key (+8) equals the argument
   and returns that entry's value (+0xC), or -1 if none. List node: [0] = entry ptr, [2] = next. */
undefined4 particles_lookup_by_key(int key)
{
  int *node;
  int entry;

  node = *(int **)(g_particles_71abf4 + 4);
  while( true ) {
    if (node == (int *)0x0) {
      return 0xffffffff;
    }
    entry = *node;
    if ((entry != 0) && (*(int *)(entry + 8) == key)) break;
    node = (int *)node[2];
  }
  return *(undefined4 *)(entry + 0xc);
}
