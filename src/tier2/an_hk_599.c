#include "mfl_tier2.h"
/*0042a3f0*/ extern void * course_get_rpworld();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f44a0*/ extern struct RpWorld * RpWorldAddAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void FUN_0056a210(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0056bf10*/ extern float FUN_0056bf10(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056c080*/ extern void FUN_0056c080(undefined4 p0, undefined4 p1);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void FUN_0056cac0(undefined4 p0, undefined4 p1);
/*0056cdf0*/ extern void FUN_0056cdf0(undefined4 p0, undefined4 p1);
/*0058c8b0*/ extern void FUN_0058c8b0(undefined4 p0, undefined4 p1);
/*0058c990*/ extern int FUN_0058c990(undefined4 p0);
/*0058cb90*/ extern void * FUN_0058cb90(void);
/*0058cbe0*/ extern void FUN_0058cbe0(undefined4 p0);
/*005dd378*/ extern float k_f_0_1;
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fd0f8*/ extern undefined4 g_rw_6fd0f8;
/*006fea90*/ extern undefined4 g_rw_6fea90;
/*00701c9c*/ extern undefined4 g_rw_701c9c;

/* a 4x4 float matrix (RwMatrix) is 16 words: the size of the per-object matrix copy and the stride of the matrix table */
#define MATRIX_WORDS (0x10)
/* clears bits 0x0c (2 and 3) of the flags word at +0x8 of the physics object (inferred) */
#define PHYS_FLAGS_CLEAR_MASK (0xfffffff3)

/* rwp_object_create_from_atomic */

/* "creates one body from an atomic clone: box size, mass properties, frame, adds it to the world"
    */

/*
 * Creates one physics body for the atomic `atomic` and adds a clone of it, with its own frame, to the course world.
 *   setting_a, setting_b, setting_c: three per-body settings, passed to FUN_0056cdf0 / FUN_0056cac0 / FUN_0056ca70 (inferred)
 *   box_size: scaled by k_f_0_1 and passed to FUN_0056c080 (inferred: box size)
 *   mass: passed to FUN_0056bf10 (inferred: mass properties)
 *   user_id: stored in the per-object table g_rw_6fa4b0 at the new slot (inferred)
 * Returns the index of the new slot in the object tables (g_rw_6fa7d0 before the increment).
 */
int rwp_object_create_from_atomic(RpAtomic *atomic,undefined4 setting_a,float box_size,undefined4 setting_b,
              undefined4 setting_c,undefined4 mass,undefined4 user_id)
{
  float scaled_size;
  int slot;
  undefined4 *body;
  int phys_handle;
  RpAtomic *clone;
  RwFrame *frame;
  RpWorld *world;
  undefined4 handle;
  int word;
  undefined4 *src;
  undefined4 *dst;

  /* get a new body and clear its 3 words after the id (inferred) */
  body = (undefined4 *)FUN_0058cb90();
  handle = *body;
  body[1] = 0;
  body[2] = 0;
  body[3] = 0;
  FUN_0056cdf0(handle,setting_a);
  FUN_0056cac0(handle,setting_b);
  FUN_0056ca70(handle,setting_c);
  FUN_0056c080(handle,(scaled_size = box_size * k_f_0_1, *(int *)&scaled_size));
  FUN_0056bf10(body,mass,0);
  phys_handle = FUN_0058c990(body);
  slot = g_rw_6fa7d0;
  (&g_rw_6fa4b0)[g_rw_6fa7d0] = user_id;
  (&g_rw_6fd0f8)[slot] = 0;
  src = (undefined4 *)((LONG_PTR)RpAtomic_object_object_parent(atomic) + 0x10);
  dst = &g_rw_6fea90 + slot * MATRIX_WORDS;
  /* copy 16 words from the atomic's parent (+0x10, a matrix (inferred)) into the matrix table slot of the new object */
  for (word = MATRIX_WORDS; word != 0; word = word + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  /* clone the atomic, give the clone a new frame, attach it to the physics body and add it to the course world */
  clone = RpAtomicClone(atomic);
  (&g_rw_6fab78)[g_rw_6fa7d0] = clone;
  frame = RwFrameCreate();
  RpAtomicSetFrame((RpAtomic *)(&g_rw_6fab78)[g_rw_6fa7d0],frame);
  FUN_0058c8b0((&g_rw_6fab78)[g_rw_6fa7d0],phys_handle);
  clone = (RpAtomic *)(&g_rw_6fab78)[g_rw_6fa7d0];
  world = (RpWorld *)course_get_rpworld();
  RpWorldAddAtomic(world,clone);
  /* clear the flag bits 2 and 3 of the physics object (inferred) */
  *(uint *)(phys_handle + 8) = *(uint *)(phys_handle + 8) & PHYS_FLAGS_CLEAR_MASK;
  /* register the physics object: id from the first field of g_rw_701c9c (inferred) */
  handle = rw_first_field(g_rw_701c9c);
  FUN_0056a210(handle, phys_handle, 0xffffffff, 2);
  slot = g_rw_6fa7d0;
  g_rw_6fa7d0 = g_rw_6fa7d0 + 1;
  FUN_0058cbe0(body);
  return slot;
}


