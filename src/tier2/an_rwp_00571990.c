#include "mfl_tier2.h"

#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))

typedef struct MflIslWorld MflIslWorld;

/* A body: id at 0x20 and its owner world at 0x24 (bit set 0x5c = "wanted"). */
typedef struct MflIslBody {
  unsigned char pad00[0x20];
  unsigned short id;   /* 0x20 */
  unsigned char pad22[2];
  struct MflIslOwner *owner;   /* 0x24 */
} MflIslBody;

typedef struct MflIslOwner {
  unsigned char pad00[0x5c];
  unsigned int *bits;   /* 0x5c */
} MflIslOwner;

/* A contact of an island: body A at 0xb4 (valid when the word at 0xb8 is not 0xffff), body B at 0xbc (word at 0xc0), next contact at 0xdc. */
typedef struct MflIslContact {
  unsigned char pad00[0xb4];
  MflIslBody *body_a;   /* 0xb4 */
  unsigned short has_a;   /* 0xb8 */
  unsigned char padba[2];
  MflIslBody *body_b;   /* 0xbc */
  unsigned short has_b;   /* 0xc0 */
  unsigned char padc2[0x1a];
  struct MflIslContact *next;   /* 0xdc */
} MflIslContact;

/* A joint: bodies at 0x50 / 0x58, valid when the words at 0x54 / 0x5c are not 0xffff. */
typedef struct MflIslJoint {
  unsigned char pad00[0x50];
  MflIslBody *body_a;   /* 0x50 */
  unsigned short has_a;   /* 0x54 */
  unsigned char pad56[2];
  MflIslBody *body_b;   /* 0x58 */
  unsigned short has_b;   /* 0x5c */
} MflIslJoint;

/* A group of 40 bytes: bodies, first contact, joints, counts, flag word at 0x18. */
typedef struct MflIslGroup {
  MflIslBody **bodies;   /* 0x0 */
  MflIslContact *contacts;   /* 0x4 */
  MflIslJoint **joints;   /* 0x8 */
  unsigned int nbodies;   /* 0xc */
  unsigned int njoints;   /* 0x10 */
  unsigned char pad14[4];
  unsigned int flags;   /* 0x18 */
  unsigned char pad1c[0xc];
} MflIslGroup;

struct MflIslWorld {
  unsigned char pad00[0x74];
  int w74, w78, w7c;
  unsigned char pad80[0xc];
  int w8c, w90;
  unsigned char pad94[0x20];
  int wb4, wb8;
  unsigned char padbc[8];
  int wc4;
  unsigned int ngroups;   /* 0xc8 */
  unsigned char padcc[8];
  MflIslGroup *groups;   /* 0xd4 */
  int *wd8;   /* 0xd8 */
  int wdc, wde;
};

extern int rwp_batch_00578310(void *a, int *d8, int dc, int e0, int w74, int w78, int b4, int b8, int w8c, int w90);

#define WANTED(b) ((b)->owner && BIT_TEST((b)->owner->bits, (b)->id))

/* Marks every group whose bodies, contacts and joints touch no wanted body (bit set 0x5c of the body's owner) with flag bit 0 (= "can sleep / is static"):
   the flag is cleared first, set again only when the scan finds nothing wanted.  Runs 578310 on the world first.  Returns the world. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflIslWorld *rwp_pipeline_00571990(MflIslWorld *w)
{
  unsigned int i, j;
  MflIslGroup *g;
  MflIslContact *c;

  w->wd8[0] = w->w7c;
  w->wd8[1] = 0;
  rwp_batch_00578310(&w->wc4, w->wd8, w->wdc, w->wde, w->w74, w->w78, w->wb4, w->wb8, w->w8c, w->w90);
  for (i = 0; i < w->ngroups; i++) {
    g = &w->groups[i];
    g->flags &= ~1;
    for (j = 0; j < g->nbodies; j++)
      if (WANTED(g->bodies[j])) goto next;
    for (c = g->contacts; c; c = c->next) {
      if (c->has_a != 0xffff && WANTED(c->body_a)) goto next;
      if (c->has_b != 0xffff && WANTED(c->body_b)) goto next;
    }
    for (j = 0; j < g->njoints; j++) {
      if (g->joints[j]->has_a != 0xffff && WANTED(g->joints[j]->body_a)) goto next;
      if (g->joints[j]->has_b != 0xffff && WANTED(g->joints[j]->body_b)) goto next;
    }
    g->flags |= 1;
  next:;
  }
  return w;
}
