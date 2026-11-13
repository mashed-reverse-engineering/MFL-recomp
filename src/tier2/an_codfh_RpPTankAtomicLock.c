/* RpPTankAtomicLock (MFL.exe 00545d10). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/rpptank.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RpPTankDataLockFlags
{
    rpPTANKLFLAGNONE = ((int)0x00000000),
    rpPTANKLFLAGPOSITION = ((int)0x00000001),
    rpPTANKLFLAGCOLOR = ((int)0x00000002),
    rpPTANKLFLAGSIZE = ((int)0x00000004),
    rpPTANKLFLAGMATRIX = ((int)0x00000008),
    rpPTANKLFLAGNORMAL = ((int)0x00000010),
    rpPTANKLFLAG2DROTATE = ((int)0x00000020),
    rpPTANKLFLAGVTXCOLOR = ((int)0x00000040),
    rpPTANKLFLAGVTX2TEXCOORDS = ((int)0x00000080),
    rpPTANKLFLAGVTX4TEXCOORDS = ((int)0x00000100),

    RPPTANKLOCKFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpPTankDataLockFlags RpPTankDataLockFlags;
enum RpPTankLockFlags
{
    rpPTANKLOCKWRITE = ((int)0x40000000),
    rpPTANKLOCKREAD = ((int)0x80000000)
};
typedef enum RpPTankLockFlags RpPTankLockFlags;
#define RPPTANKSIZEPOSITION 0
#define RPPTANKSIZEMATRIX 1
#define RPPTANKSIZENORMAL 2
#define RPPTANKSIZESIZE 3
#define RPPTANKSIZECOLOR 4
#define RPPTANKSIZEVTXCOLOR 5
#define RPPTANKSIZE2DROTATE 6
#define RPPTANKSIZEVTX2TEXCOORDS 7
#define RPPTANKSIZEVTX4TEXCOORDS 8
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
RwBool RpPTankAtomicLock(RpAtomic *atomic, RpPTankLockStruct *dst, RwUInt32 dataFlags, RpPTankLockFlags lockFlag)
{
    RpPTankData *ptankGlobal = NULL;

    ptankGlobal = &(RPATOMICPTANKPLUGINDATA(atomic)->publicData);

    if(( ptankGlobal->format.dataFlags & dataFlags) == 0)
    {
        dst->data = NULL;
        dst->stride = 0;
        return (FALSE);
    }

    switch(dataFlags)
    {
        case rpPTANKLFLAGPOSITION:
            *dst = ptankGlobal->clusters[RPPTANKSIZEPOSITION];
            break;
        case rpPTANKLFLAGCOLOR:
            *dst = ptankGlobal->clusters[RPPTANKSIZECOLOR];
            break;
        case rpPTANKLFLAGSIZE:
            *dst = ptankGlobal->clusters[RPPTANKSIZESIZE];
            break;
        case rpPTANKLFLAGMATRIX:
            *dst = ptankGlobal->clusters[RPPTANKSIZEMATRIX];
            break;
        case rpPTANKLFLAGNORMAL:
            *dst = ptankGlobal->clusters[RPPTANKSIZENORMAL];
            break;
        case rpPTANKLFLAG2DROTATE:
            *dst = ptankGlobal->clusters[RPPTANKSIZE2DROTATE];
            break;
        case rpPTANKLFLAGVTXCOLOR:
            *dst = ptankGlobal->clusters[RPPTANKSIZEVTXCOLOR];
            break;
        case rpPTANKLFLAGVTX2TEXCOORDS:
            *dst = ptankGlobal->clusters[RPPTANKSIZEVTX2TEXCOORDS];
            break;
        case rpPTANKLFLAGVTX4TEXCOORDS:
            *dst = ptankGlobal->clusters[RPPTANKSIZEVTX4TEXCOORDS];
            break;
        default:
            
        break;
    }

    if( 0 != ptankGlobal->format.stride )
    {
        dst->stride = ptankGlobal->format.stride;
    }

    if( dst->data && ((lockFlag & rpPTANKLOCKWRITE ) == rpPTANKLOCKWRITE))
    {
        RPATOMICPTANKPLUGINDATA(atomic)->lockFlags |= lockFlag | dataFlags;
    }

    return (TRUE);
}
