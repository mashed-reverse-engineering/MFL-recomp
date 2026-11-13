/* LODAtomicCallBackReadChunk (MFL.exe 00567ef0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/lodatm/rplodatm.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helpers
 * LODAtomicNewCache / LODAtomicInitCache / LODAtomicHookRender inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwCorePluginID
{
    rwID_NAOBJECT = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION = MAKECHUNKID(rwVENDORID_CORE, 0x03),

    rwID_CAMERA = MAKECHUNKID(rwVENDORID_CORE, 0x05),

    rwID_TEXTURE = MAKECHUNKID(rwVENDORID_CORE, 0x06),

    rwID_MATERIAL = MAKECHUNKID(rwVENDORID_CORE, 0x07),

    rwID_MATLIST = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT = MAKECHUNKID(rwVENDORID_CORE, 0x0A),

    rwID_WORLD = MAKECHUNKID(rwVENDORID_CORE, 0x0B),

    rwID_SPLINE = MAKECHUNKID(rwVENDORID_CORE, 0x0C),

    rwID_MATRIX = MAKECHUNKID(rwVENDORID_CORE, 0x0D),

    rwID_FRAMELIST = MAKECHUNKID(rwVENDORID_CORE, 0x0E),

    rwID_GEOMETRY = MAKECHUNKID(rwVENDORID_CORE, 0x0F),

    rwID_CLUMP = MAKECHUNKID(rwVENDORID_CORE, 0x10),

    rwID_LIGHT = MAKECHUNKID(rwVENDORID_CORE, 0x12),

    rwID_UNICODESTRING = MAKECHUNKID(rwVENDORID_CORE, 0x13),

    rwID_ATOMIC = MAKECHUNKID(rwVENDORID_CORE, 0x14),

    rwID_TEXTURENATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x15),

    rwID_TEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x16),

    rwID_ANIMDATABASE = MAKECHUNKID(rwVENDORID_CORE, 0x17),

    rwID_IMAGE = MAKECHUNKID(rwVENDORID_CORE, 0x18),

    rwID_SKINANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST = MAKECHUNKID(rwVENDORID_CORE, 0x1A),

    rwID_ANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_HANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_TEAM = MAKECHUNKID(rwVENDORID_CORE, 0x1C),

    rwID_CROWD = MAKECHUNKID(rwVENDORID_CORE, 0x1D),

    rwID_DMORPHANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1E),

    rwID_RIGHTTORENDER = MAKECHUNKID(rwVENDORID_CORE, 0x1f),

    rwID_MTEFFECTNATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x20),

    rwID_MTEFFECTDICT = MAKECHUNKID(rwVENDORID_CORE, 0x21),

    rwID_TEAMDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x22),

    rwID_PITEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x23),

    rwID_TOC = MAKECHUNKID(rwVENDORID_CORE, 0x24),

    rwID_PRTSTDGLOBALDATA = MAKECHUNKID(rwVENDORID_CORE, 0x25),

    rwID_ALTPIPE = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH = MAKECHUNKID(rwVENDORID_CORE, 0x28),

    rwID_CHUNKGROUPSTART = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND = MAKECHUNKID(rwVENDORID_CORE, 0x2A),

    rwID_UVANIMDICT = MAKECHUNKID(rwVENDORID_CORE, 0x2B),

    rwID_COLLTREE = MAKECHUNKID(rwVENDORID_CORE, 0x2C),

    rwID_ENVIRONMENT = MAKECHUNKID(rwVENDORID_CORE, 0x2D),

    rwID_COREPLUGINIDMAX = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
    rwCOREPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCorePluginID RwCorePluginID ;
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
#define RwMemNative32(_m,_s) (_m)
extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type,
                                RwUInt32 *lengthOut, RwUInt32 *versionOut);
extern RpGeometry *
RpGeometryAddRef(RpGeometry *geometry);
extern RpGeometry *
RpGeometryStreamRead(RwStream *stream);
#define RpAtomicGetRenderCallBackMacro(_atomic) ((_atomic)->renderCallBack)
#define RpAtomicGetRenderCallBack(_atomic) RpAtomicGetRenderCallBackMacro(_atomic)
/* ---- end of the generated declarations ---- */

#define RPLODATOMICMAXLOD 10
typedef RwInt32 (*RpLODAtomicLODCallBack)( RpAtomic *atomic );
typedef struct _rpLODAtomicGlobalVars RpLODAtomicGlobalVars;
struct _rpLODAtomicGlobalVars
{
    RwInt32             engineOffset; 
    RwInt32             atmCacheOffset; 
    RwCamera           *atmCamera;
    RwFreeList         *atmCacheFreeList;
};
typedef struct _rpLODAtomicCache RpLODAtomicCache;
struct _rpLODAtomicCache
{
    RwBool              hook;
    RpAtomicCallBackRender atomicRender;
    RpLODAtomicLODCallBack atomicSelectLOD;
    RwReal              farRange;
    RwReal              invFarRange;
    RwInt32             curLOD;
    RpGeometry         *lodGeometry[RPLODATOMICMAXLOD];
};
#define RPLODOFFSET(_ptr,_offset) (*(RpLODAtomicCache **)(((char *)(_ptr)) + _offset))
extern RpLODAtomicGlobalVars RpLODAtomicGlobals;
extern RwInt32 LODAtomicCallBackSelectLOD(RpAtomic * atomic);
extern RpAtomic * LODAtomicCallBackRender(RpAtomic * atomic);
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(struct _rpLODAtomicCache, atomicSelectLOD) == 8 && RW_OFFSETOF(struct _rpLODAtomicCache, curLOD) == 0x14 && RW_OFFSETOF(struct _rpLODAtomicCache, lodGeometry) == 0x18);      /* MFL.exe 0x5682c3..0x5682ec (LODAtomicCallBackCopy, inlined LODAtomicInitCache): farRange / invFarRange +0xc / +0x10, atomicSelectLOD +8, curLOD +0x14, lodGeometry[10] from +0x18 */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(struct _rpLODAtomicGlobalVars, atmCacheFreeList) == 0xc);      /* MFL.exe 0x5682a2: atmCacheFreeList at 0x9488dc; RpLODAtomicGlobals is at 0x9488d0 */
/* TIER2: unvalidated authority=diffemu */
RwStream * LODAtomicCallBackReadChunk(RwStream * stream, RwInt32 __RWUNUSED__ len, RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RwStream           *result;
    RwUInt32            structSize, version;

    result = NULL;

    if (RwStreamFindChunk(stream, rwID_STRUCT, &structSize, &version))
    {
        RwInt32             i;
        RpGeometry         *lodGeometry;
        RpLODAtomicCache   *rpLODAtomicCache;

        result = stream;

        /* LODAtomicNewCache + LODAtomicInitCache, inlined as in MFL.exe */
        rpLODAtomicCache = (RpLODAtomicCache *)
            RwFreeListAlloc(RpLODAtomicGlobals.atmCacheFreeList,
                            rwID_LODATMPLUGIN | rwMEMHINTDUR_EVENT);

        if (rpLODAtomicCache != NULL)
        {
            rpLODAtomicCache->hook = FALSE;
            rpLODAtomicCache->atomicRender = NULL;
            rpLODAtomicCache->atomicSelectLOD = LODAtomicCallBackSelectLOD;

            rpLODAtomicCache->farRange = -1;
            rpLODAtomicCache->invFarRange = -1;

            rpLODAtomicCache->curLOD = -1;

            for (i = 0; i < RPLODATOMICMAXLOD; ++i)
                rpLODAtomicCache->lodGeometry[i] = NULL;
        }

        RwStreamRead(stream, rpLODAtomicCache, structSize);

        (void)RwMemNative32(rpLODAtomicCache, sizeof(RpLODAtomicCache));

        for (i = 0; i < RPLODATOMICMAXLOD; ++i)
        {
            if ((rpLODAtomicCache->lodGeometry[i] != NULL) &&
                (i != rpLODAtomicCache->curLOD))
            {
                if (RwStreamFindChunk(stream, rwID_GEOMETRY, NULL,
                                      &version))
                {

                    lodGeometry = RpGeometryStreamRead(stream);
                    RpGeometryAddRef(lodGeometry);

                    rpLODAtomicCache->lodGeometry[i] = lodGeometry;
                }
            }
        }

        if (rpLODAtomicCache->curLOD >= 0)
        {
            lodGeometry = RpAtomicGetGeometry(atomic);
            RpGeometryAddRef(lodGeometry);

            rpLODAtomicCache->lodGeometry[rpLODAtomicCache->curLOD] =
                lodGeometry;
        }

        rpLODAtomicCache->atomicSelectLOD = LODAtomicCallBackSelectLOD;

        rpLODAtomicCache->hook = FALSE;
        /* LODAtomicHookRender, inlined as in MFL.exe */
        if (rpLODAtomicCache->hook == FALSE)
        {
            rpLODAtomicCache->atomicRender =
                RpAtomicGetRenderCallBack(atomic);

            RpAtomicSetRenderCallBack(atomic, LODAtomicCallBackRender);

            rpLODAtomicCache->hook = TRUE;
        }

        RPLODOFFSET(atomic, offset) = rpLODAtomicCache;
    }
    else
    {
        RPLODOFFSET(atomic, offset) = NULL;
    }

    return (result);
}
