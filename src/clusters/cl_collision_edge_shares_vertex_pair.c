// cluster-entries: collision_edge_shares_vertex_pair
/* Installed cluster collision_edge_shares_vertex_pair (tools/cluster_install.py from output/cluster_pilot10/collision_edge_shares_vertex_pair.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* collision_edge_shares_vertex_pair */

/* "true when both vertex indices of this edge occur in the other edge" */

undefined4 collision_edge_shares_vertex_pair_n(int in_esi, int in_edx)

{
  if (((*(int *)(in_esi + 0x10) == *(int *)(in_edx + 0x10)) ||
      (*(int *)(in_esi + 0x10) == *(int *)(in_edx + 0x14))) &&
     ((*(int *)(in_esi + 0x14) == *(int *)(in_edx + 0x10) ||
      (*(int *)(in_esi + 0x14) == *(int *)(in_edx + 0x14))))) {
    return 1;
  }
  return 0;
}

/* entry thunk at the original address of collision_edge_shares_vertex_pair: register inputs (esi,edx) -> natural C collision_edge_shares_vertex_pair_n; ret 0 */
__declspec(naked) void collision_edge_shares_vertex_pair(void)
{
    __asm {
        push edx
        push edx
        push esi
        call collision_edge_shares_vertex_pair_n
        add esp, 8
        pop edx
        ret
    }
}
