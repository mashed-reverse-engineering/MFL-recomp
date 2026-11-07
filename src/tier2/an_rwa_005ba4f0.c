/* rwa_objdef_build_supported_list_005ba4f0 (0x005ba4f0): object definitions (_rwaObjDefBuildSupportedList): collect into OUT every definition below DEF (default: the NULL object definition) that supports the interface IFACE; nonzero when OUT is still valid */
#include "mfl_tier2.h"

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

typedef struct RwaIfaceSearch {      /* passed to the enum callback */
  void *iface;
  void *out;      /* list of the supported definitions; 0 after an out-of-memory */
} RwaIfaceSearch;

/*00949d5c*/ extern char *g_data_949d5c;      /* the NULL object definition */
/*005ba2b0*/ extern int rwa_objdef_005ba2b0(void *parent, void *child, RwaIfaceSearch *search);      /* _RwaObjDefBuildSupportedListCallback; 1: stop */
/*005ba200*/ extern int RwaObjDefEnum(void *def, void *a2, void *callback, void *data, int a5);
/*005bd700*/ extern int _rwaObjInterfaceDefSupported(void *iface, void *objDef, int a3);
/*005be4a0*/ extern void *RwaLListAddData(void *list, void *data);
/*005be560*/ extern void RwaLListEmpty(void *list);

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_objdef_build_supported_list_005ba4f0(char *def, void *iface, void *out)
{
  RwaIfaceSearch search;
  RwaLink *head, *l;

  search.iface = iface;
  search.out = out;
  if (!def)
    def = g_data_949d5c;
  if (rwa_objdef_005ba2b0(def, def, &search) != 1) {
    head = (RwaLink *)(def + 0x10);      /* child definitions, linked at +0x18 */
    for (l = head->next; l != head; l = l->next) {
      char *child = (char *)l - 0x18;
      if ((*((unsigned char *)l - 0xc) & 4) && search.out && _rwaObjInterfaceDefSupported(search.iface, child, 0) &&
          !RwaLListAddData(search.out, child)) {
        RwaLListEmpty(search.out);
        search.out = 0;
      }
      if (RwaObjDefEnum(child, 0, rwa_objdef_005ba2b0, &search, 0))
        break;
    }
  }
  return search.out != 0;
}
