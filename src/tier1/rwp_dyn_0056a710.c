/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_CLEAR(bits, id) ((bits)[(id) >> 5] &= ~(1 << ((id) & 0x1f)))
typedef struct RwpPairNode { struct RwpDynBody *body; struct RwpPairNode *next; } RwpPairNode;
typedef struct RwpDynWorld { char pad[0x1c]; RwpPairNode *free_nodes; RwpPairNode **buckets; char pad2[0x48]; unsigned *used_bits; } RwpDynWorld;
typedef struct RwpDynPeer { char pad[0x20]; unsigned short bucket; } RwpDynPeer;
typedef struct RwpDynBody {
  char pad0[4]; unsigned short slot; char pad1[0x4a]; RwpDynPeer *peer_a; unsigned short link_a; char pad2[2]; RwpDynPeer *peer_b; unsigned short link_b;
} RwpDynBody;
#define UNLINK_PAIR(peer, link) \
  if ((link) != 0xffff) { \
    RwpPairNode **pp = &w->buckets[(peer)->bucket]; \
    RwpPairNode *n = *pp; \
    while (n && n->body != b) { pp = &n->next; n = n->next; } \
    n = *pp; \
    *pp = n->next; \
    n->next = w->free_nodes; \
    w->free_nodes = n; \
  }
/* Returns the two pair-list nodes of a body to the free list, clears its used bit and invalidates its slot; returns the world. (Rwp library) */
RwpDynWorld *rwp_dyn_0056a710(RwpDynWorld *w, RwpDynBody *b)
{
  UNLINK_PAIR(b->peer_a, b->link_a)
  UNLINK_PAIR(b->peer_b, b->link_b)
  BIT_CLEAR(w->used_bits, b->slot);
  b->slot = 0xffff;
  return w;
}
