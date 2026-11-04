/* compile: /G5 */
/* _rwResourcesPurge (MFL.exe 0x004ea8e0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/baresour.c, function _rwResourcesPurge), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004ea8e0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned char RwUInt8;
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
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
#define rwLinkListInitialize(list)                                      \
    ( (list)->link.next = ((RwLLLink *)(list)),                         \
      (list)->link.prev = ((RwLLLink *)(list)) )
#define rwLinkListEmpty(list)                                           \
    (((list)->link.next) == (&(list)->link))
#define rwLinkListGetFirstLLLink(list)                                  \
    ((list)->link.next)
#define rwLinkListGetLastLLLink(list)                                   \
    ((list)->link.prev)
#define rwLinkListGetTerminator(list)                                   \
    (&((list)->link))
#define RWRETURNVOID() return
#define RWFUNCTION(name)       /* No op */
#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
extern void         *RwEngineInstance;
#define RWRESOURCESGLOBAL(var) (RWPLUGINOFFSET(rwResourcesGlobals,  \
    RwEngineInstance, resourcesModule.globalsOffset)->var)
typedef struct rwResources rwResources;
struct rwResources
{
    RwInt32             maxSize;
    RwInt32             currentSize;
    RwInt32             reusageSize;

    void               *memHeap;

    RwLinkList          entriesA;
    RwLinkList          entriesB;

    RwLinkList         *freeEntries;
    RwLinkList         *usedEntries;
};
typedef struct rwResourcesGlobals rwResourcesGlobals;
struct rwResourcesGlobals
{
    rwResources         res;
};
extern void         _rwResourcesPurge(void);
extern RwModuleInfo resourcesModule;

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   Get some resources
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/****************************************************************************
 _rwResourcesPurge
 Mark all used entries as free
 */
void
_rwResourcesPurge(void)
{
    RwLinkList         *usedEntries, *freeEntries;

    RWFUNCTION(RWSTRING("_rwResourcesPurge"));


    /* Stick the current free on the end of the used list */
    usedEntries = RWRESOURCESGLOBAL(res.usedEntries);
    freeEntries = RWRESOURCESGLOBAL(res.freeEntries);

    /* Copy from free into used */
    if (!rwLinkListEmpty(freeEntries))
    {
        if (rwLinkListEmpty(usedEntries))
        {
            usedEntries->link.next = freeEntries->link.next;
            (usedEntries->link.next)->prev = &(usedEntries->link);

            usedEntries->link.prev = freeEntries->link.prev;
            (usedEntries->link.prev)->next = &(usedEntries->link);

            /* Empty the list */
            rwLinkListInitialize(freeEntries);
        }
        else
        {
            RwLLLink           *first, *last;

            /* Stick together middle */
            last = rwLinkListGetLastLLLink(usedEntries);
            first = rwLinkListGetFirstLLLink(freeEntries);

            last->next = first;
            first->prev = last;

            /* Attach end */
            last = rwLinkListGetLastLLLink(freeEntries);

            last->next = rwLinkListGetTerminator(usedEntries);
            usedEntries->link.prev = last;

            /* Empty the list */
            rwLinkListInitialize(freeEntries);
        }
    }

    /* Swap over */
    RWRESOURCESGLOBAL(res.usedEntries) = freeEntries;
    RWRESOURCESGLOBAL(res.freeEntries) = usedEntries;

    /* Reset the reusage */
    RWRESOURCESGLOBAL(res.reusageSize) = 0;


    /* Done */
    RWRETURNVOID();
}
