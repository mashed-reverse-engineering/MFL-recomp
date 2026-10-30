/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpMgrSizes RwpMgrSizes;
typedef struct RwpArena { char *base; int size; char *cursor; } RwpArena;
extern int rwp_mgr_00569b30(RwpMgrSizes *cfg);
extern char *FUN_00575b20(int size, int align, int tag);
extern int rwp_mgr_00569ba0(RwpArena *arena, RwpMgrSizes *cfg, int a, int b, int c);
/* Allocates the manager arena sized by rwp_mgr_00569b30 and initialises the manager in it (rwp_mgr_00569ba0); returns the manager. (Rwp library) */
int rwp_mgr_0056a120(RwpMgrSizes *cfg, int a, int b, int c)
{
  RwpArena arena;
  arena.size = rwp_mgr_00569b30(cfg);
  arena.base = FUN_00575b20(arena.size, 0x10, 0x30900);
  arena.cursor = arena.base;
  return rwp_mgr_00569ba0(&arena, cfg, a, b, c);
}
