/* baframe destroy / clone (MFL.exe 0x004d0990 / 0x004d0a30 / 0x004d0b80 / 0x004d0d40 / 0x004d1020): RenderWare Graphics 3.7
 * (Criterion Software): _rwFrameCloneAndLinkClones, rwFrameCloneRecurse, _rwFramePurgeClone, RwFrameDestroy and
 * RwFrameAddChildNoUpdate.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/baframe.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), no CS_FRAME_LOCK (codfh threading
 * patch, not in the exe); as the exe has them inlined: _rwFrameInternalDeInit into RwFrameDestroy, RwFrameRemoveChild into
 * RwFrameDestroy and RwFrameAddChildNoUpdate, RwFrameUpdateObjects into _rwFrameCloneAndLinkClones, the first level of
 * rwSetHierarchyRoot everywhere (the deeper levels call the original) and the first level of RwFrameDestroyHierarchy into
 * rwFrameCloneRecurse's failure path; rwFrameCloneRecurse is not static (the exe keeps its symbol); RwFreeListAlloc / RwFreeListFree
 * as the engine's freeListAlloc / freeListFree slots (0x118 / 0x11c), RWFRAMEGLOBAL(frames) as the word at frameModule.globalsOffset
 * in *RwEngineInstance, RWSRCGLOBAL(dirtyFrameList) as *RwEngineInstance + 0xbc; the rwLinkList macros expanded; structs declared
 * here (codfh baframe.h / balist.h, without the 16-byte alignment attribute) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwLLLink {
    struct RwLLLink *next;
    struct RwLLLink *prev;
} RwLLLink;
typedef struct RwLinkList {
    RwLLLink link;
} RwLinkList;
struct RwObject {      /* typedef names RwObject / RwFrame / RwMatrix come from mfl_tier2.h */
    RwUInt8 type;
    RwUInt8 subType;
    RwUInt8 flags;
    RwUInt8 privateFlags;
    void *parent;
};
struct RwFrame {
    RwObject object;                    /* 0x00 */
    RwLLLink inDirtyListLink;           /* 0x08 */
    RwMatrix modelling;                 /* 0x10 */
    RwMatrix ltm;                       /* 0x50 */
    RwLinkList objectList;              /* 0x90 */
    struct RwFrame *child;              /* 0x98 */
    struct RwFrame *next;               /* 0x9c */
    struct RwFrame *root;               /* 0xa0 */
};
RW_LAYOUT_ASSERT(rw_assert_private_flags, RW_OFFSETOF(RwObject, privateFlags) == 3);
RW_LAYOUT_ASSERT(rw_assert_frame_dirty, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);
RW_LAYOUT_ASSERT(rw_assert_frame_modelling, RW_OFFSETOF(RwFrame, modelling) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_frame_objlist, RW_OFFSETOF(RwFrame, objectList) == 0x90);
RW_LAYOUT_ASSERT(rw_assert_frame_child, RW_OFFSETOF(RwFrame, child) == 0x98);
RW_LAYOUT_ASSERT(rw_assert_frame_next, RW_OFFSETOF(RwFrame, next) == 0x9c);
RW_LAYOUT_ASSERT(rw_assert_frame_root, RW_OFFSETOF(RwFrame, root) == 0xa0);

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;

#define rwFRAMEPRIVATEHIERARCHYSYNCLTM 0x01
#define rwFRAMEPRIVATEHIERARCHYSYNCOBJ 0x02
#define rwFRAMEPRIVATESUBTREESYNCLTM 0x04
#define rwFRAMEPRIVATESUBTREESYNCOBJ 0x08
#define rwID_FRAMELIST 0x0e
#define rwMEMHINTDUR_EVENT 0x00030000
#define RW_DIRTY_FRAME_LIST 0xbc      /* RwGlobals.dirtyFrameList (rw37.h RwGlobals37) */

typedef void *(*RwFreeListAllocFunc)(void *freeList, RwUInt32 hint);
typedef void *(*RwFreeListFreeFunc)(void *freeList, void *entry);

/*007d2128*/ extern void *RwEngineInstance;
/*007d1fa0*/ extern RwModuleInfo frameModule;
/*0062e514*/ extern RwUInt8 frameTKList[];

#define RW_ENGINE_SLOT(off, type) (*(type *)((RwUInt8 *)RwEngineInstance + (off)))
#define RwFreeListAlloc RW_ENGINE_SLOT(0x118, RwFreeListAllocFunc)
#define RwFreeListFree RW_ENGINE_SLOT(0x11c, RwFreeListFreeFunc)
#define RWFRAMEGLOBAL_frames RW_ENGINE_SLOT(frameModule.globalsOffset, void *)
#define RWSRC_DIRTYLIST ((RwLinkList *)((RwUInt8 *)RwEngineInstance + RW_DIRTY_FRAME_LIST))

/*004d0bf0*/ extern void rwSetHierarchyRoot(RwFrame *frame, RwFrame *root);
/*004d0e90*/ extern void rwFrameDestroyRecurse(RwFrame *frame);
/*004e9ce0*/ extern void *_rwPluginRegistryInitObject(void *reg, void *object);
/*004e9d40*/ extern void *_rwPluginRegistryDeInitObject(void *reg, void *object);
/*004e9d70*/ extern void *_rwPluginRegistryCopyObject(void *reg, void *dstObject, const void *srcObject);

/* rwSetHierarchyRoot(f, r), first level; the deeper levels call the original */
#define FRAME_SET_HIERARCHY_ROOT(f, r)                                              \
    {                                                                               \
        RwFrame *sub_;                                                              \
        (f)->root = (r);                                                            \
        for (sub_ = (f)->child; sub_; sub_ = sub_->next)                            \
        {                                                                           \
            rwSetHierarchyRoot(sub_, (r));                                          \
        }                                                                           \
    }

/* RwFrameUpdateObjects(f): mark the hierarchy and this frame dirty */
#define FRAME_UPDATE_OBJECTS(f)                                                     \
    {                                                                               \
        RwUInt32 oldFlags_ = (f)->root->object.privateFlags;                        \
        if (!(oldFlags_ & (rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ))) \
        {                                                                           \
            /* Not in the dirty list - add it */                                    \
            RwLLLink *link_ = &(f)->root->inDirtyListLink;                          \
            link_->next = RWSRC_DIRTYLIST->link.next;                               \
            link_->prev = &RWSRC_DIRTYLIST->link;                                   \
            RWSRC_DIRTYLIST->link.next->prev = link_;                               \
            RWSRC_DIRTYLIST->link.next = link_;                                     \
        }                                                                           \
        /* whole hierarchy needs resync */                                          \
        (f)->root->object.privateFlags = (RwUInt8)(oldFlags_ | (rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ)); \
        /* this frame in particular */                                              \
        (f)->object.privateFlags |= (rwFRAMEPRIVATESUBTREESYNCLTM | rwFRAMEPRIVATESUBTREESYNCOBJ); \
    }

/* RwFrameRemoveChild body */
#define FRAME_REMOVE_CHILD(c_)                                                      \
    {                                                                               \
        RwFrame *curFrame_ = ((RwFrame *)(c_)->object.parent)->child;               \
        /* Check if its the first child */                                         \
        if (curFrame_ == (c_))                                                      \
        {                                                                           \
            ((RwFrame *)(c_)->object.parent)->child = (c_)->next;                   \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            while (curFrame_->next != (c_))                                         \
            {                                                                       \
                curFrame_ = curFrame_->next;                                        \
            }                                                                       \
            /* Remove it from the sibling list */                                   \
            curFrame_->next = (c_)->next;                                           \
        }                                                                           \
        /* Now its a root it also has no siblings */                                \
        (c_)->object.parent = NULL;                                                 \
        (c_)->next = (RwFrame *)NULL;                                               \
        FRAME_SET_HIERARCHY_ROOT(c_, c_);                                           \
        /* Make it dirty */                                                         \
        FRAME_UPDATE_OBJECTS(c_);                                                   \
    }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFrame *
rwFrameCloneRecurse(RwFrame *oldFrame, RwFrame *newRoot)
{
    RwFrame *newFrame;
    RwFrame *child;

    newFrame = (RwFrame *)RwFreeListAlloc(RWFRAMEGLOBAL_frames, rwMEMHINTDUR_EVENT | rwID_FRAMELIST);
    if (!newFrame)
    {
        /* Failed to create it */
        return (RwFrame *)NULL;
    }

    if (!newRoot)
    {
        /* Not got a root yet, we have now */
        newRoot = newFrame;
    }

    /* rwObjectCopy: this initialises the type also */
    newFrame->object.type = oldFrame->object.type;
    newFrame->object.subType = oldFrame->object.subType;
    newFrame->object.flags = oldFrame->object.flags;
    newFrame->object.privateFlags = oldFrame->object.privateFlags;

    /* Nothing's attached */
    newFrame->objectList.link.prev = &newFrame->objectList.link;
    newFrame->objectList.link.next = &newFrame->objectList.link;
    newFrame->object.parent = NULL;

    /* Copy and mark as dirty */
    newFrame->modelling = oldFrame->modelling;

    newFrame->child = (RwFrame *)NULL;
    newFrame->next = (RwFrame *)NULL;

    /* Point the root to the newly created frame */
    newFrame->root = newRoot;

    /* Point the source frame to the new cloned frame (necessary for clump hierarchy cloning) */
    oldFrame->root = newFrame;

    /* Recurse over the children */
    for (child = oldFrame->child; child; child = child->next)
    {
        RwFrame *newChild = rwFrameCloneRecurse(child, newRoot);

        if (!newChild)
        {
            /* Destroy all children and root created here: RwFrameDestroyHierarchy(newFrame), first level */
            RwFrame *sub = newFrame->child;

            while (sub)
            {
                RwFrame *nextSub = sub->next;

                rwFrameDestroyRecurse(sub);
                sub = nextSub;
            }

            _rwPluginRegistryDeInitObject(frameTKList, newFrame);

            if (newFrame->object.privateFlags & (rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ))
            {
                newFrame->inDirtyListLink.prev->next = newFrame->inDirtyListLink.next;
                newFrame->inDirtyListLink.next->prev = newFrame->inDirtyListLink.prev;
            }

            RwFreeListFree(RWFRAMEGLOBAL_frames, newFrame);

            return (RwFrame *)NULL;
        }

        /* Insert on front of frame sibling list */
        newChild->next = newFrame->child;
        newFrame->child = newChild;
        newChild->object.parent = newFrame;
    }

    /* Call constructors */
    _rwPluginRegistryInitObject(frameTKList, newFrame);

    /* Copy object data */
    _rwPluginRegistryCopyObject(frameTKList, newFrame, oldFrame);

    return newFrame;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFrame *
_rwFrameCloneAndLinkClones(RwFrame *root)
{
    RwFrame *newFrame;

    /* Duplicate and set the clone's root to be the new root */
    newFrame = rwFrameCloneRecurse(root, (RwFrame *)NULL);

    if (newFrame)
    {
        /* clear flag */
        newFrame->object.privateFlags &= ~(rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ);
        FRAME_UPDATE_OBJECTS(newFrame);
    }

    return newFrame;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFrame *
_rwFramePurgeClone(RwFrame *root)
{
    RwFrame *parent = (RwFrame *)root->object.parent;

    if (parent)
    {
        FRAME_SET_HIERARCHY_ROOT(root, parent->root);
    }
    else
    {
        FRAME_SET_HIERARCHY_ROOT(root, root);
    }

    return root;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool
RwFrameDestroy(RwFrame *frame)
{
    RwFrame *child;

    /* _rwFrameInternalDeInit(frame): call destructors */
    _rwPluginRegistryDeInitObject(frameTKList, frame);

    if (frame->object.parent)
    {
        /* Its a child, detach from the parent (which adds it to the dirty list) */
        FRAME_REMOVE_CHILD(frame);
    }

    if (frame->object.privateFlags & (rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ))
    {
        frame->inDirtyListLink.prev->next = frame->inDirtyListLink.next;
        frame->inDirtyListLink.next->prev = frame->inDirtyListLink.prev;
    }

    /* Remove children's references to this frame and make them all roots */
    for (child = frame->child; child; child = child->next)
    {
        child->object.parent = NULL;
    }

    RwFreeListFree(RWFRAMEGLOBAL_frames, frame);

    return TRUE;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFrame *
RwFrameAddChildNoUpdate(RwFrame *parent, RwFrame *child)
{
    if (child->object.parent)
    {
        FRAME_REMOVE_CHILD(child);
    }

    /* add as child and re-parent */
    child->next = parent->child;
    parent->child = child;
    child->object.parent = parent;
    FRAME_SET_HIERARCHY_ROOT(child, parent->root);

    return parent;
}
