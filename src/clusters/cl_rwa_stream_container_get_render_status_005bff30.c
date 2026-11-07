// cluster-entries: rwa_stream_container_get_render_status_005bff30
/* Installed cluster rwa_stream_container_get_render_status_005bff30 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_container_get_render_status_005bff30.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (_RwaStreamContainerGetRenderStatus, register function: container in eax, entered through the generated thunk):
   combined status of the render chain nodes (list head +0x20, link at node + 0x2c, node interface at node + 0x28) for the
   container states (+0x08) 2..4 (opening: 2) and 5 (playing), by the chain mode (+0x38): 1 all nodes 3 (else 8), 2 any node 7 wins,
   all 3 gives 3 (else 6), 3 all nodes 5 (else 4).  Other states / modes: 0. */
#include "mfl_tier2.h"

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*005c1f30*/ extern int FUN_005c1f30(void *node, void *connection, int index);      /* node status query */

#define STATE(c) (*(RwUInt32 *)((c) + 8))
#define CHAIN_MODE(c) (*(RwUInt32 *)((c) + 0x38))
#define NODE_STATUS(l) FUN_005c1f30((char *)(l) - 0x2c, *(void **)((char *)(l) - 4), 0)

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_stream_container_get_render_status_005bff30_n(char *container)
{
  RwaLink *head = (RwaLink *)(container + 0x20), *l;
  int status, allThree;

  switch (STATE(container)) {
  case 2:
  case 3:
  case 4:
    return 2;
  case 5:
    l = head->next;
    switch (CHAIN_MODE(container)) {
    case 1:
      for (; l != head; l = l->next)
        if (NODE_STATUS(l) != 3)
          return 8;
      return 3;
    case 2:
      if (l == head)
        return 3;
      allThree = 1;
      for (; l != head; l = l->next) {
        status = NODE_STATUS(l);
        if (status == 7)
          return 7;
        if (status != 3)
          allThree = 0;
      }
      return allThree == 1 ? 3 : 6;
    case 3:
      for (; l != head; l = l->next)
        if (NODE_STATUS(l) != 5)
          return 4;
      return 5;
    }
    return 0;
  }
  return 0;
}

/* entry thunk at the original address of rwa_stream_container_get_render_status_005bff30: register inputs (eax) -> natural C rwa_stream_container_get_render_status_005bff30_n; ret 0 */
__declspec(naked) void rwa_stream_container_get_render_status_005bff30(void)
{
    __asm {
        push eax
        call rwa_stream_container_get_render_status_005bff30_n
        add esp, 4
        ret
    }
}
