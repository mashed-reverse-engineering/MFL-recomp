// cluster-entries: rwa_stream_container_source_status_005c0060
/* Installed cluster rwa_stream_container_source_status_005c0060 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_container_source_status_005c0060.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (register function: container in ecx, entered through the generated thunk): status parameter (3) of the
   container's source object (+0x14, interface at source + 0x58) while the container is opening / built / playing (states 3..5), else 0. */
#include "mfl_tier2.h"

/*005c1f30*/ extern int FUN_005c1f30(void *node, void *connection, int index);      /* node status query */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_stream_container_source_status_005c0060_n(char *container)
{
  char *source;

  switch (*(RwUInt32 *)(container + 8)) {
  case 3:
  case 4:
  case 5:
    source = *(char **)(container + 0x14);
    return FUN_005c1f30(source, *(void **)(source + 0x58), 3);
  }
  return 0;
}

/* entry thunk at the original address of rwa_stream_container_source_status_005c0060: register inputs (ecx) -> natural C rwa_stream_container_source_status_005c0060_n; ret 0 */
__declspec(naked) void rwa_stream_container_source_status_005c0060(void)
{
    __asm {
        push ecx
        call rwa_stream_container_source_status_005c0060_n
        add esp, 4
        ret
    }
}
