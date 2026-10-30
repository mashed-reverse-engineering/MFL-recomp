/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpLink { struct RwpLink *next; struct RwpNode *node; struct RwpLink *peer; } RwpLink;
typedef struct RwpChain { struct RwpChain *next; } RwpChain;
typedef struct RwpNode {
  int used; int kind; RwpLink *link; RwpLink *head;
  int a_count; RwpChain *a_tail; RwpChain *a_head; int b_count; RwpChain *b_tail; RwpChain *b_head;
} RwpNode;
typedef struct RwpNodeList { int pad0; int count; RwpNode nodes[1]; } RwpNodeList;
/* Appends a node to the batch node list: links one or two link records and starts the two chains with the given heads. (Rwp library) */
void rwp_batch_00578b70(RwpNodeList *list, RwpLink *a, RwpLink *b, RwpChain *chain_a, RwpChain *chain_b)
{
  RwpNode *node = &list->nodes[list->count++];
  RwpLink *head;
  node->used = 1;
  if (b != 0 && a != b) {
    a->next = a;
    a->node = node;
    a->peer = b;
    b->next = a;
    b->node = 0;
    b->peer = 0;
    node->link = a;
    head = a->peer;
    node->kind = 2;
  } else {
    a->next = a;
    a->node = node;
    a->peer = 0;
    node->link = a;
    node->kind = 1;
    head = a;
  }
  node->head = head;
  if (chain_a != 0) {
    chain_a->next = 0;
    node->a_head = chain_a;
    node->a_tail = chain_a;
    node->a_count = 1;
    node->b_count = 0;
  }
  if (chain_b != 0) {
    chain_b->next = 0;
    node->a_count = 0;
    node->b_head = chain_b;
    node->b_tail = chain_b;
    node->b_count = 1;
  }
}
