#include "mfl_tier2.h"
/* RenderWare 2D toolkit (rt2d): _rt2dScratchPath (0x563b60).  Written from the disassembly of MFL.exe (no codfh source for this build's
   version).  rt2dPathDestruct (0x564720) and Rt2dPathStreamRead (0x5647d0) were here too; they went back to rw_trap in W7 G2a (HS DIFF,
   seed 90, gates 1113_1411 / 1113_1820) and return in W8. */

#define FALSE 0

typedef struct rt2dPathNode {             /* 24 bytes */
  RwInt32 type;
  RwV2d pos;
  RwV2d normal;
  RwReal dist;
} rt2dPathNode;

typedef struct Rt2dPath Rt2dPath;
struct Rt2dPath {                         /* 0x1c bytes */
  rt2dPathNode *segments;                 /* or an RwSList when num_segments is -1 */
  RwInt32 num_segments;
  RwBool closed;
  RwBool flat;
  RwReal inset;
  Rt2dPath *next;                         /* next subpath */
  Rt2dPath *curr;                         /* the head path of the chain */
};

typedef void (*FreeListFreeFunc)(void *free_list, void *entry);
#define RwFreeListFree RW_GLOBAL(0x11c, FreeListFreeFunc)

/*007d2128*/ extern char *RwEngineInstance;
/*00947b60*/ extern char Rt2dGlobals[];                  /* Rt2dGlobalVars */
#define Rt2dPathFreeList (*(void **)(Rt2dGlobals + 0x524))      /* Rt2dGlobals.pathFreeList */
#define Rt2dScratch (*(Rt2dPath **)(Rt2dGlobals + 0x528))       /* Rt2dGlobals.scratch: a locked path (segments = RwSList) */
/*005d5e80*/ extern void *rwSListEmpty(void *list);
/*005d6030*/ extern void _rwSListDestroy(void *list);
/*00564450*/ extern RwBool Rt2dPathDestroy(Rt2dPath *path);

/* Frees a path record and the chain behind it (_rt2dPathDestroy, which the exe inlines): the node array (an
   RwSList when num_segments is -1), the record itself, then Rt2dPathDestroy of the rest of the chain. */
#define RT2D_PATH_DESTROY(path)                                   \
  do {                                                            \
    Rt2dPath *rest_ = (path)->next;                               \
    if ((path)->segments != NULL) {                               \
      if ((path)->num_segments == -1) {                           \
        _rwSListDestroy((path)->segments);                        \
      } else {                                                    \
        RwFree((path)->segments);                                 \
      }                                                           \
    }                                                             \
    (path)->segments = NULL;                                      \
    RwFreeListFree(Rt2dPathFreeList, (path));                     \
    Rt2dPathDestroy(rest_);                                       \
  } while (0)

/* The shared scratch path, emptied (Rt2dPathEmpty inlined: subpaths destroyed, node list emptied, open, flat). */
Rt2dPath *_rt2dScratchPath(void)
{
  Rt2dPath *path = Rt2dScratch;
  Rt2dPath *sub = path->next;

  if (sub != NULL) {
    RT2D_PATH_DESTROY(sub);
  }
  path->next = NULL;
  rwSListEmpty(path->segments);
  path->closed = FALSE;
  path->flat = TRUE;
  path->curr = path;
  return Rt2dScratch;
}
