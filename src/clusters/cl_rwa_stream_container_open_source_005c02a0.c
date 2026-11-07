// cluster-entries: rwa_stream_container_open_source_005c02a0
/* Installed cluster rwa_stream_container_open_source_005c02a0 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_container_open_source_005c02a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (register function: open request in ebx, container in s0, entered through the generated thunk): pick the stream
   source definition for the request, create the source object and put it on the container (+0x14, list +0x18).  Request kinds (+4):
   1 = file: the definition whose extension list (def + 0x54: +4 names, +8 count) matches the file name's extension (first match);
   2 = data type (+8): the definition whose format list (+0xc formats, +0x10 count) accepts it (the LAST match wins, the walk does not
   stop); 3 = the definition of the fixed uuid at 0x5f825c.  Kinds 1 and 2 walk the definitions that support interface 0x5f81fc.
   1: created; 0: no definition, no memory or the create failed. */
#include "mfl_tier2.h"

typedef struct RwaLListEntry { struct RwaLListEntry *next, *prev; void *data; } RwaLListEntry;

typedef struct RwaOpenRequest {
  char *fileName;           /* +0x00 */
  RwUInt32 kind;            /* +0x04: 1 file, 2 data type, 3 fixed definition */
  void *dataType;           /* +0x08 */
  RwUInt32 w0c, w10, w14, w18, w1c, w20;
} RwaOpenRequest;

typedef struct RwaSourceInfo {      /* source definition + 0x54 */
  RwUInt32 w00;
  char **extensions;        /* +0x04 */
  RwUInt32 numExtensions;   /* +0x08 */
  void **formats;           /* +0x0c */
  RwUInt32 numFormats;      /* +0x10 */
} RwaSourceInfo;

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*005f825c*/ extern undefined g_data_5f825c;      /* uuid of the kind 3 source definition */
/*005f81fc*/ extern undefined g_data_5f81fc;      /* uuid of the stream source interface */
/*005ba5c0*/ extern void *FUN_005ba5c0(int a, const void *uuid, void *out);      /* RwaObjDefFindByUUID */
/*005bda80*/ extern void *rwa_unit243b40_005bda80(const void *uuid);      /* RwaObjInterfaceDefFindByUUID */
/*005ba4f0*/ extern int FUN_005ba4f0(void *def, void *iface, RwaLListEntry *out);      /* _rwaObjDefBuildSupportedList */
/*005be560*/ extern void RwaLListEmpty(RwaLListEntry *head);
/*005bf940*/ extern char *FUN_005bf940(char *fileName);      /* _rwaGetFilenameExt */
/*005be770*/ extern int rwa_uuid_005be770(const char *a, const char *b, size_t n);      /* _rwamemicmp: 0 = equal */
/*005bcf00*/ extern int RwaWaveDefFormatWaveFormatCompare(const void *format, const void *dataType);      /* != 0: matches */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005ba030*/ extern int RwaObjDefGetDefaultCreationParams(void *def, void *params);
/*005bb0a0*/ extern char *FUN_005bb0a0(void *objdef, int b1, int b2, void *params, void *b4, void *b5);      /* RwaObjCreate */

#define SOURCE_INFO(def) (*(RwaSourceInfo **)((char *)(def) + 0x54))

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_stream_container_open_source_005c02a0_n(RwaOpenRequest *req, char *container)
{
  RwaLListEntry defs, *e;
  char *def = 0, *ext, *obj, *source;
  RwUInt32 i, out;
  RwaSourceInfo *info;

  switch (req->kind) {
  case 3:
    def = FUN_005ba5c0(0, &g_data_5f825c, &out);
    break;
  case 2:
    if (!req->dataType)
      return 0;
    defs.next = defs.prev = &defs;
    defs.data = 0;
    if (!FUN_005ba4f0(0, rwa_unit243b40_005bda80(&g_data_5f81fc), &defs))
      return 0;
    for (e = defs.prev; e != &defs; e = e->prev) {
      info = SOURCE_INFO(e->data);
      for (i = 0; i < info->numFormats; i++)
        if (RwaWaveDefFormatWaveFormatCompare(info->formats[i], req->dataType)) {
          def = e->data;
          break;
        }
    }
    RwaLListEmpty(&defs);
    break;
  case 1:
    defs.next = defs.prev = &defs;
    defs.data = 0;
    if (!FUN_005ba4f0(0, rwa_unit243b40_005bda80(&g_data_5f81fc), &defs))
      return 0;
    ext = FUN_005bf940(req->fileName);
    if (ext == req->fileName) {      /* no extension */
      RwaLListEmpty(&defs);
      return 0;
    }
    for (e = defs.prev; e != &defs && !def; e = e->prev) {
      info = SOURCE_INFO(e->data);
      for (i = 0; i < info->numExtensions; i++)
        if (!rwa_uuid_005be770(info->extensions[i], ext, rwstrlen(info->extensions[i]))) {
          def = e->data;
          break;
        }
    }
    RwaLListEmpty(&defs);
    break;
  default:
    return 0;
  }
  if (!def)
    return 0;
  obj = FUN_005bf0d0(*(unsigned short *)(def + 0x44), 0x1080e);      /* creation parameters */
  if (!obj)
    return 0;
  if (!RwaObjDefGetDefaultCreationParams(def, obj)) {
    FUN_005bf120(obj);
    return 0;
  }
  *(RwUInt32 *)obj = 0;
  *(RwUInt32 *)(obj + 4) = 1;
  *(RwUInt32 *)(obj + 8) = 0;
  *(char **)(obj + 0x10) = container + 0x28;
  *(char **)(obj + 0x2c) = req->fileName;
  *(RwUInt32 *)(obj + 0x30) = req->kind;
  *(void **)(obj + 0x28) = req->dataType;
  *(RwUInt32 *)(obj + 0x34) = req->w0c;
  *(RwUInt32 *)(obj + 0x38) = req->w10;
  *(RwUInt32 *)(obj + 0x40) = req->w18;
  *(RwUInt32 *)(obj + 0x44) = req->w14;
  *(RwUInt32 *)(container + 0x10) = req->w1c;
  *(RwUInt32 *)(obj + 0x4c) = req->w20 ? req->w1c : 0;
  *(RwUInt32 *)(obj + 0x3c) = 0;
  *(RwUInt32 *)(obj + 0x48) = 0;
  *(RwUInt32 *)(obj + 0x54) = 0;
  *(RwUInt32 *)(obj + 0x50) = 0;
  source = FUN_005bb0a0(def, 0, 0, obj, *(void **)(container + 0x50), *(void **)(container + 0x58));
  if (!source) {
    FUN_005bf120(obj);
    return 0;
  }
  {      /* link the source (link at +0x2c) at the front of the container's source list (+0x18) */
    RwaLListEntry *head = (RwaLListEntry *)(container + 0x18), *link = (RwaLListEntry *)(source + 0x2c);
    link->next = head->next;
    link->prev = head;
    head->next->prev = link;
    head->next = link;
  }
  *(char **)(container + 0x14) = source;
  FUN_005bf120(obj);
  return 1;
}

/* entry thunk at the original address of rwa_stream_container_open_source_005c02a0: register inputs (ebx,s0) -> natural C rwa_stream_container_open_source_005c02a0_n; ret 0 */
__declspec(naked) void rwa_stream_container_open_source_005c02a0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ebx
        call rwa_stream_container_open_source_005c02a0_n
        add esp, 8
        ret
    }
}
