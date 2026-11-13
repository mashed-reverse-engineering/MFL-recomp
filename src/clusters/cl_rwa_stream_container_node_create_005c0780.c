// cluster-entries: rwa_stream_container_node_create_005c0780 _RwaStreamContainerPreinstanceCodecs
/* Installed cluster rwa_stream_container_node_create_005c0780 (tools/cluster_install.py from output/cluster_pilot_a4/rwa_stream_container_node_create_005c0780.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio stream container (MFL.exe 0x005c0780, rwa.dll unit 24D4C0): _RwaStreamContainerNodeCreateEx (register function: node
   list in ecx, the two 28-byte format blocks in eax / ebx, entered through the generated thunk) allocates a 0x54-byte node, links it at
   the front of the list (link at +0x4c), copies the format blocks when given (+0xc, +0x28) and stores the four stack words.
   Written from the MFL.exe disassembly (tools/disasm_va.py). */
#include "mfl_tier2.h"

typedef struct RwaDLink { struct RwaDLink *next, *prev; } RwaDLink;
typedef struct RwaNodeFormat { RwUInt32 w[7]; } RwaNodeFormat;

typedef struct RwaContainerNode {
  RwUInt32 state;               /* +0 */
  RwUInt32 a, b;                /* +4, +8 */
  RwaNodeFormat in, out;        /* +0xc, +0x28 */
  RwUInt32 d, c;                /* +0x44, +0x48 */
  RwaDLink link;                /* +0x4c */
} RwaContainerNode;

/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwaContainerNode *rwa_stream_container_node_create_005c0780_n(RwaDLink *list, const RwaNodeFormat *in, const RwaNodeFormat *out,
                                                            RwUInt32 a, RwUInt32 b, RwUInt32 c, RwUInt32 d)
{
  RwaContainerNode *n = FUN_005bf0d0(sizeof(RwaContainerNode), 0x3080e);

  if (!n)
    return 0;
  n->link.next = list->next;
  n->link.prev = list;
  list->next->prev = &n->link;
  list->next = &n->link;
  if (in)
    n->in = *in;
  if (out)
    n->out = *out;
  n->a = a;
  n->b = b;
  n->c = c;
  n->d = d;
  n->state = 0;
  return n;
}

/* entry thunk at the original address of rwa_stream_container_node_create_005c0780: register inputs (ecx,eax,ebx,s0,s1,s2,s3) -> natural C rwa_stream_container_node_create_005c0780_n; ret 0 */
__declspec(naked) void rwa_stream_container_node_create_005c0780(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push ebx
        push eax
        push ecx
        call rwa_stream_container_node_create_005c0780_n
        add esp, 28
        ret
    }
}

/* _RwaStreamContainerPreinstanceCodecs (MFL.exe 0x005c0aa0, objects/rwastreamcontainer.c), written from the disassembly.
   Node fields as the root stores them: a = the codec object, b = codecIface, c = the media entry, d = the parent node. */
typedef struct RwaLLink { struct RwaLLink *prev, *next; void *data; } RwaLLink;      /* RWA data list, the head's data is 0 */

typedef struct RwaWaveFormat {      /* 0x1c bytes, the same block as RwaNodeFormat */
  RwUInt32 w0, w4, size;
  unsigned char b0c, channels, b0e, b0f;
  void *extra;                      /* +0x10 */
  RwUInt32 extraBytes;              /* +0x14 */
  unsigned char b18, b19, b1a, b1b;
} RwaWaveFormat;

/* codec object +0x54 -> info: +0x04 list of format pairs {input id, output id}, +0x08 pair count (no list: a direct codec) */
typedef struct RwaCodecInfo { RwUInt32 unk0; void ***pairs; RwUInt32 pairCount; } RwaCodecInfo;
#define CODEC_INFO(codec) (*(RwaCodecInfo **)((char *)(codec) + 0x54))
#define NODE_OF(l) ((RwaContainerNode *)((char *)(l) - 0x4c))

/*005bcf00*/ extern int FUN_005bcf00(const void *formatId, const void *format);      /* format matches the id */
/*005bcea0*/ extern void _rwaWaveDefFormatCopyToWaveFormat(const void *formatId, RwaWaveFormat *format, int a3);
/*005c0620*/ extern void *FUN_005c0620(void *codec, void *srcFormat, RwaWaveFormat *dstFormat, RwUInt32 entry, char *state);
/*005baf80*/ extern void FUN_005baf80(void *obj, RwUInt32 a, RwUInt32 b);
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */

/* Find a codec chain from srcFormat to dstFormat for media entry `entry` and add its nodes to the front of `nodes`.  Codecs that have a
   node for this entry already are skipped.  A codec without a pair list is tried directly; otherwise every pair whose input matches
   srcFormat: when its output matches dstFormat the codec's node is added at once, else the chain from the pair's output format to
   dstFormat is searched first (recursion) and this node becomes the parent of its head.  Returns 1 (the new node in *nodeOut when
   nodeOut is given; not on the direct-codec path), 0 when no chain exists or a node could not be allocated. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
int _RwaStreamContainerPreinstanceCodecs(void *srcFormat, RwaWaveFormat *dstFormat, RwUInt32 entry, void *codecIface,
                                         RwaLLink *codecs, RwaDLink *nodes, char *state, RwaContainerNode **nodeOut)
{
  RwaLLink *c;

  for (c = codecs->next; c != codecs; c = c->next) {
    void *codec = c->data;
    RwaCodecInfo *info = CODEC_INFO(codec);
    RwaDLink *l;
    RwUInt32 j;

    for (l = nodes->next; l != nodes; l = l->next)
      if (NODE_OF(l)->a == (RwUInt32)(mfl_uptr)codec && NODE_OF(l)->c == entry)
        goto next_codec;

    if (!info->pairs) {
      void *obj = FUN_005c0620(codec, srcFormat, dstFormat, entry, state);

      if (!obj)
        continue;
      FUN_005baf80(obj, *(RwUInt32 *)(state + 0x54), *(RwUInt32 *)(state + 0x58));
      return rwa_stream_container_node_create_005c0780_n(nodes, srcFormat, (const RwaNodeFormat *)dstFormat,
                                                         (RwUInt32)(mfl_uptr)codec, (RwUInt32)(mfl_uptr)codecIface, entry, 0) != 0;
    }

    for (j = 0; j < info->pairCount; j++) {
      void **pair = info->pairs[j];
      RwaContainerNode *node, *child;
      RwaWaveFormat format;

      if (!FUN_005bcf00(pair[0], srcFormat))
        continue;
      if (FUN_005bcf00(pair[1], dstFormat)) {
        node = rwa_stream_container_node_create_005c0780_n(nodes, srcFormat, (const RwaNodeFormat *)dstFormat,
                                                           (RwUInt32)(mfl_uptr)codec, (RwUInt32)(mfl_uptr)codecIface, entry, 0);
        if (!node)
          return 0;
        if (nodeOut)
          *nodeOut = node;
        return 1;
      }
      /* the pair's output as the intermediate format (only these fields are set, as RWA's init macro does) */
      format.w0 = *(RwUInt32 *)srcFormat;
      format.w4 = 0;
      format.size = 0;
      format.b0c = 0;
      format.channels = 0;
      format.b18 = 0;
      format.b19 = 0;
      format.extra = 0;
      format.extraBytes = 0;
      _rwaWaveDefFormatCopyToWaveFormat(pair[1], &format, 1);
      if (_RwaStreamContainerPreinstanceCodecs(&format, dstFormat, entry, codecIface, codecs, nodes, state, &child)) {
        node = rwa_stream_container_node_create_005c0780_n(nodes, srcFormat, (const RwaNodeFormat *)dstFormat,
                                                           (RwUInt32)(mfl_uptr)codec, (RwUInt32)(mfl_uptr)codecIface, entry,
                                                           (RwUInt32)(mfl_uptr)child);
        if (node) {
          if (nodeOut)
            *nodeOut = node;
          return 1;
        }
        /* out of memory: drop every node of this entry */
        for (l = nodes->next; l != nodes; ) {
          RwaContainerNode *n = NODE_OF(l);

          l = l->next;
          if (n->c == entry) {
            if (n->link.next) {
              n->link.prev->next = n->link.next;
              n->link.next->prev = n->link.prev;
            }
            FUN_005bf120(n);
          }
        }
        return 0;
      }
    }
  next_codec:;
  }
  return 0;
}
