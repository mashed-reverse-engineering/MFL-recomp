#include "mfl_tier2.h"
/*00457430*/ extern int course_spline_objects_create();
/*00457620*/ extern int load_shadow_models();
/*004817b0*/ extern int atomic_plugin_get_bbox();
/*0049a9b0*/ extern int fx_slot_find_free();
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004a5e50*/ extern int object_set_colour();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*005dd340*/ extern float k_f_0_5;
/*006a2648*/ extern float g_course_tierra_6a2648;
/*006a264c*/ extern float g_course_tierra_6a264c;
/*006a2650*/ extern float g_course_tierra_6a2650;
/*006a2654*/ extern int g_course_tierra_6a2654;
/*006a2658*/ extern undefined4 g_course_tierra_6a2658;
/*006a265c*/ extern undefined4 g_course_tierra_6a265c;
/*006a2660*/ extern undefined4 g_course_tierra_6a2660;
/*006a2664*/ extern undefined4 g_course_tierra_6a2664;
/*008c4b20*/ extern undefined4 g_course_tierra_8c4b20;

/* mts_matrices: 16 matrix lists of 0xc00 bytes each (list k starts at k * 0xc00) */
#define MTS_LIST0_FIRST_RECORD (0x30) /* (inferred) first record of list 0 */
#define MTS_LIST1_FIRST_RECORD (0xc30) /* (inferred) first record of list 1 */
#define MTS_LIST2_FIRST_RECORD (0x1830) /* (inferred) first record of list 2 */
#define MTS_LIST3_START (0x2400) /* start of list 3 (3 * 0xc00) */
#define MTS_LIST4_START (0x3000) /* start of list 4 (4 * 0xc00) */
#define MTS_LIST5_START (0x3c00) /* start of list 5 (5 * 0xc00) */
#define MTS_LIST6_START (0x4800) /* start of list 6 (6 * 0xc00) */
#define MTS_RECORD_STRIDE (0x40) /* (inferred) size of one RwMatrix record */
#define FX_SLOT_TYPE_LIST0 (0xd) /* (inferred) fx type used for list 0 records */
#define FX_SLOT_TYPE_LIST1 (7) /* (inferred) fx type used for list 1 records */
#define FX_SLOT_TYPE_LIST2 (0xc) /* (inferred) fx type used for list 2 records */
#define FX_SLOT_FIND_ARG4 (0x14) /* (inferred) 4th argument of fx_slot_find_free */
#define OBJECT_COLOUR_ALL_BITS (0xffffffff) /* (inferred) 1st argument of object_set_colour */
#define FLOAT_BITS_0_1 (0x3dcccccd) /* bit pattern of 0.1f */
#define FLOAT_BITS_1_0 (0x3f800000) /* bit pattern of 1.0f */
/* course_tierra_n_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/*
 * Course setup for Tierra N (inferred from the callees; the name is from the decompiler).
 * 1. Creates object frames from the first atomic of clumps[20], clumps[21], clumps[17] and clumps[19],
 *    using matrix lists 3, 4, 5 and 6 (mts_matrix_counts[3..6]).
 * 2. Reads the bounding box of clumps[21] (inferred layout: min xyz at [0..2], max xyz at [3..5])
 *    and stores its centre in g_course_tierra_6a2648 (x), g_course_tierra_6a264c (y), g_course_tierra_6a2650 (z).
 * 3. Sets the object colour of the clumps[17] and clumps[19] objects.
 * 4. For matrix lists 0, 1 and 2 (mts_matrix_counts[0..2]) allocates one fx slot per record and stores the
 *    slot ids in g_course_tierra_8c4b20, counting them in g_course_tierra_6a2654.
 * 5. Calls course_spline_objects_create and load_shadow_models.
 * world: the CourseWorld being set up. Returns nothing.
 */
void course_tierra_n_cb0(struct CourseWorld *world)

{
  int world_addr = (int)world;
  undefined4 atomic_or_slot;
  float *bbox;
  float half_extent_y;
  int entry_index;
  int entry_ptr;

  /* clumps[20] atomic, frames from matrix list 3 */
  atomic_or_slot = (undefined4)(clump_first_atomic((RpClump *)(world->clumps[20])));
  g_course_tierra_6a2658 =
       object_frames_create_with_matrices(atomic_or_slot,world->mts_matrix_counts[3],world_addr + MTS_LIST3_START);
  /* clumps[21] atomic: its bounding box gives the centre values below */
  atomic_or_slot = (undefined4)(clump_first_atomic((RpClump *)(world->clumps[21])));
  bbox = (float *)atomic_plugin_get_bbox(atomic_or_slot);
  g_course_tierra_6a2648 = (bbox[3] - *bbox) * k_f_0_5;
  half_extent_y = (bbox[4] - bbox[1]) * k_f_0_5;
  g_course_tierra_6a2650 = bbox[5] - bbox[2];
  g_course_tierra_6a2650 = g_course_tierra_6a2650 * k_f_0_5;
  g_course_tierra_6a2648 = g_course_tierra_6a2648 + *bbox;
  g_course_tierra_6a264c = half_extent_y + bbox[1];
  g_course_tierra_6a2650 = g_course_tierra_6a2650 + bbox[2];
  /* same clumps[21] atomic, frames from matrix list 4 */
  g_course_tierra_6a265c =
       object_frames_create_with_matrices(atomic_or_slot,world->mts_matrix_counts[4],world_addr + MTS_LIST4_START);
  /* clumps[17] atomic, frames from matrix list 5, then coloured */
  atomic_or_slot = (undefined4)(clump_first_atomic((RpClump *)(world->clumps[17])));
  g_course_tierra_6a2660 =
       object_frames_create_with_matrices(atomic_or_slot,world->mts_matrix_counts[5],world_addr + MTS_LIST5_START);
  object_set_colour(g_course_tierra_6a2660,OBJECT_COLOUR_ALL_BITS,FLOAT_BITS_0_1,FLOAT_BITS_0_1,FLOAT_BITS_0_1,FLOAT_BITS_1_0);
  /* clumps[19] atomic, frames from matrix list 6, then coloured */
  atomic_or_slot = (undefined4)(clump_first_atomic((RpClump *)(world->clumps[19])));
  g_course_tierra_6a2664 =
       object_frames_create_with_matrices(atomic_or_slot,world->mts_matrix_counts[6],world_addr + MTS_LIST6_START);
  object_set_colour(g_course_tierra_6a2664,OBJECT_COLOUR_ALL_BITS,FLOAT_BITS_0_1,FLOAT_BITS_0_1,FLOAT_BITS_0_1,FLOAT_BITS_1_0);
  g_course_tierra_6a2654 = 0;
  /* list 0: one fx slot per record. Here entry_index is the record pointer and entry_ptr the counter;
     the two loops below use the roles the other way round. */
  if (0 < world->mts_matrix_counts[0]) {
    entry_index = world_addr + MTS_LIST0_FIRST_RECORD;
    entry_ptr = 0;
    do {
      atomic_or_slot = fx_slot_find_free(FX_SLOT_TYPE_LIST0,entry_index,0,FX_SLOT_FIND_ARG4);
      (&g_course_tierra_8c4b20)[g_course_tierra_6a2654] = atomic_or_slot;
      g_course_tierra_6a2654 = g_course_tierra_6a2654 + 1;
      entry_ptr = entry_ptr + 1;
      entry_index = entry_index + MTS_RECORD_STRIDE;
    } while (entry_ptr < world->mts_matrix_counts[0]);
  }
  /* list 1: one fx slot per record */
  entry_index = 0;
  if (0 < world->mts_matrix_counts[1]) {
    entry_ptr = world_addr + MTS_LIST1_FIRST_RECORD;
    do {
      atomic_or_slot = fx_slot_find_free(FX_SLOT_TYPE_LIST1,entry_ptr,0,FX_SLOT_FIND_ARG4);
      (&g_course_tierra_8c4b20)[g_course_tierra_6a2654] = atomic_or_slot;
      g_course_tierra_6a2654 = g_course_tierra_6a2654 + 1;
      entry_index = entry_index + 1;
      entry_ptr = entry_ptr + MTS_RECORD_STRIDE;
    } while (entry_index < world->mts_matrix_counts[1]);
  }
  /* list 2: one fx slot per record */
  entry_index = 0;
  if (0 < world->mts_matrix_counts[2]) {
    entry_ptr = world_addr + MTS_LIST2_FIRST_RECORD;
    do {
      atomic_or_slot = fx_slot_find_free(FX_SLOT_TYPE_LIST2,entry_ptr,0,FX_SLOT_FIND_ARG4);
      (&g_course_tierra_8c4b20)[g_course_tierra_6a2654] = atomic_or_slot;
      g_course_tierra_6a2654 = g_course_tierra_6a2654 + 1;
      entry_index = entry_index + 1;
      entry_ptr = entry_ptr + MTS_RECORD_STRIDE;
    } while (entry_index < world->mts_matrix_counts[2]);
  }
  course_spline_objects_create(world_addr);
  load_shadow_models(world_addr);
  return;
}
