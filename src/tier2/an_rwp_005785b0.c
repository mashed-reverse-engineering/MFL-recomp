#include "mfl_tier2.h"
#include "rwp_common.h"

/* Island building of the Rwp batch: walks the pair records of one step and links the bodies they touch into union-find groups.  A body is
   active when its bit is set in its info's bitmap; a pair of an active and an inactive body (or of two bodies of equal activity) is handled the
   same way: the body's cell (one 16 byte cell per body slot, a union-find node whose group is found by following the parent links with path
   compression) is attached to a new group (578b70), the pair is added to the group of the cell (578c10) or two groups are merged (578ca0).
   The first loop takes `n_list` pair records through a pointer list, the second one `n_recs` records of 0xe0 bytes. */

typedef struct MflUfCell {
  struct MflUfCell *parent;   /* 0x0: 0 = not in a group, itself = root */
  void *group;   /* 0x4: group record (valid at the root) */
  int pad8;
  void *body;   /* 0xc: the body the cell was created for */
} MflUfCell;
typedef struct MflBodyInfo {
  unsigned char pad00[0x60];
  unsigned int *active;   /* 0x60: bitmap by body slot */
} MflBodyInfo;
typedef struct MflPairBody {
  unsigned char pad00[0x20];
  unsigned short slot;   /* 0x20 */
  unsigned char pad22[2];
  MflBodyInfo *info;   /* 0x24 */
} MflPairBody;
typedef struct MflPairRec {
  unsigned char pad00[8];
  int extra;   /* 0x8: first word of the part handed to the group builders */
  unsigned char pad0c[0x44];
  MflPairBody *a;   /* 0x50 */
  unsigned short a_id;   /* 0x54: 0xffff = none */
  unsigned char pad56[2];
  MflPairBody *b;   /* 0x58 */
  unsigned short b_id;   /* 0x5c */
} MflPairRec;
typedef struct MflPairBlock {
  unsigned char pad00[0xac];
  int used;   /* 0xac */
  unsigned char padb0[4];
  MflPairBody *a;   /* 0xb4 */
  unsigned short a_id;   /* 0xb8 */
  unsigned char padba[2];
  MflPairBody *b;   /* 0xbc */
  unsigned short b_id;   /* 0xc0 */
  unsigned char padc2[0x12];
  int extra;   /* 0xd4 */
  unsigned char padd8[8];
} MflPairBlock;

extern void rwp_batch_00578b70(void *table, MflUfCell *a, MflUfCell *b, void *x4, void *x5);
extern void rwp_batch_00578c10(void *group, MflUfCell *a, MflUfCell *b, void *x4, void *x5);
extern void rwp_batch_00578ca0(void *table, void *ga, void *gb, void *x4, void *x5);

/* active: the id is set and the body's bit is set */
#define ACTIVE(o, id) ((id) != 0xffff && ((o)->info->active[(o)->slot >> 5] & (1u << ((o)->slot & 0x1f))))
/* group of a cell: 0 for a free cell, the root's group otherwise; every cell on the path to the root is pointed at the root */
#define FIND(out, s) do { \
    MflUfCell *c_ = (s)->parent, *r_, *n_; \
    if (!c_) (out) = 0; \
    else if (c_ == (s)) (out) = (s)->group; \
    else { \
      r_ = c_; \
      while (r_->parent != r_) r_ = r_->parent; \
      n_ = (s); \
      do { MflUfCell *p_ = c_; n_->parent = r_; n_ = p_; c_ = p_->parent; } while (n_ != c_); \
      (out) = r_->group; \
    } } while (0)
/* one pair: OA / OB bodies, IA / IB their ids, X4 / X5 the 4th / 5th argument slots of the builders */
#define PAIR(OA, IA, OB, IB, X4, X5) do { \
    int fa_ = ACTIVE(OA, IA), fb_ = ACTIVE(OB, IB); \
    if (fa_ != fb_) { \
      MflPairBody *o_ = fa_ ? (OA) : (OB); \
      MflUfCell *s_ = cells + o_->slot; \
      void *g_; \
      if (!s_->parent) { s_->body = o_; rwp_batch_00578b70(table, s_, 0, X4, X5); } \
      else { FIND(g_, s_); rwp_batch_00578c10(g_, 0, s_, X4, X5); } \
    } else { \
      MflPairBody *pa_ = (OA), *pb_ = (OB); \
      MflUfCell *sa_ = cells + pa_->slot, *sb_ = cells + pb_->slot; \
      int ea_ = sa_->parent == 0, eb_ = sb_->parent == 0; \
      void *ga_, *gb_; \
      if (ea_) sa_->body = pa_; \
      if (eb_) sb_->body = pb_; \
      if (ea_ != eb_) { \
        if (ea_) { FIND(gb_, sb_); rwp_batch_00578c10(gb_, sb_, sa_, X4, X5); } \
        else { FIND(ga_, sa_); rwp_batch_00578c10(ga_, sa_, sb_, X4, X5); } \
      } else if (ea_) rwp_batch_00578b70(table, sa_, sb_, X4, X5); \
      else { FIND(ga_, sa_); FIND(gb_, sb_); rwp_batch_00578ca0(table, ga_, gb_, X4, X5); } \
    } } while (0)

void rwp_batch_005785b0(void *table, MflUfCell *cells, int unused3, int unused4, int unused5, MflPairRec **list, unsigned n_list,
                        MflPairBlock *recs, unsigned n_recs)
{
  unsigned i;
  for (i = 0; i < n_list; i++) {
    MflPairRec *r = list[i];
    PAIR(r->a, r->a_id, r->b, r->b_id, (void *)&r->extra, (void *)0);
  }
  for (i = 0; i < n_recs; i++) {
    MflPairBlock *r = recs + i;
    if (!r->used) continue;
    PAIR(r->a, r->a_id, r->b, r->b_id, (void *)0, (void *)&r->extra);
  }
}
