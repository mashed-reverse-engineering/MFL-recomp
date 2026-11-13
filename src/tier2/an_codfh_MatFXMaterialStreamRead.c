/* MatFXMaterialStreamRead (MFL.exe 0054f9e0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helpers MatFXMaterialGetData / _rpMatFXStreamReadTexture and the setters MFL.exe inlines written out by hand, MatFXGetData called
 * where MFL.exe calls it, the coefficients read with RwStreamReadInt32 as in MFL.exe, release build macros expanded (RWFUNCTION / RWASSERT
 * dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data
 * and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
#define __RWUNUSEDRELEASE__ 
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
enum RwCorePluginID
{
    rwID_NAOBJECT = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION = MAKECHUNKID(rwVENDORID_CORE, 0x03),

    rwID_CAMERA = MAKECHUNKID(rwVENDORID_CORE, 0x05),

    rwID_TEXTURE = MAKECHUNKID(rwVENDORID_CORE, 0x06),

    rwID_MATERIAL = MAKECHUNKID(rwVENDORID_CORE, 0x07),

    rwID_MATLIST = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT = MAKECHUNKID(rwVENDORID_CORE, 0x0A),

    rwID_WORLD = MAKECHUNKID(rwVENDORID_CORE, 0x0B),

    rwID_SPLINE = MAKECHUNKID(rwVENDORID_CORE, 0x0C),

    rwID_MATRIX = MAKECHUNKID(rwVENDORID_CORE, 0x0D),

    rwID_FRAMELIST = MAKECHUNKID(rwVENDORID_CORE, 0x0E),

    rwID_GEOMETRY = MAKECHUNKID(rwVENDORID_CORE, 0x0F),

    rwID_CLUMP = MAKECHUNKID(rwVENDORID_CORE, 0x10),

    rwID_LIGHT = MAKECHUNKID(rwVENDORID_CORE, 0x12),

    rwID_UNICODESTRING = MAKECHUNKID(rwVENDORID_CORE, 0x13),

    rwID_ATOMIC = MAKECHUNKID(rwVENDORID_CORE, 0x14),

    rwID_TEXTURENATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x15),

    rwID_TEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x16),

    rwID_ANIMDATABASE = MAKECHUNKID(rwVENDORID_CORE, 0x17),

    rwID_IMAGE = MAKECHUNKID(rwVENDORID_CORE, 0x18),

    rwID_SKINANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST = MAKECHUNKID(rwVENDORID_CORE, 0x1A),

    rwID_ANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_HANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_TEAM = MAKECHUNKID(rwVENDORID_CORE, 0x1C),

    rwID_CROWD = MAKECHUNKID(rwVENDORID_CORE, 0x1D),

    rwID_DMORPHANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1E),

    rwID_RIGHTTORENDER = MAKECHUNKID(rwVENDORID_CORE, 0x1f),

    rwID_MTEFFECTNATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x20),

    rwID_MTEFFECTDICT = MAKECHUNKID(rwVENDORID_CORE, 0x21),

    rwID_TEAMDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x22),

    rwID_PITEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x23),

    rwID_TOC = MAKECHUNKID(rwVENDORID_CORE, 0x24),

    rwID_PRTSTDGLOBALDATA = MAKECHUNKID(rwVENDORID_CORE, 0x25),

    rwID_ALTPIPE = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH = MAKECHUNKID(rwVENDORID_CORE, 0x28),

    rwID_CHUNKGROUPSTART = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND = MAKECHUNKID(rwVENDORID_CORE, 0x2A),

    rwID_UVANIMDICT = MAKECHUNKID(rwVENDORID_CORE, 0x2B),

    rwID_COLLTREE = MAKECHUNKID(rwVENDORID_CORE, 0x2C),

    rwID_ENVIRONMENT = MAKECHUNKID(rwVENDORID_CORE, 0x2D),

    rwID_COREPLUGINIDMAX = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
    rwCOREPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCorePluginID RwCorePluginID ;
enum RwErrorCodePlugin_errcore
{
    E_RW_BADWINDOW,
    E_RW_CHUNKTYPEGET,
    E_RW_DEVICEERROR,
    E_RW_DEVICEOPEN,
    E_RW_DEVICESTART,
    E_RW_ENDOFSTREAM,
    E_RW_FRAMEDESTROYSTATIC,
    E_RW_FRAMEBUFFERMISMATCH,
    E_RW_FREELISTFREED,
    E_RW_FREELISTINVADDRESS,
    E_RW_FREELISTINVMEMBOUND,
    E_RW_FREELISTTRASH,
    E_RW_INSUFFICIENTRESOURCES,
    E_RW_INVSTREAMACCESSTYPE,
    E_RW_INVSTREAMTYPE,
    E_RW_NEGCLIPPLANE,
    E_RW_NOCAMERA,
    E_RW_NOFRAMEBUFFER,
    E_RW_NOPARENT,
    E_RW_RASTERRECT,
    E_RW_RASTERSTACK,
    E_RW_RASTERSTACKEMPTY,
    E_RW_READTEXMASK,
    E_RW_STREAMOPEN,
    E_RW_SYSTEMREQUEST,
    E_RW_ZEROLENGTH,
    E_RX_CANNOT_TRANSFER_BETWEEN_NODES,
    E_RX_CANNOT_TRANSFER_FROM_NODE_TO_PIPELINE,
    E_RX_CYCLICPIPELINE,
    E_RX_DEP_DEPENDENCIESMISMATCH,
    E_RX_DEP_DUPLICATECLUSTERDEFS,
    E_RX_DEP_NULLCLUSTERDEF,
    E_RX_DEP_OUTOFMEMORY,
    E_RX_EMPTYPIPELINE,
    E_RX_FRAGMENTEDPIPELINE,
    E_RX_IM3DNOTACTIVE,
    E_RX_INVALIDENTRYPOINT,
    E_RX_INVALIDPRIMTYPE,
    E_RX_INVALIDRESUMEPIPELINE,
    E_RX_LOCKEDPIPE,
    E_RX_NODETOOMANYCLUSTERSOFINTEREST,
    E_RX_NODETOOMANYOUTPUTS,
    E_RX_PIPELINETOOMANYNODES,
    E_RX_NODE_EXECUTION_FAILED,
    E_RX_NOVERTS,
    E_RX_PACKETPTRINVALID,
    E_RX_PACKETSCOPYFAILED,
    E_RX_RUNFROMNOTVALID,
    E_RX_RUNTOANDRUNFROM,
    E_RX_RUNTONOTVALID,
    E_RX_TOOMANYVERTS,
    E_RX_UNFINISHEDPIPELINE,
    E_RX_UNLOCKEDPIPE,
    E_RX_UNPROCESSED_PACKETS,
    E_RX_UNSATISFIED_REQUIREMENTS,
    E_RW_INSUFFICIENTRASTER,
    E_RW_INSUFFICIENTDIC,
    rwLASTERROR_errcore = RWFORCEENUMSIZEINT
};
typedef enum RwErrorCodePlugin_errcore RwErrorCodePlugin_errcore;
extern RwError *RwErrorGet(RwError *code);
extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type,
                                RwUInt32 *lengthOut, RwUInt32 *versionOut);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
struct RwRaster
{
    RwRaster *parent;
    RwUInt8 *cpPixels;
    RwUInt8 *palette;
    RwInt32 width, height, depth;
    RwInt32 stride;
    RwInt16 nOffsetX, nOffsetY;
    RwUInt8 cType;
    RwUInt8 cFlags;
    RwUInt8 privateFlags;
    RwUInt8 cFormat;

    RwUInt8 *originalPixels;
    RwInt32 originalWidth;
    RwInt32 originalHeight;
    RwInt32 originalStride;
};
#define RwRasterGetWidthMacro(_raster) ((_raster)->width)
#define RwRasterGetWidth(_raster) RwRasterGetWidthMacro(_raster)
#define rwTEXTUREBASENAMELENGTH 32
typedef struct RwTexDictionary RwTexDictionary;
struct RwTexture
{
    RwRaster *raster;
    RwTexDictionary *dict;
    RwLLLink lInDictionary;

    RwChar name[rwTEXTUREBASENAMELENGTH];
    RwChar mask[rwTEXTUREBASENAMELENGTH];

    RwUInt32 filterAddressing;

    RwInt32 refCount;
};
#define RwTextureGetRasterMacro(_tex) ((_tex)->raster)
#define RwTextureAddRefMacro(_tex) (((_tex)->refCount++), (_tex))
#define RwTextureGetRaster(_tex) RwTextureGetRasterMacro(_tex)
#define RwTextureAddRef(_tex) RwTextureAddRefMacro(_tex)
extern RwBool RwTextureDestroy(RwTexture * texture);
extern RwTexture *
RwTextureStreamRead(RwStream *stream);
struct RpMaterial
{
        RwTexture *texture;
        RwRGBA color;
        RxPipeline *pipeline;
        RwSurfaceProperties surfaceProps;
        RwInt16 refCount;
        RwInt16 pad;
};
#define rwID_MATERIALEFFECTSPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20)
extern RpMaterial *
RpMatFXMaterialSetEffects( RpMaterial *material,
                           RpMatFXMaterialFlags flags );
extern RpMaterial *
RpMatFXMaterialSetBumpMapTexture( RpMaterial *material,
                                  RwTexture *texture );
extern RpMaterial *
RpMatFXMaterialSetDualBlendModes( RpMaterial *material,
                                  RwBlendFunction srcBlendMode,
                                  RwBlendFunction dstBlendMode );
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpMaterial, color) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpMaterial, surfaceProps.ambient) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpMaterial, surfaceProps.diffuse) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpMaterial, surfaceProps.specular) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpMaterial, texture) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwRaster, height) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwRaster, width) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwTexture, name) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RwTexture, filterAddressing) == 0x50);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpMaterial, color) == 4);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpMaterial, pipeline) == 8);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpMaterial, refCount) == 0x18);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, sizeof(RwTexture) == 0x58);      /* exe: exe static textureTKList.sizeOfStruct at 0x0062e6d0 */
RW_LAYOUT_ASSERT(rw37_row_14, sizeof(RwRaster) == 0x34);      /* exe: exe static rasterTKList.sizeOfStruct at 0x0062e718 */
RW_LAYOUT_ASSERT(rw37_row_15, sizeof(RpMaterial) == 0x1c);      /* exe: exe static materialTKList.sizeOfStruct at 0x0062ec44 */
RW_LAYOUT_ASSERT(rw37_row_16, sizeof(RwSurfaceProperties) == 12);      /* RpMaterial: surfaceProps 0xc, refCount 0x18 (an_codfh_RpMaterialStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef struct RwMatFXInfo RwMatFXInfo;
struct RwMatFXInfo
{
        RwModuleInfo Module;
        RwFreeList * MaterialData;
};
extern RwMatFXInfo MatFXInfo;
extern RwStream *
_rpMatFXStreamReadTexture(RwStream * stream,
                          RwTexture ** texture);
#define rpMATFXALIGNMENT rwFRAMEALIGNMENT
typedef struct rpMatFXMaterialData RWALIGN(rpMatFXMaterialData, rpMATFXALIGNMENT);
struct MatFXDualData
{
    RwTexture          *texture;
    RwBlendFunction     srcBlendMode;
    RwBlendFunction     dstBlendMode;
    
};
struct MatFXBumpMapData
{
    RwFrame   *frame;
    RwTexture *texture;
    RwTexture *bumpTexture;
    RwReal    coef;
    RwReal    invBumpWidth;
};
enum MatFXPass
{
    rpSECONDPASS = 0,
    rpTHIRDPASS  = 1,
    rpMAXPASS    = 2
};
typedef enum MatFXPass MatFXPass;
typedef struct MatFXBumpMapData RWALIGN(MatFXBumpMapData, rpMATFXALIGNMENT);
typedef struct MatFXEnvMapData RWALIGN(MatFXEnvMapData, rpMATFXALIGNMENT);
struct MatFXEnvMapData
{
    RwFrame   *frame;
    RwTexture *texture;
    RwReal    coef;
    RwBool    useFrameBufferAlpha;
};
typedef struct MatFXUVAnimData RWALIGN(MatFXUVAnimData, rpMATFXALIGNMENT);
struct MatFXUVAnimData
{
    RwMatrix *baseTransform;
    RwMatrix *dualTransform;
};
typedef struct MatFXDualData RWALIGN(MatFXDualData, rpMATFXALIGNMENT);
typedef union MatFXEffectUnion RWALIGN(MatFXEffectUnion, rpMATFXALIGNMENT);
union MatFXEffectUnion
{
    MatFXBumpMapData  bumpMap;
    MatFXEnvMapData   envMap;
    MatFXDualData     dual;
    MatFXUVAnimData   uvAnim;
    
};
typedef struct MatFXEffectData RWALIGN(MatFXEffectData, rpMATFXALIGNMENT);
struct MatFXEffectData
{
    MatFXEffectUnion     data;
    RpMatFXMaterialFlags flag;
    
};
struct rpMatFXMaterialData
{
    MatFXEffectData      data[rpMAXPASS];
    RpMatFXMaterialFlags flags;
};
extern RwInt32 MatFXMaterialDataOffset;
#define MATFXMATERIALGETDATA(material) ((rpMatFXMaterialData **) (((RwUInt8 *)material)+ MatFXMaterialDataOffset))
extern RwBool
_rpMatFXSetupDualRenderState(MatFXDualData *dualData, RwRenderState nState);
extern RwMatFXInfo MatFXInfo;
#define _rpMatFXMaterialPlatformEnvMapSetBlend(material) 
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RwMatFXInfo, MaterialData) == 8 && RW_OFFSETOF(MatFXEnvMapData, useFrameBufferAlpha) == 0xc);      /* MFL.exe 0x54f9fb MatFXInfo.MaterialData at 0x7da728 (MatFXInfo 0x7da720); 0x54fd0d useFrameBufferAlpha [eax+0xc] */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(MatFXBumpMapData, texture) == 4 && RW_OFFSETOF(MatFXBumpMapData, bumpTexture) == 8 && RW_OFFSETOF(MatFXBumpMapData, coef) == 0xc && RW_OFFSETOF(MatFXBumpMapData, invBumpWidth) == 0x10);      /* MFL.exe 0x54fe7f / 0x54fe82 / 0x54fef4 / 0x54fe90 */
RW_LAYOUT_ASSERT(rw37_own_2, RW_OFFSETOF(MatFXDualData, texture) == 0 && sizeof(MatFXUVAnimData) <= 0x14);      /* MFL.exe 0x54fb73 dual->texture [esi]; uvAnim is a member of the 0x14 byte union */

/* MatFXGetData (MFL.exe 0x54f770, installed row an_codfh_MatFXGetData.c): called where MFL.exe calls it, inlined elsewhere */
extern MatFXEffectUnion * MatFXGetData(RpMaterial * material, RpMatFXMaterialFlags flags);
/* _rpMatFXSetupDualRenderState: empty in the D3D9 build, folded into stub_return_one (0x4a34a0), as in an_codfh_rpmatfx_set.c */
extern RwInt32 stub_return_one(void *data, RwInt32 state);
#define _rpMatFXSetupDualRenderState(dualData, state) stub_return_one((dualData), (state))

/* MatFXGetData(material, flags) inlined: the effect data of the pass that has the effect, else NULL */
#define MATFXGETDATAINLINE(_effect, _material, _flags)                  \
MACRO_START                                                             \
{                                                                       \
    rpMatFXMaterialData *_md = *MATFXMATERIALGETDATA(_material);        \
    RwUInt8             _pass;                                          \
                                                                        \
    (_effect) = (MatFXEffectUnion *)NULL;                               \
    for (_pass = 0; _pass < rpMAXPASS; ++_pass)                         \
    {                                                                   \
        if (_md->data[_pass].flag == (_flags))                          \
        {                                                               \
            (_effect) = &_md->data[_pass].data;                         \
            break;                                                      \
        }                                                               \
    }                                                                   \
}                                                                       \
MACRO_STOP

/* _rpMatFXStreamReadTexture(stream, &texture) (static in rpmatfx.c, inlined in MFL.exe): _ok = what it returns (NULL on failure, else the
 * stream, which the caller tests) */
#define MATFXSTREAMREADTEXTURE(_stream, _texture, _ok)                  \
MACRO_START                                                             \
{                                                                       \
    RwBool              _present;                                       \
    RwError             _err;                                           \
                                                                        \
    (_ok) = FALSE;                                                      \
    if (RwStreamReadInt32((_stream), (RwInt32 *)&_present, sizeof(RwBool))) \
    {                                                                   \
        if (_present)                                                   \
        {                                                               \
            if (RwStreamFindChunk((_stream), rwID_TEXTURE,              \
                                  (RwUInt32 *)NULL, (RwUInt32 *)NULL))  \
            {                                                           \
                RwErrorGet(&_err);                                      \
                *(_texture) = RwTextureStreamRead(_stream);             \
                (_ok) = ((_stream) != NULL);                            \
                if (!*(_texture))                                       \
                {                                                       \
                    RwErrorGet(&_err);                                  \
                    if ((_err.errorCode != E_RW_NOERROR)                \
                        && (_err.errorCode != E_RW_READTEXMASK))        \
                    {                                                   \
                        RwErrorSet(&_err);                              \
                        (_ok) = FALSE;                                  \
                    }                                                   \
                }                                                       \
            }                                                           \
        }                                                               \
        else                                                            \
        {                                                               \
            *(_texture) = (RwTexture *)NULL;                            \
            (_ok) = ((_stream) != NULL);                                \
        }                                                               \
    }                                                                   \
}                                                                       \
MACRO_STOP

/* TIER2: unvalidated authority=diffemu */
RwStream * MatFXMaterialStreamRead(RwStream *stream, RwInt32 binaryLength __RWUNUSEDRELEASE__, void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    RpMaterial             *material;
    rpMatFXMaterialData    *materialData;
    RpMatFXMaterialFlags    flags;
    RwUInt8                 pass;

    material = (RpMaterial *) object;

    /* MatFXMaterialGetData (static, inlined): the material's effect data, created on first use */
    materialData = *MATFXMATERIALGETDATA(material);
    if (NULL == materialData)
    {
        materialData = (rpMatFXMaterialData *)
            RwFreeListAlloc(MatFXInfo.MaterialData,
                rwID_MATERIALEFFECTSPLUGIN | rwMEMHINTDUR_EVENT);
        if (NULL == materialData)
        {
            return ((RwStream *) NULL);
        }
        memset(materialData, 0, sizeof(*materialData));
        *MATFXMATERIALGETDATA(material) = materialData;
    }

    if (!RwStreamReadInt32(stream, (RwInt32 *)&flags, sizeof(RwInt32)))
    {
        return ((RwStream *)NULL);
    }

    RpMatFXMaterialSetEffects(material, flags);

    for (pass = 0; pass < rpMAXPASS; ++pass)
    {
        RpMatFXMaterialFlags effect;
        MatFXEffectUnion   *effectData;
        RwBool              ok;

        if (!RwStreamReadInt32(stream, (RwInt32 *)&effect, sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }

        switch (effect)
        {
            case rpMATFXEFFECTBUMPMAP:
            {
                /* MFL.exe reads the coefficient with RwStreamReadInt32 (the codfh source: RwStreamReadReal) */
                RwReal              coef;
                RwTexture          *texture = NULL;
                RwTexture          *bumpTexture = NULL;
                MatFXBumpMapData   *bumpMapData;

                bumpMapData = &(materialData->data[pass].data.bumpMap);

                if (!RwStreamReadInt32(stream, (RwInt32 *)&coef, sizeof(RwReal)))
                {
                    return ((RwStream *)NULL);
                }

                MATFXSTREAMREADTEXTURE(stream, &texture, ok);
                if (!ok)
                {
                    return ((RwStream *)NULL);
                }

                MATFXSTREAMREADTEXTURE(stream, &bumpTexture, ok);
                if (!ok)
                {
                    if (texture)
                    {
                        RwTextureDestroy(texture);
                    }

                    return ((RwStream *)NULL);
                }

                if (texture != NULL)
                {
                    RwRaster    *raster;
                    RwInt32     nWidth;
                    RwReal      width;

                    bumpMapData->texture = texture;
                    bumpMapData->bumpTexture = bumpTexture;

                    raster = RwTextureGetRaster(bumpMapData->texture);

                    nWidth = RwRasterGetWidth(raster);
                    width = (RwReal) nWidth;

                    bumpMapData->invBumpWidth = 1.0f / width;
                }
                else if (bumpTexture != NULL)
                {
                    RpMatFXMaterialSetBumpMapTexture(material, bumpTexture);

                    RwTextureDestroy(bumpTexture);
                }
                else
                {
                    bumpMapData->texture = (RwTexture *)NULL;
                    bumpMapData->bumpTexture = (RwTexture *)NULL;
                }

                /* RpMatFXMaterialSetBumpMapCoefficient(material, coef), inlined */
                MATFXGETDATAINLINE(effectData, material, rpMATFXEFFECTBUMPMAP);
                effectData->bumpMap.coef = -coef;

                break;
            }
            case rpMATFXEFFECTENVMAP:
            {
                RwReal              coef;
                RwBool              useFrameBufferAlpha;
                RwTexture          *texture=0;

                if (!RwStreamReadInt32(stream, (RwInt32 *)&coef, sizeof(RwReal)))
                {
                    return ((RwStream *)NULL);
                }

                if (!RwStreamReadInt32(stream,
                        (RwInt32 *)&useFrameBufferAlpha, sizeof(RwInt32)))
                {
                    return ((RwStream *)NULL);
                }

                MATFXSTREAMREADTEXTURE(stream, &texture, ok);
                if (!ok)
                {
                    return ((RwStream *)NULL);
                }

                if (texture != NULL)
                {
                    /* RpMatFXMaterialSetEnvMapTexture(material, texture), inlined */
                    MatFXEnvMapData    *envMapData = &MatFXGetData(material, rpMATFXEFFECTENVMAP)->envMap;

                    (void)RwTextureAddRef(texture);
                    if (NULL != envMapData->texture)
                    {
                        RwTextureDestroy(envMapData->texture);
                        envMapData->texture = (RwTexture *)NULL;
                    }
                    envMapData->texture = texture;

                    RwTextureDestroy(texture);
                }

                /* RpMatFXMaterialSetEnvMapCoefficient / RpMatFXMaterialSetEnvMapFrameBufferAlpha, inlined */
                MATFXGETDATAINLINE(effectData, material, rpMATFXEFFECTENVMAP);
                effectData->envMap.coef = coef;
                MATFXGETDATAINLINE(effectData, material, rpMATFXEFFECTENVMAP);
                effectData->envMap.useFrameBufferAlpha = useFrameBufferAlpha;

                break;
            }
            case rpMATFXEFFECTDUAL:
            {
                RwBlendFunction     blendFuncs[2];
                RwTexture          *texture=0;

                if (!RwStreamReadInt32(stream,
                        (RwInt32 *)blendFuncs, sizeof(blendFuncs)))
                {
                    return ((RwStream *)NULL);
                }

                MATFXSTREAMREADTEXTURE(stream, &texture, ok);
                if (!ok)
                {
                    return ((RwStream *)NULL);
                }

                if (texture != NULL)
                {
                    /* RpMatFXMaterialSetDualTexture(material, texture), inlined */
                    MatFXDualData      *dualData = &MatFXGetData(material, rpMATFXEFFECTDUAL)->dual;

                    (void)RwTextureAddRef(texture);
                    if (NULL != dualData->texture)
                    {
                        RwTextureDestroy(dualData->texture);
                        dualData->texture = (RwTexture *)NULL;
                    }
                    dualData->texture = texture;

                    _rpMatFXSetupDualRenderState(dualData, rwRENDERSTATETEXTUREADDRESS);
                    _rpMatFXSetupDualRenderState(dualData, rwRENDERSTATETEXTURERASTER);

                    RwTextureDestroy(texture);
                }

                RpMatFXMaterialSetDualBlendModes(
                    material, blendFuncs[0], blendFuncs[1]);

                break;
            }
            default:
            {
                break;
            }
        }
    }

    return (stream);
}
