// cluster-entries: movie_renderer_delete
/* Installed cluster movie_renderer_delete (tools/cluster_install.py from output/cluster_pilot7/movie_renderer_delete.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004b3bca*/ extern void __cdecl _free_004b45fd(void *p0);
/* movie_renderer_delete */

/* "destructor: app_fn_4a3110 and free when flag 1" */

static int movie_renderer_destruct_r(int p0);
extern void movie_renderer_destruct(void);
void * movie_renderer_delete_n(void *in_ecx, uint param_2)

{
  movie_renderer_destruct_r(in_ecx);
  if ((param_2 & 1) != 0) {
    _free_004b45fd(in_ecx);
  }
  return in_ecx;
}

/* movie_renderer_destruct_r: C -> original movie_renderer_destruct (registers ecx) */
__declspec(naked) int movie_renderer_destruct_r(int p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call movie_renderer_destruct
        ret
    }
}

/* entry thunk at the original address of movie_renderer_delete: register inputs (ecx,s0) -> natural C movie_renderer_delete_n; ret 4 */
__declspec(naked) void movie_renderer_delete(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call movie_renderer_delete_n
        add esp, 8
        ret 4
    }
}
