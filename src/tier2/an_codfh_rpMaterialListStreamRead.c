/* _rpMaterialListStreamRead (MFL.exe 0x005040c0): RenderWare Graphics 3.7 (Criterion Software), reads a material list (struct chunk
 * with the material index array, then one material chunk per new material) from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamatlst.c @e578672,
 * not byte-identical, changes: PC build (RwStreamReadInt is the exe's RwStreamReadInt32), RWFUNCTION / RWASSERT / RWRETURN
 * expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN),
 * _rpMaterialListInitialize / _rpMaterialListSetSize / _rpMaterialListDeinitialize / _rpMaterialListGetMaterial /
 * RpMaterialAddRef written out (the exe inlines them here; SetSize on a just initialised list is a plain RwMalloc), no codfh
 * world-arena check, the structs declared here (codfh bamatlst.h / bamateri.h / baerr.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RpMaterial {
    RwTexture *texture;
    RwRGBA color;
    void *pipeline;                     /* RxPipeline * */
    RwReal surfaceProps[3];             /* RwSurfaceProperties */
    short refCount;
    short pad;
} RpMaterial;
typedef struct RpMaterialList {
    RpMaterial **materials;
    RwInt32 numMaterials;
    RwInt32 space;
} RpMaterialList;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_mat_ref, RW_OFFSETOF(RpMaterial, refCount) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_matlist_num, RW_OFFSETOF(RpMaterialList, numMaterials) == 4);
RW_LAYOUT_ASSERT(rw_assert_matlist_space, RW_OFFSETOF(RpMaterialList, space) == 8);

#define rwID_STRUCT 0x1
#define rwID_MATERIAL 0x7
#define rwID_WORLDPLUGIN 0x2
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rwMEMHINTDUR_FUNCTION 0x10000
#define rwMEMHINTDUR_EVENT 0x30000
#define rwMEMHINTFLAG_RESIZABLE 0x1000000
#define rwID_MATLIST 0x8
#define rwID_MATERIALMODULE 0x501
#define E_RW_BADVERSION ((RwInt32)0x80000004)
#define E_RW_NOMEM ((RwInt32)0x80000013)

/*007d2128*/ extern void *RwEngineInstance;
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
/*004f82e0*/ extern RpMaterial *RpMaterialStreamRead(RwStream *stream);
/*004f81d0*/ extern RwBool RpMaterialDestroy(RpMaterial *material);
/*00503e10*/ extern RwInt32 _rpMaterialListAppendMaterial(RpMaterialList *matList, RpMaterial *material);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RWERROR_SET(args) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror args; RwErrorSet(&error); }

/* _rpMaterialListDeinitialize (inlined by the exe) */
#define rpMaterialListDeinitializeInline(matList)                               \
    do {                                                                        \
        RpMaterial **materialArray = (matList)->materials;                      \
        if (materialArray)                                                      \
        {                                                                       \
            RwInt32 materialCount = (matList)->numMaterials;                    \
            RwInt32 nI;                                                         \
            for (nI = 0; nI < materialCount; ++nI)                              \
            {                                                                   \
                RpMaterialDestroy(materialArray[nI]);                           \
                materialArray[nI] = NULL;                                       \
            }                                                                   \
            RwFree(materialArray);                                              \
            (matList)->materials = NULL;                                        \
        }                                                                       \
        (matList)->numMaterials = 0;                                            \
        (matList)->space = 0;                                                   \
    } while (0)

/* TIER2: validated 2026-11-10 authority=diffemu */
RpMaterialList *_rpMaterialListStreamRead(RwStream *stream, RpMaterialList *matList)
{
    RwInt32 i, len, *matindex;
    RwUInt32 size, version;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, &size, &version))
    {
        return NULL;
    }

    if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
    {
        if (!RwStreamReadInt32(stream, &len, sizeof(len)))
        {
            return NULL;
        }

        /* _rpMaterialListInitialize */
        matList->space = 0;
        matList->materials = NULL;
        matList->numMaterials = 0;

        if (len == 0)
        {
            /* zero entry material list simply return the initialized matList we've created */
            return matList;
        }

        /* make the list as large as needed (_rpMaterialListSetSize) */
        if (matList->space < len)
        {
            RpMaterial **materials;
            RwUInt32 memSize = sizeof(RpMaterial *) * len;

            materials = (RpMaterial **)RwMalloc(memSize, rwMEMHINTDUR_EVENT | rwID_MATLIST | rwMEMHINTFLAG_RESIZABLE);
            if (!materials)
            {
                RWERROR_SET((E_RW_NOMEM, memSize));
                rpMaterialListDeinitializeInline(matList);
                return NULL;
            }

            /* Shove in the new */
            matList->materials = materials;
            matList->space = len;
        }

        matindex = (RwInt32 *)RwMalloc(sizeof(RwInt32) * len, rwMEMHINTDUR_FUNCTION | rwID_MATERIALMODULE);

        if (!RwStreamReadInt32(stream, matindex, sizeof(RwInt32) * len))
        {
            RwFree(matindex);
            rpMaterialListDeinitializeInline(matList);
            return NULL;
        }

        for (i = 0; i < len; ++i)
        {
            RpMaterial *material;

            /* new material */
            if (matindex[i] < 0)
            {
                if (!RwStreamFindChunk(stream, rwID_MATERIAL, NULL, &version))
                {
                    RwFree(matindex);
                    rpMaterialListDeinitializeInline(matList);
                    return NULL;
                }
                if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
                {
                    if (!(material = RpMaterialStreamRead(stream)))
                    {
                        RwFree(matindex);
                        rpMaterialListDeinitializeInline(matList);
                        return NULL;
                    }
                }
                else
                {
                    RWERROR_SET((E_RW_BADVERSION));
                    RwFree(matindex);
                    rpMaterialListDeinitializeInline(matList);
                    return NULL;
                }
            }
            else
            {
                material = matList->materials[matindex[i]];      /* _rpMaterialListGetMaterial */
                material->refCount++;                            /* RpMaterialAddRef */
            }

            /* Add the material to the end of the list - this bumps up the reference count */
            _rpMaterialListAppendMaterial(matList, material);

            /* We want to drop the reference count back down, so that only the material list owns it
             * (so it goes away when the material list does) */
            RpMaterialDestroy(material);
        }

        RwFree(matindex);

        /* All done */
        return matList;
    }

    RWERROR_SET((E_RW_BADVERSION));
    return NULL;
}
