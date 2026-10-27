#include "mfl_tier2.h"
/* byte offset of the callback pointer inside obj (inferred) */
#define CALLBACK_OFFSET (0x3c)
/* FUN_005ba030 */
/*
 * Calls the callback stored in obj at CALLBACK_OFFSET as callback(0, arg) (inferred).
 * Returns obj if a callback was set, 0 if the slot is empty.
 */
int FUN_005ba030(char *obj,undefined4 arg)
{
  /* only call the callback if the slot is filled */
  if (*(code **)(obj + CALLBACK_OFFSET) != (code *)0x0) {
    (**(code **)(obj + CALLBACK_OFFSET))(0,arg);
    return obj;
  }
  return 0;
}
