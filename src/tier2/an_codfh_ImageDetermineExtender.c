/* ImageDetermineExtender (MFL.exe 004ddfa0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/baimage.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, RwFexist
 * called directly (MFL.exe calls the file-system manager function, not the fileFuncs slot), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by
 * rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by
 * differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwstrcpy RWSRCGLOBAL(stringFuncs).vecStrcpy
#define rwstrlen RWSRCGLOBAL(stringFuncs).vecStrlen
typedef struct RwImage RwImage;
typedef RwImage *(*RwImageCallBackRead)(const RwChar * imageName);
typedef RwImage *(*RwImageCallBackWrite)(RwImage *image, const RwChar *imageName);
extern RwBool
RwFexist( const RwChar * name );
/* ---- end of the generated declarations ---- */

typedef struct rwImageFormat rwImageFormat;
struct rwImageFormat
{
        RwChar              lcExtension[20]; 
        RwChar              ucExtension[20]; 
        RwImageCallBackRead readImage;
        RwImageCallBackWrite writeImage;
        rwImageFormat      *nextFormat;
};
typedef struct rwImageGlobals rwImageGlobals;
struct rwImageGlobals
{
        RwFreeList         *imageFreeList;
        RwChar             *imagePath;
        RwInt32             imagePathSize;

        RwUInt8             gammaTable[256];
        RwUInt8             invGammaTable[256];
        RwReal              gammaVal;

        RwUInt8            *scratchMem;
        RwInt32             scratchMemSize;

        RwFreeList         *imageFormatFreeList;
        rwImageFormat      *imageFormats;
};
/* RwFexist: MFL.exe calls the function at 0x561060 directly (the file-system manager's RwFexist, between RwFeof 0x561050 and
 * RtFSManagerSyncAllFiles 0x561120), not through RWSRCGLOBAL(fileFuncs).rwfexist: the declaration above, no macro */
#define RWIMAGEGLOBAL(var) (RWPLUGINOFFSET(rwImageGlobals, RwEngineInstance, imageModule.globalsOffset)->var)
extern RwModuleInfo imageModule;
/* TIER2: unvalidated authority=diffemu */
RwChar * ImageDetermineExtender(RwChar * pathname, void *data)
{
    RwChar            **extender = (RwChar **) data;
    rwImageFormat      *imageFormat;
    RwChar             *extPos;

    extPos = pathname + rwstrlen(pathname);

    imageFormat = RWIMAGEGLOBAL(imageFormats);
    while (imageFormat)
    {
        
        rwstrcpy(extPos, imageFormat->lcExtension);
        if (RwFexist(pathname))
        {
            
            *extender = imageFormat->lcExtension;
            return ((char *)NULL);
        }

        rwstrcpy(extPos, imageFormat->ucExtension);
        if (RwFexist(pathname))
        {
            
            *extender = imageFormat->ucExtension;
            return ((char *)NULL);
        }

        imageFormat = imageFormat->nextFormat;
    }

    return (pathname);
}
