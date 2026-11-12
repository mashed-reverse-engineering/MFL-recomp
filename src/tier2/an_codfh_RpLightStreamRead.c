/* RpLightStreamRead (MFL.exe 0x004f4d50): RenderWare Graphics 3.7 (Criterion Software), reads a light (struct chunk + extension
 * chunks) from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/balight.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 / RwMemFloat32ToReal are no-ops and gone), RWAPIFUNCTION / RWASSERT /
 * RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN),
 * memset as a zero initialiser, RWMINUSCOSFROMTAN / rwObjectSetPrivateFlags / RpLightSetFlags written out, rwSqrt as the
 * exe's _rwSqrt call, the structs declared here (codfh balight.h / baerr.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct _rpLight {
    RwReal radius;
    RwReal red;
    RwReal green;
    RwReal blue;
    RwReal minusCosAngle;
    RwUInt32 typeAndFlags;
} _rpLight;
typedef struct RpLight {
    RwUInt8 type;                   /* RwObjectHasFrame.object */
    RwUInt8 subType;
    RwUInt8 flags;
    RwUInt8 privateFlags;
    void *parent;
    void *lFrameNext;               /* RwObjectHasFrame.lFrame */
    void *lFramePrev;
    void *sync;                     /* RwObjectHasFrame.sync */
    RwReal radius;
    RwRGBAReal color;
    RwReal minusCosAngle;
} RpLight;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwPluginRegistry RwPluginRegistry;
RW_LAYOUT_ASSERT(rw_assert_rplite_size, sizeof(_rpLight) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_light_flags, RW_OFFSETOF(RpLight, flags) == 2);
RW_LAYOUT_ASSERT(rw_assert_light_pflags, RW_OFFSETOF(RpLight, privateFlags) == 3);
RW_LAYOUT_ASSERT(rw_assert_light_radius, RW_OFFSETOF(RpLight, radius) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_light_color, RW_OFFSETOF(RpLight, color) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_light_cos, RW_OFFSETOF(RpLight, minusCosAngle) == 0x28);

#define rwID_STRUCT 0x1
#define rwID_WORLDPLUGIN 0x2
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rpLIGHTPRIVATENOCHROMA 0x01
#define E_RW_BADVERSION ((RwInt32)0x80000004)

/*0062ebcc*/ extern RwPluginRegistry lightTKList;
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004f4ef0*/ extern RpLight *RpLightCreate(RwInt32 type);
/*004d5410*/ extern RwReal _rwSqrt(const RwReal num);
/*004f1ea0*/ extern const void *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg, RwStream *stream, void *object);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

/* TIER2: validated 2026-11-10 authority=diffemu */
RpLight *RpLightStreamRead(RwStream *stream)
{
    RwUInt32 size;
    RwUInt32 version;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, &size, &version))
    {
        return NULL;
    }

    if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
    {
        RpLight *light;
        _rpLight lite = { 0 };

        if (RwStreamRead(stream, &lite, size) != size)
        {
            return NULL;
        }

        light = RpLightCreate((RwInt32)((lite.typeAndFlags >> 16) & 0xFF));
        if (!light)
        {
            return NULL;
        }

        light->radius = lite.radius;
        light->color.red = lite.red;
        light->color.green = lite.green;
        light->color.blue = lite.blue;

        if (version < 0x30300)
        {
            /* This was stored with tanAngle rather than minusCosAngle (RWMINUSCOSFROMTAN) */
            const RwReal TanAngle2 = lite.minusCosAngle * lite.minusCosAngle;
            const RwReal CosAngle2 = ((RwReal)1) / (((RwReal)1) + TanAngle2);

            light->minusCosAngle = -_rwSqrt(CosAngle2);
        }
        else
        {
            light->minusCosAngle = lite.minusCosAngle;
        }

        if ((light->color.red == light->color.green) && (light->color.red == light->color.blue))
        {
            /* Its a shade of grey, so set the 'grey' flag (no chroma) */
            light->privateFlags = rpLIGHTPRIVATENOCHROMA;
        }
        else
        {
            /* It's got color, turn off the optimisation */
            light->privateFlags = 0;
        }

        light->flags = (RwUInt8)(lite.typeAndFlags & 0xFF);      /* RpLightSetFlags */

        /* LIGHT EXTENSION DATA */
        if (!_rwPluginRegistryReadDataChunks(&lightTKList, stream, light))
        {
            return NULL;
        }

        return light;
    }

    {
        RwError error;

        error.pluginID = rwID_WORLDPLUGIN;
        error.errorCode = _rwerror(E_RW_BADVERSION);
        RwErrorSet(&error);
    }
    return NULL;
}
