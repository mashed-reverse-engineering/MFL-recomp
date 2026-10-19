#include "mfl_tier2.h"
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005ddc28*/ extern float k_f_0;
/*00629424*/ extern undefined4 g_gfx_629424;
/*006c40a8*/ extern undefined4 g_particles_6c40a8;
/*006c40b4*/ extern undefined4 g_particles_6c40b4;
/*006c40b8*/ extern undefined4 g_gfx_6c40b8;
extern double fabs(double);
#define PARTICLE_STRIDE (0x50) /* bytes per particle record */
#define PARTICLE_AGE_OFFSET (0x44) /* byte offset of the age float in a record (inferred) */
#define PARTICLE_ALIVE_FLOAT_INDEX (0x12) /* float index of the alive word (right.z of the second matrix), 0 = free */
#define MATRIX_FLOAT_COUNT (0x10) /* floats in the 4x4 source matrix */
#define PARAM_FLAGS_BYTE_OFFSET (3) /* byte offset of the flags byte in params (inferred) */
/* particle_pool_find_oldest_or_free */

/* "picks the slot with the largest age in pool p (blocks of 0x50 bytes) when full" */

/* Allocates one particle in pool `pool_index` (records of PARTICLE_STRIDE bytes).
 * If the pool is full, the record with the largest age is evicted first (its alive word is cleared, live count decremented).
 * Then the first free record is initialised: src_matrix (16 floats) is copied into it, the matrix is scaled by |src_matrix[9]|,
 * the first float of params goes to the record's value field and the flags byte of params to its flags.
 * src_matrix: 4x4 transform source (inferred). params: per-particle values, NULL means g_gfx_629424 (inferred).
 * Returns nothing; if no free record is found the function returns early. */

void particle_pool_find_oldest_or_free(int pool_index,RwReal *src_matrix,RwReal *params)

{
  int *alive_count_ptr;
  float oldest_age;
  int pool_capacity;
  RwReal particle_value;
  int iVar5;
  RwReal *age_cursor;
  float *tail_age_ptr;
  int scanned_count;
  float *oldest_record;
  RwMatrix *pool;
  int group_last_index;
  RwMatrix *copy_dst;
  RwV3d scale;

  iVar5 = g_gfx_6c40b8;
  if (params == (RwReal *)0x0) {
    params = (RwReal *)&g_gfx_629424;
  }
  pool = *(RwMatrix **)(g_particles_6c40a8 + pool_index * 4);
  pool_capacity = *(int *)(g_particles_6c40b4 + pool_index * 4);
  /* pool full: find the record with the largest age (4 records per pass, then the remainder) */
  if (*(int *)(g_gfx_6c40b8 + pool_index * 4) == pool_capacity) {
    oldest_record = (float *)0x0;
    scanned_count = 0;
    oldest_age = k_f_0;
    if (3 < pool_capacity) {
      group_last_index = 3;
      age_cursor = &pool[3].at.y;
      do {
        if (oldest_age < age_cursor[-0x28]) {
          oldest_record = age_cursor + -0x39;
          oldest_age = age_cursor[-0x28];
        }
        if (oldest_age < age_cursor[-0x14]) {
          oldest_record = age_cursor + -0x25;
          oldest_age = age_cursor[-0x14];
        }
        if (oldest_age < *age_cursor) {
          oldest_record = age_cursor + -0x11;
          oldest_age = *age_cursor;
        }
        if (oldest_age < age_cursor[0x14]) {
          oldest_record = age_cursor + 3;
          oldest_age = age_cursor[0x14];
        }
        group_last_index = group_last_index + 4;
        scanned_count = scanned_count + 4;
        age_cursor = age_cursor + 0x50;
      } while (group_last_index < pool_capacity);
    }
    /* remaining records that did not fit a group of 4 */
    if (scanned_count < pool_capacity) {
      tail_age_ptr = (float *)(scanned_count * PARTICLE_STRIDE + PARTICLE_AGE_OFFSET + (int)pool);
      scanned_count = pool_capacity - scanned_count;
      do {
        if (oldest_age < *tail_age_ptr) {
          oldest_record = tail_age_ptr + -0x11;
          oldest_age = *tail_age_ptr;
        }
        tail_age_ptr = tail_age_ptr + 0x14;
        scanned_count = scanned_count + -1;
      } while (scanned_count != 0);
    }
    oldest_record[PARTICLE_ALIVE_FLOAT_INDEX] = 0.0;
    alive_count_ptr = (int *)(iVar5 + pool_index * 4);
    *alive_count_ptr = *alive_count_ptr + -1;
  }
  /* find the first free record (alive word == 0); iVar5 is the record index here */
  iVar5 = 0;
  if (0 < pool_capacity) {
    while (pool[1].right.z != 0.0) {
      iVar5 = iVar5 + 1;
      pool = (RwMatrix *)&pool[1].up;
      if (pool_capacity <= iVar5) {
        return;
      }
    }
    /* copy the source matrix into the record; scale = |src_matrix[9]| (meaning inferred) */
    scale.x = src_matrix[9];
    copy_dst = pool;
    for (iVar5 = MATRIX_FLOAT_COUNT; iVar5 != 0; iVar5 = iVar5 + -1) {
      (copy_dst->right).x = *src_matrix;
      src_matrix = src_matrix + 1;
      copy_dst = (RwMatrix *)&(copy_dst->right).y;
    }
    scale.x = fabs(scale.x);
    scale.y = scale.x;
    RwMatrixScale(pool,&scale,rwCOMBINEPRECONCAT);
    particle_value = *params;
    pool[1].right.y = 0.0;
    pool[1].right.x = particle_value;
    *(int *)&pool[1].right.z = 1;
    *(ushort *)&pool[1].flags = (ushort)*(byte *)((char *)params + PARAM_FLAGS_BYTE_OFFSET);
    iVar5 = g_gfx_6c40b8;
    *(undefined2 *)((int)&pool[1].flags + 2) = 0;
    *(int *)(iVar5 + pool_index * 4) = *(int *)(iVar5 + pool_index * 4) + 1;
  }
  return;
}


