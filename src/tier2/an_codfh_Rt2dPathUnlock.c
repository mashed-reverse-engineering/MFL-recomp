/* Rt2dPathUnlock (MFL.exe 005645e0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/path.c @e578672,
 * not byte-identical, changes: function cut out of the file by tools/codfh_loop.py, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RwSList RwSList;
typedef struct rt2dPathNode rt2dPathNode;
typedef struct Rt2dPath Rt2dPath;
struct RwSList
{
    RwUInt8    *listElements;
    RwInt32     numElementsFilled;
    RwInt32     numElementsAlloced;
    RwInt32     entrySize;
};
#define rwSListGetNumEntries(sList) \
       _rwSListGetNumEntries(sList)
struct Rt2dPath
{
    void                *segments;
    RwInt32             numSegment;
    RwBool              closed;
    RwBool              flat;
    RwReal              inset;
    Rt2dPath           *next;
    Rt2dPath           *curr;
};
#define _rt2dPathIsLocked(path) (path->numSegment == -1)
typedef enum rt2dLinetype rt2dLinetype;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
enum rt2dLinetype
{
    rt2dMOVE = 0,
    rt2dLINE = 1,
    rt2dCURVE = 2,
    rt2DLINETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
struct rt2dPathNode
{
    rt2dLinetype        type;
    RwV2d               pos, normal;
    RwReal              dist;
};
/*007d2128*/ extern void *RwEngineInstance;

/*005d6080*/ extern void * _rwSListToArray(struct RwSList *p0);
Rt2dPath * Rt2dPathUnlock(Rt2dPath *path);      /* self-recursive: prototype before the definition (recur_check) */

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
Rt2dPath * Rt2dPathUnlock(Rt2dPath *path)
{
    Rt2dPath *result = path;

    if(NULL != path && _rt2dPathIsLocked(path))
    {
        RwInt32 vcount = rwSListGetNumEntries((RwSList *)(path->segments));
        rt2dPathNode       *dst;

        dst = (rt2dPathNode *)
            _rwSListToArray((RwSList *)(path->segments));

        path->segments = dst;
        path->numSegment = vcount;

        Rt2dPathUnlock(path->next);
    }

    return (result);
}
