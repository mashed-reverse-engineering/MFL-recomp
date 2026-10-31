#include "mfl_tier2.h"

extern int rwp_pipeline_005738a0(void *owner);

/* A triangle as the caller has it: normal, a code and pointers to the three vertices. */
typedef struct MflTriIn {
  int n[3];   /* 0x0 */
  int pad0c[3];
  unsigned int code;   /* 0x18 */
  int *p0;   /* 0x1c */
  int *p1;   /* 0x20 */
  int *p2;   /* 0x24 */
} MflTriIn;

/* One stored triangle record, 0x4c bytes. */
typedef struct MflTriRec {
  int p0[3], pad0c;
  int p1[3], pad1c;
  int p2[3], pad2c;
  int n[3], pad3c;
  void *owner;   /* 0x40 */
  unsigned int code;   /* 0x44 */
  unsigned int nibble;   /* 0x48 */
} MflTriRec;

/* A growing list of records: base pointer, count, capacity and the table of 4 bit flags (one nibble per code) of the owner. */
typedef struct MflTriList {
  MflTriRec *recs;
  unsigned int count;   /* 0x4 */
  unsigned int capacity;   /* 0x8 */
  int pad0c;
  unsigned int **flags;   /* 0x10 */
} MflTriList;

/* Appends a record for the triangle tri owned by owner to the list (returns 0 when it is full, else tri).  The nibble of the owner's
   flag table for the triangle's code is stored in the record (0xf without a table). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
MflTriIn *rwp_trilist_005748c0(int unused1, void *owner, MflTriIn *tri, int unused4, MflTriList *list)
{
  unsigned int i = list->count;
  MflTriRec *r;
  int *q;

  if (i >= list->capacity) return 0;
  r = &list->recs[i];
  list->count = i + 1;
  if (owner) list->flags = (unsigned int **)rwp_pipeline_005738a0(owner);
  q = tri->p0; r->p0[0] = q[0]; r->p0[1] = q[1]; r->p0[2] = q[2];
  q = tri->p1; r->p1[0] = q[0]; r->p1[1] = q[1]; r->p1[2] = q[2];
  q = tri->p2; r->p2[0] = q[0]; r->p2[1] = q[1]; r->p2[2] = q[2];
  r->n[0] = tri->n[0]; r->n[1] = tri->n[1]; r->n[2] = tri->n[2];
  r->owner = owner;
  r->code = tri->code;
  if (list->flags) {
    r->nibble = (*list->flags)[tri->code >> 3] >> ((tri->code & 7) << 2) & 0xf;
    return tri;
  }
  r->nibble = 0xf;
  return tri;
}
