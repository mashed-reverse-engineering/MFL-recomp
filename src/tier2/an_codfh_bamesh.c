/* bamesh module open (MFL.exe 0x00500bf0): RenderWare Graphics 3.7 (Criterion Software), mesh module open (rpMeshOpen = codfh
 * _rpMeshOpen): the build-mesh free list and the primitive type <-> mesh header flag tables.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamesh.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), MeshFreeListsCreate / MeshFreeListsDestroy
 * inlined (as in the exe: the destroy of a NULL free list vanishes), RPMESHGLOBAL expanded to the byte offset into
 * *RwEngineInstance, sizeof(RpBuildMesh) written as its exe value 0xc, the globals declared here with compile-time layout
 * asserts.  Module-level only: no mesh object is touched.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rpBUILDMESHSIZE 0xc                 /* sizeof(RpBuildMesh) */
#define rpMESH_HINT 0x40502                 /* rwMEMHINTDUR_GLOBAL | rwID_MESHMODULE */
#define rpMESHHEADERTRISTRIP 0x0001
#define rpMESHHEADERTRIFAN 0x0002
#define rpMESHHEADERLINELIST 0x0004
#define rpMESHHEADERPOLYLINE 0x0008
#define rpMESHHEADERPOINTLIST 0x0010
#define rpMESHHEADERPRIMTYPEOR 0x1f
#define rwPRIMTYPELINELIST 1
#define rwPRIMTYPEPOLYLINE 2
#define rwPRIMTYPETRILIST 3
#define rwPRIMTYPETRISTRIP 4
#define rwPRIMTYPETRIFAN 5
#define rwPRIMTYPEPOINTLIST 6
#define rwPRIMTYPEOR 7

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rpMeshGlobals {
    short nextSerialNum;
    void *triStripListEntryFreeList;        /* RwFreeList * */
    RwUInt8 meshFlagsToPrimType[rpMESHHEADERPRIMTYPEOR];
    RwUInt8 primTypeToMeshFlags[rwPRIMTYPEOR];
} rpMeshGlobals;
typedef struct rpMeshStatic {
    void *BuildMeshFreeList;                /* RwFreeList * */
} rpMeshStatic;
RW_LAYOUT_ASSERT(rw_assert_mesh_flags, RW_OFFSETOF(rpMeshGlobals, meshFlagsToPrimType) == 8 && RW_OFFSETOF(rpMeshGlobals, primTypeToMeshFlags) == 0x27);

/*007d2128*/ extern void *RwEngineInstance;
/*00947778*/ extern RwModuleInfo meshModule;
/*007d53c0*/ extern rpMeshStatic MeshStatic;
/*004e90b0*/ extern void *RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint);
/*004e92b0*/ extern RwBool RwFreeListDestroy(void *freeList);
#define RPMESHGLOBAL(var) (((rpMeshGlobals *)((RwUInt8 *)RwEngineInstance + meshModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rpMeshOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Cache the global data offset (same for all instances) */
    meshModule.globalsOffset = offset;

    if (0 == meshModule.numInstances)
    {
        /* MeshFreeListsCreate() */
        MeshStatic.BuildMeshFreeList = RwFreeListCreate(rpBUILDMESHSIZE, 50, sizeof(RwUInt32), rpMESH_HINT);
        if (NULL == MeshStatic.BuildMeshFreeList)
        {
            /* MeshFreeListsDestroy() */
            if (NULL != MeshStatic.BuildMeshFreeList)
            {
                RwFreeListDestroy(MeshStatic.BuildMeshFreeList);
                MeshStatic.BuildMeshFreeList = NULL;
            }
            return NULL;
        }
    }

    /* Do the setup for this instance */
    RPMESHGLOBAL(nextSerialNum) = 1;

    /* One more module instance */
    ++meshModule.numInstances;

    /* Set up LUTs for: RwPrimitiveType <--> rpMESHHEADER[primtype] */
    RPMESHGLOBAL(meshFlagsToPrimType)[0 /*rpMESHHEADERTRILIST */ ] = rwPRIMTYPETRILIST;
    RPMESHGLOBAL(meshFlagsToPrimType)[rpMESHHEADERTRISTRIP] = rwPRIMTYPETRISTRIP;
    RPMESHGLOBAL(meshFlagsToPrimType)[rpMESHHEADERTRIFAN] = rwPRIMTYPETRIFAN;
    RPMESHGLOBAL(meshFlagsToPrimType)[rpMESHHEADERLINELIST] = rwPRIMTYPELINELIST;
    RPMESHGLOBAL(meshFlagsToPrimType)[rpMESHHEADERPOLYLINE] = rwPRIMTYPEPOLYLINE;
    RPMESHGLOBAL(meshFlagsToPrimType)[rpMESHHEADERPOINTLIST] = rwPRIMTYPEPOINTLIST;
    RPMESHGLOBAL(primTypeToMeshFlags)[rwPRIMTYPELINELIST] = rpMESHHEADERLINELIST;
    RPMESHGLOBAL(primTypeToMeshFlags)[rwPRIMTYPEPOLYLINE] = rpMESHHEADERPOLYLINE;
    RPMESHGLOBAL(primTypeToMeshFlags)[rwPRIMTYPETRILIST] = 0 /*rpMESHHEADERTRILIST */;
    RPMESHGLOBAL(primTypeToMeshFlags)[rwPRIMTYPETRISTRIP] = rpMESHHEADERTRISTRIP;
    RPMESHGLOBAL(primTypeToMeshFlags)[rwPRIMTYPETRIFAN] = rpMESHHEADERTRIFAN;
    RPMESHGLOBAL(primTypeToMeshFlags)[rwPRIMTYPEPOINTLIST] = rpMESHHEADERPOINTLIST;
    return instance;
}
