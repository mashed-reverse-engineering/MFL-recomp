#include "mfl_tier2.h"
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*005df5d8*/ extern float k_f_146;
/*005df5dc*/ extern float k_f_n122;
/*008bf154*/ extern undefined4 g_course_8bf154[0x1000];
/*008c3f64*/ extern undefined4 g_course_8c3f64;
/*008c3fc0*/ extern undefined4 g_course_8c3fc0;
/*008c4014*/ extern char g_course_object_8c4014[];

#define OFFSET_TABLE_LEN (80)
#define OFFSET_GROUP_SIZE (10)
#define GROUPS_PER_LANE (4)
#define COURSE_OBJECT_RECORD_WORDS (0x3f)
#define FLOAT_1_0_BITS (0x3f800000)
#define MATRIX_FLAGS_IDENTITY_ORTHONORMAL (rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL)
/* course_object_offsets_init */

/* "fills the table of 80 float offsets (10, 35, 60 ... 235 then pairs) used to place the course
   objects" */

/* Places every course object record in g_course_8bf154 (COURSE_OBJECT_RECORD_WORDS words each, walked until
   g_course_object_8c4014). For each record: computes its position (x and y from the course tables, z from the
   base and the offset table), moves the record's atomic frame to that position, and copies the atomic's bounding
   sphere back into the record. Field offsets are relative to `record` (inferred). No parameters, no return value. */

void course_object_offsets_init(void)

{
  int tmp;
  int lane;
  int object_index;
  int *record;
  int *next_record;
  struct {
    RwV3d local_18c;
    float local_180 [OFFSET_TABLE_LEN];
    RwMatrix local_40;
  } frame;
  
  frame.local_180[0] = 10.0;
  frame.local_180[1] = 35.0;
  frame.local_180[2] = 60.0;
  frame.local_180[3] = 85.0;
  frame.local_180[4] = 110.0;
  frame.local_180[5] = 135.0;
  frame.local_180[6] = 160.0;
  frame.local_180[7] = 185.0;
  frame.local_180[8] = 210.0;
  frame.local_180[9] = 235.0;
  frame.local_180[10] = 15.0;
  frame.local_180[0xb] = 25.0;
  frame.local_180[0xc] = 65.0;
  frame.local_180[0xd] = 75.0;
  frame.local_180[0xe] = 115.0;
  frame.local_180[0xf] = 140.0;
  frame.local_180[0x10] = 155.0;
  frame.local_180[0x11] = 175.0;
  frame.local_180[0x12] = 200.0;
  frame.local_180[0x13] = 230.0;
  frame.local_180[0x14] = 10.0;
  frame.local_180[0x15] = 30.0;
  frame.local_180[0x16] = 60.0;
  frame.local_180[0x17] = 90.0;
  frame.local_180[0x18] = 115.0;
  frame.local_180[0x19] = 130.0;
  frame.local_180[0x1a] = 150.0;
  frame.local_180[0x1b] = 180.0;
  frame.local_180[0x1c] = 215.0;
  frame.local_180[0x1d] = 240.0;
  frame.local_180[0x1e] = 15.0;
  frame.local_180[0x1f] = 30.0;
  frame.local_180[0x20] = 50.0;
  frame.local_180[0x21] = 80.0;
  frame.local_180[0x22] = 100.0;
  frame.local_180[0x23] = 135.0;
  frame.local_180[0x24] = 165.0;
  frame.local_180[0x25] = 190.0;
  frame.local_180[0x26] = 210.0;
  frame.local_180[0x27] = 225.0;
  frame.local_180[0x28] = 15.0;
  frame.local_180[0x29] = 40.0;
  frame.local_180[0x2a] = 65.0;
  frame.local_180[0x2b] = 85.0;
  frame.local_180[0x2c] = 100.0;
  frame.local_180[0x2d] = 125.0;
  frame.local_180[0x2e] = 160.0;
  frame.local_180[0x2f] = 195.0;
  frame.local_180[0x30] = 210.0;
  frame.local_180[0x31] = 235.0;
  frame.local_180[0x32] = 10.0;
  frame.local_180[0x33] = 35.0;
  frame.local_180[0x34] = 60.0;
  frame.local_180[0x35] = 80.0;
  frame.local_180[0x36] = 110.0;
  frame.local_180[0x37] = 135.0;
  frame.local_180[0x38] = 155.0;
  frame.local_180[0x39] = 185.0;
  frame.local_180[0x3a] = 205.0;
  frame.local_180[0x3b] = 230.0;
  frame.local_180[0x3c] = 15.0;
  frame.local_180[0x3d] = 30.0;
  frame.local_180[0x3e] = 55.0;
  frame.local_180[0x3f] = 95.0;
  frame.local_180[0x40] = 115.0;
  frame.local_180[0x41] = 130.0;
  frame.local_180[0x42] = 165.0;
  frame.local_180[0x43] = 180.0;
  frame.local_180[0x44] = 215.0;
  frame.local_180[0x45] = 240.0;
  frame.local_180[0x46] = 10.0;
  frame.local_180[0x47] = 40.0;
  frame.local_180[0x48] = 60.0;
  frame.local_180[0x49] = 85.0;
  frame.local_180[0x4a] = 120.0;
  frame.local_180[0x4b] = 140.0;
  frame.local_180[0x4c] = 165.0;
  frame.local_180[0x4d] = 185.0;
  frame.local_180[0x4e] = 210.0;
  frame.local_180[0x4f] = 230.0;
  /* Loop over the records; object_index numbers them in order */
  object_index = 0;
  record = g_course_8bf154;
  do {
    lane = record[-1];
    tmp = *record + lane * GROUPS_PER_LANE;
    frame.local_18c.x = *(RwReal *)((char *)((undefined *)&g_course_8c3fc0) + tmp * 4);
    record[-6] = object_index;
    frame.local_18c.y = *(float *)&(&g_course_8c3f64)[record[-2] * 3];
    frame.local_18c.z = k_f_146;
    /* z base: lane 1 uses k_f_n122, other lanes use k_f_146 (inferred) */
    if (lane == 1) {
      frame.local_18c.z = k_f_n122;
    }
    if (lane == 1) {
      frame.local_18c.z = frame.local_180[record[1] + tmp * OFFSET_GROUP_SIZE] + frame.local_18c.z;
    }
    else {
      frame.local_18c.z = frame.local_18c.z - frame.local_180[record[1] + tmp * OFFSET_GROUP_SIZE];
    }
    *(float *)&record[-9] = frame.local_18c.x;
    *(float *)&record[-8] = frame.local_18c.y;
    /* Identity-type matrix translated to the new position, applied to the atomic's frame */
    frame.local_40.flags = frame.local_40.flags | MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
    *(float *)&record[-7] = frame.local_18c.z;
    frame.local_40.at.z = 1.0;
    frame.local_40.up.y = 1.0;
    frame.local_40.right.x = 1.0;
    frame.local_40.up.x = 0.0;
    frame.local_40.right.z = 0.0;
    frame.local_40.right.y = 0.0;
    frame.local_40.at.y = 0.0;
    frame.local_40.at.x = 0.0;
    frame.local_40.up.z = 0.0;
    frame.local_40.pos.z = 0.0;
    frame.local_40.pos.y = 0.0;
    frame.local_40.pos.x = 0.0;
    RwMatrixTranslate(&frame.local_40,&frame.local_18c,rwCOMBINEREPLACE);
    RwFrameTransform(*(RwFrame **)(record[-5] + 4),&frame.local_40,rwCOMBINEREPLACE);
    /* Resync the atomic's bounding sphere if its interpolator marked it dirty */
    if ((RpAtomic_interpolator_flags(((RpAtomic *)record[-5])) & rpINTERPOLATORDIRTYSPHERE) != 0) {
      _rpAtomicResyncInterpolatedSphere((RpAtomic *)record[-5]);
    }
    /* Copy the atomic's bounding sphere (center x/y/z, radius) into the record; tmp is reused for the radius */
    tmp = record[-5];
    record[-0xd] = *(int *)(tmp + 0x1c);
    record[-0xc] = *(int *)(tmp + 0x20);
    record[-0xb] = *(int *)(tmp + 0x24);
    tmp = *(int *)(tmp + 0x28);
    record[6] = FLOAT_1_0_BITS;
    record[0xf] = 1;
    next_record = record + COURSE_OBJECT_RECORD_WORDS;
    object_index = object_index + 1;
    record[-10] = tmp;
    record = next_record;
  } while ((int)next_record < ((int)&g_course_object_8c4014));
  return;
}


