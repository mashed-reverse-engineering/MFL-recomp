/* bamateri (MFL.exe 0x004f8140 / 0x004f81d0): RenderWare Graphics 3.7 (Criterion Software), material lifecycle: RpMaterialCreate and
 * RpMaterialDestroy.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamateri.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build); codfh's world-arena branches (the
 * RwMalloc path of RpMaterialCreate, the dword_50F250 range test of RpMaterialDestroy) are not in the exe; RwFreeListAlloc /
 * RwFreeListFree as the engine's freeListAlloc / freeListFree slots (0x118 / 0x11c); RWMATERIALGLOBAL(matFreeList) as the word at
 * materialModule.globalsOffset in *RwEngineInstance; RpMaterialSetColor / SetSurfaceProperties / SetTexture(NULL) expanded; the
 * struct declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwSurfaceProperties {
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
} RwSurfaceProperties;

struct RpMaterial {      /* typedef name RpMaterial comes from mfl_tier2.h */
    RwTexture *texture;                 /* 0x00 */
    RwRGBA color;                       /* 0x04 */
    RxPipeline *pipeline;               /* 0x08 */
    RwSurfaceProperties surfaceProps;   /* 0x0c */
    short refCount;                     /* 0x18 */
    short pad;
};
RW_LAYOUT_ASSERT(rw_assert_material_color, RW_OFFSETOF(RpMaterial, color) == 4);
RW_LAYOUT_ASSERT(rw_assert_material_surf, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_material_ref, RW_OFFSETOF(RpMaterial, refCount) == 0x18);

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;

#define rwID_MATERIAL 0x07
#define rwMEMHINTDUR_EVENT 0x00030000

typedef void *(*RwFreeListAllocFunc)(void *freeList, RwUInt32 hint);
typedef void *(*RwFreeListFreeFunc)(void *freeList, void *entry);

/*007d2128*/ extern void *RwEngineInstance;
/*007d5390*/ extern RwModuleInfo materialModule;
/*0062ec44*/ extern RwUInt8 materialTKList[];
/*0062ec5c*/ extern RwSurfaceProperties defaultSurfaceProperties;

#define RW_ENGINE_SLOT(off, type) (*(type *)((RwUInt8 *)RwEngineInstance + (off)))
#define RwFreeListAlloc RW_ENGINE_SLOT(0x118, RwFreeListAllocFunc)
#define RwFreeListFree RW_ENGINE_SLOT(0x11c, RwFreeListFreeFunc)
#define RWMATERIALGLOBAL_matFreeList RW_ENGINE_SLOT(materialModule.globalsOffset, void *)

/*004e9ce0*/ extern void *_rwPluginRegistryInitObject(void *reg, void *object);
/*004e9d40*/ extern void *_rwPluginRegistryDeInitObject(void *reg, void *object);
/*004d5bf0*/ extern RwBool RwTextureDestroy(RwTexture *texture);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpMaterial *
RpMaterialCreate(void)
{
    RpMaterial *material;
    RwRGBA color;

    material = (RpMaterial *)RwFreeListAlloc(RWMATERIALGLOBAL_matFreeList, rwMEMHINTDUR_EVENT | rwID_MATERIAL);
    if (!material)
    {
        return (RpMaterial *)NULL;
    }

    material->refCount = 1;

    /* White is an appropriate material color (especially for textured materials) */
    color.red = 0xff;
    color.green = 0xff;
    color.blue = 0xff;
    color.alpha = 0xff;

    material->color = color;
    material->texture = (RwTexture *)NULL;
    material->pipeline = (RxPipeline *)NULL;
    material->surfaceProps = defaultSurfaceProperties;

    _rwPluginRegistryInitObject(materialTKList, material);

    return material;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool
RpMaterialDestroy(RpMaterial *material)
{
    if (1 == material->refCount)
    {
        _rwPluginRegistryDeInitObject(materialTKList, material);

        /* RpMaterialSetTexture(material, NULL) */
        if (material->texture)
        {
            RwTextureDestroy(material->texture);
        }
        material->texture = (RwTexture *)NULL;

        RwFreeListFree(RWMATERIALGLOBAL_matFreeList, material);
    }
    else
    {
        --material->refCount;
    }

    return TRUE;
}
