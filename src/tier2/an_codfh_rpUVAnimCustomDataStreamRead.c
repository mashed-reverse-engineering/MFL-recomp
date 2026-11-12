/* _rpUVAnimCustomDataStreamRead (MFL.exe 0x00562210): RenderWare Graphics 3.7 (Criterion Software), reads the UV animation custom data
 * (name, node to UV channel map) of an animation from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/uvanim/rpuvanim.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded, RwFreeListAlloc expanded to the engine's freeListAlloc slot,
 * the struct declared here (codfh rpuvanim.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define RP_UVANIM_MAXNAME 32
#define RP_UVANIM_MAXSLOTS 8
typedef struct _rpUVAnimCustomData {
    char name[RP_UVANIM_MAXNAME];
    RwUInt32 nodeToUVChannelMap[RP_UVANIM_MAXSLOTS];
    RwInt32 refCount;
} _rpUVAnimCustomData;
RW_LAYOUT_ASSERT(rw_assert_uvcustom_map, RW_OFFSETOF(_rpUVAnimCustomData, nodeToUVChannelMap) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_uvcustom_ref, RW_OFFSETOF(_rpUVAnimCustomData, refCount) == 0x40);

#define rwID_UVANIMPLUGIN 0x135
#define rwMEMHINTDUR_EVENT 0x30000

/* RwFreeListAlloc: RwGlobals.freeListAlloc 0x118 (rw37.h) */
typedef void *(*RwFreeListAllocFunc)(void *, RwUInt32);
#define RwFreeListAlloc RW_GLOBAL(0x118, RwFreeListAllocFunc)

/*007d2128*/ extern void *RwEngineInstance;
/*007da8b0*/ extern void *_rpUVAnimCustomDataFreeList;
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);

/* TIER2: validated 2026-11-10 authority=diffemu */
_rpUVAnimCustomData *_rpUVAnimCustomDataStreamRead(RwStream *stream)
{
    _rpUVAnimCustomData *customData;

    /* Allocate custom data */
    customData = (_rpUVAnimCustomData *)RwFreeListAlloc(_rpUVAnimCustomDataFreeList, rwID_UVANIMPLUGIN | rwMEMHINTDUR_EVENT);
    if (!customData)
    {
        return NULL;
    }

    if (!RwStreamRead(stream, customData->name, RP_UVANIM_MAXNAME))
    {
        return NULL;
    }

    if (!RwStreamReadInt32(stream, (RwInt32 *)customData->nodeToUVChannelMap, sizeof(RwInt32) * RP_UVANIM_MAXSLOTS))
    {
        return NULL;
    }

    /* Caller will own */
    customData->refCount = 1;

    return customData;
}
