/* RpAtomicClone (MFL.exe 004f6ae0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, RpAtomicSetFrame inlined by hand,
 * RpAtomicSetGeometry inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN ->
 * return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
#define rwObjectCopy(d,s) MACRO_START { ((RwObject *)(d))->type = ((const RwObject *)(s))->type; ((RwObject *)(d))->subType = ((const RwObject *)(s))->subType; ((RwObject *)(d))->flags = ((const RwObject *)(s))->flags; ((RwObject *)(d))->privateFlags = ((const RwObject *)(s))->privateFlags; ((RwObject *)(d))->parent = NULL; } MACRO_STOP
#define rwObjectGetFlags(o) (((const RwObject *)(o))->flags)
#define rwObjectSetFlags(o,f) (((RwObject *)(o))->flags) = (RwUInt8)(f)
#define rwObjectGetPrivateFlags(c) (((const RwObject *)(c))->privateFlags)
#define rwObjectSetPrivateFlags(c,f) (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
struct RwPluginRegistry
{
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
extern const
RwPluginRegistry *_rwPluginRegistryCopyObject(const RwPluginRegistry * reg,
                                             void *dstObject,
                                             const void *srcObject);
#define rwPluginRegistryCopyObject(reg,dstObject,srcObject) _rwPluginRegistryCopyObject(reg, dstObject, srcObject)
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
extern void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame);
#define rwObjectHasFrameSetFrame(object,frame) _rwObjectHasFrameSetFrame(object, frame)
struct RpMaterial
{
        RwTexture *texture;
        RwRGBA color;
        RxPipeline *pipeline;
        RwSurfaceProperties surfaceProps;
        RwInt16 refCount;
        RwInt16 pad;
};
extern RwBool
RpGeometryDestroy(RpGeometry *geometry);
typedef struct RpVertexNormal RpVertexNormal;
struct RpWorldSector
{
    RwInt32 type;

    RpTriangle *triangles;

    RwV3d *vertices;
    RpVertexNormal *normals;

    RwTexCoords *texCoords[rwMAXTEXTURECOORDS];

    RwRGBA *preLitLum;

    RwResEntry *repEntry;

    RwLinkList collAtomicsInWorldSector;

    RwLinkList lightsInWorldSector;

    RwBBox boundingBox;

    RwBBox tightBoundingBox;

    RpMeshHeader *mesh;

    RxPipeline *pipeline;

    RwUInt16 matListWindowBase;

    RwUInt16 numVertices;
    RwUInt16 numTriangles;
    RwUInt16 pad;
};
struct RpSector
{
    RwInt32 type;
};
enum RpAtomicPrivateFlag
{
    rpATOMICPRIVATEWORLDBOUNDDIRTY = 0x01,
    rpATOMICPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
#define RpAtomicSetFlagsMacro(_atomic,_flags) (rwObjectSetFlags(_atomic, _flags), (_atomic))
#define RpAtomicGetFlagsMacro(_atomic) (rwObjectGetFlags(_atomic))
#define RpAtomicSetFlags(_atomic,_flags) RpAtomicSetFlagsMacro(_atomic, _flags)
#define RpAtomicGetFlags(_atomic) RpAtomicGetFlagsMacro(_atomic)
extern RpAtomic *
RpAtomicCreate(void);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpMaterial, color) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpMaterial, surfaceProps.ambient) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpMaterial, surfaceProps.diffuse) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpMaterial, surfaceProps.specular) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpMaterial, texture) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpMaterial, color) == 4);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpMaterial, pipeline) == 8);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpMaterial, refCount) == 0x18);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_16, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_17, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_18, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_19, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_20, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_21, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_22, sizeof(RpMaterial) == 0x1c);      /* exe: exe static materialTKList.sizeOfStruct at 0x0062ec44 */
RW_LAYOUT_ASSERT(rw37_row_23, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
RW_LAYOUT_ASSERT(rw37_row_24, sizeof(RwSurfaceProperties) == 12);      /* RpMaterial: surfaceProps 0xc, refCount 0x18 (an_codfh_RpMaterialStreamRead.c) */
RW_LAYOUT_ASSERT(rw37_row_25, RW_OFFSETOF(RpSector, type) == 0);      /* MFL.exe 0x4f410d / 0x4f4111 (WorldLightSync): mov ecx,[esi]; test ecx,ecx; jge = sect->type < 0 (world sector); the plane sector's type at the same offset 0x4f417f */
/* ---- end of the generated declarations ---- */

extern RwPluginRegistry atomicTKList;
extern RpWorld *RpAtomicGetWorld(const RpAtomic *atomic);
extern RpGeometry *RpGeometryAddRef(RpGeometry *geometry);

/* TIER2: unvalidated authority=diffemu */
RpAtomic * RpAtomicClone(RpAtomic *atomic)
{
    RpAtomic *newAtomic;

    newAtomic = RpAtomicCreate();
    if (!newAtomic)
    {
        return ((RpAtomic *)NULL);
    }

    rwObjectCopy(newAtomic, atomic);

    RpAtomicSetFlags(newAtomic, RpAtomicGetFlags(atomic));

    /* RpAtomicSetFrame(newAtomic, NULL), inlined in MFL.exe: the frame link, then the world bounding sphere marked dirty */
    rwObjectHasFrameSetFrame(newAtomic, (RwFrame *)NULL);
    rwObjectSetPrivateFlags(newAtomic, rwObjectGetPrivateFlags(newAtomic) | rpATOMICPRIVATEWORLDBOUNDDIRTY);

    /* RpAtomicSetGeometry(newAtomic, atomic->geometry, 0), inlined in MFL.exe (0x4f6b24..0x4f6b94): reference the new geometry,
     * release the old one, take the geometry's first morph target bounding sphere, resync the frame's objects when in a world */
    {
        RpGeometry *geometry = atomic->geometry;

        if (geometry != newAtomic->geometry)
        {
            RwFrame    *frame;

            if (geometry)
            {
                RpGeometryAddRef(geometry);
            }
            if (newAtomic->geometry)
            {
                RpGeometryDestroy(newAtomic->geometry);
            }
            newAtomic->geometry = geometry;
            if (geometry)
            {
                newAtomic->boundingSphere = geometry->morphTarget[0].boundingSphere;
            }
            frame = RpAtomicGetFrame(newAtomic);
            if (frame && RpAtomicGetWorld(newAtomic))
            {
                RwFrameUpdateObjects(frame);
            }
        }
    }
    newAtomic->boundingSphere = atomic->boundingSphere;
    newAtomic->renderCallBack = atomic->renderCallBack;
    newAtomic->pipeline = atomic->pipeline;

    newAtomic->interpolator = atomic->interpolator;

    rwPluginRegistryCopyObject(&atomicTKList, newAtomic, atomic);

    return (newAtomic);
}
