#include "mfl_tier2.h"
/*00557140*/ extern RwBool RtQuatConvertFromMatrix(struct RtQuat *p0, RwMatrix *p1);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define REC_POS_X_OFF (0x10)
#define REC_POS_Y_OFF (0x14)
#define REC_POS_Z_OFF (0x18)
#define REC_NEXT_OFF (0x1c)
#define REC_VALUE_OFF (0x20)
#define REC_SIZE (0x24)
#define REC_WORDS (9)
#define BUF_CURRENT_OFF (0x1c)
#define BUF_RECORD_COUNT_OFF (0x168)
#define BUF_LAST_VALUE_OFF (0x174)
#define BUF_FIRST_VALUE_OFF (0x178)
#define BUF_STARTED_OFF (0x194)
/* replay_buffer_record_frame */
/* "(buffer, matrix, value, index): stores position, quaternion and value of one replay record" */
/*
 * Stores one replay sample (position from matrix, orientation as quaternion, and value) in the
 * buffer's current record and moves the buffer's cursor on to the next record (inferred).
 * The sample is written only if sample_index is below the threshold at +0x170 or nothing was
 * recorded yet (inferred). +0x170 is incremented on every call (inferred).
 */
void replay_buffer_record_frame(char *buffer,RwMatrix *matrix,undefined4 value,int sample_index)
{
  char *next_record;
  undefined4 *next_record_words;
  void *new_record;
  char *scratch; /* loop counter, then reused as the current record pointer */
  undefined4 *cursor; /* walks the new record while zeroing, later the current record */

  /* Record the sample when it is due, or when nothing has been recorded yet */
  if ((*(int *)(buffer + 0x170) > sample_index) || (*(int *)(buffer + BUF_STARTED_OFF) == 0)) {
    *(RwReal *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_POS_X_OFF) = (matrix->pos).x;
    *(RwReal *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_POS_Y_OFF) = (matrix->pos).y;
    *(RwReal *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_POS_Z_OFF) = (matrix->pos).z;
    RtQuatConvertFromMatrix(*(RtQuat **)(buffer + BUF_CURRENT_OFF),matrix);
    *(undefined4 *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_VALUE_OFF) = value;
    /* First use of this record: allocate the next record and zero its REC_WORDS words */
    if (*(int *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF) == 0) {
      new_record = RwMalloc(REC_SIZE,0);
      *(void **)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF) = new_record;
      cursor = *(undefined4 **)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF);
      for (scratch = REC_WORDS; scratch != 0; scratch = scratch + -1) {
        *cursor = 0;
        cursor = cursor + 1;
      }
    }
    /* Copy the position of the current record into the next record */
    scratch = *(char **)(buffer + BUF_CURRENT_OFF);
    next_record = *(char **)(scratch + REC_NEXT_OFF);
    *(RwV3d *)(next_record + REC_POS_X_OFF) = *(RwV3d *)(scratch + REC_POS_X_OFF);
    /* Copy the orientation (quaternion) of the current record into the next record, then advance the cursor to it */
    cursor = *(undefined4 **)(buffer + BUF_CURRENT_OFF);
    next_record_words = (undefined4 *)cursor[7];
    *(RtQuat *)next_record_words = *(RtQuat *)cursor;
    *(undefined4 *)(buffer + BUF_CURRENT_OFF) = *(undefined4 *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF);
    *(undefined4 *)(buffer + BUF_LAST_VALUE_OFF) = value;
    *(int *)(buffer + BUF_RECORD_COUNT_OFF) = *(int *)(buffer + BUF_RECORD_COUNT_OFF) + 1;
    *(undefined4 *)(buffer + 0x170) = 1;
    if (*(int *)(buffer + BUF_STARTED_OFF) == 0) {
      *(undefined4 *)(buffer + BUF_FIRST_VALUE_OFF) = value;
    }
    *(undefined4 *)(buffer + BUF_STARTED_OFF) = 1;
  }
  *(int *)(buffer + 0x170) = *(int *)(buffer + 0x170) + 1;
  return;
}
