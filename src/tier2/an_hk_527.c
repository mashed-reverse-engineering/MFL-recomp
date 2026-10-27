#include "mfl_tier2.h"
/*0075bfa8*/ extern char *g_particles_75bfa8;
/*0075bfac*/ extern char *g_particles_75bfac;
/*0075bfb0*/ extern char *g_particles_75bfb0;
/*0075bfb4*/ extern char *g_particles_75bfb4;
/*0075bfb8*/ extern char *g_particles_75bfb8;
/*0075bfbc*/ extern char *g_particles_75bfbc;
/*0075bfc0*/ extern char *g_particles_75bfc0;
/*0075bfc4*/ extern char *g_particles_75bfc4;
/*0075bfc8*/ extern char *g_particles_75bfc8;
/*0075bfcc*/ extern char *g_particles_75bfcc;
/*0075bfd0*/ extern char *g_particles_75bfd0;
/*0075bfd4*/ extern char *g_particles_75bfd4;
/*0075bfd8*/ extern char *g_particles_75bfd8;
/*0075bfdc*/ extern char *g_particles_75bfdc;
/*0075cda8*/ extern char *g_particles_75cda8;
/* particles_slot_set_vectors */

/* "stores three vectors into slot (system, index) (max 16 each)" */

/* (inferred names) slot (system, slot_count) gets vec_a (twice, at +0 and +0x28), vec_b, vec_c and scalar (twice); returns 0 when the system already holds 16 slots */
undefined4
particles_slot_set_vectors
          (int system,undefined4 *vec_a,undefined4 scalar,undefined4 *vec_b,
          undefined4 *vec_c)

{
  int slot_count;
  undefined4 vec_a_z;
  int slot_offset;
  
  slot_count = ((int *)&g_particles_75cda8)[system];
  if (0xf < slot_count) {
    return 0;
  }
  slot_offset = (system * 0x10 + slot_count) * 0x38;
  *(undefined4 *)((char *)&g_particles_75bfa8 + slot_offset) = *vec_a;
  *(undefined4 *)((char *)&g_particles_75bfac + slot_offset) = vec_a[1];
  *(undefined4 *)((char *)&g_particles_75bfb0 + slot_offset) = vec_a[2];
  *(undefined4 *)((char *)&g_particles_75bfb8 + slot_offset) = *vec_b;
  *(undefined4 *)((char *)&g_particles_75bfbc + slot_offset) = vec_b[1];
  *(undefined4 *)((char *)&g_particles_75bfc0 + slot_offset) = vec_b[2];
  *(undefined4 *)((char *)&g_particles_75bfc4 + slot_offset) = *vec_c;
  *(undefined4 *)((char *)&g_particles_75bfc8 + slot_offset) = vec_c[1];
  *(undefined4 *)((char *)&g_particles_75bfcc + slot_offset) = vec_c[2];
  *(undefined4 *)((char *)&g_particles_75bfb4 + slot_offset) = scalar;
  *(undefined4 *)((char *)&g_particles_75bfd0 + slot_offset) = *vec_a;
  *(undefined4 *)((char *)&g_particles_75bfd4 + slot_offset) = vec_a[1];
  vec_a_z = vec_a[2];
  ((int *)&g_particles_75cda8)[system] = slot_count + 1;
  *(undefined4 *)((char *)&g_particles_75bfd8 + slot_offset) = vec_a_z;
  *(undefined4 *)((char *)&g_particles_75bfdc + slot_offset) = scalar;
  return 1;
}


