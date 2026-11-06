/* rwa_obj_handle_release_005b71e0 (0x005b71e0): object handles (RwaObjHandleRelease): drop a reference; the last one removes the interface, destroys an unshared object and frees the handle */
#include "mfl_tier2.h"

/*005bd6c0*/ extern int RwaObjInterfaceRemove(void *iface);      /* 1: removed */
/*005baf80*/ extern void RwaObjDestroy(char *obj, void *a, void *b);
/*005bed80*/ extern void RwaFreeListFree(void *list, void *item);
/*007dab30*/ extern undefined g_data_7dab30;      /* handle free list */

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;
typedef struct RwaObjHandle {
  char *obj;                        /* +0x00 */
  void *iface;                      /* +0x04 */
  RwaLink link;                     /* +0x08 */
  RwUInt32 flags;                   /* +0x10 1: not from the free list, 2: keep the object */
  RwUInt32 refs;                    /* +0x14 extra references */
} RwaObjHandle;

/* result (optional): [0], [1] destroy arguments, [2] the released handle, [3] the removed interface */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_obj_handle_release_005b71e0(RwaObjHandle *h, void **result)
{
  void *local[4];
  RwUInt32 *count;
  char *obj, *data;

  local[0] = 0;
  local[1] = 0;
  local[2] = 0;
  local[3] = 0;
  if (!result)
    result = local;
  if (h->refs) {
    h->refs--;
    return;
  }
  if (h->iface && RwaObjInterfaceRemove(h->iface) == 1)
    result[3] = h->iface;
  obj = h->obj;
  data = *(char **)(obj + 0x20);
  count = data ? (RwUInt32 *)((*(RwUInt32 *)(*(char **)obj + 0x40) & 0xfffffff) + data) : (RwUInt32 *)(obj + 0x28);
  if (--*count == 0 && !(*(unsigned char *)&h->flags & 2))
    RwaObjDestroy(obj, result[0], result[1]);
  h->link.prev->next = h->link.next;
  h->link.next->prev = h->link.prev;
  if (!(*(unsigned char *)&h->flags & 1))
    RwaFreeListFree(&g_data_7dab30, h);
  result[2] = h;
}
