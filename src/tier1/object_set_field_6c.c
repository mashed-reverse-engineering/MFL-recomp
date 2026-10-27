#include "mfl_tier2.h"
/* object_set_field_6c */
/* "(object, value)" */
#define OBJECT_FIELD_6C_OFFSET (0x6c)
/* Stores value as a 32-bit word at byte offset 0x6c inside object (the field's meaning is not known yet).
   object: base pointer to the object; value: the 4-byte value written. Returns nothing. */
void object_set_field_6c(char *object,undefined4 value)
{
  *(undefined4 *)(object + OBJECT_FIELD_6C_OFFSET) = value;
  return;
}
