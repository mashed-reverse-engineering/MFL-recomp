/* PlaneSectorStreamRead (MFL.exe 0x004fa440): RenderWare Graphics 3.7 (Criterion Software), reads a BSP plane sector (struct chunk,
 * then the left and right subtrees: world sector or plane sector chunks) of a binary world from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/babinwor.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 / RwMemFloat32ToReal are no-ops and gone), no codfh world-arena abort check,
 * a missing left plane sector chunk tidies up like every other failure (the exe; codfh returns at once there), RWFUNCTION / RWASSERT /
 * RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN),
 * BinaryWorldMalloc written out (the exe inlines it), external linkage (the exe's symbol), the structs declared here (codfh
 * babinwor.h / bapipew.h / baerr.h) with compile-time layout asserts.  Verified by tools/diffemu (tier 2, chunk-stream spec
 * s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct _rpPlaneSector {         /* RpPlaneSectorChunkInfo */
    RwInt32 type;
    RwReal value;
    RwBool leftIsWorldSector;
    RwBool rightIsWorldSector;
    RwReal leftValue;
    RwReal rightValue;
} _rpPlaneSector;
typedef struct RpSector RpSector;
typedef struct RpPlaneSector {
    RwInt32 type;
    RwReal value;
    RpSector *leftSubTree;
    RpSector *rightSubTree;
    RwReal leftValue;
    RwReal rightValue;
} RpPlaneSector;
typedef struct RpWorld {                /* the head: RwObject (privateFlags at +3) */
    RwUInt8 type;
    RwUInt8 subType;
    RwUInt8 flags;
    RwUInt8 privateFlags;
} RpWorld;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_rpplane_size, sizeof(_rpPlaneSector) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_plane_size, sizeof(RpPlaneSector) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_plane_left, RW_OFFSETOF(RpPlaneSector, leftSubTree) == 8);
RW_LAYOUT_ASSERT(rw_assert_plane_lval, RW_OFFSETOF(RpPlaneSector, leftValue) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_world_pflags, RW_OFFSETOF(RpWorld, privateFlags) == 3);

#define TRUE 1
#define FALSE 0
#define rwID_STRUCT 0x1
#define rwID_ATOMICSECT 0x9
#define rwID_PLANESECT 0xa
#define rwID_WORLDPLUGIN 0x2
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rpWORLDSECTORSOVERLAP 0x40000000
#define rpWORLDSINGLEMALLOC 0x01
#define E_RW_BADVERSION ((RwInt32)0x80000004)
#define E_RW_NOMEM ((RwInt32)0x80000013)

/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004fa060*/ extern void *WorldSectorStreamRead(RwStream *stream, RwUInt8 **binaryWorldMallocAddr, RpWorld *world, RwUInt32 flags);
/*004fa440*/ extern RpPlaneSector *PlaneSectorStreamRead(RwStream *stream, RwUInt8 **binaryWorldMallocAddr, RpWorld *world, RwUInt32 flags);      /* recursive: the shadow build renames the definition to _c and adds the wrapper after it */
/*004f5090*/ extern RpSector *_rpWorldSectorDeinstanceAll(RpSector *sector);
/*004f5140*/ extern RpSector *_rpWorldSectorDestroyRecurse(RpSector *sector);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RWERROR_SET(args) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror args; RwErrorSet(&error); }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpPlaneSector *PlaneSectorStreamRead(RwStream *stream, RwUInt8 **binaryWorldMallocAddr, RpWorld *world, RwUInt32 flags)
{
    RpPlaneSector *planeSector;
    _rpPlaneSector ps = { 0 };
    RwUInt32 size, version;
    RwBool result;

    result = TRUE;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, &size, &version))
    {
        return NULL;
    }

    if ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION))
    {
        RWERROR_SET((E_RW_BADVERSION));
        return NULL;
    }

    /* The size read from the stream is the size stored, no question about it */
    if (RwStreamRead(stream, &ps, size) != size)
    {
        return NULL;
    }

    /* BinaryWorldMalloc: from the pool of pre-allocated memory */
    planeSector = (RpPlaneSector *)*binaryWorldMallocAddr;
    *binaryWorldMallocAddr += sizeof(RpPlaneSector);
    if (!planeSector)
    {
        RWERROR_SET((E_RW_NOMEM, (sizeof(RpPlaneSector))));
        return NULL;
    }

    /* Use contents */
    planeSector->type = ps.type;
    planeSector->value = ps.value;
    if (flags & rpWORLDSECTORSOVERLAP)
    {
        planeSector->leftValue = ps.leftValue;
        planeSector->rightValue = ps.rightValue;
    }
    else
    {
        planeSector->leftValue = ps.value;
        planeSector->rightValue = ps.value;
    }

    /* Left */
    if (ps.leftIsWorldSector)
    {
        if (!RwStreamFindChunk(stream, rwID_ATOMICSECT, NULL, &version))
        {
            /* Deallocation is performed in one block when we return */
            result = FALSE;
        }

        if (result && ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION)))
        {
            RWERROR_SET((E_RW_BADVERSION));
            result = FALSE;
        }

        if (result && (!(planeSector->leftSubTree = (RpSector *)WorldSectorStreamRead(stream, binaryWorldMallocAddr, world, flags))))
        {
            result = FALSE;
        }
    }
    else
    {
        if (!RwStreamFindChunk(stream, rwID_PLANESECT, NULL, &version))
        {
            result = FALSE;
        }

        if (result && ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION)))
        {
            RWERROR_SET((E_RW_BADVERSION));
            result = FALSE;
        }

        if (result && (!(planeSector->leftSubTree = (RpSector *)PlaneSectorStreamRead(stream, binaryWorldMallocAddr, world, flags))))
        {
            result = FALSE;
        }
    }

    /* Right */
    if (result)
    {
        if (ps.rightIsWorldSector)
        {
            if (!RwStreamFindChunk(stream, rwID_ATOMICSECT, NULL, &version))
            {
                result = FALSE;
            }

            if (result && ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION)))
            {
                RWERROR_SET((E_RW_BADVERSION));
                result = FALSE;
            }

            if (result && (!(planeSector->rightSubTree = (RpSector *)WorldSectorStreamRead(stream, binaryWorldMallocAddr, world, flags))))
            {
                result = FALSE;
            }
        }
        else
        {
            if (!RwStreamFindChunk(stream, rwID_PLANESECT, NULL, &version))
            {
                result = FALSE;
            }

            if (result && ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION)))
            {
                RWERROR_SET((E_RW_BADVERSION));
                result = FALSE;
            }

            if (result && (!(planeSector->rightSubTree = (RpSector *)PlaneSectorStreamRead(stream, binaryWorldMallocAddr, world, flags))))
            {
                result = FALSE;
            }
        }
    }

    /* Was there an error ? */
    if (result == FALSE)
    {
        if (planeSector->leftSubTree)
        {
            if (world->privateFlags & rpWORLDSINGLEMALLOC)
            {
                _rpWorldSectorDeinstanceAll(planeSector->leftSubTree);
            }
            else
            {
                _rpWorldSectorDestroyRecurse(planeSector->leftSubTree);
            }

            planeSector->leftSubTree = NULL;
        }

        return NULL;
    }

    return planeSector;
}
