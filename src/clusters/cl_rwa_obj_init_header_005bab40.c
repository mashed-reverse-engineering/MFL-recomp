// cluster-entries: rwa_obj_init_header_005bab40 rwa_obj_create_005bb0a0
/* Installed cluster rwa_obj_init_header_005bab40 (tools/cluster_install.py from output/cluster_pilot_l2/rwa_obj_init_header_005bab40.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RWA object creation (objects/rwaobj.c): the object header init (register function: definition on the stack, object in edx,
   data offset in eax, flags in ecx) and its caller, RwaObjCreate.  Cluster: rwa_obj_init_header_005bab40 is entered through the
   generated thunk. */
#include "mfl_tier2.h"

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*005ba1d0*/ extern int FUN_005ba1d0(void *parentDef, char *def);      /* _rwaObjDefCreateSupported */
/*005bae90*/ extern RwUInt32 FUN_005bae90(char *def, RwUInt32 flags, RwUInt32 *dataOffset);      /* _rwaObjCalcSize */
/*005ba050*/ extern int FUN_005ba050(char *def, RwUInt32 size, RwUInt32 shared);      /* _rwaObjDefCreateInstanceAllocator */
/*005ba0f0*/ extern char *FUN_005ba0f0(char *def, RwUInt32 size);      /* _rwaObjDefAllocObj */
/*005ba140*/ extern RwUInt32 FUN_005ba140(void *obj, void *lookup, void *arg);      /* _rwaObjDefFreeObj */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */

/*007dae30*/ extern RwaLink g_data_7dae30;        /* the object list */
/*007dae60*/ extern void **g_data_7dae60;         /* the root object */

typedef void (*RwaParamsInit)(int, void *);
typedef char *(*RwaObjAlloc)(char *def, int user, void *params, RwUInt32 size);
typedef int (*RwaObjInit)(char *obj, void **parent, void *params, RwaObjAlloc alloc, int user);

#define DEF_DATA_OFFSET(def) (*(RwUInt32 *)((def) + 0x40) & 0xfffffff)
#define DEF_LAYOUT(def) (*(char **)((def) + 0x4c))

/* Header of a new object: definition, flags, data block; with flag 1 the parameter list heads (3 word nodes, then 2 word nodes)
   are laid out behind the list pair, flag 2 clears the first data word; the child list is emptied. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_obj_init_header_005bab40_n(char *def, char *obj, int dataOffset, RwUInt32 flags)
{
  char *data = obj + dataOffset;
  RwUInt32 n, *lists, *p;

  *(char **)obj = def;
  *(RwUInt32 *)(obj + 0x1c) = flags;
  *(char **)(obj + 0x20) = data;
  if (flags & 1) {
    if (data == 0)
      lists = (RwUInt32 *)(obj + ((flags & 2) ? 0x2c : 0x28));
    else
      lists = (RwUInt32 *)(DEF_DATA_OFFSET(def) + data + ((flags & 2) ? 4 : 0));
    p = lists + 2;
    lists[0] = (RwUInt32)p;
    lists[1] = (RwUInt32)(p + *(unsigned short *)(DEF_LAYOUT(def) + 0xc) * 3);
    for (n = *(unsigned short *)(DEF_LAYOUT(def) + 0xc); n != 0; n--) {
      p[0] = (RwUInt32)p;
      p[1] = (RwUInt32)p;
      p[2] = (RwUInt32)p;
      p += 3;
    }
    p = (RwUInt32 *)lists[1];
    for (n = *(unsigned short *)(DEF_LAYOUT(def) + 0xe); n != 0; n--) {
      p[0] = (RwUInt32)p;
      p[1] = (RwUInt32)p;
      p += 2;
    }
  }
  if (*(unsigned char *)(obj + 0x1c) & 2) {
    char *d = *(char **)(obj + 0x20);
    if (d == 0)
      p = (RwUInt32 *)(obj + 0x28);
    else
      p = (RwUInt32 *)(DEF_DATA_OFFSET(*(char **)obj) + d);
    *p = 0;
  }
  p = (RwUInt32 *)(obj + 0xc);
  p[0] = (RwUInt32)p;
  *(RwUInt32 **)(obj + 0x10) = p;
  *(RwUInt32 *)(obj + 0x18) = 0;
  *(RwUInt32 *)(obj + 0x14) = 0;
}

/* RwaObjCreate: an object of definition def under parent (default: the root object); params default to the definition's own
   (stack buffer, heap when larger than 0x200 bytes); memory from alloc (flag 0x20) or the definition's instance allocator. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_obj_create_005bb0a0(char *def, void **parent, RwUInt32 flags, void *params, RwaObjAlloc alloc, int user)
{
  void *heap;
  RwUInt32 dataOffset;
  char buf[0x200];
  RwUInt32 size;
  char *obj;
  RwaObjInit init;

  flags &= 7;
  if (g_data_7dae60) {
    if (!parent)
      parent = g_data_7dae60;
    if (!FUN_005ba1d0(*parent, def))
      return 0;
  }
  heap = 0;
  if (!params) {
    params = buf;
    if (*(unsigned short *)(def + 0x44) > 0x200) {
      heap = FUN_005bf0d0(*(unsigned short *)(def + 0x44), 0x1080b);
      if (!heap)
        return 0;
      params = heap;
    }
    if (*(RwaParamsInit *)(def + 0x3c))
      (*(RwaParamsInit *)(def + 0x3c))(0, params);
    else
      params = 0;
  }
  size = FUN_005bae90(def, flags, &dataOffset);
  if (alloc && (obj = alloc(def, user, params, size)) != 0)
    flags |= 0x20;
  else if ((*(int *)(def + 0x48) == 0 && !FUN_005ba050(def, size, (flags >> 2) & 1)) || (obj = FUN_005ba0f0(def, size)) == 0) {
    if (heap)
      FUN_005bf120(heap);
    return 0;
  }
  *(void ***)(obj + 0x24) = parent;
  rwa_obj_init_header_005bab40_n(def, obj, dataOffset, flags);
  if (g_data_7dae60) {
    RwaLink *head = (RwaLink *)(parent + 3), *l = (RwaLink *)(obj + 0x14);
    l->next = head->next;
    l->prev = head;
    head->next->prev = l;
    head->next = l;
  }
  init = *(RwaObjInit *)(*(char **)obj + 0x28);
  if (init && !init(obj, parent, params, alloc, user)) {
    if (heap)
      FUN_005bf120(heap);
    if (g_data_7dae60) {
      RwaLink *l = (RwaLink *)(obj + 0x14);
      l->prev->next = l->next;
      l->next->prev = l->prev;
    }
    FUN_005ba140(obj, 0, 0);
    return 0;
  }
  if (heap)
    FUN_005bf120(heap);
  {
    RwaLink *l = (RwaLink *)(obj + 4);
    l->next = g_data_7dae30.next;
    l->prev = &g_data_7dae30;
    g_data_7dae30.next->prev = l;
    g_data_7dae30.next = l;
  }
  return obj;
}

/* entry thunk at the original address of rwa_obj_init_header_005bab40: register inputs (s0,edx,eax,ecx) -> natural C rwa_obj_init_header_005bab40_n; ret 0 */
__declspec(naked) void rwa_obj_init_header_005bab40(void)
{
    __asm {
        push edx
        push ecx
        push eax
        push edx
        push dword ptr [esp + 20]
        call rwa_obj_init_header_005bab40_n
        add esp, 16
        pop edx
        ret
    }
}
