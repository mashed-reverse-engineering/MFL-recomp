/* WorldDeInitLightExt (MFL.exe 004f4250). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * LightDestroyTies with _rpLightTieDestroy inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION / RWASSERT
 * dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data
 * and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
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
typedef struct RpLightTie RpLightTie;
struct RpLightTie
{

    RwLLLink lightInWorldSector;
    RpLight *light;

    RwLLLink WorldSectorInLight;
    RpWorldSector *sect;
};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpLight, object.object.flags) == 0x2);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpLight, object.object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpLight, radius) == 0x14);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpLight, color) == 0x18);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpLight, minusCosAngle) == 0x28);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, sizeof(RpLight) == 0x40);      /* exe: exe static lightTKList.sizeOfStruct at 0x0062ebcc */
RW_LAYOUT_ASSERT(rw37_row_6, sizeof(RpLightTie) == 0x18);      /* MFL.exe 0x4f3829 (WorldObjectOpen): push 0x18, entry size of the light tie free list */
RW_LAYOUT_ASSERT(rw37_row_7, sizeof(RwRGBAReal) == 0x10);      /* RpLight: color 0x18, minusCosAngle 0x28 (an_codfh_RpLightStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef struct rpWorldObjGlobals rpWorldObjGlobals;
struct rpWorldObjGlobals
{
    RwFreeList         *tieFreeList;
    RwFreeList         *lightTieFreeList;
    RwUInt32            clumpsInFrustumID;
};
#define RPWORLDOBJGLOBAL(var) (RWPLUGINOFFSET(rpWorldObjGlobals, RwEngineInstance, worldObjModule.globalsOffset)->var)
extern RwModuleInfo worldObjModule;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rpWorldObjGlobals, lightTieFreeList) == 4);      /* MFL.exe 0x4f486b / 0x4f4291 light tie free list [globals+4] */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(RpLightTie, WorldSectorInLight) == 0xc);      /* MFL.exe 0x4f4840 tie = link - 0xc; links at +0 and +0xc unlinked (0x4f4845..0x4f485d) */

/* LightDestroyTies(light) with _rpLightTieDestroy (static / private in baworobj.c, inlined in MFL.exe): unlink every tie of the light
 * from its sector and from the light, give it back to the light tie free list */
#define LIGHTDESTROYTIES(_light)                                        \
MACRO_START                                                             \
{                                                                       \
    RwLLLink           *_cur, *_end;                                    \
    RpLightTie         *_tie;                                           \
                                                                        \
    _cur = rwLinkListGetFirstLLLink(&(_light)->WorldSectorsInLight);    \
    _end = rwLinkListGetTerminator(&(_light)->WorldSectorsInLight);     \
    while (_cur != _end)                                                \
    {                                                                   \
        _tie = rwLLLinkGetData(_cur, RpLightTie, WorldSectorInLight);   \
        _cur = rwLLLinkGetNext(_cur);                                   \
        rwLinkListRemoveLLLink(&_tie->WorldSectorInLight);              \
        rwLinkListRemoveLLLink(&_tie->lightInWorldSector);              \
        RwFreeListFree(RPWORLDOBJGLOBAL(lightTieFreeList), _tie);       \
    }                                                                   \
}                                                                       \
MACRO_STOP

/* TIER2: unvalidated authority=diffemu */
void * WorldDeInitLightExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpLight            *light = (RpLight *) object;

    LIGHTDESTROYTIES(light);

    return (object);
}
