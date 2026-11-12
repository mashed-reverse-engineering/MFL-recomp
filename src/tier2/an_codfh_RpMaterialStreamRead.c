/* RpMaterialStreamRead (MFL.exe 0x004f82e0): RenderWare Graphics 3.7 (Criterion Software), reads a material (struct chunk, optional
 * texture chunk, extension chunks) from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamateri.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 / RwMemFloat32ToReal are no-ops and gone), RWAPIFUNCTION / RWASSERT /
 * RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN), memset as a
 * zero initialiser, RpMaterialCreate / RpMaterialDestroy / RpMaterialSetTexture(NULL) / the colour and surface setters written out
 * (the exe inlines them here: free list through RwEngineInstance, no codfh world-arena check), lastSeenRightsPluginId /
 * lastSeenExtraData are the exe's globals, the structs declared here (codfh bamateri.h / baerr.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwSurfaceProperties {
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
} RwSurfaceProperties;
typedef struct _rpMaterial {            /* RpMaterialChunkInfo */
    RwInt32 flags;
    RwRGBA color;
    RwInt32 unused;
    RwBool textured;
    RwSurfaceProperties surfaceProps;
} _rpMaterial;
typedef struct RpMaterial {
    RwTexture *texture;
    RwRGBA color;
    void *pipeline;                     /* RxPipeline * */
    RwSurfaceProperties surfaceProps;
    short refCount;
    short pad;
} RpMaterial;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rpMaterialGlobals {
    void *matFreeList;                  /* RwFreeList * */
} rpMaterialGlobals;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwPluginRegistry RwPluginRegistry;
RW_LAYOUT_ASSERT(rw_assert_rpmat_size, sizeof(_rpMaterial) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_rpmat_textured, RW_OFFSETOF(_rpMaterial, textured) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_mat_color, RW_OFFSETOF(RpMaterial, color) == 4);
RW_LAYOUT_ASSERT(rw_assert_mat_pipeline, RW_OFFSETOF(RpMaterial, pipeline) == 8);
RW_LAYOUT_ASSERT(rw_assert_mat_surf, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_mat_ref, RW_OFFSETOF(RpMaterial, refCount) == 0x18);

#define rwID_STRUCT 0x1
#define rwID_TEXTURE 0x6
#define rwID_WORLDPLUGIN 0x2
#define rwID_MATERIAL 0x7
#define rwMEMHINTDUR_EVENT 0x30000
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define E_RW_BADVERSION ((RwInt32)0x80000004)

/* RwFreeListAlloc / RwFreeListFree: RwGlobals.freeListAlloc 0x118 / freeListFree 0x11c (rw37.h) */
typedef void *(*RwFreeListAllocFunc)(void *, RwUInt32);
typedef void *(*RwFreeListFreeFunc)(void *, void *);
#define RwFreeListAlloc RW_GLOBAL(0x118, RwFreeListAllocFunc)
#define RwFreeListFree RW_GLOBAL(0x11c, RwFreeListFreeFunc)

/*007d2128*/ extern void *RwEngineInstance;
/*007d5390*/ extern RwModuleInfo materialModule;
/*0062ec44*/ extern RwPluginRegistry materialTKList;
/*0062ec5c*/ extern RwSurfaceProperties defaultSurfaceProperties;
/*007d538c*/ extern RwUInt32 g_rp_7d538c;      /* lastSeenRightsPluginId */
/*007d5364*/ extern RwUInt32 g_rp_7d5364;      /* lastSeenExtraData */
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004df350*/ extern RwTexture *RwTextureStreamRead(RwStream *stream);
/*004d5bf0*/ extern RwBool RwTextureDestroy(RwTexture *texture);
/*004e9ce0*/ extern RwBool _rwPluginRegistryInitObject(const RwPluginRegistry *reg, void *object);
/*004e9d40*/ extern RwBool _rwPluginRegistryDeInitObject(const RwPluginRegistry *reg, void *object);
/*004f1ea0*/ extern const void *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg, RwStream *stream, void *object);
/*004f1fd0*/ extern RwBool _rwPluginRegistryInvokeRights(const RwPluginRegistry *reg, RwUInt32 id, void *obj, RwUInt32 extraData);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RWERROR_SET(code) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror(code); RwErrorSet(&error); }
#define lastSeenRightsPluginId g_rp_7d538c
#define lastSeenExtraData g_rp_7d5364
#define RWMATERIALGLOBAL(var) (((rpMaterialGlobals *)((RwUInt8 *)RwEngineInstance + materialModule.globalsOffset))->var)

/* RpMaterialDestroy (inlined by the exe) */
#define RpMaterialDestroyInline(material)                                       \
    do {                                                                        \
        if (1 == (material)->refCount)                                          \
        {                                                                       \
            _rwPluginRegistryDeInitObject(&materialTKList, (material));         \
            if ((material)->texture)                                            \
            {                                                                   \
                RwTextureDestroy((material)->texture);                          \
            }                                                                   \
            (material)->texture = NULL;                                         \
            RwFreeListFree(RWMATERIALGLOBAL(matFreeList), (material));          \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            --(material)->refCount;                                             \
        }                                                                       \
    } while (0)

/* TIER2: validated 2026-11-10 authority=diffemu */
RpMaterial *RpMaterialStreamRead(RwStream *stream)
{
    RwUInt32 size;
    RwUInt32 version;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, &size, &version))
    {
        return NULL;
    }

    if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
    {
        RpMaterial *material;
        _rpMaterial mat = { 0 };

        if (RwStreamRead(stream, &mat, size) != size)
        {
            return NULL;
        }

        /* Create the material (RpMaterialCreate) */
        material = (RpMaterial *)RwFreeListAlloc(RWMATERIALGLOBAL(matFreeList), rwMEMHINTDUR_EVENT | rwID_MATERIAL);
        if (!material)
        {
            return NULL;
        }
        {
            RwRGBA color;

            material->refCount = 1;

            /* White is an appropriate material color (especially for textured materials) */
            color.red = 0xff;
            color.green = 0xff;
            color.blue = 0xff;
            color.alpha = 0xff;

            material->color = color;
            material->texture = NULL;  /* Non textured */

            /* use the default material pipeline */
            material->pipeline = NULL;

            material->surfaceProps = defaultSurfaceProperties;

            /* Initialize memory allocated to toolkits */
            _rwPluginRegistryInitObject(&materialTKList, material);
        }

        /* We want the color */
        material->color = mat.color;

        /* set the surfaceProps */
        material->surfaceProps = mat.surfaceProps;

        /* Check if it has a texture */
        material->texture = NULL;
        if (mat.textured)
        {
            /* Read in the texture */
            if (!RwStreamFindChunk(stream, rwID_TEXTURE, NULL, &version))
            {
                RpMaterialDestroyInline(material);
                return NULL;
            }
            if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
            {
                /* If we don't get the material, the polygons will just be the
                 * color of the underlying material - usually white
                 */
                material->texture = RwTextureStreamRead(stream);
            }
            else
            {
                RpMaterialDestroyInline(material);
                RWERROR_SET(E_RW_BADVERSION);
                return NULL;
            }
        }

        /* A bit ugly. If a rights owner is spotted, this will be set */
        lastSeenRightsPluginId = 0;
        lastSeenExtraData = 0;

        /* MATERIAL EXTENSION DATA */
        if (!_rwPluginRegistryReadDataChunks(&materialTKList, stream, material))
        {
            RpMaterialDestroyInline(material);
            return NULL;
        }

        if (lastSeenRightsPluginId)
        {
            /* Silently ignore errors if any */
            _rwPluginRegistryInvokeRights(&materialTKList, lastSeenRightsPluginId, material, lastSeenExtraData);
        }

        return material;
    }

    RWERROR_SET(E_RW_BADVERSION);
    return NULL;
}
