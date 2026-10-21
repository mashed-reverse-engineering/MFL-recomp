#include "mfl_tier2.h"
typedef struct PoolNode {
  int key;
  struct PoolNode *prev;
  struct PoolNode *next;
  unsigned short slot;
  unsigned short pad;
  int data;
} PoolNode;
typedef struct PoolList {
  short count;
  short pad;
  PoolNode *head;
  PoolNode *tail;
  int unused_c;
  int *slots;
} PoolList;
/* pool_list_unlink */
/* "unlinks the node with key param_2 from the pool's list" */
int __cdecl pool_list_unlink(PoolList *list, int key)
{
  PoolNode *node;
  PoolNode *next;
  PoolNode *tail;
  int result = 0;

  node = list->head;
  if (node == 0) {
    return result;
  }
  if (node->key == key) {
    next = node->next;
    if (next == 0) {
      list->head = 0;
      list->tail = 0;
    }
    else {
      list->head = next;
      next->prev = 0;
    }
  }
  else {
    tail = list->tail;
    if (tail->key == key) {
      next = tail->prev;
      list->tail = next;
      next->next = 0;
      node = tail;
    }
    else {
      node = node->next;
      if (node == 0) {
        return result;
      }
      while (node != tail) {
        if (node->key == key) {
          node->prev->next = node->next;
          node->next->prev = node->prev;
          goto unlinked;
        }
        node = node->next;
        if (node == 0) {
          return result;
        }
      }
      return result;
    }
  }
unlinked:
  list->count--;
  node->key = 0;
  node->next = 0;
  node->prev = 0;
  node->data = 0;
  list->slots[node->slot] = 0;
  result = 1;
  return result;
}
