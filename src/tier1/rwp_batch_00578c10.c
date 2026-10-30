/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpLink { struct RwpLink *next; struct RwpNode *node; struct RwpLink *peer; } RwpLink;
typedef struct RwpChain { struct RwpChain *next; } RwpChain;
typedef struct RwpNode {
  int used; int kind; RwpLink *link; RwpLink *head;
  int a_count; RwpChain *a_tail; RwpChain *a_head; int b_count; RwpChain *b_tail; RwpChain *b_head;
} RwpNode;
typedef struct RwpLinkSet {
  int link_total; int link_count; int pad; RwpLink *link_last;
  int a_count; RwpChain *a_first; RwpChain *a_last; int b_count; RwpChain *b_first; RwpChain *b_last;
} RwpLinkSet;
/* Appends a link and the first records of two singly linked chains to a link set, counting each. (Rwp library) */
void rwp_batch_00578c10(RwpLinkSet *set, RwpNode *node, RwpLink *link, RwpChain *chain_a, RwpChain *chain_b)
{
  if (node != 0) {
    set->link_total++;
    link->next = (RwpLink *)node;
    link->node = 0;
    link->peer = 0;
    set->link_last->peer = link;
    set->link_last = link;
    set->link_count++;
  }
  if (chain_a != 0) {
    chain_a->next = 0;
    if (set->a_count == 0) {
      set->a_last = chain_a;
      set->a_first = chain_a;
      set->a_count = 1;
    } else {
      set->a_last->next = chain_a;
      set->a_last = chain_a;
      set->a_count++;
    }
  }
  if (chain_b != 0) {
    chain_b->next = 0;
    if (set->b_count == 0) {
      set->b_last = chain_b;
      set->b_first = chain_b;
      set->b_count = 1;
    } else {
      set->b_last->next = chain_b;
      set->b_last = chain_b;
      set->b_count++;
    }
  }
}
