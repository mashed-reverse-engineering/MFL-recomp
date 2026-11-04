/* compile: /G5 */
/* _rwVectorSetMultFn and its same-file callees: RenderWare Graphics 3.7 (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/bavector.c, functions VectorMultPoint (0x004d5010, static), VectorMultVector (0x004d5160, static), _rwVectorSetMultFn (0x004d4ed0)), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; these functions stay
 * in one file because cl 13.00 compiles a static function and its callers in the same file with a register calling
 * convention and inlines same-file callees (/Ob2). The declarations they need are copied from the codfh headers (PC build).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe (tools/cmpfn.py /O2 /G5, relocations masked): VectorMultPoint 0x004d5010, VectorMultVector 0x004d5160, _rwVectorSetMultFn 0x004d4ed0. Static functions keep their linkage:
 * the overlay replaces only the external ones, the static copies serve the callers in this file. */
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
#define FALSE 0
#define TRUE !FALSE
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef union RwSplitBits RwSplitBits;
union RwSplitBits
{
    RwReal nReal;
    volatile RwInt32 nInt;
    volatile RwUInt32 nUInt;
};
typedef struct RwV3d RwV3d;
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)       /* No op */
#define RWASSERT(condition)    /* No op */
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
typedef RwV3d *(*rwVectorMultFn) (RwV3d * pointOut,
                                  const RwV3d * pointIn,
                                  const RwMatrix * matrix);
typedef RwV3d *(*rwVectorsMultFn) (RwV3d * pointsOut,
                                   const RwV3d * pointsIn,
                                   RwInt32 numPoints,
                                   const RwMatrix * matrix);
typedef struct rwVectorGlobals rwVectorGlobals;
struct rwVectorGlobals
{
    RwSplitBits *SqrtTab;
    RwSplitBits *InvSqrtTab;

    rwVectorMultFn  multPoint;
    rwVectorsMultFn multPoints;
    rwVectorMultFn  multVector;
    rwVectorsMultFn multVectors;
};
extern RwBool _rwVectorSetMultFn(rwVectorMultFn multPoint,
                                 rwVectorsMultFn multPoints,
                                 rwVectorMultFn multVector,
                                 rwVectorsMultFn multVectors);
#define RWVECTORGLOBAL(var) (RWPLUGINOFFSET(rwVectorGlobals, RwEngineInstance, vectorModule.globalsOffset)->var)
extern RwModuleInfo vectorModule;
extern RwV3d * VectorMultPoints(RwV3d *pointsOut, const RwV3d *pointsIn, RwInt32 numPoints, const RwMatrix *matrix);
extern RwV3d * VectorMultVectors(RwV3d *pointsOut, const RwV3d *pointsIn, RwInt32 numPoints, const RwMatrix *matrix);

/****************************************************************************
 VectorMultPoint
 On entry   : out point, in point, matrix
 On exit    : out point on success
 */
/* MFL.exe 0x004d5010 */
static RwV3d *
VectorMultPoint(RwV3d *pointOut,
                const RwV3d *pointIn,
                const RwMatrix *matrix)
{
    RwReal  scale;
    RwReal  imageX;
    RwReal  imageY;
    RwReal  imageZ;

    RWFUNCTION(RWSTRING("VectorMultPoint"));
    RWASSERT(pointOut);
    RWASSERT(pointIn);
    RWASSERT(matrix);

    scale = pointIn->x;   /* X */
    imageX = ((scale) * (matrix->right.x));
    imageY = ((scale) * (matrix->right.y));
    imageZ = ((scale) * (matrix->right.z));

    scale = pointIn->y;   /* Y */
    imageX = ((imageX) + (((scale) * (matrix->up.x))));
    imageY = ((imageY) + (((scale) * (matrix->up.y))));
    imageZ = ((imageZ) + (((scale) * (matrix->up.z))));

    scale = pointIn->z;   /* Z */
    imageX = ((imageX) + (((scale) * (matrix->at.x))));
    imageY = ((imageY) + (((scale) * (matrix->at.y))));
    imageZ = ((imageZ) + (((scale) * (matrix->at.z))));

    /* W */
    pointOut->x = ((imageX) + (matrix->pos.x));
    pointOut->y = ((imageY) + (matrix->pos.y));
    pointOut->z = ((imageZ) + (matrix->pos.z));

    RWRETURN(pointOut);
}

/****************************************************************************
 VectorMultVector
 On entry   : out point, in point, matrix
 On exit    : point on success
 */
/* MFL.exe 0x004d5160 */
static RwV3d *
VectorMultVector(RwV3d *pointOut,
                 const RwV3d *pointIn,
                 const RwMatrix *matrix)
{
    RwReal  scale;
    RwReal  imageX;
    RwReal  imageY;
    RwReal  imageZ;

    RWFUNCTION(RWSTRING("VectorMultVector"));
    RWASSERT(pointOut);
    RWASSERT(pointIn);
    RWASSERT(matrix);

    scale = pointIn->x;   /* X */
    imageX = ((scale) * (matrix->right.x));
    imageY = ((scale) * (matrix->right.y));
    imageZ = ((scale) * (matrix->right.z));

    scale = pointIn->y;   /* Y */
    imageX = ((imageX) + (((scale) * (matrix->up.x))));
    imageY = ((imageY) + (((scale) * (matrix->up.y))));
    imageZ = ((imageZ) + (((scale) * (matrix->up.z))));

    scale = pointIn->z;   /* Z */
    pointOut->x = ((((scale) * (matrix->at.x))) + (imageX));
    pointOut->y = ((((scale) * (matrix->at.y))) + (imageY));
    pointOut->z = ((((scale) * (matrix->at.z))) + (imageZ));

    RWRETURN(pointOut);
}

/****************************************************************************
 _rwVectorSetMultFn
 On entry   : Vector for multiplying a points (with translation)
              Vector for multiplying points (with translation)
              Vector for multiplying a vector (without translation)
              Vector for multiplying vectors (without translation)
 On exit    : point on success
 */
/* MFL.exe 0x004d4ed0 */
RwBool
_rwVectorSetMultFn(rwVectorMultFn multPoint,
                   rwVectorsMultFn multPoints,
                   rwVectorMultFn multVector,
                   rwVectorsMultFn multVectors)
{
    RWFUNCTION(RWSTRING("_rwVectorSetMultFn"));

    if (NULL == multPoint)
    {
        multPoint = VectorMultPoint;
    }

    RWVECTORGLOBAL(multPoint) = multPoint;

    if (NULL == multPoints)
    {
        multPoints = VectorMultPoints;
    }

    RWVECTORGLOBAL(multPoints) = multPoints;

    if (NULL == multVector)
    {
        multVector = VectorMultVector;
    }

    RWVECTORGLOBAL(multVector) = multVector;

    if (NULL == multVectors)
    {
        multVectors = VectorMultVectors;
    }

    RWVECTORGLOBAL(multVectors) = multVectors;

    RWRETURN(TRUE);
}
