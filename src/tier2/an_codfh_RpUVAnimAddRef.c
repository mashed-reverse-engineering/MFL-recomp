/* RpUVAnimAddRef (MFL.exe 00562300). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/uvanim/rpuvanim.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define RP_UVANIM_MAXSLOTS (8)
#define RP_UVANIM_MAXNAME (32)
typedef struct _rpUVAnimCustomData _rpUVAnimCustomData;
struct _rpUVAnimCustomData
{
    RwChar name[RP_UVANIM_MAXNAME];
    RwUInt32 nodeToUVChannelMap[RP_UVANIM_MAXSLOTS];
    RwUInt32 refCount;
};
typedef RtAnimAnimation RpUVAnim;
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(_rpUVAnimCustomData, nodeToUVChannelMap) == 0x20);      /* an_codfh_rpUVAnimCustomDataStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(_rpUVAnimCustomData, refCount) == 0x40);      /* an_codfh_rpUVAnimCustomDataStreamRead.c */
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
RpUVAnim * RpUVAnimAddRef(RpUVAnim *anim)
{

    {
        _rpUVAnimCustomData *data=(_rpUVAnimCustomData *)anim->customData;
        ++data->refCount;
    }
    return (anim);
}
