/* DefaultGeomAnimCB (MFL.exe 00555850). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/morph/rpmorph.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

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
#define RWATOMICGETANIM(atomic) ((RpMorphAtomicAnim *)(((RwUInt8 *)atomic) + atomicAnimOffset))
typedef struct RpMorphAtomicAnim RpMorphAtomicAnim;
struct RpMorphAtomicAnim
{

    RpMorphInterpolator *interp;
    RwReal              position;
    RpGeometry         *lastGeom;
};
extern RwInt32      atomicAnimOffset;
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(RpMorphInterpolator) == 0x14);      /* an_codfh_rpmorph.c */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpMorphAtomicAnim, lastGeom) == 8);      /* MFL.exe 0x555233 / 0x55523c (RpMorphAtomicSetTime): mov eax,[atomicAnimOffset + atomic + 0x8]; interp at +0 here (0x55585f) */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(RpMorphInterpolator, time) == 8 && RW_OFFSETOF(RpMorphInterpolator, next) == 0x10);      /* MFL.exe 0x555862 fsub [ecx+0x8]; 0x55587a mov eax,[ecx+0x10] */
/* TIER2: unvalidated authority=diffemu */
RwReal DefaultGeomAnimCB(RpAtomic * atomic, RwReal position)
{
    RpMorphAtomicAnim  *atomicAnim;
    RwReal              newPos;

    atomicAnim = RWATOMICGETANIM(atomic);
    newPos = position - atomicAnim->interp->time;
    if (newPos < (RwReal) (0.0))
    {
        newPos = (RwReal) (0.0);
    }

    atomicAnim->interp = atomicAnim->interp->next;

    return (newPos);
}
