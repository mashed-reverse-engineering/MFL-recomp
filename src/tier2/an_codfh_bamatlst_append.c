/* bamatlst append (MFL.exe 0x00503e10): RenderWare Graphics 3.7 (Criterion Software): _rpMaterialListAppendMaterial.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamatlst.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build); no codfh world arena branch (the PC 3.7
 * build reallocates with RwRealloc, or RwMalloc for an empty list); RwMalloc / RwRealloc as the engine's memoryFuncs slots
 * (0x108 / 0x110), RpMaterialAddRef written out (refCount at +0x18); RWERROR expanded as the release build has it (_rwerror +
 * RwErrorSet, plugin ID rwID_WORLDPLUGIN); structs declared here (codfh bamatlst.h / bamateri.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

struct RpMaterial {      /* typedef name RpMaterial comes from mfl_tier2.h */
    void *texture;                      /* 0x00 RwTexture * */
    RwRGBA color;                       /* 0x04 */
    void *pipeline;                     /* 0x08 RxPipeline * */
    RwReal surfaceProps[3];             /* 0x0c RwSurfaceProperties: ambient, specular, diffuse */
    short refCount;                     /* 0x18 */
    short pad;
};
typedef struct RpMaterialList {
    RpMaterial **materials;
    RwInt32 numMaterials;
    RwInt32 space;
} RpMaterialList;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_mat_refcount, RW_OFFSETOF(RpMaterial, refCount) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_matlist_space, RW_OFFSETOF(RpMaterialList, space) == 8);

#define rwMATERIALLISTGRANULARITY 20
#define rwID_MATLIST 0x08
#define rwID_WORLDPLUGIN 0x2
#define rwMEMHINTDUR_EVENT 0x00030000
#define rwMEMHINTFLAG_RESIZABLE 0x01000000
#define E_RW_NOMEM ((RwInt32)0x80000013)

typedef void *(*RwReallocFunc)(void *mem, size_t newSize, RwUInt32 hint);

/*007d2128*/ extern void *RwEngineInstance;
#define RwRealloc (*(RwReallocFunc *)((RwUInt8 *)RwEngineInstance + 0x110))      /* memoryFuncs.rwrealloc; RwMalloc from mfl_tier2.h */

/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwInt32
_rpMaterialListAppendMaterial(RpMaterialList *matList, RpMaterial *material)
{
    RpMaterial **materials;
    RwUInt32 count;
    size_t memSize;

    /* See if there is a blank entry we can use */
    if (matList->space > matList->numMaterials)
    {
        materials = &(matList->materials)[matList->numMaterials];

        (*materials) = material;
        material->refCount++;

        ++matList->numMaterials;

        return matList->numMaterials - 1;
    }

    /* Need to allocates some more space */
    count = (matList->space + rwMATERIALLISTGRANULARITY);

    memSize = sizeof(RpMaterial *) * count;

    if (matList->materials)
    {
        materials = (RpMaterial **)RwRealloc(matList->materials, memSize,
            rwMEMHINTDUR_EVENT | rwID_MATLIST | rwMEMHINTFLAG_RESIZABLE);
    }
    else
    {
        materials = (RpMaterial **)RwMalloc(memSize,
            rwMEMHINTDUR_EVENT | rwID_MATLIST | rwMEMHINTFLAG_RESIZABLE);
    }

    if (!materials)
    {
        RwError error;

        error.pluginID = rwID_WORLDPLUGIN;
        error.errorCode = _rwerror(E_RW_NOMEM, memSize);
        RwErrorSet(&error);
        return -1;
    }

    /* Shove in the new */
    matList->materials = materials;
    matList->space += rwMATERIALLISTGRANULARITY;

    materials[matList->numMaterials] = material;
    material->refCount++;
    ++matList->numMaterials;

    return matList->numMaterials - 1;
}
