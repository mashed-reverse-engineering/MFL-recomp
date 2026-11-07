// cluster-entries: rwa_stream_container_update_volumes_005c09e0
/* Installed cluster rwa_stream_container_update_volumes_005c09e0 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_container_update_volumes_005c09e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (register function: volume requests in edx, container in s0, stream in s1, entered through the generated thunk):
   for every request (16 bytes, stream +4 of them) whose word 0 has the dirty bit 31: clear it, find render chain node <index> (from
   the nearer end of the chain list +0x20, stream +8 nodes) and set its volume parameter (1) from the request's data at +4. */
#include "mfl_tier2.h"

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

typedef struct RwaVolumeRequest {
  RwUInt32 node;      /* bit 31: dirty, low bits: chain index */
  RwUInt32 data[3];   /* the parameter data */
} RwaVolumeRequest;

/*005c1e80*/ extern void RwaObjIntSetParamData(char *obj, void *iface, int param, void *data);

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void rwa_stream_container_update_volumes_005c09e0_n(RwaVolumeRequest *req, char *container, char *stream)
{
  RwaLink *head = (RwaLink *)(container + 0x20), *l;
  RwUInt32 i, k, idx, n;

  for (i = 0; i < *(RwUInt32 *)(stream + 4); i++, req++) {
    if (!(req->node & 0x80000000))
      continue;
    idx = req->node &= 0x7fffffff;
    n = *(RwUInt32 *)(stream + 8);
    if (idx < n >> 1)
      for (l = head->next, k = idx; k; k--)
        l = l->next;
    else
      for (l = head->prev, k = n - 1 - idx; k; k--)
        l = l->prev;
    RwaObjIntSetParamData((char *)l - 0x2c, *(void **)((char *)l + 0x2c), 1, req->data);
  }
}

/* entry thunk at the original address of rwa_stream_container_update_volumes_005c09e0: register inputs (edx,s0,s1) -> natural C rwa_stream_container_update_volumes_005c09e0_n; ret 0 */
__declspec(naked) void rwa_stream_container_update_volumes_005c09e0(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push edx
        call rwa_stream_container_update_volumes_005c09e0_n
        add esp, 12
        ret
    }
}
