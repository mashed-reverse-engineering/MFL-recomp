/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBody { char pad0[8]; unsigned flags; unsigned short layer; char pad1[0xe]; int shape; char pad2[0xc]; int link; } RwpBody;
typedef struct RwpPair { RwpBody *a; unsigned short a_tag; unsigned short pad; RwpBody *b; unsigned short b_tag; unsigned short pad2; int shape; } RwpPair;
typedef struct RwpHandle { RwpBody *body; char pad[0x10]; } RwpHandle;
typedef struct RwpPipeline { char pad00[0x74]; RwpBody **bodies; unsigned body_count; char pad7c[0x4]; RwpPair *pairs; unsigned pair_count; unsigned pair_capacity; int f8c; int f90; int f94; char pad98[0x348]; RwpHandle *handles; unsigned handle_count; } RwpPipeline;
extern int rwp_batch_00579040(RwpPipeline *p, RwpPair *pairs, unsigned count, int a, int b);
/* Collects the pipeline's bodies on layer 2 or above that are flagged 0x10 into the pair list, clears the handle links and runs the pair pass (rwp_batch_00579040); returns the pipeline. (Rwp library) */
RwpPipeline *rwp_pipeline_005718a0(RwpPipeline *p)
{
  unsigned i;
  for (i = 0; i < p->body_count; i++) {
    RwpBody *body = p->bodies[i];
    if (body->layer > 1 && (body->flags & 0x10) && p->pair_count < p->pair_capacity) {
      RwpPair *pair = &p->pairs[p->pair_count++];
      pair->a_tag = 0x8000;
      pair->a = body;
      pair->b_tag = 0x8000;
      pair->b = body;
      pair->shape = body->shape;
    }
  }
  for (i = 0; i < p->handle_count; i++)
    if (p->handles[i].body != 0) p->handles[i].body->link = 0;
  p->handle_count = 0;
  p->f90 = rwp_batch_00579040(p, p->pairs, p->pair_count, p->f8c, p->f94);
  return p;
}
