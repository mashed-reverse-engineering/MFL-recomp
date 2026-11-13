/* RpMorphAtomicSetTime (MFL.exe 00555220). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/morph/rpmorph.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
enum RpInterpolatorFlag
{
    rpINTERPOLATORDIRTYINSTANCE = 0x01,
    rpINTERPOLATORDIRTYSPHERE = 0x02,
    rpINTERPOLATORNOFRAMEDIRTY = 0x04,
    rpINTERPOLATORFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
#define RpAtomicGetInterpolatorMacro(_atomic) (&((_atomic)->interpolator))
#define RpInterpolatorSetStartMorphTargetMacro(_intrp,_target,_atomic) ((_intrp)->startMorphTarget = (RwInt16) (_target), (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE | rpINTERPOLATORDIRTYSPHERE ), ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))? ((RpAtomicGetFrame(_atomic))? (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))): (0)): (0)), (_intrp))
#define RpInterpolatorSetEndMorphTargetMacro(_intrp,_target,_atomic) ((_intrp)->endMorphTarget = (RwInt16) (_target), (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE | rpINTERPOLATORDIRTYSPHERE ), ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))? ((RpAtomicGetFrame(_atomic))? (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))): (0)): (0)), (_intrp))
#define RpInterpolatorSetValueMacro(_intrp,_value,_atomic) ((_intrp)->position = (_value), (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE | rpINTERPOLATORDIRTYSPHERE ), ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))? ((RpAtomicGetFrame(_atomic))? (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))): (0)): (0)), (_intrp))
#define RpInterpolatorSetScaleMacro(_intrp,_scale,_atomic) ((_intrp)->time = (_scale), (_intrp)->recipTime = (RwReal) (1.0) / (_scale), (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE | rpINTERPOLATORDIRTYSPHERE ), ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))? ((RpAtomicGetFrame(_atomic))? (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))): (0)): (0)), (_intrp))
#define RpAtomicGetInterpolator(_atomic) RpAtomicGetInterpolatorMacro(_atomic)
#define RpInterpolatorSetStartMorphTarget(_intrp,_target,_atomic) RpInterpolatorSetStartMorphTargetMacro(_intrp, _target, _atomic)
#define RpInterpolatorSetEndMorphTarget(_intrp,_target,_atomic) RpInterpolatorSetEndMorphTargetMacro(_intrp, _target, _atomic)
#define RpInterpolatorSetValue(_intrp,_value,_atomic) RpInterpolatorSetValueMacro(_intrp, _value, _atomic)
#define RpInterpolatorSetScale(_intrp,_scale,_atomic) RpInterpolatorSetScaleMacro(_intrp, _scale, _atomic)
#define rwID_MORPHPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x05)
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID rwID_MORPHPLUGIN
enum e_rwdb_CriterionMorph
{

E_RP_MORPH_INVALIDINTERP,
E_RP_MORPH_NOGEOMETRY,

    e_rwdb_CriterionMorphLAST = RWFORCEENUMSIZEINT
};
typedef enum e_rwdb_CriterionMorph e_rwdb_CriterionMorph;
typedef struct RpMorphInterpolator RpMorphInterpolator;
struct RpMorphInterpolator
{
    RwInt32  flags; 
    RwInt16  startMorphTarget; 
    RwInt16  endMorphTarget; 
    RwReal   time; 
    RwReal   recipTime; 
    RpMorphInterpolator *next; 
};
typedef RwReal (*RpMorphGeometryCallBack)(RpAtomic *atomic, RwReal position);
extern RpAtomic                 *RpMorphAtomicSetTime(RpAtomic *atomic, RwReal time);
#define RPGEOMETRYGETANIM(geometry) ((RpMorphGeometryAnim *)(((RwUInt8 *)geometry) + geomAnimOffset))
#define RWATOMICGETANIM(atomic) ((RpMorphAtomicAnim *)(((RwUInt8 *)atomic) + atomicAnimOffset))
typedef struct RpMorphGeometryAnim RpMorphGeometryAnim;
struct RpMorphGeometryAnim
{

    RwInt32             numInterpolators;
    RpMorphGeometryCallBack fpAnimCntrlCB;
    RpMorphInterpolator *allInterps;
};
typedef struct RpMorphAtomicAnim RpMorphAtomicAnim;
struct RpMorphAtomicAnim
{

    RpMorphInterpolator *interp;
    RwReal              position;
    RpGeometry         *lastGeom;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpMorphAtomicAnim, position) == 4 && RW_OFFSETOF(RpMorphAtomicAnim, lastGeom) == 8);      /* MFL.exe 0x555266 mov [edi+0x4],ecx (position = time); 0x55523c / 0x555254 lastGeom at +8 (RpMorphAtomicSetTime) */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(RpMorphInterpolator, startMorphTarget) == 4 && RW_OFFSETOF(RpMorphInterpolator, endMorphTarget) == 6 && RW_OFFSETOF(RpMorphInterpolator, time) == 8);      /* MFL.exe 0x555269 / 0x555296 / 0x5552e8 (RpMorphAtomicSetTime) */
extern RwInt32      atomicAnimOffset;
extern RwInt32      geomAnimOffset;
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(RpMorphInterpolator) == 0x14);      /* an_codfh_rpmorph.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpMorphGeometryAnim, allInterps) == 8);      /* an_codfh_rpmorph.c */
/* TIER2: unvalidated authority=diffemu */
RpAtomic * RpMorphAtomicSetTime(RpAtomic * atomic, RwReal time)
{
    RpGeometry         *geometry;

    geometry = RpAtomicGetGeometry(atomic);

    if (geometry)
    {
        RpMorphAtomicAnim  *atomicAnim = RWATOMICGETANIM(atomic);
        RpMorphGeometryAnim *geomAnim = RPGEOMETRYGETANIM(geometry);
        RpInterpolator     *interpolator =
            RpAtomicGetInterpolator(atomic);

        if ((atomicAnim->lastGeom != geometry) || (!atomicAnim->interp))
        {
            atomicAnim->interp = geomAnim->allInterps;
            atomicAnim->lastGeom = geometry;
        }

        if (atomicAnim->interp)
        {
            RwInt16             start;
            RwInt16             end;
            RwReal              scale;

            atomicAnim->position = time;

            start = atomicAnim->interp->startMorphTarget;
            RpInterpolatorSetStartMorphTarget(interpolator, start, atomic);

            end = atomicAnim->interp->endMorphTarget;
            RpInterpolatorSetEndMorphTarget(interpolator, end, atomic);
            RpInterpolatorSetValue(interpolator, atomicAnim->position, atomic);
            scale = atomicAnim->interp->time;
            RpInterpolatorSetScale(interpolator, scale, atomic);
        }

        return (atomic);
    }

    RWERROR((E_RP_MORPH_NOGEOMETRY));
    return ((RpAtomic *)NULL);
}
