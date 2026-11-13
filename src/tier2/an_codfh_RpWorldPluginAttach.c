/* RpWorldPluginAttach (MFL.exe 004f5ed0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworld.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                  RwPluginObjectConstructor initCB,
                                  RwPluginObjectDestructor termCB);
#define rwPRIMTYPEOR (rwPRIMTYPELINELIST | rwPRIMTYPEPOLYLINE | rwPRIMTYPETRILIST | rwPRIMTYPETRISTRIP | rwPRIMTYPETRIFAN | rwPRIMTYPEPOINTLIST)
#define rpMESHHEADERPRIMTYPEOR (0 | rpMESHHEADERTRISTRIP | rpMESHHEADERTRIFAN | rpMESHHEADERLINELIST | rpMESHHEADERPOLYLINE | rpMESHHEADERPOINTLIST)
enum RpMeshHeaderFlags
{

    rpMESHHEADERTRISTRIP = 0x0001,
    rpMESHHEADERTRIFAN = 0x0002,
    rpMESHHEADERLINELIST = 0x0004,
    rpMESHHEADERPOLYLINE = 0x0008,
    rpMESHHEADERPOINTLIST = 0x0010,

    rpMESHHEADERPRIMMASK = 0x00FF,
    rpMESHHEADERUNINDEXED = 0x0100,

    rpMESHHEADERFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpMeshHeaderFlags RpMeshHeaderFlags;
typedef struct rpMeshGlobals rpMeshGlobals;
struct rpMeshGlobals
{
    RwInt16 nextSerialNum;
    RwFreeList *triStripListEntryFreeList;
    RwUInt8 meshFlagsToPrimType[rpMESHHEADERPRIMTYPEOR];
    RwUInt8 primTypeToMeshFlags[rwPRIMTYPEOR];
};
extern void *_rpMeshOpen(void *instance, RwInt32 offset,
                                RwInt32 size);
extern void *_rpMeshClose(void *instance, RwInt32 offset,
                                 RwInt32 size);
typedef struct rpGeometryGlobals rpGeometryGlobals;
struct rpGeometryGlobals
{
        RwFreeList *geomFreeList;
};
enum RwCriterionWorldID
{

    rwID_NAWORLDID = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x00),

    rwID_MATERIALMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x01),

    rwID_MESHMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x02),

    rwID_GEOMETRYMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x03),

    rwID_CLUMPMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x04),

    rwID_LIGHTMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x05),

    rwID_COLLISIONMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x06),

    rwID_WORLDMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x07),

    rwID_RANDOMMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x08),

    rwID_WORLDOBJMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x09),

    rwID_SECTORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0A),

    rwID_BINWORLDMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0B),

    rwID_WORLDPIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0D),

    rwID_BINMESHPLUGIN = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0E),

    rwID_RXWORLDDEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0F),

    rwID_NATIVEDATAPLUGIN = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x10),

    rwID_VERTEXFMTPLUGIN = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x11),
    rwCRITERIONWORLDIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionWorldID RwCriterionWorldID;
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(rpMeshGlobals, meshFlagsToPrimType) == 8);      /* an_codfh_bamesh.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(rpMeshGlobals, primTypeToMeshFlags) == 0x27);      /* an_codfh_bamesh.c */
RW_LAYOUT_ASSERT(rw37_row_2, sizeof(rpGeometryGlobals) == 4);      /* MFL.exe 0x4f5f0e (RpWorldPluginAttach): push 0x4 (rwID_GEOMETRYMODULE 0x503) */
/* ---- end of the generated declarations ---- */

typedef struct rpMaterialGlobals rpMaterialGlobals;
struct rpMaterialGlobals
{
    RwFreeList           *matFreeList;
};
extern void *_rpMaterialClose(void *instance, RwInt32 offset, RwInt32 size);
extern void *_rpMaterialOpen(void *instance, RwInt32 offset, RwInt32 size);
extern void *
_rpGeometryClose(void *instance,
                 RwInt32 offset,
                 RwInt32 size);
extern void *
_rpGeometryOpen(void *instance,
                RwInt32 offset,
                RwInt32 size);
extern void *_rpSectorOpen(void *instance, RwInt32 offset, RwInt32 size);
extern void *_rpSectorClose(void *instance, RwInt32 offset, RwInt32 size);
typedef struct rpLightGlobals rpLightGlobals;
struct rpLightGlobals
{
    RwFreeList         *lightFreeList;
};
extern void *_rpLightOpen(void *instance, RwInt32 offset, RwInt32 size);
extern void *_rpLightClose(void *instance, RwInt32 offset, RwInt32 size);
typedef struct rpClumpGlobals rpClumpGlobals;
struct rpClumpGlobals
{
    RwFreeList         *atomicFreeList;
    RwFreeList         *clumpFreeList;
};
extern void *
_rpClumpOpen(void *instance,
             RwInt32 offset,
             RwInt32 size);
extern void *
_rpClumpClose(void *instance,
             RwInt32 offset,
             RwInt32 size);
extern RwBool _rpClumpRegisterExtensions(void);
extern void *_rpBinaryWorldOpen(void *instance,
                                RwInt32 offset,
                                RwInt32 size);
extern void *_rpBinaryWorldClose(void *instance,
                                 RwInt32 offset,
                                 RwInt32 size);
extern RwBool _rxWorldDevicePluginAttach(void);
extern RwBool _rpWorldObjRegisterExtensions(void);
typedef struct rpWorldGlobals rpWorldGlobals;
struct rpWorldGlobals
{
        RwFreeList         *worldListFreeList;
        RwLinkList          worldListHead;
};
extern void * WorldClose( void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldOpen( void *instance, RwInt32 offset, RwInt32 size __RWUNUSED__ );
RW_LAYOUT_ASSERT(rw37_priv_0, RW_OFFSETOF(rpWorldGlobals, worldListHead) == 4);      /* an_codfh_baworld.c */
/* the sizes MFL.exe pushes as the first argument of the RwEngineRegisterPlugin calls */
RW_LAYOUT_ASSERT(rw37_own_0, sizeof(rpMaterialGlobals) == 4);      /* MFL.exe 0x4f5ee0: push 0x4 (rwID_MATERIALMODULE 0x501) */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(rpClumpGlobals) == 8);      /* MFL.exe 0x4f5f26: push 0x8 (rwID_CLUMPMODULE 0x504) */
RW_LAYOUT_ASSERT(rw37_own_2, sizeof(rpLightGlobals) == 4);      /* MFL.exe 0x4f5f43: push 0x4 (rwID_LIGHTMODULE 0x505) */
RW_LAYOUT_ASSERT(rw37_own_3, sizeof(rpWorldGlobals) == 0xc);      /* MFL.exe 0x4f5f71: push 0xc (rwID_WORLDMODULE 0x507) */
/* TIER2: unvalidated authority=diffemu */
RwBool RpWorldPluginAttach(void)
{
    RwInt32             state;

    state = 0;
    state |= RwEngineRegisterPlugin(sizeof(rpMaterialGlobals), rwID_MATERIALMODULE,
                                    _rpMaterialOpen, _rpMaterialClose);
    state |= RwEngineRegisterPlugin(sizeof(rpMeshGlobals), rwID_MESHMODULE,
                                    _rpMeshOpen, _rpMeshClose);
    state |= RwEngineRegisterPlugin(sizeof(rpGeometryGlobals), rwID_GEOMETRYMODULE,
                                    _rpGeometryOpen, _rpGeometryClose);
    state |= RwEngineRegisterPlugin(sizeof(rpClumpGlobals), rwID_CLUMPMODULE,
                                    _rpClumpOpen, _rpClumpClose);
    state |= RwEngineRegisterPlugin(sizeof(rpLightGlobals), rwID_LIGHTMODULE,
                                    _rpLightOpen, _rpLightClose);
    state |= RwEngineRegisterPlugin(0, rwID_SECTORMODULE,
                                    _rpSectorOpen, _rpSectorClose);
    state |= RwEngineRegisterPlugin(sizeof(rpWorldGlobals), rwID_WORLDMODULE,
                                    WorldOpen, WorldClose);
    state |= RwEngineRegisterPlugin(0, rwID_BINWORLDMODULE,
                                    _rpBinaryWorldOpen,
                                    _rpBinaryWorldClose); 

    if (state < 0)
    {
        
        return (FALSE);
    }

    if (!_rpWorldObjRegisterExtensions())
    {
        
        return (FALSE);
    }

    if (!_rpClumpRegisterExtensions())
    {
        
        return (FALSE);
    }

    if (!_rxWorldDevicePluginAttach())
    {
        return (FALSE);
    }

    return (TRUE);
}
