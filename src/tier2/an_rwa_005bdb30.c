/* rwa_obj_interface_attach_005bdb30 (0x005bdb30): object interfaces (RwaObjInterfaceAttach): the binding of an interface to an object definition (shared, reference counted), mapping the interface's parameters / methods to the definition's by uuid */
#include "mfl_tier2.h"

/*005bd700*/ extern int _rwaObjInterfaceDefSupported(void *intf, void *objDef, int a3);
/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0 = equal */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */

#define RWA_HINT 0x3080a

typedef struct RwaIntfEntry { const void *uuid; char pad4[0x10]; } RwaIntfEntry;      /* 0x14: parameter / method of an interface */
typedef struct RwaDefEntry { const void *uuid; char pad4[0x10]; RwUInt32 index; } RwaDefEntry;      /* 0x18: of a definition, +0x14 its slot */
typedef struct RwaIntfDef {
  char pad0[0xc];
  RwaIntfEntry *methods;   /* +0x0c */
  RwaIntfEntry *params;    /* +0x10 */
  unsigned short numParams, numMethods;      /* +0x14 */
} RwaIntfDef;

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

typedef struct RwaDefIntfs {      /* objDef+0x4c */
  char pad0[4];
  RwaLink bindings;        /* +0x04 list of RwaIntfBinding.link */
  unsigned short numParams, numMethods;      /* +0x0c */
  RwaDefEntry *params;     /* +0x10 */
  RwaDefEntry *methods;    /* +0x14 */
} RwaDefIntfs;

typedef struct RwaSlotMap { RwUInt32 slot; unsigned short index; unsigned short pad; } RwaSlotMap;      /* 8 */

typedef struct RwaIntfBinding {
  RwaIntfDef *intf;        /* +0x00 */
  RwaSlotMap *params;      /* +0x04 */
  RwaSlotMap *methods;     /* +0x08 */
  RwUInt32 refs;           /* +0x0c */
  RwUInt32 userAlloc;      /* +0x10 allocated by the caller's allocator */
  RwaLink link;            /* +0x14 */
} RwaIntfBinding;          /* + the two maps (0x1c) */

typedef void *(*RwaAllocFn)(RwUInt32 size, void *data);

/* Returns the binding, NULL when the definition does not support the interface or the allocation fails. */
/* TIER2: unvalidated 2026-11-10 authority=static */
RwaIntfBinding *rwa_obj_interface_attach_005bdb30(RwaIntfDef *intf, char *objDef, RwaAllocFn alloc, void *allocData)
{
  RwaDefIntfs *defs = *(RwaDefIntfs **)(objDef + 0x4c);
  RwaLink *head = &defs->bindings;
  RwaLink *n;
  RwaIntfBinding *b;
  RwUInt32 size, i;
  unsigned short count, j;

  for (n = head->next; n != head; n = n->next) {
    b = (RwaIntfBinding *)((char *)n - 0x14);
    if (b->intf == intf) {
      b->refs++;
      return b;
    }
  }
  if (!_rwaObjInterfaceDefSupported(intf, objDef, 1))
    return 0;
  size = (intf->numParams + intf->numMethods) * sizeof(RwaSlotMap) + sizeof(RwaIntfBinding);
  if (alloc) {
    b = (RwaIntfBinding *)alloc(size, allocData);
    if (!b)
      return 0;
    b->userAlloc = 1;
  } else {
    b = (RwaIntfBinding *)FUN_005bf0d0(size, RWA_HINT);
    if (!b)
      return 0;
    b->userAlloc = 0;
  }
  b->params = (RwaSlotMap *)(b + 1);
  b->methods = b->params + intf->numParams;
  b->intf = intf;
  b->refs = 0;

  count = (*(RwaDefIntfs **)(objDef + 0x4c))->numParams;
  for (i = 0; i < intf->numParams; i++)
    for (j = 0; j < count; j++)
      if (RwaUUIDCompare(defs->params[j].uuid, intf->params[i].uuid) == 0) {
        b->params[i].index = j;
        b->params[i].slot = defs->params[j].index;
      }
  count = (*(RwaDefIntfs **)(objDef + 0x4c))->numMethods;
  for (i = 0; i < intf->numMethods; i++)
    for (j = 0; j < count; j++)
      if (RwaUUIDCompare(defs->methods[j].uuid, intf->methods[i].uuid) == 0) {
        b->methods[i].index = j;
        b->methods[i].slot = defs->methods[j].index;
      }

  b->link.prev = head;
  b->link.next = head->next;
  head->next->prev = &b->link;
  head->next = &b->link;
  return b;
}
