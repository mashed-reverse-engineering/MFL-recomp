// cluster-entries: rwa_free_list_destroy_005bee60 rwa_free_list_module_close_005beee0
/* Installed cluster rwa_free_list_destroy_005bee60 (tools/cluster_install.py from output/cluster_pilot_a4/rwa_free_list_destroy_005bee60.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio free lists (MFL.exe 0x005bee60, rwa.dll unit 243670): the free list destroy helper (register function: free list in
   edi, entered through the generated thunk) frees every block, then the list itself (back to the free list of free lists 7dbbf0, or to
   the RWA heap when it is that list or none exists; a static list, flag 1, stays); _rwaFreeListModuleClose (0x005beee0, its caller)
   destroys every registered free list (link at +0x1c in the list of all free lists 7dbbc0), then the free list of free lists.
   Written from the MFL.exe disassembly (tools/disasm_va.py).  The tier 1 file rwa_unit243670_005beec0.c keeps its own static copy. */
#include "mfl_tier2.h"

typedef struct RwaDLink { struct RwaDLink *next, *prev; } RwaDLink;

typedef struct RwaFreeList {
  RwUInt32 entrySize, entriesPerBlock, alignment, bitmapSize;
  RwaDLink blocks;              /* +0x10 */
  RwUInt32 flags;               /* +0x18: bit 0 static (caller's memory) */
  RwaDLink link;                /* +0x1c: in the list of all free lists */
} RwaFreeList;

/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005bed80*/ extern void RwaFreeListFree(void *list, void *item);

/*007dbbc0*/ extern RwaDLink g_data_7dbbc0;          /* all free lists */
/*007dbbec*/ extern RwUInt32 g_data_7dbbec;
/*007dbbf0*/ extern RwaFreeList *g_data_7dbbf0;      /* the free list the free lists come from */

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void rwa_free_list_destroy_005bee60_n(RwaFreeList *fl)
{
  RwaDLink *head = &fl->blocks, *b;

  while ((b = head->next) != head) {
    b->prev->next = b->next;
    b->next->prev = b->prev;
    FUN_005bf120(b);
  }
  if (!(fl->flags & 1)) {
    if (g_data_7dbbf0 != fl && g_data_7dbbf0)
      RwaFreeListFree(g_data_7dbbf0, fl);
    else
      FUN_005bf120(fl);
  }
}

/* _rwaFreeListModuleClose: returns 0 */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwInt32 rwa_free_list_module_close_005beee0(void)
{
  RwaDLink *l;
  RwaFreeList *fl;

  while ((l = g_data_7dbbc0.next) != &g_data_7dbbc0) {
    fl = (RwaFreeList *)((char *)l - 0x1c);
    fl->link.prev->next = l->next;
    fl->link.next->prev = fl->link.prev;
    rwa_free_list_destroy_005bee60_n(fl);
  }
  fl = g_data_7dbbf0;
  fl->link.prev->next = fl->link.next;
  fl->link.next->prev = fl->link.prev;
  rwa_free_list_destroy_005bee60_n(fl);
  g_data_7dbbf0 = 0;
  g_data_7dbbec = 0;
  return 0;
}

/* entry thunk at the original address of rwa_free_list_destroy_005bee60: register inputs (edi) -> natural C rwa_free_list_destroy_005bee60_n; ret 0 */
__declspec(naked) void rwa_free_list_destroy_005bee60(void)
{
    __asm {
        push edi
        call rwa_free_list_destroy_005bee60_n
        add esp, 4
        ret
    }
}
