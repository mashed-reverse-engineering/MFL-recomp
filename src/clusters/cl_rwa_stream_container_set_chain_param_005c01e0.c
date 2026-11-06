// cluster-entries: rwa_stream_container_set_chain_param_005c01e0
/* Installed cluster rwa_stream_container_set_chain_param_005c01e0 (tools/cluster_install.py from output/cluster_pilot_l2c/rwa_stream_container_set_chain_param_005c01e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container: set parameter `value` (bool) on every render chain object; value 2 first stores the stream's +8 word in
   the container (+0x3c, InterlockedExchange).  Register function (stream eax, container ecx, value edi), entered through the
   generated thunk. */
#include "mfl_tier2.h"

/*005bfdc0*/ extern char *_rwaStreamContainerEnumRenderChains(char *container, void *callback, int data);
/*005c01c0*/ extern void rwa_unit2452a0_005c01c0(void);      /* the chain callback */
/*005c1f10*/ extern void RwaObjIntSetParamBool(char *obj, void *iface, int param, int value);
/*005dd170*/ __declspec(dllimport) long __stdcall InterlockedExchange(long *target, long value);

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_container_set_chain_param_005c01e0_n(char *stream, char *container, int value)
{
  char *obj, *next;

  if (value == 2)
    InterlockedExchange((long *)(container + 0x3c), *(long *)(stream + 8));
  obj = _rwaStreamContainerEnumRenderChains(container, (void *)rwa_unit2452a0_005c01c0, value);
  do {      /* (as the original: the first object is not tested) */
    next = *(char **)(obj + 0x38);
    RwaObjIntSetParamBool(obj, *(void **)(obj + 0x28), 0, value);
    obj = next;
  } while (obj);
}

/* entry thunk at the original address of rwa_stream_container_set_chain_param_005c01e0: register inputs (eax,ecx,edi) -> natural C rwa_stream_container_set_chain_param_005c01e0_n; ret 0 */
__declspec(naked) void rwa_stream_container_set_chain_param_005c01e0(void)
{
    __asm {
        push edi
        push ecx
        push eax
        call rwa_stream_container_set_chain_param_005c01e0_n
        add esp, 12
        ret
    }
}
