/* rwa_obj_handle_create_using_obj_005b7450 (0x005b7450): object handles (RwaObjHandleCreateUsingObj): a handle on an object through one of its interfaces, counted on the object and listed */
#include "mfl_tier2.h"

/*005bec50*/ extern void *rwa_unit243670_005bec50(void *freeList, RwUInt32 hint);      /* RwaFreeListAlloc */
/*005bed80*/ extern void RwaFreeListFree(void *freeList, void *block);
/*005bdb30*/ extern void *RwaObjInterfaceAttach(void *intf, void *objDef, void *alloc, void *allocData);
/*007dab30*/ extern undefined g_data_7dab30;      /* free list of handles */
/*007dab28*/ extern struct RwaLink g_data_7dab28;  /* list of all handles */

#define RWA_HINT 0x3080b
#define OH_CALLER_BLOCK 1      /* handle flags (+0x10) */
#define OH_ALIVE        2

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

typedef struct RwaHandleCreateParams {      /* see RwaVoiceManagerRegister */
  RwUInt32 numParams, a, b;
  void *alloc;             /* +0x0c allocator for the interface binding */
  void *allocData;         /* +0x10 */
  void *block;             /* +0x14 the handle's memory, NULL: from the free list */
} RwaHandleCreateParams;

typedef struct RwaObj {
  char *def;               /* +0x00: +0x40 object size (low 28 bits), +0x4c interfaces (+0 default interface) */
  char pad4[0x1c];
  char *ext;               /* +0x20 extension block, the reference count behind the object data */
  char pad24[4];
  RwUInt32 refs;           /* +0x28 */
} RwaObj;

typedef struct RwaObjHandle {
  RwaObj *obj;             /* +0x00 */
  void *binding;           /* +0x04 */
  RwaLink link;            /* +0x08 */
  RwUInt32 flags;          /* +0x10 */
  void *x14;               /* +0x14 */
} RwaObjHandle;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaObjHandle *rwa_obj_handle_create_using_obj_005b7450(RwaObj *obj, void *intf, RwaHandleCreateParams *params)
{
  RwaHandleCreateParams defaults;
  RwaObjHandle *h;
  RwUInt32 *refs;

  if (!params) {
    defaults.allocData = 0;
    defaults.alloc = 0;
    defaults.b = 0;
    defaults.a = 0;
    defaults.numParams = 2;
    defaults.block = 0;
    params = &defaults;
  }
  h = (RwaObjHandle *)params->block;
  if (h)
    h->flags = OH_CALLER_BLOCK;
  else {
    h = (RwaObjHandle *)rwa_unit243670_005bec50(&g_data_7dab30, RWA_HINT);
    h->flags = 0;
  }
  h->flags |= OH_ALIVE;
  h->obj = obj;
  h->binding = 0;
  h->x14 = 0;
  if (intf || (intf = **(void ***)(obj->def + 0x4c)) != 0) {      /* the object's default interface */
    h->binding = RwaObjInterfaceAttach(intf, obj->def, params->alloc, params->allocData);
    if (!h->binding) {
      if (!(h->flags & OH_CALLER_BLOCK))
        RwaFreeListFree(&g_data_7dab30, h);
      return 0;
    }
  }
  if (h->obj->ext)
    refs = (RwUInt32 *)(h->obj->ext + (*(RwUInt32 *)(h->obj->def + 0x40) & 0x0fffffff));
  else
    refs = &h->obj->refs;
  (*refs)++;
  h->link.prev = 0;
  h->link.next = 0;
  h->link.next = g_data_7dab28.next;
  h->link.prev = &g_data_7dab28;
  g_data_7dab28.next->prev = &h->link;
  g_data_7dab28.next = &h->link;
  return h;
}
