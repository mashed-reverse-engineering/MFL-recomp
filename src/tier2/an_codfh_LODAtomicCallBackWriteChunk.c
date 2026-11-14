/* LODAtomicCallBackWriteChunk (MFL.exe 005681c0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/lodatm/rplodatm.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * chunk version 0x37002 build 0x24 as MFL.exe writes it, LODAtomicCallBackGetChunkSize added from the MFL.exe disassembly, release build
 * macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK
 * includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked;
 * compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLIBRARYCURRENTRWVERSION 3
#define rwLIBRARYCURRENTMAJORREVISION 6
#define rwLIBRARYCURRENTMINORREVISION 0
#define rwLIBRARYCURRENTBINARYREVISION 3
#define rwLIBRARYCURRENTVERSION ((rwLIBRARYCURRENTRWVERSION << 16) | (rwLIBRARYCURRENTMAJORREVISION << 12) | (rwLIBRARYCURRENTMINORREVISION << 8) | (rwLIBRARYCURRENTBINARYREVISION))
#define RWBUILDNUMBER 0xffff
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
extern RwStream *
RwStreamWrite(RwStream * stream,
              const void *buffer,
              RwUInt32 length);
#define RwStreamWriteChunkHeader(stream,type,size) _rwStreamWriteVersionedChunkHeader( stream, type, size, rwLIBRARYCURRENTVERSION, RWBUILDNUMBER)
extern RwStream *_rwStreamWriteVersionedChunkHeader(RwStream *stream,
                                                   RwInt32 type,
                                                   RwInt32 size,
                                                   RwUInt32 version,
                                                   RwUInt32 buildNum);
extern const RpGeometry *
RpGeometryStreamWrite(const RpGeometry *geometry,
                      RwStream *stream);
/* ---- end of the generated declarations ---- */

/* MFL.exe writes the chunk header as RW 3.7.0.2 build 0x24 (0x5681d9 push 0x37002, 0x5681d7 push 0x24); the codfh headers say 3.6.0.3 */
#undef rwLIBRARYCURRENTVERSION
#define rwLIBRARYCURRENTVERSION 0x37002
#undef RWBUILDNUMBER
#define RWBUILDNUMBER 0x24
#define RPLODATOMICMAXLOD 10
typedef RwInt32 (*RpLODAtomicLODCallBack)( RpAtomic *atomic );
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
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwStream * LODAtomicCallBackWriteChunk(RwStream * stream, RwInt32 __RWUNUSED__ len, RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RwInt32             i;
    RpLODAtomicCache   *rpLODAtomicCache;

    rpLODAtomicCache = RPLODOFFSET(atomic, offset);

    if (rpLODAtomicCache != NULL)
    {
        RwStreamWriteChunkHeader(stream, rwID_STRUCT,
                                 sizeof(RpLODAtomicCache));

        RwStreamWrite(stream, rpLODAtomicCache,
                      sizeof(RpLODAtomicCache));

        for (i = 0; i < RPLODATOMICMAXLOD; ++i)
        {
            if ((rpLODAtomicCache->lodGeometry[i] != NULL) &&
                (i != rpLODAtomicCache->curLOD))
                RpGeometryStreamWrite(rpLODAtomicCache->lodGeometry[i],
                                      stream);
        }
    }

    return (stream);
}

/* LODAtomicCallBackGetChunkSize (MFL.exe 00568230), the size callback that goes with the writer above (same source file):
 * written from the MFL.exe disassembly in the same form: the struct chunk (header + cache) plus a geometry chunk per written LOD */
extern RwInt32 RpGeometryStreamGetSize(const RpGeometry *geometry);
#define LODCHUNKHEADERSIZE 12      /* rwCHUNKHEADERSIZE: type, size, version words */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 LODAtomicCallBackGetChunkSize(const RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RwInt32             i;
    RwInt32             chunkSize = 0;
    RpLODAtomicCache   *rpLODAtomicCache;

    rpLODAtomicCache = RPLODOFFSET(atomic, offset);

    if (rpLODAtomicCache != NULL)
    {
        chunkSize = LODCHUNKHEADERSIZE + sizeof(RpLODAtomicCache);

        for (i = 0; i < RPLODATOMICMAXLOD; ++i)
        {
            if ((rpLODAtomicCache->lodGeometry[i] != NULL) &&
                (i != rpLODAtomicCache->curLOD))
                chunkSize += RpGeometryStreamGetSize(rpLODAtomicCache->lodGeometry[i]) + LODCHUNKHEADERSIZE;
        }
    }

    return (chunkSize);
}
