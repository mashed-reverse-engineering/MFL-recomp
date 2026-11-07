// cluster-entries: rwa_stream_container_instantiate_source_005c06b0
/* Installed cluster rwa_stream_container_instantiate_source_005c06b0 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_container_instantiate_source_005c06b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (_rwaStreamContainerInstaniateSource, register function: container in edi, stream in s0, entered through the
   generated thunk): give every render chain node (list +0x20, link at node + 0x2c) that has a start parameter its start value
   (param 0) from the container's codec table (+0x0c -> +0x18, 0x28 bytes per node, value pointer at +0x14).  With stream flag 2 and
   a precision below 32 bits (+0x44) the value is rounded down to a multiple of the block size (+0x4c) in fixed point. */
#include "mfl_tier2.h"

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*005c1f10*/ extern void RwaObjIntSetParamBool(void *obj, void *iface, int param, RwUInt32 value);

#define NODE_OF(l) ((char *)(l) - 0x2c)

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void rwa_stream_container_instantiate_source_005c06b0_n(char *container, char *stream)
{
  RwaLink *head = (RwaLink *)(container + 0x20), *l;
  RwUInt32 offset = 0, value, ref, shift;
  int bits;
  char *node;

  for (l = head->next; l != head; l = l->next, offset += 0x28) {
    node = NODE_OF(l);
    if (!*(RwUInt32 *)(*(char **)(*(char **)node + 0x54) + 0xc))      /* the node's class has no start parameter */
      continue;
    value = **(RwUInt32 **)(*(char **)(*(char **)(container + 0xc) + 0x18) + offset + 0x14);
    if ((*stream & 2) && *(RwUInt32 *)(container + 0x44) < 0x20) {
      ref = *(RwUInt32 *)(container + 0x40);
      bits = 2;
      while (value == ref && ref) {      /* value equal to the reference or one of its halvings */
        ref >>= 1;
        bits--;
      }
      shift = (*(RwUInt32 *)(container + 0x44) - bits) & 31;
      value = ((value << shift) / *(RwUInt32 *)(container + 0x4c) * *(RwUInt32 *)(container + 0x4c)) >> shift;
    }
    RwaObjIntSetParamBool(node, *(void **)(node + 0x58), 0, value);
  }
}

/* entry thunk at the original address of rwa_stream_container_instantiate_source_005c06b0: register inputs (edi,s0) -> natural C rwa_stream_container_instantiate_source_005c06b0_n; ret 0 */
__declspec(naked) void rwa_stream_container_instantiate_source_005c06b0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push edi
        call rwa_stream_container_instantiate_source_005c06b0_n
        add esp, 8
        ret
    }
}
