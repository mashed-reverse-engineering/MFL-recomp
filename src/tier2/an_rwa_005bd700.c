/* rwa_obj_interface_def_supported_005bd700 (0x005bd700): object interfaces (_rwaObjInterfaceDefSupported): IFACE if every uuid of its two required lists is found in the matching list of OBJDEF's parameter tables, else 0 */
#include "mfl_tier2.h"

typedef struct RwaIfaceParam { void *uuid; RwUInt32 w04[4]; } RwaIfaceParam;      /* 0x14 bytes */
typedef struct RwaObjParam { void *uuid; RwUInt32 w04[5]; } RwaObjParam;          /* 0x18 bytes */

typedef struct RwaObjInterface {
  RwUInt32 w00[3];
  RwaIfaceParam *listB;      /* +0x0c */
  RwaIfaceParam *listA;      /* +0x10 */
  unsigned short countA;     /* +0x14 */
  unsigned short countB;     /* +0x16 */
} RwaObjInterface;

typedef struct RwaObjParamTables {
  RwUInt32 w00[3];
  unsigned short countA;     /* +0x0c */
  unsigned short countB;     /* +0x0e */
  RwaObjParam *listA;        /* +0x10 */
  RwaObjParam *listB;        /* +0x14 */
} RwaObjParamTables;

#define OBJDEF_PARAMS(d) (*(RwaObjParamTables **)((d) + 0x4c))

/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0 = equal */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
RwaObjInterface *rwa_obj_interface_def_supported_005bd700(RwaObjInterface *iface, char *objDef, int unused)
{
  RwaObjParamTables *tables = OBJDEF_PARAMS(objDef);
  unsigned i, j, count = iface->countA, n = tables->countA;
  int ok = 1;

  for (i = 0; i < count; i++) {
    for (j = 0; j < n; j++)
      if (RwaUUIDCompare(tables->listA[j].uuid, iface->listA[i].uuid) == 0)
        goto foundA;
    ok = 0;      /* required uuid not offered */
  foundA:;
  }
  count = iface->countB;      /* (the original reloads both counts here, the B list base stays the first tables pointer) */
  n = OBJDEF_PARAMS(objDef)->countB;
  for (i = 0; i < count; i++) {
    for (j = 0; j < n; j++)
      if (RwaUUIDCompare(tables->listB[j].uuid, iface->listB[i].uuid) == 0)
        goto foundB;
    ok = 0;
  foundB:;
  }
  return ok ? iface : 0;
}
