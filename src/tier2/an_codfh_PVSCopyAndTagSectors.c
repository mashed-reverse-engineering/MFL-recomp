/* PVSCopyAndTagSectors (MFL.exe 00548a70). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/pvs/rppvs.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, one
 * reciprocal for the three axis divisions and int32fromreal as the statement macro PVSINT32FROMREAL (MFL.exe inlines both), release build
 * macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK
 * includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked;
 * compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
double __cdecl log(double);
#define RwLog(_x) log(_x)
/* ---- end of the generated declarations ---- */

typedef RwUInt8     RpPVSVisMap;
#define PVSFROMWORLDSECTOR(sector) ((RpPVS *)(((char *)(sector))+rpPVSGlobals.sectorOffset))
enum _rpPVSPartitionId
{
    rpNAPVSPARTITIONID = 0,
    rpPVSFRONT,
    rpPVSBACK,
    rpPVSSPLIT,
    rpPVSCOPLANAR,
    rpPVSPARTITIONIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum _rpPVSPartitionId _rpPVSPartitionId;
typedef struct _rpPVSPolyList _rpPVSPolyList;
typedef struct _rpPVSPolyList *_rpPVSPolyListPtr;
typedef struct RpPVS RpPVS;
struct RpPVS
{
    RwInt32             sectorID;
    RwInt32             vismaplength;
    RwInt32             sampleKey;

    RpPVSVisMap        *vismap;

    _rpPVSPolyListPtr         sectailpoly; 
    _rpPVSPartitionId         potential;

    RwUInt32            numpols;
    RwBBox              sbox;
    RwBBox              gbox;
    RwReal              diagonal;
    RwV3d               centre;
    RwInt32             axessig[3];
};
typedef struct RpPVSGlobalVars RpPVSGlobalVars;
struct RpPVSGlobalVars
{
    RpWorld            *World;

    RwInt32             worldOffset;
    RwInt32             sectorOffset;

    RwBool              collis;
    RwBool              bfc;

    RwInt32             progress_count;
    RwReal              diagonal;

    RwReal              gran;

    RwInt32             InSector;
    RwV3d               ViewPos;
    RpPVS             *CurrPVS;
};
extern RpPVSGlobalVars    rpPVSGlobals;
#define rpPVSMAX(x,y) ((x)>(y) ? (x) : (y))
extern RpPVSGlobalVars     rpPVSGlobals;
/* PVSINT32FROMREAL: the SDK's int32fromreal (truncating fistp under a saved / restored FPU control word, recomp/cheat_allow.txt row
 * PVSINT32FROMREAL as for RwFastRealToUInt32_r), written as a statement macro because MFL.exe inlines it and diffemu cannot relocate a
 * call to a static helper.  Port note: the x87 precision-control bits have no ARM equivalent; the rounding mode is set to truncate here, so
 * a port maps it to a truncating conversion (the NOASM branch), it must not drop it. */
#if defined(NOASM)
#define PVSINT32FROMREAL(res, value) { RwReal pvsValue_ = (value); (res) = (RwInt32)pvsValue_; }
#else
#define PVSINT32FROMREAL(res, value)                                                    \
    {                                                                                  \
        RwReal   pvsValue_ = (value);                                                  \
        RwUInt16 pvsSave_, pvsWork_;                                                   \
        RwInt32  pvsRes_;                                                              \
        __asm { __asm fnstcw pvsSave_ __asm fld dword ptr pvsValue_ __asm mov ax, pvsSave_ __asm or ah, 0ch \
                __asm mov pvsWork_, ax __asm fldcw pvsWork_ __asm fistp dword ptr pvsRes_ __asm fldcw pvsSave_ } \
        (res) = pvsRes_;                                                               \
    }
#endif
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpPVSGlobalVars, InSector) == 0x20);      /* an_codfh_rppvs.c */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RpWorldSector * PVSCopyAndTagSectors(RpWorldSector * sector, void * data __RWUNUSED__)
{
    RpPVS              *pvsCur = PVSFROMWORLDSECTOR(sector);
    RwV3d               v;
    RwReal              max;
    RwV3d               diagshift;
    RwBBox              box;

    box = pvsCur->sbox;

    pvsCur->centre.x =
        (box.sup.x +
         box.inf.x) / 2.0f;
    pvsCur->centre.y =
        (box.sup.y +
         box.inf.y) / 2.0f;
    pvsCur->centre.z =
        (box.sup.z +
         box.inf.z) / 2.0f;

    RwV3dSub(&v, &box.sup, &box.inf);

    diagshift.x = box.sup.x - box.inf.x;
    diagshift.y = box.sup.y - box.inf.y;
    diagshift.z = box.sup.z - box.inf.z;

    max = rpPVSMAX(diagshift.x, diagshift.y);
    max = rpPVSMAX(diagshift.z, max);

    /* MFL.exe divides through one reciprocal (0x548b07 fdivr 1.0, then fmul per axis), not three divisions */
    diagshift.x *= (1.0f / max);
    diagshift.y *= (1.0f / max);
    diagshift.z *= (1.0f / max);

    {
        RwInt32 sig;

        PVSINT32FROMREAL(sig, (float)((((RwLog((1.0f) / diagshift.x)) / RwLog(2.0f)))));
        pvsCur->axessig[0] = -sig;
        PVSINT32FROMREAL(sig, (float)((((RwLog((1.0f) / diagshift.y)) / RwLog(2.0f)))));
        pvsCur->axessig[1] = -sig;
        PVSINT32FROMREAL(sig, (float)((((RwLog((1.0f) / diagshift.z)) / RwLog(2.0f)))));
        pvsCur->axessig[2] = -sig;
    }

    return (sector);
}
