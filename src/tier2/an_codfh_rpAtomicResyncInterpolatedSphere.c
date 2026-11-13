/* rpAtomicResyncInterpolatedSphere (MFL.exe 004f6100). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwObjectGetPrivateFlags(c) (((const RwObject *)(c))->privateFlags)
#define rwObjectSetPrivateFlags(c,f) (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
#define CS_GLOBAL_GEOMETRY_LOCK() 
#define CS_GLOBAL_GEOMETRY_UNLOCK() 
enum RpInterpolatorFlag
{
    rpINTERPOLATORDIRTYINSTANCE = 0x01,
    rpINTERPOLATORDIRTYSPHERE = 0x02,
    rpINTERPOLATORNOFRAMEDIRTY = 0x04,
    rpINTERPOLATORFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
enum RpAtomicPrivateFlag
{
    rpATOMICPRIVATEWORLDBOUNDDIRTY = 0x01,
    rpATOMICPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
void rpAtomicResyncInterpolatedSphere(RpAtomic * atomic)
{
    RpGeometry         *geom;
	RpInterpolator     *interpolator = &atomic->interpolator;

	CS_GLOBAL_GEOMETRY_LOCK();

    if (atomic->geometry)
    {

		geom = RpAtomicGetGeometry(atomic);

        if ((interpolator->startMorphTarget == interpolator->endMorphTarget)
            || (interpolator->startMorphTarget >= geom->numMorphTargets)
            || (interpolator->endMorphTarget >= geom->numMorphTargets))
        {
            
            if (
                (interpolator->startMorphTarget >= geom->numMorphTargets)
                || (interpolator->endMorphTarget >= geom->numMorphTargets))
            {

                atomic->boundingSphere = geom->morphTarget[0].boundingSphere;
            }
            else
            {

                atomic->boundingSphere =
                    geom->morphTarget[interpolator->
                                      startMorphTarget].boundingSphere;
            }
        }
        else
        {
            
            RpMorphTarget      *startMorphTarget =

                &geom->morphTarget[interpolator->startMorphTarget];
            RpMorphTarget      *endMorphTarget =

                &geom->morphTarget[interpolator->endMorphTarget];
            RwReal              scale =

                ((interpolator->recipTime) * (interpolator->position));

            atomic->boundingSphere.radius =
                ((((((endMorphTarget->boundingSphere.radius)
                     -
                     (startMorphTarget->boundingSphere.radius))) * (scale))) +
                 (startMorphTarget->boundingSphere.radius));

            RwV3dSub(&atomic->boundingSphere.center,
                     &endMorphTarget->boundingSphere.center,
                     &startMorphTarget->boundingSphere.center);
            RwV3dScale(&atomic->boundingSphere.center,
                       &atomic->boundingSphere.center, scale);
            RwV3dAdd(&atomic->boundingSphere.center,
                     &atomic->boundingSphere.center,
                     &startMorphTarget->boundingSphere.center);
        }

        interpolator->flags &= ~rpINTERPOLATORDIRTYSPHERE;

        rwObjectSetPrivateFlags(atomic,
                                rwObjectGetPrivateFlags(atomic) |
                                rpATOMICPRIVATEWORLDBOUNDDIRTY);

    }
	
	CS_GLOBAL_GEOMETRY_UNLOCK();

    return;
}
