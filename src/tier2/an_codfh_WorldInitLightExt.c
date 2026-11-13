/* WorldInitLightExt (MFL.exe 004f3fc0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;
    RwReal green;
    RwReal blue;
    RwReal alpha;
};
typedef struct RpLight RpLight;
struct RpLight
{
        RwObjectHasFrame object;
        RwReal radius;
        RwRGBAReal color;
        RwReal minusCosAngle;
        RwLinkList WorldSectorsInLight;
        RwLLLink inWorld;
        RwUInt16 lightFrame;
        RwUInt16 pad;
};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpLight, object.object.flags) == 0x2);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpLight, object.object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpLight, radius) == 0x14);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpLight, color) == 0x18);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpLight, minusCosAngle) == 0x28);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, sizeof(RpLight) == 0x40);      /* exe: exe static lightTKList.sizeOfStruct at 0x0062ebcc */
RW_LAYOUT_ASSERT(rw37_row_6, sizeof(RwRGBAReal) == 0x10);      /* RpLight: color 0x18, minusCosAngle 0x28 (an_codfh_RpLightStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef struct RpWorldLightExt RpWorldLightExt;
struct RpWorldLightExt
{
    
    RpWorld            *world;

    RwObjectHasFrameSyncFunction oldSync;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldLightExt, oldSync) == 4);      /* MFL.exe 0x4f3fd5: mov [ecx+0x4],edx (the saved sync function) */
#define LIGHTEXTFROMLIGHT(light) ((RpWorldLightExt *)(((RwUInt8 *)(light)) + lightExtOffset))
extern RwInt32      lightExtOffset;
extern RwObjectHasFrame * WorldLightSync(RwObjectHasFrame * object);
/* TIER2: unvalidated authority=diffemu */
void * WorldInitLightExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpLight            *light = (RpLight *) object;
    RpWorldLightExt    *lightExt = LIGHTEXTFROMLIGHT(object);

    lightExt->world = (RpWorld *)NULL;

    lightExt->oldSync = light->object.sync;
    light->object.sync = WorldLightSync;

    return (object);
}
