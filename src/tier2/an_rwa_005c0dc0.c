/* rwa_stream_container_preinstance_entries_005c0dc0 (0x005c0dc0): stream container (objects/rwastreamcontainer.c): for every media entry without a node yet, find a codec chain by codec priority and add its nodes */
#include "mfl_tier2.h"

/* the container's node lists: RW style links {next, prev} at node+0x4c */
typedef struct RwaNodeLink { struct RwaNodeLink *next, *prev; } RwaNodeLink;
/* the RWA data lists (RwaLList): {prev, next, data}, the head's data is 0 */
typedef struct RwaLLink { struct RwaLLink *prev, *next; void *data; } RwaLLink;

typedef struct RwaWaveFormat {      /* 0x1c bytes */
  RwUInt32 w0, w4, size;
  unsigned char b0c, channels, b0e, b0f;
  void *extra;      /* +0x10 */
  RwUInt32 extraBytes;      /* +0x14 */
  unsigned char b18, b19, b1a, b1b;
} RwaWaveFormat;

/* format init as RWA's macro does it */
#define WAVEFORMAT_INIT(f) ((f)->w0 = 0, (f)->w4 = 0, (f)->size = 0, (f)->b0c = 0, (f)->channels = 0, (f)->b18 = 0, \
                            (f)->b19 = 0, (f)->extra = 0, (f)->extraBytes = 0)

/* a pipeline node (0x54 bytes) */
typedef struct RwaStreamNode {
  void *obj;                        /* +0x00 the instanced object */
  void *codec;                      /* +0x04 */
  void *owner;                      /* +0x08 */
  RwaWaveFormat format;             /* +0x0c */
  char pad28[0x44 - 0x28];
  struct RwaStreamNode *parent;     /* +0x44 */
  RwUInt32 entry;                   /* +0x48 media entry index */
  RwaNodeLink link;                 /* +0x4c */
} RwaStreamNode;
#define NODE_OF(l) ((RwaStreamNode *)((char *)(l) - 0x4c))

/*005ba4f0*/ extern int FUN_005ba4f0(void *def, void *iface, RwaLLink *out);      /* _rwaObjDefBuildSupportedList */
/*005bcea0*/ extern void _rwaWaveDefFormatCopyToWaveFormat(const void *uuid, RwaWaveFormat *format, int a3);
/*005c0aa0*/ extern int _RwaStreamContainerPreinstanceCodecs(void *srcFormat, RwaWaveFormat *dstFormat, RwUInt32 entry, void *codecIface,
                                                              RwaLLink *codecs, RwaNodeLink *nodes, char *state, void *miscData);
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005be560*/ extern void RwaLListEmpty(RwaLLink *head);

#define RWA_HINT 0x3080e

/* state+0x0c -> media (+0x14 entry count, +0x18 entries of 0x28 bytes, +0x14 of an entry: its format).  owner: stored in
   every new node; codecIface: the codec interface (no interface: fail); parent: the parent of the first node of a chain;
   nodes: the container's node list (entries listed there are skipped, new chains go to its front); codecs: RwaLList of
   codec objects (obj+0x54 -> info: +0x04 format ids, +0x08 count, +0x10 priority).  Returns 1, 0 on failure. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_container_preinstance_entries_005c0dc0(char *state, void *owner, void *codecIface, RwaStreamNode *parent,
                                                      RwaNodeLink *nodes, RwaLLink *codecs)
{
  RwUInt32 i, j, minPrio, lim;
  int built = 0, found;
  RwUInt32 off;
  void *src;
  char *best, *info;
  RwaNodeLink made, *l, *next;
  RwaLLink avail;
  RwaWaveFormat format;
  RwaStreamNode *node, *tail;

  for (i = 0, off = 0; i < *(RwUInt32 *)(*(char **)(state + 0xc) + 0x14); i++, off += 0x28) {
    for (l = nodes->next; l != nodes; l = l->next)
      if (NODE_OF(l)->entry == i)
        goto next_entry;
    if (!codecIface)
      return 0;
    if (!built) {
      avail.prev = &avail;
      avail.next = &avail;
      avail.data = 0;
      if (!FUN_005ba4f0(0, codecIface, &avail))
        return 0;
      built = 1;
    }
    made.next = made.prev = &made;
    src = *(void **)(*(char **)(*(char **)(state + 0xc) + 0x18) + off + 0x14);
    minPrio = 0;
    found = 0;
    for (;;) {
      RwaLLink *c;

      best = 0;
      lim = 0xffffffff;
      for (c = codecs->next; c != codecs; c = c->next) {
        RwUInt32 prio = *(RwUInt32 *)(*(char **)((char *)c->data + 0x54) + 0x10);

        if (prio >= minPrio && minPrio <= lim) {      /* (sic: minPrio, not prio, against the limit) */
          best = (char *)c->data;
          lim = prio;
        }
      }
      if (!best)
        break;
      info = *(char **)(best + 0x54);
      minPrio = *(RwUInt32 *)(info + 0x10) + 1;
      for (j = 0; j < *(RwUInt32 *)(info + 8); j++) {
        WAVEFORMAT_INIT(&format);
        format.w0 = *(RwUInt32 *)src;
        _rwaWaveDefFormatCopyToWaveFormat((*(void ***)(info + 4))[j], &format, 0);
        if (_RwaStreamContainerPreinstanceCodecs(src, &format, i, codecIface, &avail, &made, state, 0)) {
          found = 1;
          break;
        }
      }
      if (found)
        break;
    }
    if (!found) {
      RwaLListEmpty(&avail);
      return 0;
    }

    NODE_OF(made.next)->parent = parent;
    tail = NODE_OF(made.prev);
    node = (RwaStreamNode *)FUN_005bf0d0(sizeof(RwaStreamNode), RWA_HINT);
    if (!node) {
      RwaLListEmpty(&avail);
      while ((l = made.next) != &made) {
        if (l->next) {
          l->prev->next = l->next;
          l->next->prev = l->prev;
        }
        FUN_005bf120(NODE_OF(l));
      }
      return 0;
    }
    node->link.next = made.next;
    node->link.prev = &made;
    made.next->prev = &node->link;
    made.next = &node->link;
    node->format = format;
    node->codec = best;
    node->owner = owner;
    node->entry = i;
    node->parent = tail;
    node->obj = 0;
    for (l = made.next; l != &made; l = next) {      /* move the chain to the front of the container's list */
      next = l->next;
      l->prev->next = l->next;
      l->next->prev = l->prev;
      l->next = 0;
      l->prev = 0;
      l->next = nodes->next;
      l->prev = nodes;
      nodes->next->prev = l;
      nodes->next = l;
    }
  next_entry:;
  }
  return 1;
}
