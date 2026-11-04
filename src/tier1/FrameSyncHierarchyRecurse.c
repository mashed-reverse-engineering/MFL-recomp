/* compile: /G5 */
/* FrameSyncHierarchyRecurse (MFL.exe 0x004e9f60): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/basync.c, function FrameSyncHierarchyRecurse), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004e9f60 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned char RwUInt8;
typedef float RwReal;
#define RWALIGN(type, x)   __declspec(align(x)) type
#define rwMATRIXALIGNMENT 16
#define rwFRAMEALIGNMENT 16
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef struct RwV3d RwV3d;
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};
typedef struct RwLLLink  RwLLLink;                     /*** RwLLLink ***/
struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};
#define rwLLLinkGetData(linkvar,type,entry)                             \
    ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar)                                        \
    ((linkvar)->next)
typedef struct RwLinkList RwLinkList;
struct RwLinkList
{
    RwLLLink link;
};
#define rwLinkListEmpty(list)                                           \
    (((list)->link.next) == (&(list)->link))
#define rwLinkListGetFirstLLLink(list)                                  \
    ((list)->link.next)
#define rwLinkListGetTerminator(list)                                   \
    (&((list)->link))
#define offsetof(type, member)                                          \
    ((size_t)((RwUInt8 *)&((type *) 0)->member - (RwUInt8 *)((type *) 0)))
typedef struct RwObject RwObject;
struct RwObject
{
        RwUInt8 type;                /**< Internal Use */
        RwUInt8 subType;             /**< Internal Use */
        RwUInt8 flags;               /**< Internal Use */
        RwUInt8 privateFlags;        /**< Internal Use */
        void   *parent;              /**< Internal Use */
                                     /* Often a Frame  */
};
#define rwObjectGetPrivateFlags(c)          (((const RwObject *)(c))->privateFlags)
#define rwObjectSetPrivateFlags(c,f)        (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
#define rwObjectGetParent(object)           (((const RwObject *)(object))->parent)
#define RWRETURNVOID() return
#define RWFUNCTION(name)       /* No op */
#define RWASSERT(condition)    /* No op */
#define RWASSERTISTYPE(_f, _t) \
   RWASSERT((((const RwObject *)(_f))->type)==(_t))
struct RwMatrixTag
{
    /* These are padded to be 16 byte quantities per line */
    RwV3d               right;
    RwUInt32            flags;
    RwV3d               up;
    RwUInt32            pad1;
    RwV3d               at;
    RwUInt32            pad2;
    RwV3d               pos;
    RwUInt32            pad3;
};
typedef struct RwMatrixTag RWALIGN(RwMatrix, rwMATRIXALIGNMENT);
extern RwMatrix *
RwMatrixMultiply(RwMatrix * matrixOut,
                 const RwMatrix * MatrixIn1,
                 const RwMatrix * matrixIn2);
#define rwFRAME 0
#define rwFRAMEPRIVATESUBTREESYNCLTM    0x04
#define rwFRAMEPRIVATESUBTREESYNCOBJ    0x08
struct RwFrame
{
    RwObject            object;

    RwLLLink            inDirtyListLink;

    /* Put embedded matrices here to ensure they remain 16-byte aligned */
    RwMatrix            modelling;
    RwMatrix            ltm;

    RwLinkList          objectList; /* List of objects connected to a frame */

    struct RwFrame      *child;
    struct RwFrame      *next;
    struct RwFrame      *root;   /* Root of the tree */


//@{ 20050513 DDonSS : Threadsafe
};
typedef struct RwFrame RWALIGN(RwFrame, rwFRAMEALIGNMENT);
#define RwFrameGetParentMacro(_f)   ((RwFrame *)rwObjectGetParent(_f))
#define RwFrameGetParent(_f)    RwFrameGetParentMacro(_f)
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
#define rwObjectHasFrameSync(o) \
    ((RwObjectHasFrame *)(o))->sync(o)

/****************************************************************************
 Local Types
 */
/****************************************************************************
 Local (Static) Prototypes
 */
/****************************************************************************
 Local Defines
 */
/****************************************************************************
 Globals (across program)
 */
/****************************************************************************
 Local (static) Globals
 */
/****************************************************************************
 _rwFrameSyncHierarchyRecurse
 Syncs a frames hierarchy and objects attached (recursively)
 On entry   : Root frame of the hierarchy to sync, dirty
 On exit    :
 */
// Its data (PLAN M5): literals, constants and tables where the image has them.
void
FrameSyncHierarchyRecurse(RwFrame *frame, RwInt32 flags)
{
    RWFUNCTION(RWSTRING("FrameSyncHierarchyRecurse"));


    /* NULL is a valid; termination condition */
    while (frame)
    {
        RwInt32 accumflags = flags | rwObjectGetPrivateFlags(frame);

        RWASSERTISTYPE(frame, rwFRAME);

        if (accumflags & rwFRAMEPRIVATESUBTREESYNCLTM)
        {
            /* Work out the new local transformation matrix */
            RwMatrixMultiply(&frame->ltm,
                             &frame->modelling,
                             &((RwFrameGetParent(frame))->ltm));
        }

        /* If we got here, we always need to sync the objects */
        if (!rwLinkListEmpty(&frame->objectList))
        {
            RwLLLink    *current, *end;

            current = rwLinkListGetFirstLLLink(&frame->objectList);
            end = rwLinkListGetTerminator(&frame->objectList);
            while (current != end)
            {
                RwObjectHasFrame    *object;

                object = (RwObjectHasFrame *)
                    rwLLLinkGetData(current, RwObjectHasFrame, lFrame);
                rwObjectHasFrameSync(object);
                current = rwLLLinkGetNext(current);
            }
        }

        /* clear flag */
        rwObjectSetPrivateFlags(frame, rwObjectGetPrivateFlags(frame) &
                                        ~(rwFRAMEPRIVATESUBTREESYNCLTM |
                                          rwFRAMEPRIVATESUBTREESYNCOBJ));

        /* Depth first...
         * Child has dirty status including this frame,
         * sibling has dirty status of parent (parameter in)
         */
        FrameSyncHierarchyRecurse(frame->child, accumflags);

        /* tail recursion */
        frame = frame->next;
    }

    RWRETURNVOID();
}
