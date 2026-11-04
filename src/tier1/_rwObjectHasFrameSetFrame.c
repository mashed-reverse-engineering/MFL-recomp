/* compile: /G5 */
/* _rwObjectHasFrameSetFrame (MFL.exe 0x004d1860): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/batypehf.c, function _rwObjectHasFrameSetFrame), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004d1860 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
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
#define rwLinkListAddLLLink(list, linkvar)                              \
    ( (linkvar)->next = (list)->link.next,                              \
      (linkvar)->prev = (&(list)->link),                                \
      ((list)->link.next)->prev = (linkvar),                            \
      (list)->link.next = (linkvar) )
#define rwLinkListRemoveLLLink(linkvar)                                 \
    ( ((linkvar)->prev)->next = (linkvar)->next,                        \
      ((linkvar)->next)->prev = (linkvar)->prev )
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
#define rwObjectGetParent(object)           (((const RwObject *)(object))->parent)
#define rwObjectSetParent(c,p)              (((RwObject *)(c))->parent) = (void *)(p)
#define RWRETURNVOID() return
#define RWFUNCTION(name)       /* No op */
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
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
extern void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame);

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
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   Generic types
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
 /****************************************************************************
 RwHasFrameObjectSetFrame
 On entry   : Object
 On exit    :
 */
// Its data (PLAN M5): literals, constants and tables where the image has them.
void
_rwObjectHasFrameSetFrame(void *object, RwFrame * frame)
{
    RwObjectHasFrame   *ohf = (RwObjectHasFrame *) object;

    RWFUNCTION(RWSTRING("_rwObjectHasFrameSetFrame"));


    if (rwObjectGetParent(ohf))
    {
        rwLinkListRemoveLLLink(&ohf->lFrame);
    }

    /* Set the pointer */
    rwObjectSetParent(object, frame);

    /* Add it to the frames list of objects */
    if (frame)
    {
        rwLinkListAddLLLink(&frame->objectList, &ohf->lFrame);

        /* Force the objects using this frame to be updated */
        RwFrameUpdateObjects(frame);
    }


    /* All done */
    RWRETURNVOID();
}
