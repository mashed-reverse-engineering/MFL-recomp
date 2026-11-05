/* rwa_obj_destroy_005baf80 (0x005baf80): objects (RwaObjDestroy): the definition's destroy callback, unlink, disconnect every parameter, free */
#include "mfl_tier2.h"

/*005bad90*/ extern void RwaObjDisconnectParamIndex(void *obj, RwUInt32 index, int kind);
/*005ba140*/ extern int _rwaObjDefFreeObj(void *obj, void *a2, void *a3);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

typedef struct RwaObjDef {
  char pad0[0x34];
  void (*destroy)(void *obj, void *a2, void *a3);      /* +0x34 */
  char pad38[0x4c - 0x38];
  struct { char pad[0xc]; unsigned short inputs, outputs; } *params;      /* +0x4c */
} RwaObjDef;

typedef struct RwaObj {
  RwaObjDef *def;          /* +0x00 */
  RwaLink link;            /* +0x04 in the definition's object list */
  RwaLink children;        /* +0x0c objects attached to this one */
  RwaLink *dep;            /* +0x14 dependency link (next) */
  RwaLink *depPrev;        /* +0x18 */
  RwUInt32 flags;          /* +0x1c bit 0: parameters connected */
} RwaObj;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_obj_destroy_005baf80(RwaObj *obj, void *a2, void *a3)
{
  RwaLink *n;

  if (obj->def->destroy)
    obj->def->destroy(obj, a2, a3);
  if (obj->dep) {
    obj->depPrev->next = obj->dep;
    obj->dep->prev = obj->depPrev;
    obj->depPrev = 0;
    obj->dep = 0;
  }
  for (n = obj->children.next; n != &obj->children; ) {      /* detach every child */
    RwaLink *next = n->next;

    n->prev->next = next;
    n->next->prev = n->prev;
    n = next;
  }
  if (obj->flags & 1) {
    RwUInt32 i, cnt;

    cnt = obj->def->params->inputs;
    for (i = 0; i < cnt; i++)
      RwaObjDisconnectParamIndex(obj, i, 1);
    cnt = obj->def->params->outputs;
    for (i = 0; i < cnt; i++)
      RwaObjDisconnectParamIndex(obj, i, 2);
  }
  obj->link.prev->next = obj->link.next;
  obj->link.next->prev = obj->link.prev;
  return _rwaObjDefFreeObj(obj, a2, a3);
}
