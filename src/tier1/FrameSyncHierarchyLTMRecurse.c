/* compile: /G5 */
/* FrameSyncHierarchyLTMRecurse (MFL.exe 0x004ea0b0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/basync.c, function FrameSyncHierarchyLTMRecurse), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004ea0b0 (tools/cmpfn.py /O2 /G5, relocations masked). */
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
typedef struct RwLinkList RwLinkList;
struct RwLinkList
{
    RwLLLink link;
};
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

/****************************************************************************
 FrameSyncHierarchyLTMRecurse
 Syncs a frames local transformation matrix hierarchy but not objects
 positions (recursively)
 On entry   : Root frame of the hierarchy to sync, dirty
 On exit    :
 */
void
FrameSyncHierarchyLTMRecurse(RwFrame * frame, RwInt32 flags)
{
    RWFUNCTION(RWSTRING("FrameSyncHierarchyLTMRecurse"));

	
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
            /* clear flag */
            rwObjectSetPrivateFlags(frame, rwObjectGetPrivateFlags(frame) &
                                            ~(rwFRAMEPRIVATESUBTREESYNCLTM));
        }

        /* Depth first */
        /* Child has dirty status including this frame,
         * sibling has dirty status of parent (parameter in)
         */
        FrameSyncHierarchyLTMRecurse(frame->child, accumflags);

        /* tail recursion */
        frame = frame->next;
    }

    RWRETURNVOID();
}
