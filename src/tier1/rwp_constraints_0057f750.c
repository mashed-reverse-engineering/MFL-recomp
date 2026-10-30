/* compile: /G5 */
#include "mfl_tier2.h"
extern char g_rwp_constraint_padding[];
typedef struct RwpConstraintState { char pad00[0x4]; int c04; char pad08[0xc]; int c14; char pad18[0x8]; int c20; char pad24[0x8]; int c2c; char pad30[0x8]; int c38; char pad3c[0x8]; int c44; char pad48[0x8]; int c50; char pad54[0x8]; int c5c; char pad60[0x8]; int c68; char pad6c[0x20]; int c8c; char pad90[0x8]; int c98; char pad9c[0x14]; int cb0; char padb4[0x4]; int *words; char padbc[0x8]; int *counts; char padc8[0x8]; int *sums; int idx_d0; char padd8[0x4]; int *pads; int idx_dc; char pade4[0x14]; int cursor; char padfc[0x4]; int c100; int c104; int c108; } RwpConstraintState;
extern void rwp_constraints_0057f8a0(RwpConstraintState *s, void *constraint);
/* Pads the constraint stream to a multiple of 4 words with padding constraints and advances all constraint-state counters by the emitted size. (Rwp library) */
void rwp_constraints_0057f750(RwpConstraintState *s)
{
  int w = s->words[s->cursor];
  int pad = (w + 3) & 0xfffffffc;
  int q, i;
  for (i = w; i != pad; i++) rwp_constraints_0057f8a0(s, g_rwp_constraint_padding);
  s->words[s->cursor] += w - pad;
  s->c04 += pad;
  q = pad / 4;
  s->c14 += q;
  s->c20 += q;
  s->c2c += q;
  s->c38 += q;
  s->c44 += q;
  s->c50 += q;
  s->c5c += q;
  s->c68 += q;
  s->c8c += q;
  s->c98 += pad;
  s->cb0++;
  s->pads[s->idx_dc] += pad;
  s->sums[s->idx_d0] += w;
  s->counts[s->idx_d0]++;
  s->c100 += w;
  s->c104 += w;
  s->c108 += pad;
  s->cursor++;
}
