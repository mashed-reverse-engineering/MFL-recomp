#include "mfl_tier2.h"
/*00496f10*/ extern int __cdecl smplfzx_check_active(undefined4 p0, void *p1, undefined4 p2);
/*004a8420*/ extern int debug_printf();
static int smplfzx_retire_r(int reg_esi);
extern void smplfzx_retire(void);
extern char s_SmplFzx_Retiring____d_005e0788[];

/* object id is kept to its low 16 bits before it is printed or passed on (inferred) */
#define OBJECT_ID_MASK (0xffff)

__declspec(naked) int smplfzx_retire_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call smplfzx_retire
        pop esi
        ret
    }
}

/* smplfzx_retire_object */

/* "prints 'SmplFzx Retiring : %d' and retires the object" */

/*
 * Prints the retire message for object_id, asks smplfzx_check_active() about it,
 * and passes the result on to smplfzx_retire() via smplfzx_retire_r().
 * object_id: only its low 16 bits are used (inferred from the mask).
 * unused_param_2: never read.
 */
void __cdecl smplfzx_retire_object(uint object_id,char *unused_param_2)

{
  undefined4 active_result;
  char *message_text;
  uint masked_object_id;

  message_text = "SmplFzx Retiring : %d\n";
  masked_object_id = object_id & OBJECT_ID_MASK;
  debug_printf(((char *)&s_SmplFzx_Retiring____d_005e0788), object_id & OBJECT_ID_MASK);
  active_result = smplfzx_check_active(object_id & OBJECT_ID_MASK,message_text,masked_object_id);
  smplfzx_retire_r(active_result);
  return;
}


