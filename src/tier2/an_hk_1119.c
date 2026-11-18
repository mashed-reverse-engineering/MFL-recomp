#include "mfl_tier2.h"
/* gap_4c4e70 */

/* no context */

/* object that was hit: only the word at +0x18 is copied */
typedef struct HitObject {
  char unknown_00[0x18];
  undefined4 object_tag;
} HitObject;

/* hit as delivered by the collision walk: three leading words, the word at +0x18, and three pointers to 3-word vectors */
typedef struct HitSource {
  undefined4 head_first;
  undefined4 head_second;
  undefined4 head_third;
  undefined4 unused_0c[3];
  undefined4 extra_word;
  undefined4 *vec_first;
  undefined4 *vec_second;
  undefined4 *vec_third;
} HitSource;

/* output record of the keep-closest collector (0x40 bytes) */
typedef struct HitRecord {
  undefined4 head_first;
  undefined4 head_second;
  undefined4 head_third;
  undefined4 head_fourth;
  undefined4 vec_first[3];
  undefined4 vec_second[3];
  undefined4 vec_third[3];
  undefined4 object_word;
  float distance;
  HitObject *object;
} HitRecord;

typedef struct HitContext {
  HitRecord *record;
  int *count;
  float *best;
  HitObject *object;
} HitContext;

HitSource * gap_4c4e70(char *unused_1,HitSource *hit,float distance,HitContext *ctx)

{
  int *count_ptr;
  HitRecord *out_record;
  HitObject *hit_object;

  count_ptr = ctx->count;
  out_record = ctx->record;
  hit_object = ctx->object;
  /* Keep-closest collector (inferred): ctx->record=output record (may be null), ctx->count=hit counter, ctx->best=best distance, ctx->object=hit object. The record is filled for the first hit and for any hit closer than the best so far. */
  if (*count_ptr == 0) {
    *ctx->best = distance;
  }
  else if (!(distance < *ctx->best)) goto count_hit;      /* original: fcomp, test ah,5, jp (NaN skips) */
  if (out_record != (HitRecord *)0x0) {
    out_record->head_first = hit->head_first;
    out_record->head_second = hit->head_second;
    out_record->head_third = hit->head_third;
    out_record->head_fourth = hit->extra_word;
    out_record->vec_first[0] = hit->vec_first[0];
    out_record->vec_first[1] = hit->vec_first[1];
    out_record->vec_first[2] = hit->vec_first[2];
    out_record->vec_second[0] = hit->vec_second[0];
    out_record->vec_second[1] = hit->vec_second[1];
    out_record->vec_second[2] = hit->vec_second[2];
    out_record->vec_third[0] = hit->vec_third[0];
    out_record->vec_third[1] = hit->vec_third[1];
    out_record->vec_third[2] = hit->vec_third[2];
    out_record->object_word = hit_object->object_tag;
    out_record->distance = distance;
    out_record->object = hit_object;
    *count_ptr = *count_ptr + 1;
    return hit;
  }
count_hit:
  *count_ptr = *count_ptr + 1;
  return hit;
}
