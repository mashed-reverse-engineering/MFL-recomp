/* ReallocAndFixupSuperBlock (MFL.exe 004e6a80). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwCriterionCoreID
{

    rwID_NACOREID = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x00),

    rwID_VECTORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x01),

    rwID_MATRIXMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x02),

    rwID_FRAMEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x03),

    rwID_STREAMMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x04),

    rwID_CAMERAMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x05),

    rwID_IMAGEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x06),

    rwID_RASTERMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x07),

    rwID_TEXTUREMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x08),

    rwID_PIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x09),

    rwID_IMMEDIATEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0A),

    rwID_RESOURCESMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0B),

    rwID_DEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0C),

    rwID_COLORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0D),

    rwID_POLYPIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0E),

    rwID_ERRORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0F),

    rwID_METRICSMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x10),

    rwID_DRIVERMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x11),

    rwID_CHUNKGROUPMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x12),
    rwCRITERIONCOREIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionCoreID RwCriterionCoreID;
#define RwRealloc(_p,_s,_h) ((RWSRCGLOBAL(memoryFuncs).rwrealloc)((_p),(_s), (_h)))
enum RwMemoryHintFlag
{

    rwMEMHINTFLAG_RESIZABLE = 0x01000000,
    rwMEMHINTFLAG_MASK = 0xFF000000,
    rwMEMHINTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintFlag RwMemoryHintFlag;
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID rwID_COREPLUGIN
#define _PTRINC(_ptr,_type,_numbytes) (((_type *)NULL == (_ptr))? ((_type *)NULL): ((_ptr) = (_type *)(((RwUInt8 *)_ptr) + (_numbytes))) )
/* TIER2: unvalidated authority=diffemu */
RwBool ReallocAndFixupSuperBlock(RxPipeline *pipeline, RwUInt32 newSize)
{
    void    *oldBlock, *newBlock;
    RwUInt32 i;

    oldBlock = pipeline->superBlock;
    newBlock = RwRealloc(oldBlock, newSize,
        rwMEMHINTDUR_GLOBAL | rwMEMHINTFLAG_RESIZABLE | rwID_PIPEMODULE);
    if (NULL != newBlock)
    {
        const RwUInt32 numNodes = pipeline->numNodes;
        RwInt32 diff;

        diff = ((RwUInt8 *)newBlock) - ((RwUInt8 *)oldBlock);

        pipeline->superBlock = newBlock;
        pipeline->superBlockSize = newSize;
        pipeline->nodes = (RxPipelineNode *)pipeline->superBlock;
        _PTRINC(pipeline->embeddedPacket, RxPacket, diff);
        _PTRINC(pipeline->inputRequirements,
                RxPipelineRequiresCluster, diff);

        for (i = 0; i < numNodes; ++i)
        {
            _PTRINC(pipeline->nodes[i].outputs, RwUInt32, diff);
            _PTRINC(pipeline->nodes[i].slotClusterRefs,
                    RxPipelineCluster *, diff);
            _PTRINC(pipeline->nodes[i].slotsContinue, RwUInt32, diff);
            _PTRINC(pipeline->nodes[i].privateData, void, diff);
            _PTRINC(pipeline->nodes[i].inputToClusterSlot, RwUInt32, diff);
            _PTRINC(pipeline->nodes[i].topSortData,
                    RxPipelineNodeTopSortData, diff);
        }
    }
    else
    {
        RWERROR((E_RW_NOMEM, newSize));
        return (FALSE);
    }

    return (TRUE);
}
