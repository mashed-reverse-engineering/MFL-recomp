#include "mfl_tier2.h"

extern int rwp_capsule_005757d0(unsigned int a, unsigned int b);

/* Size description of a physics world: counts of bodies (0x8, 0x14), constraints (0xc), joints (0x18) and a parameter (0x1c). */
typedef struct MflWorldDesc {
  unsigned char pad00[8];
  unsigned int bodies;   /* 0x8 */
  unsigned int sets;   /* 0xc */
  unsigned char pad10[4];
  unsigned int groups;   /* 0x14 */
  unsigned int joints;   /* 0x18 */
  unsigned int param;   /* 0x1c */
} MflWorldDesc;

/* Number of bytes of memory a world of the given size needs: the sum of the sub-allocations (per group 328 + 48 bytes, bodies rounded
   up to a multiple of 32, 160 per joint, 16 + 4/32 per set entry, 12 per body) plus a fixed 0xc11c. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
unsigned int rwp_mgr_00569b30(MflWorldDesc *d)
{
  unsigned int total;

  total = rwp_capsule_005757d0(d->bodies, d->param) + d->groups * 328 + 0xc11c;
  total = total + d->groups * 48 + ((d->bodies + 31) >> 5 << 5);
  if (d->joints) total = total + d->joints * 160;
  if (d->sets) total = total + (((d->sets + 31) >> 5) + d->sets * 4) * 4;
  return total + d->bodies * 12;
}
