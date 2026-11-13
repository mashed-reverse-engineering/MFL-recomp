/* DestroyClumpLight (MFL.exe 004f6e40). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpClumpRemoveLight inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN ->
 * return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkInitialize(linkvar) ( (linkvar)->prev = (RwLLLink *)NULL, (linkvar)->next = (RwLLLink *)NULL )
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
typedef struct RpLight RpLight;
extern RwBool RpLightDestroy(RpLight *light);
/* ---- end of the generated declarations ---- */

typedef struct RpClumpLightExt RpClumpLightExt;
struct RpClumpLightExt
{
    RpClump            *clump;
    RwLLLink            inClumpLink;
};
#define CLUMPLIGHTEXTFROMCONSTLIGHT(light) ((const RpClumpLightExt *)(((const RwUInt8 *)(light)) + _rpClumpLightExtOffset))
extern RwInt32  _rpClumpLightExtOffset;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpClumpLightExt, inClumpLink) == 4 && sizeof(RpClumpLightExt) == 0xc);      /* MFL.exe 0x4f6e4d..0x4f6e66: link at ext+4 (next) / ext+8 (prev) unlinked and cleared, ext->clump [ext+0] cleared */
/* TIER2: unvalidated authority=diffemu */
RpLight * DestroyClumpLight(RpLight *light, void *data __RWUNUSED__)
{
    RpClumpLightExt *lightExt = (RpClumpLightExt *)CLUMPLIGHTEXTFROMCONSTLIGHT(light);

    /* RpClumpRemoveLight(lightExt->clump, light), inlined in MFL.exe */
    rwLinkListRemoveLLLink(&lightExt->inClumpLink);
    rwLLLinkInitialize(&lightExt->inClumpLink);
    lightExt->clump = (RpClump *)NULL;

    RpLightDestroy(light);

    return (light);
}
