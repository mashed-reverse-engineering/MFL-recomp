#include "mfl_tier2.h"
/* RenderWare 2D toolkit (rt2d): rt2dPathDestruct (0x564720), Rt2dPathStreamRead (0x5647d0) and _rt2dScratchPath (0x563b60).  Written from the disassembly of MFL.exe (no codfh
   source for this build's version).  Path creation and the destruction on error are inlined in the exe; they are
   written out here. */

#define FALSE 0
#define rwID_2DPATH 0x1a1
#define PATH_MEMORY_HINT 0x301a1
#define PATH_STREAM_VERSION 1

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

typedef struct PathStreamHeader {         /* what the stream holds in front of the nodes */
  RwInt32 version;
  RwInt32 num_segments;
  RwUInt32 flags;                         /* PATH_STREAM_* */
  RwReal inset;
} PathStreamHeader;
enum { PATH_STREAM_CLOSED = 0x1, PATH_STREAM_FLAT = 0x2, PATH_STREAM_HAS_NEXT = 0x4 };

typedef void *(*FreeListAllocFunc)(void *free_list, RwUInt32 hint);
typedef void (*FreeListFreeFunc)(void *free_list, void *entry);
#define RwFreeListAlloc RW_GLOBAL(0x118, FreeListAllocFunc)
#define RwFreeListFree RW_GLOBAL(0x11c, FreeListFreeFunc)

/*007d2128*/ extern char *RwEngineInstance;
/*00947b60*/ extern char Rt2dGlobals[];                  /* Rt2dGlobalVars */
#define Rt2dPathFreeList (*(void **)(Rt2dGlobals + 0x524))      /* Rt2dGlobals.pathFreeList */
#define Rt2dScratch (*(Rt2dPath **)(Rt2dGlobals + 0x528))       /* Rt2dGlobals.scratch: a locked path (segments = RwSList) */
/*005d5e80*/ extern void *rwSListEmpty(void *list);
/*004dbe70*/ extern RwUInt32 RwStreamRead(void *stream, void *buffer, RwUInt32 length);
/*004dc720*/ extern RwBool RwStreamFindChunk(void *stream, RwUInt32 type, RwUInt32 *length, RwUInt32 *version);
/*005d6030*/ extern void _rwSListDestroy(void *list);
/*00564510*/ extern Rt2dPath *Rt2dPathLock(Rt2dPath *path);
/*00564450*/ extern RwBool Rt2dPathDestroy(Rt2dPath *path);

void rt2dPathDestruct(Rt2dPath *path);
Rt2dPath *Rt2dPathStreamRead(void *stream);

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

/* Empties a path: drops its nodes and every subpath behind it (0x564720, the helper behind Rt2dPathEmpty 0x563570
   and the stream reader; no useful return value). */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void rt2dPathDestruct(Rt2dPath *path)
{
  Rt2dPath *sub;

  if (path->num_segments == -1) {
    _rwSListDestroy(path->segments);
  } else if (path->segments != NULL) {
    RwFree(path->segments);
  }
  path->segments = NULL;
  path->num_segments = 0;
  if (path->next != NULL) {
    rt2dPathDestruct(path->next);
    sub = path->next;
    if (sub != NULL) {
      RT2D_PATH_DESTROY(sub);
    }
    path->next = NULL;
  }
}

/* Reads a path chain.  A header with another version gives back the new, empty path; a failed read NULL (the
   path is freed, as in the exe, but a node array that came too short is not). */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
Rt2dPath *Rt2dPathStreamRead(void *stream)
{
  PathStreamHeader header;
  Rt2dPath *path, *result, *sub;
  rt2dPathNode *nodes;

  path = (Rt2dPath *)RwFreeListAlloc(Rt2dPathFreeList, PATH_MEMORY_HINT);      /* _rt2dPathCreate */
  if (path != NULL) {
    path->segments = NULL;
    path->num_segments = 0;
    path->closed = FALSE;
    path->flat = TRUE;
    path->next = NULL;
    path->curr = path;
    path->inset = 0.0f;
    Rt2dPathLock(path);
  }
  result = path;

  if (RwStreamRead(stream, &header, sizeof(header)) != sizeof(header)) {
    result = NULL;
  } else if (header.version == PATH_STREAM_VERSION) {
    rt2dPathDestruct(path);
    path->num_segments = header.num_segments;
    path->flat = (header.flags >> 1) & 1;
    path->curr = path;
    path->closed = header.flags & PATH_STREAM_CLOSED;
    path->inset = header.inset;
    nodes = (rt2dPathNode *)RwMalloc(path->num_segments * sizeof(rt2dPathNode), PATH_MEMORY_HINT);
    if (nodes == NULL) {
      result = NULL;
    } else if (path->num_segments != 0
               && RwStreamRead(stream, nodes, path->num_segments * sizeof(rt2dPathNode)) != path->num_segments * sizeof(rt2dPathNode)) {
      result = NULL;
    } else {
      path->segments = nodes;
      if (header.flags & PATH_STREAM_HAS_NEXT) {
        if (!RwStreamFindChunk(stream, rwID_2DPATH, NULL, NULL)) {
          result = NULL;
        } else {
          path->next = Rt2dPathStreamRead(stream);
        }
      }
      if (result != NULL) {
        for (sub = path->next; sub != NULL; sub = sub->next) {
          sub->curr = path;
        }
      }
    }
  }

  if (result == NULL && path != NULL) {
    RT2D_PATH_DESTROY(path);
  }
  return result;
}

/* Module close (0x564620): destroys the scratch path (Rt2dPathDestroy inlined) and the path free list. */
extern RwBool RwFreeListDestroy(void *free_list);
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void _rt2dPathClose(void)
{
  Rt2dPath *path = Rt2dScratch;

  if (path != NULL) {
    RT2D_PATH_DESTROY(path);
  }
  RwFreeListDestroy(Rt2dPathFreeList);
  Rt2dPathFreeList = NULL;
}

/* The shared scratch path, emptied (Rt2dPathEmpty inlined: subpaths destroyed, node list emptied, open, flat). */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
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
