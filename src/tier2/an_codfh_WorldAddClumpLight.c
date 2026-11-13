/* WorldAddClumpLight (MFL.exe 004f4640). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpWorldAddLight inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN ->
 * return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLinkListAddLLLink(list,linkvar) ( (linkvar)->next = (list)->link.next, (linkvar)->prev = (&(list)->link), ((list)->link.next)->prev = (linkvar), (list)->link.next = (linkvar) )
#define rwObjectGetSubType(o) (((const RwObject *)(o))->subType)
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;
    RwReal green;
    RwReal blue;
    RwReal alpha;
};
struct RwFrame
{
    RwObject object;

    RwLLLink inDirtyListLink;

    RwMatrix modelling;
    RwMatrix ltm;

    RwLinkList objectList;

    struct RwFrame *child;
    struct RwFrame *next;
    struct RwFrame *root;


};
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
#define rpLIGHTPOSITIONINGSTART 0x80
enum RpLightType
{
    rpNALIGHTTYPE = 0,

    rpLIGHTDIRECTIONAL,
    rpLIGHTAMBIENT,

    rpLIGHTPOINT = rpLIGHTPOSITIONINGSTART,
    rpLIGHTSPOT,
    rpLIGHTSPOTSOFT,
    rpLIGHTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpLightType RpLightType;
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
#define RpLightGetFrameMacro(_light) ((RwFrame *)rwObjectGetParent((_light)))
#define RpLightGetTypeMacro(_light) ((RpLightType)rwObjectGetSubType((_light)))
#define RpLightGetFrame(_light) RpLightGetFrameMacro(_light)
#define RpLightGetType(_light) RpLightGetTypeMacro(_light)
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpLight, object.object.flags) == 0x2);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpLight, object.object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwFrame, modelling) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RwFrame, object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpLight, radius) == 0x14);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpLight, color) == 0x18);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpLight, minusCosAngle) == 0x28);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RwFrame, objectList) == 0x90);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RwFrame, child) == 0x98);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RwFrame, next) == 0x9c);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RwFrame, root) == 0xa0);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RwFrame, ltm) == 0x50);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_row_13, sizeof(RwFrame) == 0xa4);      /* exe: exe static frameTKList.sizeOfStruct at 0x0062e514 */
RW_LAYOUT_ASSERT(rw37_row_14, sizeof(RpLight) == 0x40);      /* exe: exe static lightTKList.sizeOfStruct at 0x0062ebcc */
RW_LAYOUT_ASSERT(rw37_row_15, sizeof(RwRGBAReal) == 0x10);      /* RpLight: color 0x18, minusCosAngle 0x28 (an_codfh_RpLightStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef struct RpWorldLightExt RpWorldLightExt;
struct RpWorldLightExt
{
    RpWorld            *world;
    RwObjectHasFrameSyncFunction oldSync;
};
#define LIGHTEXTFROMLIGHT(light) ((RpWorldLightExt *)(((RwUInt8 *)(light)) + lightExtOffset))
extern RwInt32      lightExtOffset;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldLightExt, oldSync) == 4);      /* MFL.exe 0x4f464f lightExt->world [light + lightExtOffset]; 0x4f3ff8 (WorldLightSync) call [ext+4] = oldSync */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(RpWorld, lightList) == 0x34 && RW_OFFSETOF(RpWorld, directionalLightList) == 0x3c && RW_OFFSETOF(RpLight, inWorld) == 0x34);      /* MFL.exe 0x4f4659 world + 0x3c (directional), 0x4f466e world + 0x34 (positional), 0x4f4673 light->inWorld [esi+0x34] */

/* TIER2: unvalidated authority=diffemu */
RpLight * WorldAddClumpLight(RpLight *light, void *data)
{
    /* RpWorldAddLight((RpWorld *) data, light), inlined in MFL.exe: the light joins the world's directional or positional list */
    RpWorld            *world = (RpWorld *) data;

    LIGHTEXTFROMLIGHT(light)->world = world;

    if (RpLightGetType(light) < rpLIGHTPOSITIONINGSTART)
    {
        rwLinkListAddLLLink(&world->directionalLightList, &light->inWorld);
    }
    else
    {
        if (RpLightGetFrame(light))
        {
            RwFrameUpdateObjects(RpLightGetFrame(light));
        }
        rwLinkListAddLLLink(&world->lightList, &light->inWorld);
    }

    return (light);
}
