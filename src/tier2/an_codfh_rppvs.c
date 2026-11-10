/* rppvs plugin callbacks (MFL.exe 0x00548c10 - 0x00548e7e): RenderWare Graphics 3.7 (Criterion Software), potentially visible set
 * plugin: RpPVSPluginAttach, the world constructor / destructor, the sector destructor and the world / sector stream writers
 * (PVSWorldWriteChunk / PVSSectorWriteChunk: rw_trap rows, ported so the attach does not reference int3 code).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/pvs/rppvs.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), PVSInitializeSectors / PVSFindDiagonalSizeOfBBox
 * inlined into PVSWorldConstructor and RpPVSUnhook / PVSDeinitialize into PVSWorldDestructor (as in the exe), RwFree through
 * the engine function table (+0x10c), the cache / sector / world fields declared here by the exe's offsets with compile-time
 * layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define TRUE 1
#define FALSE 0
#define rwID_GPVSPLUGIN 0x12a
#define rpPVSSIZE 0x68                      /* sizeof(RpPVS) */

typedef struct RpWorldHead {                /* RpWorld: only the root sector and the bounding box are used here */
    RwUInt8 pad[0x1c];
    void *rootSector;
    RwUInt8 pad2[0x50 - 0x20];
    RwBBox boundingBox;
} RpWorldHead;
typedef struct rpPVSPolyListHead {          /* _rpPVSPolyList: the polygon geometry and the link */
    RwUInt8 pad[0xc];
    RwUInt8 geom[0x58 - 0xc];               /* data.geom */
    struct rpPVSPolyListHead *next;
} rpPVSPolyListHead;
typedef struct RpPVSCache {
    RwBool processed;
    RwBool formatted;
    RwInt32 NumWorldSectors;
    RwInt32 ptotal;
    RwInt32 paccept;
    RwBool hooked;
    RwUInt32 nextID;
    RwInt32 viscount;
    void *progressCallBack;
    rpPVSPolyListHead *polygons;
    void *renderCallBack;
} RpPVSCache;
typedef struct RpPVSHead {                  /* RpPVS: the fields the destructor touches */
    RwInt32 sectorID;
    RwInt32 vismaplength;
    RwInt32 sampleKey;
    void *vismap;
} RpPVSHead;
typedef struct RpPVSGlobalVars {
    RpWorldHead *World;
    RwInt32 worldOffset;
    RwInt32 sectorOffset;
    RwBool collis;
    RwBool bfc;
    RwInt32 progress_count;
    RwReal diagonal;
    RwReal gran;
    RwInt32 InSector;
} RpPVSGlobalVars;
RW_LAYOUT_ASSERT(rw_assert_pvs_world, RW_OFFSETOF(RpWorldHead, rootSector) == 0x1c && RW_OFFSETOF(RpWorldHead, boundingBox) == 0x50);
RW_LAYOUT_ASSERT(rw_assert_pvs_poly, RW_OFFSETOF(rpPVSPolyListHead, next) == 0x58);
RW_LAYOUT_ASSERT(rw_assert_pvs_cache, sizeof(RpPVSCache) == 0x2c);
RW_LAYOUT_ASSERT(rw_assert_pvs_globals, RW_OFFSETOF(RpPVSGlobalVars, InSector) == 0x20);

/*007d2128*/ extern void *RwEngineInstance;
/*00947ae0*/ extern RpPVSGlobalVars rpPVSGlobals;
/*004f5e70*/ extern RwInt32 RpWorldRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f5ea0*/ extern RwInt32 RpWorldRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*00500b40*/ extern RwInt32 RpWorldSectorRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*00500b70*/ extern RwInt32 RpWorldSectorRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*005491e0*/ extern RwInt32 _rpPVSWorldGetNumWorldSectors(RpWorldHead *world);
/*004d53a0*/ extern RwReal RwV3dLength(const RwV3d *in);
/*00548920*/ extern void *PVSCalculateSectorBounds(void *spSector, const RwBBox *box);
/*004f5e10*/ extern void *RpWorldForAllWorldSectors(RpWorldHead *world, void *fpCallBack, void *pData);
/*004f5940*/ extern void *RpWorldSetSectorRenderCallBack(RpWorldHead *world, void *fpCallBack);
/*005491b0*/ extern void _rpPVSDestroyPoly(void *geom);
/*00549180*/ extern void _rpPVSDestroyPolyList(rpPVSPolyListHead **list);
/*00548a70*/ extern void *PVSCopyAndTagSectors(void *sector, void *data);
/*00548e10*/ extern void *PVSSectorConstructor(void *sector, RwInt32 offset, RwInt32 size);
/*00548fd0*/ extern RwStream *PVSWorldReadChunk(RwStream *stream, RwInt32 len, void *world, RwInt32 offset, RwInt32 size);
/*004dc8b0*/ extern RwStream *RwStreamWriteInt32(RwStream *stream, const void *ints, RwUInt32 numBytes);
/*004dbfc0*/ extern RwStream *RwStreamWrite(RwStream *stream, const void *buffer, RwUInt32 length);
/*00548f70*/ extern RwInt32 PVSWorldGetChunkSize(const void *world, RwInt32 offset, RwInt32 size);
/*00548f00*/ extern RwStream *PVSSectorReadChunk(RwStream *stream, RwInt32 len, void *sector, RwInt32 offset, RwInt32 size);
/*00548e80*/ extern RwInt32 PVSSectorGetChunkSize(const void *sector, RwInt32 offset, RwInt32 size);
void *PVSWorldConstructor(RpWorldHead *wpWorld, RwInt32 offset, RwInt32 size);
void *PVSWorldDestructor(RpWorldHead *wpWorld, RwInt32 offset, RwInt32 size);
void *PVSSectorDestructor(void *sector, RwInt32 offset, RwInt32 size);
RwStream *PVSSectorWriteChunk(RwStream *s, RwInt32 len, const void *spSect, RwInt32 offset, RwInt32 size);
RwStream *PVSWorldWriteChunk(RwStream *s, RwInt32 len, RpWorldHead *wpWorld, RwInt32 offset, RwInt32 size);

#define PVSCACHEFROMWORLD(world) ((RpPVSCache *)((RwUInt8 *)(world) + rpPVSGlobals.worldOffset))
#define PVSFROMWORLDSECTOR(sector) ((RpPVSHead *)((RwUInt8 *)(sector) + rpPVSGlobals.sectorOffset))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *PVSWorldConstructor(RpWorldHead *wpWorld, RwInt32 offset, RwInt32 size)
{
    RpPVSCache *pvsCache = PVSCACHEFROMWORLD(wpWorld);
    RpPVSCache *PVSCache;
    RwV3d diag;

    /* pipeline hooking */
    pvsCache->hooked = FALSE;
    /* used during vismap allocation */
    pvsCache->nextID = 0;

    pvsCache->progressCallBack = NULL;

    pvsCache->ptotal = 0;
    pvsCache->paccept = 0;

    pvsCache->formatted = FALSE;
    pvsCache->processed = FALSE;

    pvsCache->viscount = 0;
    pvsCache->polygons = NULL;

    pvsCache->renderCallBack = NULL;

    rpPVSGlobals.World = wpWorld;
    rpPVSGlobals.collis = TRUE;
    rpPVSGlobals.bfc = TRUE;

    rpPVSGlobals.InSector = -1;

    /* PVSInitializeSectors() */
    PVSCache = PVSCACHEFROMWORLD(rpPVSGlobals.World);
    PVSCache->NumWorldSectors = _rpPVSWorldGetNumWorldSectors(rpPVSGlobals.World);

    /* PVSFindDiagonalSizeOfBBox(&rpPVSGlobals.World->boundingBox) */
    diag.x = rpPVSGlobals.World->boundingBox.sup.x - rpPVSGlobals.World->boundingBox.inf.x;
    diag.y = rpPVSGlobals.World->boundingBox.sup.y - rpPVSGlobals.World->boundingBox.inf.y;
    diag.z = rpPVSGlobals.World->boundingBox.sup.z - rpPVSGlobals.World->boundingBox.inf.z;
    rpPVSGlobals.diagonal = RwV3dLength(&diag);

    PVSCalculateSectorBounds(rpPVSGlobals.World->rootSector, &rpPVSGlobals.World->boundingBox);

    /* Create and classify the data from each sector */
    RpWorldForAllWorldSectors(rpPVSGlobals.World, (void *)PVSCopyAndTagSectors, NULL);
    return wpWorld;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *PVSWorldDestructor(RpWorldHead *wpWorld, RwInt32 offset, RwInt32 size)
{
    RpPVSCache *pvsCache = PVSCACHEFROMWORLD(wpWorld);
    rpPVSPolyListHead *temp;

    /* RpPVSUnhook(wpWorld) */
    if (pvsCache->hooked)
    {
        pvsCache->hooked = FALSE;

        /* Swap the render callback. */
        RpWorldSetSectorRenderCallBack(wpWorld, pvsCache->renderCallBack);
        pvsCache->renderCallBack = NULL;
    }

    /* PVSDeinitialize(wpWorld) */
    pvsCache = PVSCACHEFROMWORLD(wpWorld);
    temp = pvsCache->polygons;
    if (pvsCache->formatted)
    {
        while (temp != NULL)
        {
            _rpPVSDestroyPoly(temp->geom);
            temp = temp->next;
        }
        _rpPVSDestroyPolyList(&pvsCache->polygons);
        pvsCache->formatted = FALSE;
    }

    rpPVSGlobals.World = NULL;
    return wpWorld;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *PVSSectorDestructor(void *sector, RwInt32 offset, RwInt32 size)
{
    RpPVSHead *pvsCur = PVSFROMWORLDSECTOR(sector);

    pvsCur->sectorID = ~0;
    pvsCur->sampleKey = 0;

    if (pvsCur->vismap != NULL)
    {
        RwFree(pvsCur->vismap);
        pvsCur->vismap = NULL;
    }
    return sector;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwStream *PVSSectorWriteChunk(RwStream *s, RwInt32 len, const void *spSect, RwInt32 offset, RwInt32 size)
{
    const RpPVSHead *pvsCur = PVSFROMWORLDSECTOR(spSect);

    /* sector's data.... */
    RwStreamWriteInt32(s, &pvsCur->sectorID, sizeof(pvsCur->sectorID));
    RwStreamWriteInt32(s, &pvsCur->vismaplength, sizeof(pvsCur->vismaplength));
    RwStreamWriteInt32(s, &pvsCur->sampleKey, sizeof(pvsCur->sampleKey));

    /* and the vismap.... (RpPVSVisMap is one byte) */
    if (pvsCur->vismaplength != 0) RwStreamWrite(s, pvsCur->vismap, pvsCur->vismaplength);
    return s;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwStream *PVSWorldWriteChunk(RwStream *s, RwInt32 len, RpWorldHead *wpWorld, RwInt32 offset, RwInt32 size)
{
    const RpPVSCache *pvsCache = PVSCACHEFROMWORLD(wpWorld);

    /* set current world */
    rpPVSGlobals.World = wpWorld;

    RwStreamWriteInt32(s, &pvsCache->processed, sizeof(pvsCache->processed));
    RwStreamWriteInt32(s, &pvsCache->hooked, sizeof(pvsCache->hooked));
    return s;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool RpPVSPluginAttach(void)
{
    /* Note the plugin id is should be rwID_GPVSPLUGIN to avoid problems with the retired PVS plugin */
    RwInt32 offset;

    rpPVSGlobals.World = NULL;

    rpPVSGlobals.worldOffset = RpWorldRegisterPlugin(sizeof(RpPVSCache), rwID_GPVSPLUGIN, (void *)PVSWorldConstructor,
                                                     (void *)PVSWorldDestructor, NULL);
    if (rpPVSGlobals.worldOffset < 0)
    {
        return FALSE;
    }

    offset = RpWorldRegisterPluginStream(rwID_GPVSPLUGIN, (void *)PVSWorldReadChunk, (void *)PVSWorldWriteChunk, (void *)PVSWorldGetChunkSize);
    if (offset < 0)
    {
        return FALSE;
    }

    rpPVSGlobals.sectorOffset = RpWorldSectorRegisterPlugin(rpPVSSIZE, rwID_GPVSPLUGIN, (void *)PVSSectorConstructor,
                                                            (void *)PVSSectorDestructor, NULL);
    if (rpPVSGlobals.sectorOffset < 0)
    {
        return FALSE;
    }

    offset = RpWorldSectorRegisterPluginStream(rwID_GPVSPLUGIN, (void *)PVSSectorReadChunk, (void *)PVSSectorWriteChunk,
                                               (void *)PVSSectorGetChunkSize);
    if (offset < 0)
    {
        return FALSE;
    }
    return TRUE;
}
