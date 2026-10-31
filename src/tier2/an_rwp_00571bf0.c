#include "mfl_tier2.h"

/* Destination: rows of 4 words and a row count. */
typedef struct MflGatherOut {
  int (*rows)[4];
  int count;   /* 0x4 */
} MflGatherOut;
/* Source selection: a list of word offsets (units of 4) into the source table. */
typedef struct MflGatherList {
  unsigned int *idx;
  unsigned int count;   /* 0x4 */
} MflGatherList;

/* For every selected entry idx[i] of the source table (16 bytes per unit of 4): out row i = words 0, 3, 6, 9 at that position
   (four words 12 bytes apart: a strided gather); then the row count is set. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_pipeline_00571bf0(MflGatherOut *out, char *table, int unused3, int unused4, MflGatherList *list)
{
  unsigned int i;
  int *src;

  if (list->count) {
    i = 0;
    do {
      src = (int *)(table + ((list->idx[i] >> 2) << 4));
      out->rows[i][0] = src[0];
      out->rows[i][1] = src[3];
      out->rows[i][2] = src[6];
      out->rows[i][3] = src[9];
      i++;
    } while (i != list->count);
  }
  out->count = list->count;
}
