/* PVSSectorReadChunk (MFL.exe 00548f00). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/pvs/rppvs.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
#define rwID_GPVSPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2a)
/* ---- end of the generated declarations ---- */

typedef RwUInt8     RpPVSVisMap;
#define PVSFROMWORLDSECTOR(sector) ((RpPVS *)(((char *)(sector))+rpPVSGlobals.sectorOffset))
enum _rpPVSPartitionId
{
    rpNAPVSPARTITIONID = 0,
    rpPVSFRONT,
    rpPVSBACK,
    rpPVSSPLIT,
    rpPVSCOPLANAR,
    rpPVSPARTITIONIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum _rpPVSPartitionId _rpPVSPartitionId;
typedef struct _rpPVSPolyList _rpPVSPolyList;
typedef struct _rpPVSPolyList *_rpPVSPolyListPtr;
typedef struct RpPVS RpPVS;
struct RpPVS
{
    RwInt32             sectorID;
    RwInt32             vismaplength;
    RwInt32             sampleKey;

    RpPVSVisMap        *vismap;

    _rpPVSPolyListPtr         sectailpoly; 
    _rpPVSPartitionId         potential;

    RwUInt32            numpols;
    RwBBox              sbox;
    RwBBox              gbox;
    RwReal              diagonal;
    RwV3d               centre;
    RwInt32             axessig[3];
};
typedef struct RpPVSGlobalVars RpPVSGlobalVars;
struct RpPVSGlobalVars
{
    RpWorld            *World;

    RwInt32             worldOffset;
    RwInt32             sectorOffset;

    RwBool              collis;
    RwBool              bfc;

    RwInt32             progress_count;
    RwReal              diagonal;

    RwReal              gran;

    RwInt32             InSector;
    RwV3d               ViewPos;
    RpPVS             *CurrPVS;
};
extern RpPVSGlobalVars    rpPVSGlobals;
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpPVSGlobalVars, InSector) == 0x20);      /* an_codfh_rppvs.c */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpPVS, vismaplength) == 4 && RW_OFFSETOF(RpPVS, sampleKey) == 8 && RW_OFFSETOF(RpPVS, vismap) == 0xc);      /* MFL.exe 0x548f1d lea ebx,[esi+0x4]; 0x548f29 lea edx,[esi+0x8]; 0x548f52 mov [esi+0xc],eax (the RwMalloc result) */
/* TIER2: unvalidated authority=diffemu */
RwStream * PVSSectorReadChunk(RwStream * s, RwInt32 len __RWUNUSED__, RpWorldSector * spSect, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpPVS              *pvsCur = PVSFROMWORLDSECTOR(spSect);

    RwStreamReadInt32(s, &pvsCur->sectorID, sizeof(pvsCur->sectorID));
    RwStreamReadInt32(s, &pvsCur->vismaplength,
                      sizeof(pvsCur->vismaplength));
    RwStreamReadInt32(s, &pvsCur->sampleKey, sizeof(pvsCur->sampleKey));

    if (pvsCur->vismaplength != 0) pvsCur->vismap = (RpPVSVisMap *)
        RwMalloc(pvsCur->vismaplength * sizeof(RpPVSVisMap),
                 rwID_GPVSPLUGIN | rwMEMHINTDUR_EVENT);

    if (pvsCur->vismaplength != 0) RwStreamRead(s, pvsCur->vismap,
                 pvsCur->vismaplength * sizeof(RpPVSVisMap));

    return (s);
}
