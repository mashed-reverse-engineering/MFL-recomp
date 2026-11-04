/* compile: /G5 */
/* RwBBoxCalculate (MFL.exe 0x004d18d0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/babbox.c, function RwBBoxCalculate), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004d18d0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef float RwReal;
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef struct RwV3d RwV3d;
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)    /* No op */
#define RWASSERT(condition)    /* No op */
#define RwV3dAssignMacro(_target, _source)                     \
    ( *(_target) = *(_source) )
#define RwV3dAssign(o, a)               RwV3dAssignMacro(o, a)
typedef struct RwBBox RwBBox;
struct RwBBox
{
    /* Must be in this order */
    RwV3d sup;   /**< Supremum vertex. */
    RwV3d inf;   /**< Infimum vertex. */
};
extern RwBBox *RwBBoxCalculate(RwBBox *boundBox,
                               const RwV3d *verts,
                               RwInt32 numVerts);

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
 Functions
 */
/**
 * \ingroup rwbbox
 * \ref RwBBoxCalculate is used to calculate the axis-aligned
 * bounding-box of the specified array of 3D points.
 *
 * \param boundBox  Pointer to the bounding-box.
 * \param verts  An array of vertex positions.
 * \param numVerts  A RwInt32 value equal to the number of vertices in the
 * array.
 *
 * \return Returns a pointer to the bounding-box if successful, or NULL if
 * there is an error.
 *
 * \see RwBBoxAddPoint
 * \see RwBBoxContainsPoint
 * \see RwBBoxInitialize
 *
 */
// Its data (PLAN M5): literals, constants and tables where the image has them.
RwBBox *
RwBBoxCalculate(RwBBox *boundBox, const RwV3d *verts, RwInt32 numVerts)
{
    RWAPIFUNCTION(RWSTRING("RwBBoxCalculate"));
    RWASSERT(boundBox);
    RWASSERT(verts);
    RWASSERT(numVerts > 0);

    RwV3dAssign(&boundBox->inf, verts);
    RwV3dAssign(&boundBox->sup, verts);

    ++verts;
    --numVerts;

    while (numVerts--)
    {
        if (boundBox->inf.x > verts->x)
        {
            boundBox->inf.x = verts->x;
        }
        if (boundBox->inf.y > verts->y)
        {
            boundBox->inf.y = verts->y;
        }
        if (boundBox->inf.z > verts->z)
        {
            boundBox->inf.z = verts->z;
        }

        if (boundBox->sup.x < verts->x)
        {
            boundBox->sup.x = verts->x;
        }
        if (boundBox->sup.y < verts->y)
        {
            boundBox->sup.y = verts->y;
        }
        if (boundBox->sup.z < verts->z)
        {
            boundBox->sup.z = verts->z;
        }

        ++verts;
    }

    RWRETURN(boundBox);
}
