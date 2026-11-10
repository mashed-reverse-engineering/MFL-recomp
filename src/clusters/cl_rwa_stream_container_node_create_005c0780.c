// cluster-entries: rwa_stream_container_node_create_005c0780
/* Installed cluster rwa_stream_container_node_create_005c0780 (tools/cluster_install.py from output/cluster_pilot_a4/rwa_stream_container_node_create_005c0780.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio stream container (MFL.exe 0x005c0780, rwa.dll unit 24D4C0): _RwaStreamContainerNodeCreateEx (register function: node
   list in ecx, the two 28-byte format blocks in eax / ebx, entered through the generated thunk) allocates a 0x54-byte node, links it at
   the front of the list (link at +0x4c), copies the format blocks when given (+0xc, +0x28) and stores the four stack words.
   Written from the MFL.exe disassembly (tools/disasm_va.py). */
#include "mfl_tier2.h"

typedef struct RwaDLink { struct RwaDLink *next, *prev; } RwaDLink;
typedef struct RwaNodeFormat { RwUInt32 w[7]; } RwaNodeFormat;

typedef struct RwaContainerNode {
  RwUInt32 state;               /* +0 */
  RwUInt32 a, b;                /* +4, +8 */
  RwaNodeFormat in, out;        /* +0xc, +0x28 */
  RwUInt32 d, c;                /* +0x44, +0x48 */
  RwaDLink link;                /* +0x4c */
} RwaContainerNode;

/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwaContainerNode *rwa_stream_container_node_create_005c0780_n(RwaDLink *list, const RwaNodeFormat *in, const RwaNodeFormat *out,
                                                            RwUInt32 a, RwUInt32 b, RwUInt32 c, RwUInt32 d)
{
  RwaContainerNode *n = FUN_005bf0d0(sizeof(RwaContainerNode), 0x3080e);

  if (!n)
    return 0;
  n->link.next = list->next;
  n->link.prev = list;
  list->next->prev = &n->link;
  list->next = &n->link;
  if (in)
    n->in = *in;
  if (out)
    n->out = *out;
  n->a = a;
  n->b = b;
  n->c = c;
  n->d = d;
  n->state = 0;
  return n;
}

/* entry thunk at the original address of rwa_stream_container_node_create_005c0780: register inputs (ecx,eax,ebx,s0,s1,s2,s3) -> natural C rwa_stream_container_node_create_005c0780_n; ret 0 */
__declspec(naked) void rwa_stream_container_node_create_005c0780(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push ebx
        push eax
        push ecx
        call rwa_stream_container_node_create_005c0780_n
        add esp, 28
        ret
    }
}
