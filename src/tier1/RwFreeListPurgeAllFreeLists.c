/* compile: /G5 */
/* RwFreeListPurgeAllFreeLists (MFL.exe 0x004e96a0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/bamemory.c, function RwFreeListPurgeAllFreeLists), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004e96a0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned char RwUInt8;
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
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
#define rwLinkListGetFirstLLLink(list)                                  \
    ((list)->link.next)
#define rwLinkListGetTerminator(list)                                   \
    (&((list)->link))
#define offsetof(type, member)                                          \
    ((size_t)((RwUInt8 *)&((type *) 0)->member - (RwUInt8 *)((type *) 0)))
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)    /* No op */
typedef struct RwFreeList RwFreeList;
struct RwFreeList
{
    RwUInt32   entrySize;       /**<Size of an entry in the free list. */
    RwUInt32   entriesPerBlock; /**<Number of entries per free list block. */
    RwUInt32   heapSize;        /**<Size of the heap. */
    RwUInt32   alignment;       /**<Alignment of a free list entry. */
    RwLinkList blockList;       /**<List of data blocks. */
    RwUInt32   flags;           /**<Flags which affect the behavior of the
                                    free list. <BR>
                                    rwFREELISTFLAG_FREEBLOCKS */
    RwLLLink   link;            /**<Link to the free list linked list. */


//@{ 20050513 DDonSS : Threadsafe
//@} DDonSS
};
extern RwInt32      RwFreeListPurge(RwFreeList * freelist);
extern RwInt32      RwFreeListPurgeAllFreeLists(void);
extern RwLinkList _freeListList;
extern RwInt32 RwFreeListPurge(RwFreeList * freeList);

/**
 * \ingroup rwfreelist
 * \ref RwFreeListPurgeAllFreeLists is used to purge all free lists in
 * the system.  It returns the amount of memory freed as a result.
 *
 * \return Returns the amount of memory freed if successful, or -1 if there is an
 * error.
 *
 * \see RwFreeListAlloc
 * \see RwFreeListCreate
 * \see RwFreeListFree
 * \see RwFreeListPurge
 * \see RwFreeListDestroy
 * \see RwFreeListForAllUsed
 *
 */
RwInt32
RwFreeListPurgeAllFreeLists(void)
{
    RwInt32  memoryFreed = 0;
    RwLLLink *cur, *end;
    RWAPIFUNCTION(RWSTRING("RwFreeListPurgeAllFreeLists"));


	cur = rwLinkListGetFirstLLLink(&_freeListList);
    end = rwLinkListGetTerminator(&_freeListList);
    while (cur != end)
    {
        /* Purge this list */
        RwFreeList * const  freeList = ( rwLLLinkGetData(cur,
                                                         RwFreeList,
                                                         link) );

		const RwInt32 thisMemoryFreed = RwFreeListPurge(freeList);
		
        if (0 < thisMemoryFreed)
        {
            memoryFreed += thisMemoryFreed;
        }

        /* Get next */
        cur = rwLLLinkGetNext(cur);
    }


    RWRETURN(memoryFreed);
}
