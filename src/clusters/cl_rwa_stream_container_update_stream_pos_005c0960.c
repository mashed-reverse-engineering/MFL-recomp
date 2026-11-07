// cluster-entries: rwa_stream_container_update_stream_pos_005c0960
/* Installed cluster rwa_stream_container_update_stream_pos_005c0960 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_container_update_stream_pos_005c0960.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (_RwaStreamContainerUpdateStreamPos, register function: container in eax, stream in s0, params in s1, entered
   through the generated thunk): hand the new stream position (params[3] -> +4) to every render chain object (param 2; skipped when
   there is none and the chain mode is 1), re-apply the chain parameter, clear the position and pass the request to the source (param 0). */
#include "mfl_tier2.h"

/*005bfdc0*/ extern char *_rwaStreamContainerEnumRenderChains(char *container, void *callback, void *data);
/*005c01a0*/ extern void rwa_unit2452a0_005c01a0(void);      /* the chain callback */
/*005c1f10*/ extern void RwaObjIntSetParamBool(char *obj, void *iface, int param, RwUInt32 value);
/*005c1e80*/ extern void RwaObjIntSetParamData(char *obj, void *iface, int param, void *data);

static void FUN_005c01e0_r(char * p0, char * p1, RwUInt32 p2);
extern void FUN_005c01e0(void);
/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void rwa_stream_container_update_stream_pos_005c0960_n(char *container, char *stream, RwUInt32 *params)
{
  char *request = (char *)params[3], *obj, *next, *source;
  RwUInt32 pos = *(RwUInt32 *)(request + 4);

  if (pos || *(RwUInt32 *)(container + 0x38) != 1) {
    obj = _rwaStreamContainerEnumRenderChains(container, (void *)rwa_unit2452a0_005c01a0, (void *)pos);
    pos = *(RwUInt32 *)((char *)params[3] + 4);
    do {      /* (as the original: the first object is not tested) */
      next = *(char **)(obj + 0x38);
      RwaObjIntSetParamBool(obj, *(void **)(obj + 0x28), 2, pos);
      obj = next;
    } while (obj);
  }
  FUN_005c01e0_r(stream, container, *(RwUInt32 *)(container + 0x38));
  *(RwUInt32 *)((char *)params[3] + 4) = 0;
  source = *(char **)(container + 0x14);
  RwaObjIntSetParamData(source, *(void **)(source + 0x58), 0, (void *)params[3]);
}

/* FUN_005c01e0_r: C -> original FUN_005c01e0 (registers eax,ecx,edi) */
__declspec(naked) void FUN_005c01e0_r(char * p0, char * p1, RwUInt32 p2)
{
    __asm {
        push edi
        mov eax, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call FUN_005c01e0
        pop edi
        ret
    }
}

/* entry thunk at the original address of rwa_stream_container_update_stream_pos_005c0960: register inputs (eax,s0,s1) -> natural C rwa_stream_container_update_stream_pos_005c0960_n; ret 0 */
__declspec(naked) void rwa_stream_container_update_stream_pos_005c0960(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        call rwa_stream_container_update_stream_pos_005c0960_n
        add esp, 12
        ret
    }
}
