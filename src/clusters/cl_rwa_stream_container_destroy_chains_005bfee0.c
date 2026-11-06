// cluster-entries: rwa_stream_container_destroy_chains_005bfee0 rwa_stream_container_release_chains_005c0950
/* Installed cluster rwa_stream_container_destroy_chains_005bfee0 (tools/cluster_install.py from output/cluster_pilot_l2c/rwa_stream_container_destroy_chains_005bfee0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (objects/rwastreamcontainer.c): destroy the render chain objects and release the node chain (register
   function, container in esi) and its stack wrapper.  Cluster: rwa_stream_container_destroy_chains_005bfee0 is entered through
   the generated esi thunk. */
#include "mfl_tier2.h"

/*005bfdc0*/ extern char *_rwaStreamContainerEnumRenderChains(char *container, void *callback, void *data);
/*005bfe70*/ extern void _rwaStreamContainerEnumNodeChain(char *container, void *callback, void *data);
/*005bfeb0*/ extern void rwa_unit2452a0_005bfeb0(void);      /* the chain callback */
/*005baf80*/ extern void RwaObjDestroy(char *obj, RwUInt32 a, RwUInt32 b);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_container_destroy_chains_005bfee0_n(char *container)
{
  char *obj = _rwaStreamContainerEnumRenderChains(container, (void *)rwa_unit2452a0_005bfeb0, container), *next;
  RwaLink *l;

  while (obj) {
    next = *(char **)(obj + 0x38);
    l = (RwaLink *)(obj + 0x2c);
    l->prev->next = l->next;
    l->next->prev = l->prev;
    RwaObjDestroy(obj, *(RwUInt32 *)(container + 0x54), *(RwUInt32 *)(container + 0x58));
    obj = next;
  }
  _rwaStreamContainerEnumNodeChain(container, (void *)rwa_unit2452a0_005bfeb0, container);
}

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_container_release_chains_005c0950(char *container)
{
  rwa_stream_container_destroy_chains_005bfee0_n(container);
}

/* entry thunk at the original address of rwa_stream_container_destroy_chains_005bfee0: register inputs (esi) -> natural C rwa_stream_container_destroy_chains_005bfee0_n; ret 0 */
__declspec(naked) void rwa_stream_container_destroy_chains_005bfee0(void)
{
    __asm {
        push esi
        call rwa_stream_container_destroy_chains_005bfee0_n
        add esp, 4
        ret
    }
}
