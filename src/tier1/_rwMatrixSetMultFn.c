/* compile: /G5 */
/* _rwMatrixSetMultFn (MFL.exe 0x004d3780): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/bamatrix.c, function _rwMatrixSetMultFn), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004d3780 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned char RwUInt8;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWALIGN(type, x)   __declspec(align(x)) type
#define rwMATRIXALIGNMENT 16
#define RWASMCALL              /* No op */
#define RWASMAPI(TYPE) TYPE
#define FALSE 0
#define TRUE !FALSE
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
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)       /* No op */
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
#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
extern void         *RwEngineInstance;
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
typedef void (RWASMCALL * rwMatrixMultFn) (RwMatrix * dstMat,
                                           const RwMatrix * matA,
                                           const RwMatrix * matB);
typedef struct RwMatrixTolerance RwMatrixTolerance;
struct RwMatrixTolerance
{
    RwReal              Normal;
        /**< Tolerance within which matrix is deemed to be normal   */
    RwReal              Orthogonal;
        /**< Tolerance within which matrix is deemed to be orthogonal */
    RwReal              Identity;
        /**< Tolerance within which matrix is deemed to be identity */
};
typedef struct rwMatrixGlobals rwMatrixGlobals;
struct rwMatrixGlobals
{
    RwFreeList         *matrixFreeList;
    RwInt32             matrixOptimizations;
    rwMatrixMultFn      multMatrix;
    RwMatrixTolerance   tolerance;
};
extern RwBool 
_rwMatrixSetMultFn(rwMatrixMultFn multMat);
#define RWASMAPI_void RWASMAPI(void)  /* To prevent Doxygen being confused */
#define RWMATRIXGLOBAL(var)                     \
    (RWPLUGINOFFSET(rwMatrixGlobals,            \
     RwEngineInstance,                          \
     matrixModule.globalsOffset)->var)
extern RwModuleInfo matrixModule;
extern RWASMAPI_void MatrixMultiply(RwMatrix * dstMat, const RwMatrix * matA, const RwMatrix * matB);
extern __declspec(noinline) RwMatrix * MatrixOrthoNormalize(RwMatrix * dst, const RwMatrix * src);

/****************************************************************************
 _rwMatrixSetMultFn
 On entry   : Matrix multiply function (NULL for default)
 On exit    : TRUE on success
 */
RwBool
_rwMatrixSetMultFn(rwMatrixMultFn multMat)
{
    RWFUNCTION(RWSTRING("_rwMatrixSetMultFn"));

    if (NULL == multMat)
    {
        multMat = MatrixMultiply;
    }
    RWMATRIXGLOBAL(multMatrix) = multMat;

    RWRETURN(TRUE);
}
