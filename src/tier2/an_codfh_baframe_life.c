/* baframe lifecycle (MFL.exe 0x004d0c50 / 0x004d0f00 / 0x004d1160 / 0x004d1330): RenderWare Graphics 3.7 (Criterion Software):
 * RwFrameCreate, RwFrameDestroyHierarchy, RwFrameAddChild and RwFrameRemoveChild.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/baframe.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), no CS_FRAME_LOCK (codfh threading
 * patch, not in the exe); the first level of the static rwFrameDestroyRecurse inlined into RwFrameDestroyHierarchy and the first
 * level of rwSetHierarchyRoot plus RwFrameUpdateObjects inlined into RwFrameRemoveChild, and RwFrameRemoveChild plus the same two
 * inlined into RwFrameAddChild (as macros shared by both), as the exe has them (the deeper levels call
 * the original rwFrameDestroyRecurse / rwSetHierarchyRoot); RwFreeListAlloc / RwFreeListFree as the engine's freeListAlloc /
 * freeListFree slots (0x118 / 0x11c), RWFRAMEGLOBAL(frames) as the word at frameModule.globalsOffset in *RwEngineInstance,
 * RWSRCGLOBAL(dirtyFrameList) as *RwEngineInstance + 0xbc; the rwLinkList macros expanded; structs declared here (codfh
 * baframe.h / balist.h, without the 16-byte alignment attribute) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
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
/*004d0c90*/ extern RwFrame *rwFrameInternalInit(RwFrame *frame);
/*004d0e90*/ extern void rwFrameDestroyRecurse(RwFrame *frame);
/*004e9d40*/ extern void *_rwPluginRegistryDeInitObject(void *reg, void *object);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFrame *
RwFrameCreate(void)
{
    RwFrame *frame;

    frame = (RwFrame *)RwFreeListAlloc(RWFRAMEGLOBAL_frames, rwMEMHINTDUR_EVENT | rwID_FRAMELIST);
    if (!frame)
    {
        return (RwFrame *)NULL;
    }

    rwFrameInternalInit(frame);

    return frame;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool
RwFrameDestroyHierarchy(RwFrame *frame)
{
    /* rwFrameDestroyRecurse(frame), first level */
    if (frame)
    {
        RwFrame *child;

        child = frame->child;
        while (child)
        {
            RwFrame *nextChild = child->next;

            /* Destroy the first child's hierarchy */
            rwFrameDestroyRecurse(child);

            /* Skip to the next sub tree */
            child = nextChild;
        }

        /* Call destructors */
        _rwPluginRegistryDeInitObject(frameTKList, frame);

        /* Its the root -> handle removing from the list */
        if (frame->object.privateFlags & (rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ))
        {
            frame->inDirtyListLink.prev->next = frame->inDirtyListLink.next;
            frame->inDirtyListLink.next->prev = frame->inDirtyListLink.prev;
        }

        RwFreeListFree(RWFRAMEGLOBAL_frames, frame);
    }

    return TRUE;
}

/* The exe inlines these into RwFrameRemoveChild and (twice) into RwFrameAddChild. */

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
#define FRAME_REMOVE_CHILD(child)                                                   \
    {                                                                               \
        RwFrame *curFrame_ = ((RwFrame *)(child)->object.parent)->child;            \
        /* Check if its the first child */                                         \
        if (curFrame_ == (child))                                                   \
        {                                                                           \
            ((RwFrame *)(child)->object.parent)->child = (child)->next;             \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            while (curFrame_->next != (child))                                      \
            {                                                                       \
                curFrame_ = curFrame_->next;                                        \
            }                                                                       \
            /* Remove it from the sibling list */                                   \
            curFrame_->next = (child)->next;                                        \
        }                                                                           \
        /* Now its a root it also has no siblings */                                \
        (child)->object.parent = NULL;                                              \
        (child)->next = (RwFrame *)NULL;                                            \
        FRAME_SET_HIERARCHY_ROOT(child, child);                                     \
        /* Make it dirty */                                                         \
        FRAME_UPDATE_OBJECTS(child);                                                \
    }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFrame *
RwFrameAddChild(RwFrame *parent, RwFrame *child)
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

    /* Handle if its dirty */
    if (child->object.privateFlags & (rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ))
    {
        child->inDirtyListLink.prev->next = child->inDirtyListLink.next;
        child->inDirtyListLink.next->prev = child->inDirtyListLink.prev;

        /* clear flag */
        child->object.privateFlags &= ~(rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ);
    }

    /* The child's local transformation matrix is definitely dirty */
    FRAME_UPDATE_OBJECTS(child);

    return parent;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFrame *
RwFrameRemoveChild(RwFrame *child)
{
    FRAME_REMOVE_CHILD(child);

    return child;
}
