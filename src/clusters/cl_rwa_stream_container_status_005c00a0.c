// cluster-entries: rwa_stream_container_status_005c00a0 rwa_stream_container_update_current_status_005c0130
/* Installed cluster rwa_stream_container_status_005c00a0 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_container_status_005c00a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container status (register function: container in eax, entered through the generated thunk) and its caller
   _rwaStreamContainerStateUpdateCurrentStatus (5c0130).  States (+0x08): 2..4 opening (2), 6 stopped (9), 5 playing: the render
   chain status 5bff30 checked against the source status 5c0060 by the chain mode (+0x38). */
#include "mfl_tier2.h"

/*005c1f30*/ extern int FUN_005c1f30(void *node, void *connection, int index);      /* node status query */

#define STATE(c) (*(RwUInt32 *)((c) + 8))

static int _RwaStreamContainerGetRenderStatus_r(char * p0);
extern void _RwaStreamContainerGetRenderStatus(void);
static int rwa_unit2452a0_005c0060_r(char * p0);
extern void rwa_unit2452a0_005c0060(void);
/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_stream_container_status_005c00a0_n(char *container)
{
  int chain, source;

  switch (STATE(container)) {
  case 2:
  case 3:
  case 4:
    return 2;
  case 6:
    return 9;
  case 5:
    chain = _RwaStreamContainerGetRenderStatus_r(container);
    source = rwa_unit2452a0_005c0060_r(container);
    switch (*(RwUInt32 *)(container + 0x38)) {
    case 1:
      return source == 3 && chain == 3 ? 3 : 8;
    case 2:
      return chain;
    case 3:
      return (source == 5 || source == 3) && chain == 5 ? chain : 4;
    }
    return 0;
  }
  return 0;
}

/* _rwaStreamContainerStateUpdateCurrentStatus: out[0] container status, out[1] source status parameter (states 3..5, else 0),
   out[2] render chain status (returned).  The second argument is unused. */
/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_stream_container_update_current_status_005c0130(char *container, void *unused, int *out)
{
  char *source;

  out[0] = rwa_stream_container_status_005c00a0_n(container);
  switch (STATE(container)) {
  case 3:
  case 4:
  case 5:
    source = *(char **)(container + 0x14);
    out[1] = FUN_005c1f30(source, *(void **)(source + 0x58), 3);
    break;
  default:
    out[1] = 0;
  }
  return out[2] = _RwaStreamContainerGetRenderStatus_r(container);
}

/* _RwaStreamContainerGetRenderStatus_r: C -> original _RwaStreamContainerGetRenderStatus (registers eax) */
__declspec(naked) int _RwaStreamContainerGetRenderStatus_r(char * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call _RwaStreamContainerGetRenderStatus
        ret
    }
}

/* rwa_unit2452a0_005c0060_r: C -> original rwa_unit2452a0_005c0060 (registers ecx) */
__declspec(naked) int rwa_unit2452a0_005c0060_r(char * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call rwa_unit2452a0_005c0060
        ret
    }
}

/* entry thunk at the original address of rwa_stream_container_status_005c00a0: register inputs (eax) -> natural C rwa_stream_container_status_005c00a0_n; ret 0 */
__declspec(naked) void rwa_stream_container_status_005c00a0(void)
{
    __asm {
        push eax
        call rwa_stream_container_status_005c00a0_n
        add esp, 4
        ret
    }
}
