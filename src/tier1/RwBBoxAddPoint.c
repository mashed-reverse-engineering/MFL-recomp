/* compile: /G5 */
/* RwBBoxAddPoint (MFL.exe 0x004d19d0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/babbox.c, function RwBBoxAddPoint), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004d19d0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
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
typedef struct RwBBox RwBBox;
struct RwBBox
{
    /* Must be in this order */
    RwV3d sup;   /**< Supremum vertex. */
    RwV3d inf;   /**< Infimum vertex. */
};
extern RwBBox *RwBBoxAddPoint(RwBBox *boundBox,
                              const RwV3d *vertex);

/**
 * \ingroup rwbbox
 * \ref RwBBoxAddPoint is used to adjust the specified bounding-box so
 * that it contains the specified vertex. The bounding-box must have been
 * previously initialized with a single vertex or calculated from an array of
 * vertices.
 *
 * \param boundBox  Pointer to the bounding-box.
 * \param vertex  Pointer to a RwV3d value containing the position of the vertex.
 *
 * \return Returns a pointer to the bounding-box if successful, or NULL if
 * there is an error.
 *
 * \see RwBBoxCalculate
 * \see RwBBoxContainsPoint
 * \see RwBBoxInitialize
 *
 */
RwBBox *
RwBBoxAddPoint(RwBBox *boundBox, const RwV3d *vertex)
{
    RWAPIFUNCTION(RWSTRING("RwBBoxAddPoint"));
    RWASSERT(boundBox);
    RWASSERT(vertex);

    if (boundBox->inf.x > vertex->x)
    {
        boundBox->inf.x = vertex->x;
    }
    if (boundBox->inf.y > vertex->y)
    {
        boundBox->inf.y = vertex->y;
    }
    if (boundBox->inf.z > vertex->z)
    {
        boundBox->inf.z = vertex->z;
    }

    if (boundBox->sup.x < vertex->x)
    {
        boundBox->sup.x = vertex->x;
    }
    if (boundBox->sup.y < vertex->y)
    {
        boundBox->sup.y = vertex->y;
    }
    if (boundBox->sup.z < vertex->z)
    {
        boundBox->sup.z = vertex->z;
    }

    RWRETURN(boundBox);
}
