// cluster-entries: rwa_stream_container_node_list_purge_005c07f0 rwa_stream_container_add_source_nodes_005c0cf0 rwa_stream_container_build_pipeline_005c10b0
/* Installed cluster rwa_stream_container_node_list_purge_005c07f0 (tools/cluster_install.py from output/cluster_pilot_au7/rwa_stream_container_node_list_purge_005c07f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container pipeline (objects/rwastreamcontainer.c): the node list purge (register function, list in esi), and its two
   callers: the source node setup and the pipeline build.  Cluster: rwa_stream_container_node_list_purge_005c07f0 is entered
   through the generated esi thunk. */
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
  char *obj;                        /* +0x00 the instanced object (+0x2c: its link in a container list, +0x4c entry) */
  void *codec;                      /* +0x04 object definition */
  void *owner;                      /* +0x08 interface the node was made for */
  RwaWaveFormat format;             /* +0x0c */
  RwaWaveFormat format2;            /* +0x28 */
  struct RwaStreamNode *parent;     /* +0x44 */
  RwUInt32 entry;                   /* +0x48 media entry index (-1: the root) */
  RwaNodeLink link;                 /* +0x4c */
} RwaStreamNode;
#define NODE_OF(l) ((RwaStreamNode *)((char *)(l) - 0x4c))

/* a media entry (0x28 bytes): +0x0c samples per frame, +0x14 format, +0x1a frame size */
typedef struct RwaMediaEntry {
  char pad0[0xc];
  RwUInt32 samplesPerFrame;         /* +0x0c */
  char pad10[4];
  RwaWaveFormat *format;            /* +0x14 */
  char pad18[2];
  unsigned short frameSize;         /* +0x1a */
  char pad1c[0xc];
} RwaMediaEntry;

/* build result (pipeline build arg 2) */
typedef struct RwaPipelineInfo {
  RwUInt32 flags;                   /* 1: a codec without a delay, 3: entries of different sizes */
  RwUInt32 w4;
  RwUInt32 entries;                 /* +0x08 */
} RwaPipelineInfo;

/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005be560*/ extern void RwaLListEmpty(RwaLLink *head);
/*005ba4f0*/ extern int FUN_005ba4f0(void *def, void *iface, RwaLLink *out);      /* _rwaObjDefBuildSupportedList */
/*005bda80*/ extern void *RwaObjInterfaceDefFindByUUID(const void *uuid);
/*005ba5c0*/ extern void *sub_23DCA0();      /* RwaObjDefFindByUUID(a1, uuid, out): declared `()` as in cl_rwa_soft_voice_def_init_005d8920.c (other files declare it with 4 parameters) */
/*005ba030*/ extern int RwaObjDefGetDefaultCreationParams(void *def, void *params);
/*005bb0a0*/ extern char *FUN_005bb0a0(void *objdef, int b1, int b2, void *params, int b4, int b5);      /* RwaObjCreate */
/*005c1ff0*/ extern void FUN_005c1ff0(char *parentObj, RwUInt32 entry, char *obj, int a4);      /* connect a node object to its parent */
/*005c0620*/ extern char *_RwaStreamContainerCreateCodec(void *def, RwaWaveFormat *src, RwaWaveFormat *dst, RwUInt32 entry, char *state);
/*005bd090*/ extern RwUInt32 RwaWaveFormatGetSamplesPerAudioFrame(const RwaWaveFormat *format);
/*005bcc50*/ extern RwUInt32 RwaWaveFormatGetFrameSize(const RwaWaveFormat *format);
/*005c0dc0*/ extern int rwa_stream_container_preinstance_entries_005c0dc0(char *state, void *owner, void *codecIface, RwaStreamNode *parent,
                                                                         RwaNodeLink *nodes, RwaLLink *codecs);
/*005c0760*/ extern void rwa_stream_container_renderer_playback_end_005c0760(void *renderer, char *container);
/*005dd080*/ __declspec(dllimport) long __stdcall InterlockedIncrement(long *value);
/*005f82ec*/ extern undefined g_data_5f82ec;      /* interface uuid: mixing node (several entries) */
/*005f81ec*/ extern undefined g_data_5f81ec;      /* object definition uuid of the mixer */
/*005f81fc*/ extern undefined g_data_5f81fc;      /* interface uuid: single entry root */
/*005f829c*/ extern undefined g_data_5f829c;      /* interface uuid: renderer */
/*005f826c*/ extern undefined g_data_5f826c;      /* interface uuid: codec */

#define RWA_HINT 0x3080e

static void * rwa_unit2452a0_005c0560_r(void * p0, RwaLLink * p1);
extern void rwa_unit2452a0_005c0560(void);
/* free every node of a node list (unlinking those that are linked) */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_container_node_list_purge_005c07f0_n(RwaNodeLink *list)
{
  RwaNodeLink *l;

  while ((l = list->next) != list) {
    if (l->next) {
      l->prev->next = l->next;
      l->next->prev = l->prev;
    }
    FUN_005bf120(NODE_OF(l));
  }
}

/* one node per media entry whose source format a codec of `codecs` takes directly (owner, parent: stored in the node); the
   nodes go to the front of `nodes`.  Out of memory: the list is purged, 0. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_container_add_source_nodes_005c0cf0(char *state, void *owner, RwaStreamNode *parent, RwaNodeLink *nodes, RwaLLink *codecs)
{
  RwUInt32 i;

  for (i = 0; i < *(RwUInt32 *)(*(char **)(state + 0xc) + 0x14); i++) {
    RwaWaveFormat *src = ((RwaMediaEntry *)*(char **)(*(char **)(state + 0xc) + 0x18))[i].format;
    void *codec = rwa_unit2452a0_005c0560_r(src, codecs);
    RwaStreamNode *node;

    if (!codec)
      continue;
    node = (RwaStreamNode *)FUN_005bf0d0(sizeof(RwaStreamNode), RWA_HINT);
    if (!node) {
      rwa_stream_container_node_list_purge_005c07f0_n(nodes);
      return 0;
    }
    node->link.next = nodes->next;
    node->link.prev = nodes;
    nodes->next->prev = &node->link;
    nodes->next = &node->link;
    if (src)
      node->format = *src;
    node->codec = codec;
    node->owner = owner;
    node->entry = i;
    node->parent = parent;
    node->obj = 0;
  }
  return 1;
}

/* link an object's container link (obj+0x2c) behind `after` */
#define OBJ_LINK(o) ((RwaNodeLink *)((o) + 0x2c))
#define LINK_AFTER(after, l) ((l)->next = (after)->next, (l)->prev = (after), (after)->next->prev = (l), (after)->next = (l))

/* Build the container's processing pipeline (state: the container; +0x0c media: +0x14 entry count, +0x18 entries): a root
   node (a mixer for several entries, else the container's own source object), the codec / renderer chains per entry, then
   create the objects from the root outwards (renderers / codecs into the lists at state+0x20 / +0x18).  info: flags and the
   entry count.  Returns 1, 0 on failure. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_container_build_pipeline_005c10b0(char *state, RwaPipelineInfo *info)
{
  RwaStreamNode *n;
  RwUInt32 srcLen;
  char *params;
  RwaNodeLink *l;
  RwaMediaEntry *ent;
  struct {      /* the frame as the original lays it out: callees see the lists (and the words around them) by address */
    RwaNodeLink nodes;              /* +0x00 */
    RwaStreamNode *head;            /* +0x08 */
    union { RwUInt32 maxParams; RwaStreamNode *top; };              /* +0x0c (sizes pass, then the build loop) */
    void *renderIface;                                              /* +0x10 */
    union { char *media; RwaNodeLink *after; };                    /* +0x14 */
    union { RwUInt32 maxRate; RwaWaveFormat *savedFormat; };        /* +0x18 */
    union { RwUInt32 minDelay; RwaStreamNode *node; };              /* +0x1c */
    RwaLLink avail;                 /* +0x20 */
    void *codecIface;               /* +0x2c */
    RwUInt32 savedSamples;          /* +0x30 */
    unsigned short savedFrame;      /* +0x34 */
    RwaStreamNode root;             /* +0x38 */
  } f;

  *(RwUInt32 *)(state + 0x40) = 0;
  *(RwUInt32 *)(state + 0x44) = 0x20;
  if (!*(char **)(state + 0xc))
    return 0;
  info->entries = *(RwUInt32 *)(*(char **)(state + 0xc) + 0x14);
  f.nodes.next = f.nodes.prev = &f.nodes;
  if (info->entries > 1) {      /* a mixer node is the root */
    void *iface = RwaObjInterfaceDefFindByUUID(&g_data_5f82ec);
    void *def = sub_23DCA0(0, &g_data_5f81ec, 0);

    f.head = (RwaStreamNode *)FUN_005bf0d0(sizeof(RwaStreamNode), RWA_HINT);
    if (!f.head)
      return 0;
    f.head->link.next = f.nodes.next;
    f.head->link.prev = &f.nodes;
    f.nodes.next->prev = &f.head->link;
    f.nodes.next = &f.head->link;
    f.head->codec = def;
    f.head->owner = iface;
    f.head->entry = 0xffffffff;
    f.head->parent = 0;
    f.head->obj = 0;
  } else {      /* the container's source object is the root */
    f.root.parent = 0;
    f.root.owner = RwaObjInterfaceDefFindByUUID(&g_data_5f81fc);
    f.root.obj = *(char **)(state + 0x14);
    f.root.codec = *(void **)f.root.obj;
    f.root.entry = 0xffffffff;
    WAVEFORMAT_INIT(&f.root.format);
    WAVEFORMAT_INIT(&f.root.format2);
    f.root.link.prev = 0;
    f.root.link.next = 0;
    f.head = &f.root;
  }
  f.renderIface = RwaObjInterfaceDefFindByUUID(&g_data_5f829c);
  f.codecIface = RwaObjInterfaceDefFindByUUID(&g_data_5f826c);
  f.avail.prev = f.avail.next = &f.avail;
  f.avail.data = 0;
  if (!FUN_005ba4f0(0, f.renderIface, &f.avail))
    return 0;
  if (!rwa_stream_container_add_source_nodes_005c0cf0(state, f.renderIface, f.head, &f.nodes, &f.avail)) {
    RwaLListEmpty(&f.avail);
    return 0;
  }
  if (!rwa_stream_container_preinstance_entries_005c0dc0(state, f.renderIface, f.codecIface, f.head, &f.nodes, &f.avail)) {
    rwa_stream_container_node_list_purge_005c07f0_n(&f.nodes);
    RwaLListEmpty(&f.avail);
    return 0;
  }
  RwaLListEmpty(&f.avail);

  /* sizes: the largest creation parameter block, the largest renderer source length, the smallest codec delay */
  f.media = *(char **)(*(char **)(state + 0xc) + 0x18);
  f.minDelay = 0xffffffff;
  srcLen = 0;
  f.maxParams = 0;
  f.maxRate = 0;
  for (l = f.nodes.next; l != &f.nodes; l = l->next) {
    n = NODE_OF(l);
    if (n->owner == f.renderIface) {
      RwUInt32 delay = *(RwUInt32 *)(*(char **)((char *)n->codec + 0x54) + 0xc);

      if (!delay)
        info->flags |= 1;
      else {
        RwUInt32 rate = ((RwaMediaEntry *)f.media)[n->entry].format->w0;

        if (rate > f.maxRate)
          f.maxRate = rate;
        if (delay < f.minDelay)
          f.minDelay = delay;
      }
      if (((RwaMediaEntry *)f.media)[n->entry].format->w0 > srcLen) {
        if (srcLen)
          info->flags |= 3;
        srcLen = ((RwaMediaEntry *)*(char **)(*(char **)(state + 0xc) + 0x18))[n->entry].format->w0;
      }
    }
    if (*(unsigned short *)((char *)n->codec + 0x44) > f.maxParams)
      f.maxParams = *(unsigned short *)((char *)n->codec + 0x44);
  }
  *(RwUInt32 *)(state + 0x44) = f.minDelay;
  *(RwUInt32 *)(state + 0x40) = f.maxRate;
  params = (char *)FUN_005bf0d0(f.maxParams, 0x1080e);
  if (!params)
    goto fail_nodes;

  if (f.head != &f.root) {      /* create the mixer */
    if (!RwaObjDefGetDefaultCreationParams(f.head->codec, params))
      goto fail;
    InterlockedIncrement((long *)(state + 4));
    *(char **)(params + 8) = *(char **)(state + 0xc);
    *(void **)(params + 0xc) = *(void **)(state + 0x10);
    *(char **)(params + 0x10) = state + 0x28;
    f.head->obj = FUN_005bb0a0(f.head->codec, 0, 4, params, *(int *)(state + 0x50), *(int *)(state + 0x58));
    if (!f.head->obj)
      goto fail;
    LINK_AFTER((RwaNodeLink *)(state + 0x18), OBJ_LINK(f.head->obj));
    FUN_005c1ff0(*(char **)(state + 0x14), 0, f.head->obj, 0);
  }

  for (;;) {
    /* the next renderer chain still to create; none left: done */
    for (l = f.nodes.next; l != &f.nodes; l = l->next)
      if ((f.node = NODE_OF(l))->owner == f.renderIface)
        break;
    if (l == &f.nodes)
      break;
    do {
      /* the outermost node of the chain without an object */
      for (n = f.node; n; n = n->parent) {
        if (n->obj) {
          n = f.top;
          break;
        }
        f.top = n;
      }
      if (!RwaObjDefGetDefaultCreationParams(n->codec, params))
        goto fail;
      *(char **)(params + 8) = *(char **)(state + 0xc);
      *(char **)(params + 0x10) = state + 0x28;
      if (n->owner == f.renderIface) {
        RwaNodeLink *list;

        ent = &((RwaMediaEntry *)*(char **)(*(char **)(state + 0xc) + 0x18))[n->entry];
        f.after = 0;
        if (n->parent->owner == f.codecIface) {      /* the renderer gets the codec's output format */
          f.savedFormat = ent->format;
          f.savedSamples = ent->samplesPerFrame;
          f.savedFrame = ent->frameSize;
          ent->format = &n->format;
          ent->samplesPerFrame = RwaWaveFormatGetSamplesPerAudioFrame(&n->format);
          ent->frameSize = (unsigned short)RwaWaveFormatGetFrameSize(ent->format);
        }
        *(RwaMediaEntry **)(params + 0x1c) = ent;
        *(void **)(params + 0xc) = *(void **)(state + 0x10);
        *(char **)(params + 0x28) = state;
        *(void **)(params + 0x24) = (void *)rwa_stream_container_renderer_playback_end_005c0760;
        InterlockedIncrement((long *)(state + 4));
        n->obj = FUN_005bb0a0(n->codec, *(int *)(state + 0x48), 4, params, *(int *)(state + 0x50), *(int *)(state + 0x58));
        if (n->parent->owner == f.codecIface) {
          ent->format = f.savedFormat;
          ent->frameSize = f.savedFrame;
          ent->samplesPerFrame = f.savedSamples;
        }
        if (!n->obj)
          goto fail;
        *(RwUInt32 *)(n->obj + 0x4c) = n->entry;
        /* the renderers at state+0x20 stay sorted by entry */
        list = (RwaNodeLink *)(state + 0x20);
        for (l = list->next; l != list; l = l->next) {
          if (*(RwUInt32 *)((char *)l + 0x20) > n->entry)
            break;
          f.after = l;
        }
        LINK_AFTER(f.after ? f.after : list, OBJ_LINK(n->obj));
      } else {
        n->obj = _RwaStreamContainerCreateCodec(n->codec, &n->format, &n->format2, n->entry, state);
        if (!n->obj)
          goto fail;
        LINK_AFTER((RwaNodeLink *)(state + 0x18), OBJ_LINK(n->obj));
      }
      FUN_005c1ff0(n->parent->obj, n->parent == f.head ? n->entry : 0, n->obj, 0);
    } while (n != f.node);
    if (f.node != f.head) {      /* the chain is built: drop its nodes */
      for (n = f.node; n != f.head; ) {
        RwaStreamNode *done = n;

        n = n->parent;
        if (done->link.next) {
          done->link.prev->next = done->link.next;
          done->link.next->prev = done->link.prev;
        }
        FUN_005bf120(done);
      }
    }
  }
  for (l = ((RwaNodeLink *)(state + 0x20))->next; l != (RwaNodeLink *)(state + 0x20); l = l->next)
    *(RwUInt32 *)((char *)l + 0x20) = 1;
  FUN_005bf120(params);
  rwa_stream_container_node_list_purge_005c07f0_n(&f.nodes);
  return 1;

fail:
  FUN_005bf120(params);
fail_nodes:
  rwa_stream_container_node_list_purge_005c07f0_n(&f.nodes);
  return 0;
}

/* rwa_unit2452a0_005c0560_r: C -> original rwa_unit2452a0_005c0560 (registers edi,s0) */
__declspec(naked) void * rwa_unit2452a0_005c0560_r(void * p0, RwaLLink * p1)
{
    __asm {
        push edi
        push dword ptr [esp + 12]
        mov edi, dword ptr [esp + 12]
        call rwa_unit2452a0_005c0560
        add esp, 4
        pop edi
        ret
    }
}

/* entry thunk at the original address of rwa_stream_container_node_list_purge_005c07f0: register inputs (esi) -> natural C rwa_stream_container_node_list_purge_005c07f0_n; ret 0 */
__declspec(naked) void rwa_stream_container_node_list_purge_005c07f0(void)
{
    __asm {
        push esi
        call rwa_stream_container_node_list_purge_005c07f0_n
        add esp, 4
        ret
    }
}
