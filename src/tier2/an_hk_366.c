#include "mfl_tier2.h"
/*00496ea0*/ extern int __cdecl smplfzx_get_object(undefined4 p0);
static undefined4 smplfzx_current_object_get_field_r(int reg_esi);
extern void smplfzx_current_object_get_field(void);
#define OBJECT_POSITION_PTR_OFFSET (0x60)
__declspec(naked) undefined4 smplfzx_current_object_get_field_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call smplfzx_current_object_get_field
        pop esi
        ret
    }
}

/* smplfzx_object_get_position */

/* "copies the position vector of the smplfzx object" */

/*
 * Copies the 3-component position vector of the smplfzx object identified by
 * object_id into out_position[0..2]. Returns 1 on success, 0 if no object
 * with that id exists (out_position is left untouched in that case).
 * The position vector is reached through a pointer stored at
 * OBJECT_POSITION_PTR_OFFSET inside the object (inferred).
 */
undefined4 smplfzx_object_get_position(undefined4 object_id,undefined4 *out_position)

{
  undefined4 *position_vec;
  int smplfzx_object;
  
  smplfzx_object = smplfzx_get_object(object_id);
  if (smplfzx_object != 0) {
    /* object found: fetch its position pointer and copy the three components */
    smplfzx_object = smplfzx_current_object_get_field_r(object_id);
    position_vec = *(undefined4 **)(smplfzx_object + OBJECT_POSITION_PTR_OFFSET);
    *out_position = *position_vec;
    out_position[1] = position_vec[1];
    out_position[2] = position_vec[2];
    return 1;
  }
  return 0;
}


