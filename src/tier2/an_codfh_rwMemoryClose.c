/* rwMemoryClose (MFL.exe 004e9800). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helper _rwFreeListModuleClose and RwFreeListDestroy inlined by hand (MFL.exe has them inlined), release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
#define rwFREELISTFLAG_STATIC 0x00000001
/* ---- end of the generated declarations ---- */

extern RwLinkList _freeListList;
extern RwBool     _freeListModuleOpen;
extern RwFreeList *_masterFreeListPtr;
/* RwFreeListDestroy, inlined here in MFL.exe (0x4e9815..0x4e988b and 0x4e989b..0x4e9928): unlink the free list, free its blocks, then give
 * the list itself back to the master free list (or free it when it is the master list); a static list keeps its memory */
#define FREELISTDESTROYINLINE(_freeList)                                \
MACRO_START                                                             \
{                                                                       \
    RwLLLink           *_cur;                                           \
                                                                        \
    rwLinkListRemoveLLLink(&(_freeList)->link);                         \
    _cur = rwLinkListGetFirstLLLink(&(_freeList)->blockList);           \
    while (_cur != rwLinkListGetTerminator(&(_freeList)->blockList))    \
    {                                                                   \
        rwLinkListRemoveLLLink(_cur);                                   \
        RwFree(_cur);                                                   \
        _cur = rwLinkListGetFirstLLLink(&(_freeList)->blockList);       \
    }                                                                   \
    if (!((_freeList)->flags & rwFREELISTFLAG_STATIC))                  \
    {                                                                   \
        if ((_freeList) != _masterFreeListPtr && _masterFreeListPtr != NULL) \
        {                                                               \
            RwFreeListFree(_masterFreeListPtr, (_freeList));            \
        }                                                               \
        else                                                            \
        {                                                               \
            RwFree(_freeList);                                          \
        }                                                               \
    }                                                                   \
}                                                                       \
MACRO_STOP

/* TIER2: unvalidated authority=diffemu */
void rwMemoryClose(void)
{
    /* _rwFreeListModuleClose(), static in bamemory.c, inlined in MFL.exe */
    RwFreeList *freeList;
    RwLLLink    *link, *lastLink;

    link     = rwLinkListGetFirstLLLink(&_freeListList);
    lastLink = rwLinkListGetTerminator(&_freeListList);
    while (link != lastLink)
    {
        freeList = rwLLLinkGetData(link, RwFreeList, link);
        FREELISTDESTROYINLINE(freeList);
        link     = rwLinkListGetFirstLLLink(&_freeListList);
        lastLink = rwLinkListGetTerminator(&_freeListList);
    }

    freeList = _masterFreeListPtr;
    FREELISTDESTROYINLINE(freeList);
    _masterFreeListPtr = NULL;

    _freeListModuleOpen = FALSE;
}
