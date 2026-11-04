/* compile: /G5 */
/* RwErrorGet (MFL.exe 0x004ea1c0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/baerr.c, function RwErrorGet), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004ea1c0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned char RwUInt8;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWECODE(a,b) a,
typedef struct RwError RwError;
struct RwError
{
    RwInt32     pluginID;  /**< Internal Use */
    RwInt32     errorCode; /**< Internal Use */
};
typedef struct rwErrorGlobals rwErrorGlobals;
struct rwErrorGlobals
{
    RwError     errorCode;                      /* Error code for last error */
};
enum RwErrorCodeCommon
{
    E_RW_NOERROR = (int)0x80000000L,
RWECODE(E_RW_BADENGINESTATE,
        "Engine in incorrect state for this operation")
RWECODE(E_RW_BADOPEN,
        "Error opening the file %s")
RWECODE(E_RW_BADPARAM,
        "Invalid Parameter passed. %s")
RWECODE(E_RW_BADVERSION,
        "The binary file format version is incompatible with this library")
RWECODE(E_RW_DEBUGSTACK,
        "Debug Library has Stack Depth mismatch")
RWECODE(E_RW_DEFAULTPIPELINECREATION,
        "Creation of a default pipeline (%s) failed")
RWECODE(E_RW_FRAMENOMATRIX,
        "The frame does not have an associated matrix")
RWECODE(E_RW_INVIMAGEDEPTH,
        "Invalid Image Depth")
RWECODE(E_RW_INVIMAGEFORMAT,
        "Image has no pixel memory allocated")
RWECODE(E_RW_INVIMAGEMASK,
        "The mask and image are not the same size")
RWECODE(E_RW_INVIMAGESIZE,
        "Destination and source images are of differing sizes")
RWECODE(E_RW_INVRASTERDEPTH,
        "Invalid Raster depth")
RWECODE(E_RW_INVRASTERFORMAT,
        "Unrecognized raster format")
RWECODE(E_RW_INVRASTERLOCKREQ,
        "Invalid Raster lock request")
RWECODE(E_RW_INVRASTERMIPLEVEL,
        "Invalid Raster mipmap level")
RWECODE(E_RW_INVRASTERSIZE,
        "Invalid Raster size")
RWECODE(E_RW_INVRASTERUNLOCKREQ,
        "Invalid Raster unlock request")
RWECODE(E_RW_NOFRAME,
        "Unable to find Frame")
RWECODE(E_RW_NOMEM,
        "Unable to allocate %d bytes of memory")
RWECODE(E_RW_NOMIPMAPGENERATIONCALLBACK,
        "No Mipmap generation callback set - use RtMipmapUseDefaultMipmapGenerationCallback")
RWECODE(E_RW_NOTSSEENABLEDCPU,
        "Not SSE enabled CPU")
RWECODE(E_RW_NULLP,
        "NULL pointer passed to library routine")
RWECODE(E_RW_PLUGININIT,
        "Plugin has already been initialized")
RWECODE(E_RW_PLUGINNOTINIT,
        "Plugin not initialized")
RWECODE(E_RW_RANGE,
        "A supplied parameter was outside the expected range")
RWECODE(E_RW_READ,
        "Read error on stream")
RWECODE(E_RW_REDUNDANT_FUNCTION,
        "Call to redundant function - scheduled to be dropped from future releases")
RWECODE(E_RW_WRITE,
        "Write error on stream")
RWECODE(E_RX_MESHES_RANGES_OVERLAP,
	"\n    Geometry is in an invalid format for RxPipeline rendering.\n    There may be visible artifacts and/or decreased performance.\n    Use RpGeometrySortByMaterial.\n    [stream %p type %s]")
RWECODE(E_RW_STRING_TRUNCATION,
        "strlen(%s) >= %d; truncating at character #%d  == %c")

    E_RW_LASTERROR = RWFORCEENUMSIZEINT
};
typedef enum RwErrorCodeCommon RwErrorCodeCommon;
extern RwError *RwErrorGet(RwError *code);
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)    /* No op */
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
#define RWERRORGLOBAL(var) \
    (RWPLUGINOFFSET(rwErrorGlobals, RwEngineInstance, \
    errorModule.globalsOffset)->var)
extern RwModuleInfo errorModule;

/**
 * \ingroup rwerror
 * \ref RwErrorGet is used to retrieve the error code for the
 * last reported error. The error code identifies both the numerical value
 * of the error and the plugin that issued it. The error code is reset to
 * E_RW_NOERROR once it has been read.
 *
 * This function is available in both debug and release versions of the
 * RenderWare library. In a debug library an error message is also
 * written to the debug stream when an error is raised.
 *
 * \param code  A pointer to an \ref RwError value that will receive the error
 *              code.
 *
 * \return Returns a pointer to an RwError code if successful.
 *
 * \see RwDebugSetHandler
 * \see RwDebugSendMessage
 * \see RwDebugSetTraceState
 *
 */
RwError *
RwErrorGet(RwError *code)
{
    RWAPIFUNCTION(RWSTRING("RwErrorGet"));
    RWASSERT(code);

    *code = RWERRORGLOBAL(errorCode);
    RWERRORGLOBAL(errorCode).pluginID = 0x0;
    RWERRORGLOBAL(errorCode).errorCode = (RwInt32) E_RW_NOERROR;

    RWRETURN(code);
}
