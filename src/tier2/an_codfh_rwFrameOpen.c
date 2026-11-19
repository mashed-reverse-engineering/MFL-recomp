/* rwFrameOpen (MFL.exe 004d08d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/baframe.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the static free list is the
 * MFL.exe block at 0x7d1fa8, rwFrameClose added from the MFL.exe disassembly, release build macros expanded (RWFUNCTION / RWASSERT dropped,
 * RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and
 * functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation
 * (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLinkListInitialize(list) ( (list)->link.next = ((RwLLLink *)(list)), (list)->link.prev = ((RwLLLink *)(list)) )
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
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
struct RwPluginRegistry
{
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);      /* an_codfh__rwPluginRegistryAddPlugin.c */
/* ---- end of the generated declarations ---- */

typedef struct rwFrameGlobals rwFrameGlobals;
struct rwFrameGlobals
{
    RwFreeList         *frames;
};
extern RwPluginRegistry frameTKList;
#define RWFRAMEGLOBAL(var) (RWPLUGINOFFSET(rwFrameGlobals, RwEngineInstance, frameModule.globalsOffset)->var)
extern RwPluginRegistry    frameTKList;
extern RwModuleInfo frameModule;
extern RwInt32 _rwFrameFreeListBlockSize;
extern RwInt32 _rwFrameFreeListPreallocBlocks;
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void * rwFrameOpen(void *instance, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    /* the function static is the MFL.exe word block at 0x7d1fa8 */
#define frameFreeList g_data_7d1fa8
    extern RwFreeList frameFreeList;

    frameModule.globalsOffset = offset;

    RWFRAMEGLOBAL(frames) = RwFreeListCreateAndPreallocateSpace(frameTKList.sizeOfStruct,
        _rwFrameFreeListBlockSize, rwFRAMEALIGNMENT, _rwFrameFreeListPreallocBlocks,
        &frameFreeList, rwMEMHINTDUR_GLOBAL | rwID_FRAMELIST);

    if (!RWFRAMEGLOBAL(frames))
    {
        
        return (NULL);
    }

    rwLinkListInitialize(&RWSRCGLOBAL(dirtyFrameList));

    ++frameModule.numInstances;

    return (instance);
#undef frameFreeList
}

/* rwFrameClose (MFL.exe 004d0950), the module close next to it: written from the MFL.exe disassembly in the same form */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void * rwFrameClose(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
    if (RWFRAMEGLOBAL(frames))
    {
        RwFreeListDestroy(RWFRAMEGLOBAL(frames));
        RWFRAMEGLOBAL(frames) = (RwFreeList *)NULL;
    }

    --frameModule.numInstances;

    return (instance);
}
