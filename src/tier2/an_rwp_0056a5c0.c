/* rwp_world_add_joint (0x0056a5c0): register a joint with the world and link it into the joint lists of its bodies */
#include "mfl_tier2.h"
#include "rwp_common.h"

/* The joint's own id in the world (its link bit), at +4. */
#define JOINT_ID(j) (*(unsigned short *)((char *)(j) + 4))

/* Inserts a node for joint jt into the joint list of `body`, keeping the list ordered by descending joint id (ids above body->id first). */
#define LINK_JOINT(w, node, jt, body) do {                    \
    unsigned short key_ = (body)->id;                           \
    MflDynNode **pp_ = &(w)->links[key_];                       \
    while (*pp_ && JOINT_ID((*pp_)->joint) > key_)              \
      pp_ = &(*pp_)->next;                                      \
    (node)->next = *pp_;                                        \
    *pp_ = (node);                                              \
    (node)->joint = (jt);                                       \
  } while (0)

/* Gives joint jt the link id `id` (0xffffffff = the first free bit of bits_link, 0 when all nlinks are taken) and takes one list node from
   the free chain per attached body (has_a / has_b != 0xffff); 0 when the chain runs out (a node already taken goes back).  Returns w. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflDynWorld *rwp_world_add_joint(MflDynWorld *w, MflDynJoint *jt, unsigned int id)
{
  unsigned int i = 0, nwords, word, bit;
  MflDynNode *node_a = 0, *node_b = 0;

  if (id == 0xffffffff) {
    nwords = (w->nlinks + 31) >> 5;
    while (i < nwords && w->bits_link[i] == 0xffffffff)
      i++;
    if (i == nwords)
      return 0;
    id = i << 5;
    word = w->bits_link[i];
    for (bit = 1; word & bit; bit += bit)
      id++;
    if (id >= w->nlinks)
      return 0;
  }
  if (jt->has_a != 0xffff) {
    node_a = (MflDynNode *)w->chain_head;
    if (!node_a)
      return 0;
    w->chain_head = (char *)node_a->next;
  }
  if (jt->has_b != 0xffff) {
    node_b = (MflDynNode *)w->chain_head;
    if (!node_b) {
      w->chain_head = (char *)node_a;
      return 0;
    }
    w->chain_head = (char *)node_b->next;
  }
  BIT_SET(w->bits_link, id);
  JOINT_ID(jt) = (unsigned short)id;
  if (node_a)
    LINK_JOINT(w, node_a, jt, jt->body_a);
  if (node_b)
    LINK_JOINT(w, node_b, jt, jt->body_b);
  return w;
}
