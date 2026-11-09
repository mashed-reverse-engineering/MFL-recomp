/* basync (MFL.exe 0x004e9db0 / 0x004ea030): RenderWare Graphics 3.7 (Criterion Software), frame hierarchy synchronisation:
 * _rwFrameSyncDirty (all dirty hierarchies: LTMs and attached objects) and _rwFrameSyncHierarchyLTM (LTMs only).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/basync.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build); the static FrameSyncHierarchy inlined into
 * _rwFrameSyncDirty and the first level of FrameSyncHierarchyRecurse / FrameSyncHierarchyRecurseNoLTM / FrameSyncHierarchyLTMRecurse
 * written out where the exe inlined it (the deeper levels call the byte-identical tier-1 functions, same call order as the exe);
 * RwMatrixCopy as a struct assignment; RWSRCGLOBAL(dirtyFrameList) as *RwEngineInstance + 0xbc; structs declared here (codfh
 * baframe.h / balist.h / bamatrix.h, without the 16-byte alignment attribute) with compile-time layout asserts.
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
typedef struct RwObjectHasFrame {
    RwObject object;
    RwLLLink lFrame;                    /* 0x08 */
    struct RwObjectHasFrame *(*sync)(struct RwObjectHasFrame *object);      /* 0x10 */
} RwObjectHasFrame;
RW_LAYOUT_ASSERT(rw_assert_private_flags, RW_OFFSETOF(RwObject, privateFlags) == 3);
RW_LAYOUT_ASSERT(rw_assert_frame_dirty, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);
RW_LAYOUT_ASSERT(rw_assert_frame_modelling, RW_OFFSETOF(RwFrame, modelling) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_frame_ltm, RW_OFFSETOF(RwFrame, ltm) == 0x50);
RW_LAYOUT_ASSERT(rw_assert_frame_objects, RW_OFFSETOF(RwFrame, objectList) == 0x90);
RW_LAYOUT_ASSERT(rw_assert_frame_child, RW_OFFSETOF(RwFrame, child) == 0x98);
RW_LAYOUT_ASSERT(rw_assert_frame_next, RW_OFFSETOF(RwFrame, next) == 0x9c);
RW_LAYOUT_ASSERT(rw_assert_hasframe_sync, RW_OFFSETOF(RwObjectHasFrame, sync) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_matrix_size, sizeof(RwMatrix) == 0x40);

#define rwFRAMEPRIVATEHIERARCHYSYNCLTM 0x01
#define rwFRAMEPRIVATEHIERARCHYSYNCOBJ 0x02
#define rwFRAMEPRIVATESUBTREESYNCLTM 0x04
#define rwFRAMEPRIVATESUBTREESYNCOBJ 0x08
#define RW_DIRTY_FRAME_LIST 0xbc      /* RwGlobals.dirtyFrameList (rw37.h RwGlobals37: stdFunc 0x48 + 29 pointers) */

/*007d2128*/ extern void *RwEngineInstance;
#define RWSRC_DIRTYLIST ((RwLinkList *)((RwUInt8 *)RwEngineInstance + RW_DIRTY_FRAME_LIST))

/*004d3cd0*/ extern RwMatrix *RwMatrixMultiply(RwMatrix *matrixOut, const RwMatrix *matrixIn1, const RwMatrix *matrixIn2);
/*004e9f60*/ extern void FrameSyncHierarchyRecurse(RwFrame *frame, RwInt32 flags);
/*004e9fe0*/ extern void FrameSyncHierarchyRecurseNoLTM(RwFrame *frame);
/*004ea0b0*/ extern void FrameSyncHierarchyLTMRecurse(RwFrame *frame, RwInt32 flags);

/* Always handle the objects attached to a frame */
#define SYNC_OBJECTS(f)                                                             \
    {                                                                               \
        RwLLLink *current = (f)->objectList.link.next, *end = &(f)->objectList.link; \
        while (current != end)                                                      \
        {                                                                           \
            RwObjectHasFrame *object = (RwObjectHasFrame *)((RwUInt8 *)current - RW_OFFSETOF(RwObjectHasFrame, lFrame)); \
            object->sync(object);                                                   \
            current = current->next;                                                \
        }                                                                           \
    }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool _rwFrameSyncDirty(void)
{
    RwLLLink *cur = RWSRC_DIRTYLIST->link.next, *end = &RWSRC_DIRTYLIST->link;

    while (cur != end)
    {
        RwFrame *frame = (RwFrame *)((RwUInt8 *)cur - RW_OFFSETOF(RwFrame, inDirtyListLink));
        RwInt32 oldFlags = frame->object.privateFlags;
        RwFrame *child;

        /* FrameSyncHierarchy: sync up the hierarchy */
        if (oldFlags & rwFRAMEPRIVATEHIERARCHYSYNCLTM)
        {
            RwInt32 flags = oldFlags & rwFRAMEPRIVATESUBTREESYNCLTM;

            if (flags)
            {
                /* Root of hierarchy has no parent matrix */
                frame->ltm = frame->modelling;
            }
            SYNC_OBJECTS(frame);

            /* Do the children (FrameSyncHierarchyRecurse, first level) */
            for (child = frame->child; child; child = child->next)
            {
                RwInt32 accumflags = flags | child->object.privateFlags;

                if (accumflags & rwFRAMEPRIVATESUBTREESYNCLTM)
                {
                    RwMatrixMultiply(&child->ltm, &child->modelling, &((RwFrame *)child->object.parent)->ltm);
                }
                SYNC_OBJECTS(child);
                child->object.privateFlags &= (RwUInt8)~(rwFRAMEPRIVATESUBTREESYNCLTM | rwFRAMEPRIVATESUBTREESYNCOBJ);
                FrameSyncHierarchyRecurse(child->child, accumflags);
            }
        }
        else
        {
            SYNC_OBJECTS(frame);

            /* Do the children (FrameSyncHierarchyRecurseNoLTM, first level) */
            for (child = frame->child; child; child = child->next)
            {
                SYNC_OBJECTS(child);
                child->object.privateFlags &= (RwUInt8)~rwFRAMEPRIVATESUBTREESYNCOBJ;
                FrameSyncHierarchyRecurseNoLTM(child->child);
            }
        }

        /* clear flag */
        frame->object.privateFlags = (RwUInt8)(oldFlags & ~(rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATEHIERARCHYSYNCOBJ |
                                                           rwFRAMEPRIVATESUBTREESYNCLTM | rwFRAMEPRIVATESUBTREESYNCOBJ));

        /* Onto the next */
        cur = cur->next;
    }

    /* We have dealt with them all */
    RWSRC_DIRTYLIST->link.next = &RWSRC_DIRTYLIST->link;
    RWSRC_DIRTYLIST->link.prev = &RWSRC_DIRTYLIST->link;

    return 1;      /* TRUE */
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void _rwFrameSyncHierarchyLTM(RwFrame *frame)
{
    RwInt32 oldFlags = frame->object.privateFlags;
    RwFrame *child;

    if (oldFlags & rwFRAMEPRIVATESUBTREESYNCLTM)
    {
        /* Root of hierarchy has no parent matrix */
        frame->ltm = frame->modelling;
    }

    /* Do the children (FrameSyncHierarchyLTMRecurse, first level) */
    for (child = frame->child; child; child = child->next)
    {
        RwInt32 accumflags = oldFlags | child->object.privateFlags;

        if (accumflags & rwFRAMEPRIVATESUBTREESYNCLTM)
        {
            RwMatrixMultiply(&child->ltm, &child->modelling, &((RwFrame *)child->object.parent)->ltm);
            child->object.privateFlags &= (RwUInt8)~rwFRAMEPRIVATESUBTREESYNCLTM;
        }
        FrameSyncHierarchyLTMRecurse(child->child, accumflags);
    }

    /* clear flag */
    frame->object.privateFlags = (RwUInt8)(oldFlags & ~(rwFRAMEPRIVATEHIERARCHYSYNCLTM | rwFRAMEPRIVATESUBTREESYNCLTM));
}
