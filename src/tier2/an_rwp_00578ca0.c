#include "mfl_tier2.h"

/* A member of a group's chain of entries: the group entry it belongs to (0), the group record that owns the chain (4) and the next
   member (8). */
typedef struct MflUfEntry {
  struct MflUfEntry *parent;   /* 0x0 */
  struct MflUfGroup *owner;   /* 0x4 */
  struct MflUfEntry *next;   /* 0x8 */
} MflUfEntry;
/* A singly linked item list: the first word of an item is its next pointer. */
typedef struct MflUfItem { struct MflUfItem *next; } MflUfItem;
/* A group record (40 bytes): union-find rank and size, the chain of entries (first at 8, last at 0xc) and two item lists (count, head, tail). */
typedef struct MflUfGroup {
  unsigned int rank;   /* 0x0 */
  unsigned int size;   /* 0x4 */
  MflUfEntry *first;   /* 0x8 */
  MflUfEntry *last;   /* 0xc */
  unsigned int n1;   /* 0x10 */
  MflUfItem *head1;   /* 0x14 */
  MflUfItem *tail1;   /* 0x18 */
  unsigned int n2;   /* 0x1c */
  MflUfItem *head2;   /* 0x20 */
  MflUfItem *tail2;   /* 0x24 */
} MflUfGroup;
/* The table of groups: the count at 4, the records from 8. */
typedef struct MflUfTable {
  int pad0;
  int count;   /* 0x4 */
  MflUfGroup groups[1];   /* 0x8 */
} MflUfTable;

/* Union of two groups a and b of the table (union by rank): the lower ranked group is chained behind the other one (entries, sizes, both
   item lists), the group count drops by one and the last record of the table moves into the freed slot.  The items item1 / item2 (when
   given) are appended to the merged group's lists 1 / 2.  With a == b only the items are appended. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_00578ca0(MflUfTable *t, MflUfGroup *a, MflUfGroup *b, MflUfItem *item1, MflUfItem *item2)
{
  MflUfGroup *win, *lose;
  unsigned int ra, rb;

  if (a != b) {
    rb = b->rank;
    ra = a->rank;
    if (ra > rb) {
      win = a;
      lose = b;
    } else {
      win = b;
      lose = a;
      if (rb == ra) {
        rb++;
        b->rank = rb;
      }
    }
    lose->first->parent = win->first;
    lose->first->owner = 0;
    win->last->next = lose->first;
    win->last = lose->last;
    win->size = win->size + lose->size;
    if (lose->n1) {
      if (!win->n1) {
        win->head1 = lose->head1;
        win->tail1 = lose->tail1;
        win->n1 = lose->n1;
      } else {
        win->tail1->next = lose->head1;
        win->tail1 = lose->tail1;
        win->n1 = win->n1 + lose->n1;
      }
    }
    if (lose->n2 > 0) {
      if (!win->n2) {
        win->head2 = lose->head2;
        win->tail2 = lose->tail2;
        win->n2 = lose->n2;
      } else {
        win->tail2->next = lose->head2;
        win->tail2 = lose->tail2;
        win->n2 = win->n2 + lose->n2;
      }
    }
    if (item1) {
      item1->next = 0;
      if (!win->n1) {
        win->tail1 = item1;
        win->head1 = item1;
        win->n1 = 1;
      } else {
        win->tail1->next = item1;
        win->tail1 = item1;
        win->n1++;
      }
    }
    if (item2) {
      item2->next = 0;
      if (!win->n2) {
        win->tail2 = item2;
        win->head2 = item2;
        win->n2 = 1;
      } else {
        win->tail2->next = item2;
        win->tail2 = item2;
        win->n2++;
      }
    }
    t->count = t->count - 1;
    if (t->count != 0) {
      *lose = t->groups[t->count];
      lose->first->owner = lose;
    }
    return;
  }
  if (item1) {
    item1->next = 0;
    if (!a->n1) {
      a->tail1 = item1;
      a->head1 = item1;
      a->n1 = 1;
    } else {
      a->tail1->next = item1;
      a->tail1 = item1;
      a->n1++;
    }
  }
  if (item2) {
    item2->next = 0;
    if (!a->n2) {
      a->n2 = 1;
      a->tail2 = item2;
      a->head2 = item2;
    } else {
      a->tail2->next = item2;
      a->tail2 = item2;
      a->n2++;
    }
  }
}
