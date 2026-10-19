#include "mfl_tier2.h"
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*004220f0*/ extern int vehicle_get_position_158();
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*005dd330*/ extern float float_1_0;
/*008c3fc0*/ extern undefined4 g_course_8c3fc0;
/*008c4478*/ extern undefined4 g_course_highway_8c4478;
extern double fabs(double);
#define OBJECT_TABLE_HEADER_SIZE (8) /* bytes before the record array in g_course_highway_8c4478 (inferred) */
#define OBJECT_RECORD_SIZE (0x54)
#define OBJECT_TABLE_BYTES (0x2a0) /* 8 records * OBJECT_RECORD_SIZE */
#define OBJECT_COUNT_OFFSET (0x50) /* int entry count inside a record */
#define OBJECT_ENTRY_SIZE (0x14) /* 4 ints per entry */
/* course_object_distance_table_build */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Rebuilds the course object table in g_course_highway_8c4478: clears all 8 records, then for each of the
   4 vehicles (vehicle = 0..3) and each of the 8 course objects (object = 0..7, x in g_course_8c3fc0[object]),
   appends an entry {x bits, y bits, z bits, vehicle + 1} to that object's list when the vehicle's slot is
   in use, the vehicle's y is <= 1.0 and |vehicle x - object x| < 1.0. Positions are copied as raw int bits.
   Takes no parameters, returns nothing.
   (was: "clears 0x2A0 bytes at 0x8C4480 and fills distances of each vehicle to the objects") */

void course_object_distance_table_build(void)

{
  int *entry;
  char *object_rec;
  int slot_used;
  int vehicle;
  int pos_x_bits;
  int pos_y_bits;
  int pos_z_bits;
  struct {
    int inner;
    int outer;
    float x;
    float y;
    int gap;
    float pos[3];
    int pad_top;
  } frame;

  /* clear all object records */
  mem_zero((char *)&g_course_highway_8c4478 + OBJECT_TABLE_HEADER_SIZE,OBJECT_TABLE_BYTES);
  vehicle = 0;
  /* loop over the 4 vehicles: read this vehicle's position */
  do {
    vehicle_get_position_158(vehicle,&frame.pos[0]);
    pos_x_bits = ((int *)frame.pos)[0];
    pos_y_bits = ((int *)frame.pos)[1];
    pos_z_bits = ((int *)frame.pos)[2];
    *(int *)&frame.x = pos_x_bits;
    *(int *)&frame.y = pos_y_bits;
    /* outer steps 0, 4 and inner runs 0..3: object = outer + inner covers objects 0..7 */
    frame.outer = 0;
    do {
      frame.inner = 0;
      do {
        slot_used = race_setup_slot_used(vehicle);
        /* slot in use, vehicle y <= 1.0, and vehicle x within 1.0 of this object's x: append to its list */
        if (((slot_used != 0) && !(frame.y > float_1_0)) &&
           ((float)fabs(frame.x - *(float *)((char *)((undefined *)&g_course_8c3fc0) + (frame.outer + frame.inner) * 4)) < float_1_0)) {
          object_rec = (char *)&g_course_highway_8c4478 + OBJECT_TABLE_HEADER_SIZE + (frame.outer + frame.inner) * OBJECT_RECORD_SIZE;
          /* append after the current entries: entry index = record count */
          entry = (int *)(object_rec + *(int *)(object_rec + OBJECT_COUNT_OFFSET) * OBJECT_ENTRY_SIZE);
          entry[0] = pos_x_bits;
          entry[1] = pos_y_bits;
          entry[3] = vehicle + 1;
          entry[2] = pos_z_bits;
          *(int *)(object_rec + OBJECT_COUNT_OFFSET) = *(int *)(object_rec + OBJECT_COUNT_OFFSET) + 1;
        }
        frame.inner = frame.inner + 1;
      } while (frame.inner < 4);
      frame.outer = frame.outer + 4;
    } while (frame.outer < 8);
    vehicle = vehicle + 1;
  } while (vehicle < 4);
  return;
}
