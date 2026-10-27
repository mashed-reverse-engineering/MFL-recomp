#include "mfl_tier2.h"

#define FX_OBJECT_STRIDE_DWORDS 0xcc
#define FX_OBJECTS_TABLE_END ((int)&g_particles_769788)

/*0075f828*/ extern undefined4 g_particles_75f828;
/*00769788*/ extern char g_particles_769788[];


/* fx_objects_clear */

/* Clears the FX object table at 0x75F828 by resetting header fields (exact fields unknown). */
/* "clears the object table at 0x75F828" */

void fx_objects_clear(void)
{
  int *object_ptr;
  
  object_ptr = &g_particles_75f828;
  do {
    /* Clear active/status flags (exact fields unknown) */
    *object_ptr = 0;
    object_ptr[1] = 0;
    /* Advance to the next FX object entry */
    object_ptr = object_ptr + FX_OBJECT_STRIDE_DWORDS;
  } while ((int)object_ptr < FX_OBJECTS_TABLE_END);
  return;
}

