/* compile: /G5 */
/* BuildTreeCountLeafNodes (MFL.exe 0x0054ee10): RenderWare Graphics 3.7 (Criterion Software), codfh name, P7 near miss
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/plugin/collis/ctbuild.c, function BuildTreeCountLeafNodes), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * P7 (near-miss pilot) fixes for MFL's PC build: RWALIGN is empty (no __declspec(align(16)) on RwMatrix/RwCamera), RwGlobals has no
 * RW 3.6 fileFuncs table (stringFuncs at +0xc4 (vecStrlen +0xf4), memoryFuncs at +0x108, memoryAlloc +0x118), Rt2dGlobals has no codfh `unknown` field.
 * Byte-identical to MFL.exe 0x0054ee10 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef int  RwInt32;
typedef float RwReal;
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)
#define rpCOLLTREE_MAXDEPTH                 32
typedef struct BuildSector BuildSector;
struct BuildSector
{
    RwInt32             type;   /* = -1 for poly sector, >=0 for plane sector */
};
typedef struct BuildPlaneSector BuildPlaneSector;
struct BuildPlaneSector
{
    RwInt32             type;   /* plane type */
    BuildSector      *leftSubTree;
    BuildSector      *rightSubTree;
    RwReal              leftValue;
    RwReal              rightValue;
};

/******************************************************************************
 *  Count the number of leaf nodes in the BSP tree
 */
RwInt32
BuildTreeCountLeafNodes(BuildSector * tree)
{
    BuildSector      *sector = tree;
    BuildSector      *sectorStack[rpCOLLTREE_MAXDEPTH + 1];
    RwInt32             nStack = 0;
    RwInt32             nLeaves = 0;

    RWFUNCTION(RWSTRING("BuildTreeCountLeafNodes"));

    sectorStack[0] = NULL;

    while (sector)
    {
        if (sector->type < 0)  /* Leaf node */
        {
            ++nLeaves;
            sector = sectorStack[nStack];
            --nStack;
        }
        else                   /* Branch node */
        {
            BuildPlaneSector *planeSector =
                (BuildPlaneSector *) sector;

            /* Put right on stack, and go left */
            ++nStack;
            sectorStack[nStack] = planeSector->rightSubTree;
            sector = planeSector->leftSubTree;
        }
    }

    RWRETURN(nLeaves);
}
