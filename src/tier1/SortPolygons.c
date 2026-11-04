/* compile: /G5 */
/* SortPolygons (MFL.exe 0x0054ed10): RenderWare Graphics 3.7 (Criterion Software), codfh name, P7b near miss
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/plugin/collis/ctbuild.c, function SortPolygons), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * P7 delta classes for MFL's PC build: RWALIGN is empty (no __declspec(align(16))), RwGlobals has no RW 3.6 fileFuncs table,
 * Rt2dGlobals has no codfh `unknown` field; P7b: RenderWare library version 3.7.0.2 (stream range 0x35000..0x37002), static same-unit
 * callees the code calls or takes the address of are extern (bound to the MFL function by name / alias), not copied.
 * Byte-identical to MFL.exe 0x0054ed10 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef unsigned int RwUInt32;
typedef unsigned short RwUInt16;
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
typedef struct RwBBox RwBBox;
struct RwBBox
{
    /* Must be in this order */
    RwV3d sup;   /**< Supremum vertex. */
    RwV3d inf;   /**< Infimum vertex. */
};
#define RWRETURNVOID() return
#define RWFUNCTION(name)
typedef struct RpTriangle RpTriangle;
struct RpTriangle
{
    RwUInt16            vertIndex[3];   /**< vertex indices */
    RwUInt16            matIndex;       /**< Index into material list */
};
#define CLIPPOLYGONLEFT      (1)
typedef struct BuildVertex BuildVertex;
struct BuildVertex
{
    RwV3d              *pos;
    RwUInt32            flags;
    RwReal              dist;
};
typedef struct BuildPoly BuildPoly;
struct BuildPoly
{
    RpTriangle         *poly;
    RwUInt32            flags;
};
typedef struct BuildData BuildData;
struct BuildData
{
    /* Data for whole tree */
    RwUInt32            numVertices;
    BuildVertex        *vertices;
    BuildPoly          *polygons; /* Sorted on completion */
    RpTriangle         *origPolys;

    /* Data for current sector in build process */
    RwUInt32            bspDepth;
    RwBBox              bbox;
    RwUInt32            numPolygons;
    RwUInt32            polygonOffset; /* Offset into polygons[] */

    /* Pointer array for sorting the subset of vertices for polygons in
     * current sector */
    RwUInt32            numSortVerts;
    BuildVertex       **sortVerts;
};

/******************************************************************************
 *
 *  Sort the polygons according to whether they are in left or right
 *  sector using flags set by GetClipStats(). On exit, the
 *  flags are no longer valid.
 */
void
SortPolygons(BuildData * data)
{
    BuildPoly       *polygons = data->polygons + data->polygonOffset;
    RwUInt32            leftCount;
    RwUInt32            iPoly;

    RWFUNCTION(RWSTRING("SortPolygons"));

    leftCount = 0;

    /* For each polygon */
    for (iPoly = 0; iPoly < data->numPolygons; ++iPoly)
    {
        /* Is polygon on the left? */
        if (polygons[iPoly].flags == CLIPPOLYGONLEFT)
        {
            /* Move it to first section of array. */
            if (iPoly > leftCount)
            {
                RpTriangle  *const swap = polygons[leftCount].poly;

                polygons[leftCount].poly = polygons[iPoly].poly;
                polygons[iPoly].poly = swap;
            }

            ++leftCount;
        }
    }

    RWRETURNVOID();
}
