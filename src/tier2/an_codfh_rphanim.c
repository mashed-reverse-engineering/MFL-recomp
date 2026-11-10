/* rphanim callbacks (MFL.exe 0x00543dc0 / 0x00544380): RenderWare Graphics 3.7 (Criterion Software), hierarchical animation plugin:
 * the frame destructor (HAnimDestructor) and RpHAnimHierarchyAttach.  RpHAnimPluginAttach / HAnimOpen stay original (they
 * reference rw_trap rows: the key frame blend / add / mul-recip / stream write callbacks, HAnimWrite, HAnimSize).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RpHAnimHierarchyDestroy inlined into
 * HAnimDestructor and HAnimFramesLinkToHierarchy's body into RpHAnimHierarchyAttach (as in the exe), RwFree / RwFreeListFree through
 * the engine function table (+0x10c / +0x11c), the hierarchy / node fields declared here by the exe's offsets with
 * compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rpHANIMDEFAULTFRAMEID (-1)
#define rpHANIMHIERARCHYSUBHIERARCHY 1
#define rpHANIMHIERARCHYNOMATRICES 2

typedef struct RpHAnimNodeInfo {
    RwInt32 nodeID;
    RwInt32 nodeIndex;
    RwInt32 flags;
    void *pFrame;                           /* RwFrame * */
} RpHAnimNodeInfo;
typedef struct RpHAnimHierarchy {
    RwInt32 flags;
    RwInt32 numNodes;
    void *pMatrixArray;                     /* RwMatrix * */
    void *pMatrixArrayUnaligned;
    RpHAnimNodeInfo *pNodeInfo;
    void *parentFrame;                      /* RwFrame * */
    void *parentHierarchy;
    RwInt32 rootParentOffset;
    void *currentAnim;                      /* RtAnimInterpolator * */
} RpHAnimHierarchy;
typedef struct RpHAnimFrameExtension {
    RwInt32 id;
    RpHAnimHierarchy *hierarchy;
} RpHAnimFrameExtension;
typedef struct RpHAnimAtomicGlobalVars {
    RwInt32 engineOffset;
    void *HAnimFreeList;                    /* RwFreeList * */
} RpHAnimAtomicGlobalVars;
RW_LAYOUT_ASSERT(rw_assert_hanim_node, sizeof(RpHAnimNodeInfo) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_hanim_hierarchy, RW_OFFSETOF(RpHAnimHierarchy, parentFrame) == 0x14 && RW_OFFSETOF(RpHAnimHierarchy, currentAnim) == 0x20);

/*007d2128*/ extern void *RwEngineInstance;
/*00947ac8*/ extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
/*0053e3b0*/ extern RwBool RtAnimInterpolatorDestroy(void *anim);
/*004d0f70*/ extern void *RwFrameUpdateObjects(void *frame);
/*004d1420*/ extern void *RwFrameForAllChildren(void *frame, void *callBack, void *data);
/*005443d0*/ extern void *HAnimFramesLinkToHierarchy(void *rootFrame, void *pData);      /* stays original (self-referencing callback, harness) */
void *HAnimDestructor(void *object, RwInt32 offset, RwInt32 size);

typedef void (*RwFreeListFreeFn)(void *freeList, void *pData);
#define RwFreeListFree(fl, p) (*(RwFreeListFreeFn *)((RwUInt8 *)RwEngineInstance + 0x11c))((fl), (p))      /* memoryFuncs.rwfreelistfree */
#define RPHANIMFRAMEGETDATA(frame) ((RpHAnimFrameExtension *)((RwUInt8 *)(frame) + RpHAnimAtomicGlobals.engineOffset))
#define RpHAnimFrameGetID(frame) (RPHANIMFRAMEGETDATA(frame)->id)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *HAnimDestructor(void *object, RwInt32 offset, RwInt32 size)
{
    RpHAnimFrameExtension *frameExt = RPHANIMFRAMEGETDATA(object);

    if (frameExt->hierarchy)
    {
        RpHAnimHierarchy *pHierarchy = frameExt->hierarchy;
        RwInt32 frameNum;

        /* run through all the frames this hierarchy points at since they also point back at the hierarchy. Set the pointers to
         * NULL and then destroy the hierarchy */
        for (frameNum = 0; frameNum < pHierarchy->numNodes; frameNum++)
        {
            pHierarchy->pNodeInfo[frameNum].pFrame = NULL;
        }

        if (pHierarchy->parentFrame == object)
        {
            /* RpHAnimHierarchyDestroy(pHierarchy) */
            void *parentFrame = pHierarchy->parentFrame;

            if (!(pHierarchy->flags & rpHANIMHIERARCHYSUBHIERARCHY))
            {
                if (!(pHierarchy->flags & rpHANIMHIERARCHYNOMATRICES))
                {
                    RwFree(pHierarchy->pMatrixArrayUnaligned);
                }
                RwFree(pHierarchy->pNodeInfo);
            }
            pHierarchy->pMatrixArrayUnaligned = NULL;
            pHierarchy->pMatrixArray = NULL;
            pHierarchy->pNodeInfo = NULL;

            RtAnimInterpolatorDestroy(pHierarchy->currentAnim);
            RwFreeListFree(RpHAnimAtomicGlobals.HAnimFreeList, pHierarchy);

            if (parentFrame)
            {
                RPHANIMFRAMEGETDATA(parentFrame)->hierarchy = NULL;
            }
        }
        frameExt->hierarchy = NULL;
    }
    frameExt->id = rpHANIMDEFAULTFRAMEID;
    return object;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpHAnimHierarchy *RpHAnimHierarchyAttach(RpHAnimHierarchy *hierarchy)
{
    /* HAnimFramesLinkToHierarchy(hierarchy->parentFrame, hierarchy), inlined as in the exe */
    void *rootFrame = hierarchy->parentFrame;
    int i;

    for (i = 0; i < hierarchy->numNodes; ++i)
    {
        if (RpHAnimFrameGetID(rootFrame) == hierarchy->pNodeInfo[i].nodeID)
        {
            hierarchy->pNodeInfo[i].pFrame = rootFrame;
        }
    }

    RwFrameUpdateObjects(rootFrame);
    RwFrameForAllChildren(rootFrame, (void *)HAnimFramesLinkToHierarchy, hierarchy);
    return hierarchy;
}
