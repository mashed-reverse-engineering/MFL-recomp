/* GeomAnimReadStream (MFL.exe 005556b0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/morph/rpmorph.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpMorphGeometryCreateInterpolators inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION / RWASSERT
 * dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data
 * and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
#define RwMemNative32(_m,_s) (_m)
#define RwMemFloat32ToReal(_m,_s) (_m)
#define RwStreamReadInt(_stream,_ints,_numBytes) RwStreamReadInt32(_stream, _ints, _numBytes)
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
/* ---- end of the generated declarations ---- */

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
extern RpGeometry               *RpMorphGeometryCreateInterpolators(RpGeometry *geometry, RwInt32 numInterps);
#define RPGEOMETRYGETANIM(geometry) ((RpMorphGeometryAnim *)(((RwUInt8 *)geometry) + geomAnimOffset))
typedef struct RpMorphGeometryAnim RpMorphGeometryAnim;
struct RpMorphGeometryAnim
{

    RwInt32             numInterpolators;
    RpMorphGeometryCallBack fpAnimCntrlCB;
    RpMorphInterpolator *allInterps;
};
typedef struct _rpBinaryAnimInterpolator _rpBinaryAnimInterpolator;
struct _rpBinaryAnimInterpolator
{
    
    RwInt32             flags;
    RwInt32             startMorphTarget;
    RwInt32             endMorphTarget;
    RwReal              time;

    RwInt32             nextInterpIndex;
};
extern RwInt32      geomAnimOffset;
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(RpMorphInterpolator) == 0x14);      /* an_codfh_rpmorph.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpMorphGeometryAnim, allInterps) == 8);      /* an_codfh_rpmorph.c */
/* TIER2: unvalidated authority=diffemu */
RwStream * GeomAnimReadStream(RwStream * stream, RwInt32 __RWUNUSED__ binaryLength, void *object, RwInt32 __RWUNUSED__ offsetInObject, RwInt32 __RWUNUSED__ sizeInObject)
{
    RpMorphGeometryAnim *geomAnim;
    RwInt32             numInterpolators;
    RwInt32             i;

    geomAnim = RPGEOMETRYGETANIM(object);

    if (!RwStreamReadInt
        (stream, &numInterpolators, sizeof(numInterpolators)))
    {

        return ((RwStream *)NULL);
    }

    if (numInterpolators > 0)
    {
        /* RpMorphGeometryCreateInterpolators((RpGeometry *) object, numInterpolators), inlined in MFL.exe (0x5556ef..0x55575b): the old
         * interpolators freed, a zeroed array of numInterpolators allocated; the result is the geometry */
        {
            RpMorphGeometryAnim *anim = RPGEOMETRYGETANIM(object);
            RwUInt32            size = numInterpolators * sizeof(RpMorphInterpolator);

            if (anim->allInterps)
            {
                RwFree(anim->allInterps);
                anim->numInterpolators = 0;
                anim->allInterps = (RpMorphInterpolator *)NULL;
            }

            anim->allInterps = (RpMorphInterpolator *)RwMalloc(size, 0x30105);      /* rwID_MORPHPLUGIN | rwMEMHINTDUR_EVENT */
            if (!anim->allInterps)
            {
                return ((RwStream *)NULL);
            }
            anim->numInterpolators = numInterpolators;
            memset(anim->allInterps, 0, size);

            if (!object)
            {
                return ((RwStream *)NULL);
            }
        }

        for (i = 0; i < numInterpolators; i++)
        {
            _rpBinaryAnimInterpolator binAnimInterp;
            RwUInt32            size;

            size = sizeof(binAnimInterp);
            if (RwStreamRead(stream, &binAnimInterp, size) != size)
            {

                return ((RwStream *)NULL);
            }

            (void)RwMemNative32(&binAnimInterp, sizeof(binAnimInterp));
            (void)RwMemFloat32ToReal(&binAnimInterp.time,
                               sizeof(binAnimInterp.time));

            geomAnim->allInterps[i].flags = binAnimInterp.flags;
            geomAnim->allInterps[i].startMorphTarget =
                (RwInt16) binAnimInterp.startMorphTarget;
            geomAnim->allInterps[i].endMorphTarget =
                (RwInt16) binAnimInterp.endMorphTarget;
            geomAnim->allInterps[i].time = binAnimInterp.time;
            geomAnim->allInterps[i].recipTime =
                ((RwReal) (1.0)) / (binAnimInterp.time);
            geomAnim->allInterps[i].next =
                &geomAnim->allInterps[binAnimInterp.nextInterpIndex];
        }
    }

    return (stream);
}
