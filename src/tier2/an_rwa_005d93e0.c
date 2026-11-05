/* rwa_soft_dsp_voice_destroy_lookup_005d93e0 (0x005d93e0): software dsp: destroy lookup of the embedded voice (see 0x5da0f0) */
#include "mfl_tier2.h"

/*005ba5c0*/ extern void *FUN_005ba5c0(int a, const void *uuid, int b);      /* RwaObjDefFindByUUID */
/*005ba140*/ extern RwUInt32 FUN_005ba140(void *obj, void *lookup, void *arg);      /* _rwaObjDefFreeObj */
/*005f834c*/ extern undefined g_rwa_soft_output_lookup_uuid;

typedef RwUInt32 (*RwaDestroyFn)(void *obj, void *arg, void *a3, void *a4);
typedef struct RwaLookupLink { RwaDestroyFn next; void *arg; char *obj; } RwaLookupLink;

#define RWA_FREED_FILL 0xcdcdcdcdu      /* debug fill of a freed block */

/* obj = the object being destroyed (its first word = its definition).  With a next lookup in the chain the call goes there
   (the original tail-jumps with the caller's further arguments unchanged, a3 / a4 here).  Else: for an object of the
   definition the uuid names, the embedded voice block (0x128 bytes at link->obj+0x34) is filled with 0xcd and the fill word
   returned (the original's eax after rep stos); any other object is freed by its definition. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 rwa_soft_dsp_voice_destroy_lookup_005d93e0(void **obj, RwaLookupLink *link, void *a3, void *a4)
{
  RwUInt32 *p;
  int i;

  if (link->next)
    return link->next(obj, link->arg, a3, a4);
  if (*obj != FUN_005ba5c0(0, &g_rwa_soft_output_lookup_uuid, 0))
    return FUN_005ba140(obj, 0, link->arg);
  p = (RwUInt32 *)(link->obj + 0x34);
  for (i = 0; i < 0x4a; i++)
    p[i] = RWA_FREED_FILL;
  return RWA_FREED_FILL;
}
