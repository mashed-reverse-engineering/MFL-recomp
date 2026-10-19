#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
#define CAMERA_FLAG_BITS (0x20003) /* OR'd into camera word 6 (inferred: flag bits) */
/* camera_set_from_record */

/* "copies position and quaternion from a camera record"
 * camera: output camera object, filled in place (words are 32-bit, offsets in words).
 * record: input record of 12 32-bit values: [0..2] position x,y,z; [3..6] quaternion x,y,z,w (inferred);
 *         [7..9] values converted to int; [10..11] floats (meaning unknown).
 * Keeps the old flag bits of camera word 6 and sets CAMERA_FLAG_BITS on top.
 */

void camera_set_from_record(undefined4 *camera,RwReal *record)

{
  RwReal quat_x;
  RwReal quat_y;
  RwReal quat_z;
  RwReal quat_w;
  RwReal record_float_10;
  RwReal record_float_11;
  uint old_flags;
  undefined4 record_int_7;
  undefined4 record_int_8;
  undefined4 record_int_9;
  RwV3d position;

  /* read the record: position, quaternion, then the int and float fields */
  position.x = *record;
  position.y = record[1];
  position.z = record[2];
  quat_x = record[3];
  quat_y = record[4];
  quat_z = record[5];
  quat_w = record[6];
  record_int_7 = ((int)(*(float *)((char *)record + 28)));
  record_int_8 = ((int)(*(float *)((char *)record + 32)));
  record_int_9 = ((int)(*(float *)((char *)record + 36)));
  record_float_10 = record[10];
  record_float_11 = record[0xb];

  /* camera header: type word 0, the two int fields in words 1 and 2, flags in word 6 */
  camera[1] = record_int_7;
  old_flags = camera[6];
  camera[2] = record_int_8;
  *camera = 1;
  camera[0xd] = 0x3f800000;
  camera[8] = 0x3f800000;
  (((RwMatrix *)(camera + 3))->right).x = 1.0;

  /* clear the other words of the camera */
  camera[7] = 0;
  camera[5] = 0;
  camera[4] = 0;
  camera[0xc] = 0;
  camera[0xb] = 0;
  camera[9] = 0;
  camera[0x11] = 0;
  camera[0x10] = 0;
  camera[0xf] = 0;
  camera[6] = old_flags | CAMERA_FLAG_BITS;

  /* camera matrix (words 3..18) becomes a pure translation to the record position */
  RwMatrixTranslate((RwMatrix *)(camera + 3),&position,rwCOMBINEREPLACE);

  /* store the quaternion and the remaining record values into the camera's tail words */
  *(float *)&camera[0x2b] = quat_x;
  *(float *)&camera[0x2c] = quat_y;
  camera[0x25] = 0;
  camera[0x24] = 0;
  camera[0x23] = 0;
  camera[0x28] = 0;
  camera[0x27] = 0;
  camera[0x26] = 0;
  camera[0x29] = 0x41400000;
  *(float *)&camera[0x2d] = quat_z;
  camera[0x2f] = 0;
  camera[0x2e] = 0;
  camera[0x30] = 0x3f800000;
  *(float *)&camera[0x31] = quat_w;
  camera[0x33] = record_int_9;
  *(float *)&camera[0x34] = record_float_10;
  *(float *)&camera[0x35] = record_float_11;
  return;
}


