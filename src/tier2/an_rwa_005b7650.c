/* rwa_voice_manager_register_005b7650 (0x005b7650): voices (RwaVoiceManagerRegister): one block for the output handle and a handle per voice, created through the output / voice interfaces */
#include "mfl_tier2.h"

/*005bda80*/ extern void *rwa_unit243b40_005bda80(const void *uuid);      /* RwaObjInterfaceDefFindByUUID */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005b7450*/ extern void *RwaObjHandleCreateUsingObj(void *obj, void *intf, void *params);
/*005b7330*/ extern void FUN_005b7330(void *handle, int param, int count, void *data);      /* set a handle parameter */
/*005b71e0*/ extern void RwaObjHandleRelease(void *handle, int a2);
/*005b7640*/ extern void rwa_unit23e610_005b7640(void);      /* bump allocator of the creation block */
/*005f7854*/ extern undefined g_rwa_output_int_uuid;      /* output interface */
/*005f76a4*/ extern undefined g_data_5f76a4;              /* voice interface */
/*007dab68*/ extern void **g_data_7dab68;      /* the voice handles */
/*007dab70*/ extern RwUInt32 g_data_7dab70;    /* registered */
/*007dab74*/ extern void *g_data_7dab74;       /* the output handle */
/*007dab78*/ extern RwUInt32 g_data_7dab78;    /* voice count */

#define RWA_HINT 0x3080d

typedef struct RwaObjIntf { char pad0[0x14]; unsigned short nParams, nMethods; } RwaObjIntf;

typedef struct RwaHandleCreateParams {      /* handles are carved from one block (allocator + cursor) */
  RwUInt32 numParams;        /* 2 */
  RwUInt32 a, b;
  void (*alloc)(void);
  char **next;               /* the allocation cursor */
  void *block;
} RwaHandleCreateParams;

typedef struct RwaHandleParam { RwUInt32 count; void **values; } RwaHandleParam;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_voice_manager_register_005b7650(void *output, void **voices, RwUInt32 numVoices)
{
  RwaObjIntf *outIntf = (RwaObjIntf *)rwa_unit243b40_005bda80(&g_rwa_output_int_uuid);
  RwaObjIntf *voiceIntf = (RwaObjIntf *)rwa_unit243b40_005bda80(&g_data_5f76a4);
  RwaHandleCreateParams cp;
  RwaHandleParam hp;
  char *block;
  RwUInt32 n = numVoices, i, size;
#define cursor (*(char **)&numVoices)      /* the original keeps the allocation cursor in the (then dead) count argument */

  size = (voiceIntf->nParams + voiceIntf->nMethods) * 8 + 0x28 + (n * 3 + 3) * 8 +
         (outIntf->nParams + outIntf->nMethods) * 8 + 0x1c + n * 4;      /* the handles' parameter / method slots, n handles, the array */
  block = (char *)FUN_005bf0d0(size, RWA_HINT);
  if (!block)
    return 0;
  cp.b = 0;
  cp.a = 0;
  cp.next = &cursor;
  cp.numParams = 2;
  cp.alloc = rwa_unit23e610_005b7640;
  cp.block = block;
  cursor = block + 0x18;
  g_data_7dab74 = RwaObjHandleCreateUsingObj(output, outIntf, &cp);
  if (!g_data_7dab74) {
    FUN_005bf120(block);
    return 0;
  }
  g_data_7dab68 = (void **)cursor;      /* the voice handle array, then the handles */
  for (i = 0, cursor += n * sizeof(void *); i < n; i++) {
    cp.block = cursor;
    cursor += 0x18;
    g_data_7dab68[i] = RwaObjHandleCreateUsingObj(voices[i], voiceIntf, &cp);
    if (!g_data_7dab68[i]) {
      while (i)
        RwaObjHandleRelease(g_data_7dab68[--i], 0);
      RwaObjHandleRelease(g_data_7dab74, 0);
      g_data_7dab78 = 0;
      g_data_7dab74 = 0;
      return 0;
    }
  }
  hp.values = g_data_7dab68;
  g_data_7dab70 = 1;
  g_data_7dab78 = n;
  hp.count = n;
  FUN_005b7330(g_data_7dab74, 5, 1, &hp);
  return 1;
#undef cursor
}
