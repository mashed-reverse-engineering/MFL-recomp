/* _rtFSBuildFileName (MFL.exe 0x00561a30): RenderWare Graphics 3.7 (Criterion Software), file system toolkit: drive name +
 * file path into a buffer, every '/' or '\\' replaced by the given separator.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/fsyst/rtfsyst.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), rwstrcpy expanded to RwGlobals.stringFuncs.vecStrcpy
 * (0xcc), strlen declared here (the /Oi intrinsic) (mfl_tier2.h and the CRT headers do not mix).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_GLOBALS_STRCPY 0xcc      /* RwGlobals.stringFuncs (0xc4) .vecStrcpy (3rd pointer), rw37.h RwGlobals37 */
typedef RwChar *(*RwStrcpyFn)(RwChar *dest, const RwChar *srce);
extern size_t strlen(const char *string);      /* /O2 implies /Oi: inline repne scasb like the exe */

/*007d2128*/ extern void *RwEngineInstance;
#define rwstrcpy(dst, src) ((*(RwStrcpyFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_STRCPY))((dst), (src)))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwChar *_rtFSBuildFileName(RwChar *buffer, RwInt32 bufferLength, const RwChar *driveName, const RwChar *filePath, RwChar cSeparator)
{
    RwInt32 driveLength = strlen(driveName);
    RwInt32 pathLength = strlen(filePath);
    RwInt32 index;

    /* Check for space including null terminator */
    if (driveLength + pathLength > (bufferLength - 1))
    {
        return NULL;
    }

    /* Copy Drive Name */
    rwstrcpy(buffer, driveName);

    /* Copy Path Name, set all slashes to specified separator */
    for (index = 0; index <= pathLength; ++index)
    {
        if (filePath[index] == '/' || filePath[index] == '\\')
        {
            buffer[index + driveLength] = cSeparator;
        }
        else
        {
            buffer[index + driveLength] = filePath[index];
        }
    }
    return buffer;
}
