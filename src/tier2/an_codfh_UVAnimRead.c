/* UVAnimRead (MFL.exe 00561ec0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/uvanim/rpuvanim.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * _rwStreamReadSingleUVAnim / RpUVAnimAddRef / RpUVAnimDestroy inlined by hand (MFL.exe has them inlined), release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
typedef struct RwSList RwSList;
extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type,
                                RwUInt32 *lengthOut, RwUInt32 *versionOut);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
typedef void (*RtAnimKeyFrameMulRecipCallBack)
                                            (void *voidFrame, void *voidStart);
typedef RtAnimAnimation * (*RtAnimKeyFrameStreamReadCallBack)
                                (RwStream *stream, RtAnimAnimation *animation);
typedef RwBool (*RtAnimKeyFrameStreamWriteCallBack)
                       (const RtAnimAnimation *animation, RwStream *stream);
typedef RwInt32 (*RtAnimKeyFrameStreamGetSizeCallBack)
                                        (const RtAnimAnimation *animation);
struct RtAnimInterpolatorInfo
{
    RwInt32 typeID;

    RwInt32 interpKeyFrameSize;

    RwInt32 animKeyFrameSize;

    RtAnimKeyFrameApplyCallBack keyFrameApplyCB;

    RtAnimKeyFrameBlendCallBack keyFrameBlendCB;

    RtAnimKeyFrameInterpolateCallBack keyFrameInterpolateCB;

    RtAnimKeyFrameAddCallBack keyFrameAddCB;

    RtAnimKeyFrameMulRecipCallBack keyFrameMulRecipCB;

    RtAnimKeyFrameStreamReadCallBack keyFrameStreamReadCB;

    RtAnimKeyFrameStreamWriteCallBack keyFrameStreamWriteCB;

    RtAnimKeyFrameStreamGetSizeCallBack keyFrameStreamGetSizeCB;

    RwInt32 customDataSize;

};
extern RwUInt32
RtAnimAnimationGetNumNodes(const RtAnimAnimation *animation);
extern RtAnimInterpolator *
RtAnimInterpolatorCreate(RwInt32 numNodes,
                                RwInt32 maxInterpKeyFrameSize);
extern RwBool
RtAnimInterpolatorSetCurrentAnim(RtAnimInterpolator *animI,
                               RtAnimAnimation *anim);
typedef struct RtDictSchema RtDictSchema;
typedef struct RtDict RtDict;
typedef void *RtDictEntryType;
typedef const void *RtDictEntryConstType;
typedef void *(RtDictEntryAddRefCallBack)(RtDictEntryType entry);
typedef RwBool (RtDictEntryDestroyCallBack)(RtDictEntryType entry);
typedef const RwChar *(RtDictEntryGetNameCallBack)(RtDictEntryConstType entry);
typedef RwUInt32 (RtDictEntryStreamGetSizeCallBack)(RtDictEntryConstType entry);
typedef RtDictEntryType (RtDictEntryStreamReadCallBack)(RwStream *stream);
typedef RtDict *(RtDictStreamReadCompatibilityCallBack)(RwStream *stream);
typedef const RtDictEntryType (RtDictEntryStreamWriteCallBack)(RtDictEntryConstType entry, RwStream *stream);
struct RtDictSchema
{
    const RwChar *name;

    RwUInt32 dictChunkType;

    RwUInt32 entryChunkType;

    RwUInt32 compatibilityVersion;

    RwSList *dictionaries;

    RtDict *current;

    RtDictEntryAddRefCallBack *addRefCB;

    RtDictEntryDestroyCallBack *destroyCB;

    RtDictEntryGetNameCallBack *getNameCB;

    RtDictEntryStreamGetSizeCallBack *streamGetSizeCB;

    RtDictEntryStreamReadCallBack *streamReadCB;

    RtDictStreamReadCompatibilityCallBack *streamReadCompatibilityCB;

    RtDictEntryStreamWriteCallBack *streamWriteCB;

};
extern RtDict *
RtDictSchemaGetCurrentDict(RtDictSchema *schema);
extern RtDictEntryType
RtDictFindNamedEntry(RtDict *dictionary, const RwChar *name);
#define RP_UVANIM_MAXSLOTS (8)
#define RP_UVANIM_APPLIEDCHANNELS (2)
#define RP_UVANIM_MAXNAME (32)
typedef struct RpUVAnimMaterialGlobalVars RpUVAnimMaterialGlobalVars;
struct RpUVAnimMaterialGlobalVars
{
    RwInt32 engineOffset;
};
typedef struct _rpUVAnimCustomData _rpUVAnimCustomData;
struct _rpUVAnimCustomData
{
    RwChar name[RP_UVANIM_MAXNAME];
    RwUInt32 nodeToUVChannelMap[RP_UVANIM_MAXSLOTS];
    RwUInt32 refCount;
};
typedef RtAnimAnimation RpUVAnim;
typedef struct RpUVAnimMaterialExtension RpUVAnimMaterialExtension;
struct RpUVAnimMaterialExtension
{
    RwMatrix *uv[RP_UVANIM_APPLIEDCHANNELS];

    RtAnimInterpolator *interp[RP_UVANIM_MAXSLOTS];

};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RtAnimInterpolatorInfo) == 0x30);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RtAnimInterpolatorInfo, keyFrameStreamReadCB) == 0x20);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RtAnimInterpolatorInfo, customDataSize) == 0x2c);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RtDictSchema, getNameCB) == 0x20);      /* an_codfh_RtDictFindNamedEntry.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(_rpUVAnimCustomData, nodeToUVChannelMap) == 0x20);      /* an_codfh_rpUVAnimCustomDataStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(_rpUVAnimCustomData, refCount) == 0x40);      /* an_codfh_rpUVAnimCustomDataStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpUVAnimMaterialExtension, interp) == 8);      /* an_codfh_rpuvanim.c */
RW_LAYOUT_ASSERT(rw37_row_7, sizeof(RpUVAnimMaterialGlobalVars) == 4);      /* MFL.exe 0x561ec7 (UVAnimRead): RpUVAnimMaterialGlobals.engineOffset at 0x947b58, the only member */
/* ---- end of the generated declarations ---- */

enum MaterialExtStreamFlags
{
    
    MaterialExtHasChannel0Anim = 0x01,
    MaterialExtHasChannel1Anim = 0x02,
    MaterialExtHasChannel2Anim = 0x04,
    MaterialExtHasChannel3Anim = 0x08,
    MaterialExtHasChannel4Anim = 0x10,
    MaterialExtHasChannel5Anim = 0x20,
    MaterialExtHasChannel6Anim = 0x40,
    MaterialExtHasChannel7Anim = 0x80,
    MaterialExtStreamFlagsSizeInt = RWFORCEENUMSIZEINT
};
typedef enum MaterialExtStreamFlags MaterialExtStreamFlags;
extern RpUVAnimMaterialGlobalVars RpUVAnimMaterialGlobals;
#define RPUVANIMMATERIALGETDATA(material) ( (void *)(((RwUInt8 *)material) + RpUVAnimMaterialGlobals.engineOffset) )
RwStream    *
UVAnimRead(RwStream * stream,
          RwInt32 binaryLength,
          void *object,
          RwInt32 offsetInObject,
          RwInt32 sizeInObject);
extern RpUVAnimMaterialExtension * _rpUVAnimCreateConsistantMatrices(RpUVAnimMaterialExtension *matExt);
extern RpUVAnim * CreateDummyAnim(const RwChar *name);
extern RtDictSchema _rpUVAnimDictSchema;
extern RwFreeList *_rpUVAnimCustomDataFreeList;
extern RtAnimAnimation *RtAnimAnimationDestroy(RtAnimAnimation *animation);
RW_LAYOUT_ASSERT(rw37_own_0, sizeof(struct RpUVAnimMaterialGlobalVars) == 4);      /* MFL.exe 0x561ec7: RpUVAnimMaterialGlobals.engineOffset at 0x947b58 is the only member read */

/* TIER2: unvalidated authority=diffemu */
RwStream * UVAnimRead(RwStream * stream, RwInt32 binaryLength __RWUNUSED__, void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    MaterialExtStreamFlags flags;
    RpUVAnimMaterialExtension *matExt = RPUVANIMMATERIALGETDATA(object);

    if (   !RwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL)
        || !RwStreamReadInt32(stream, (RwInt32 *)&flags, sizeof(flags)))
    {
        return ((RwStream *) NULL);
    }

    {
        size_t i;
        for (i=0; i<RP_UVANIM_MAXSLOTS; ++i)
        {
            if (flags & (1 << i))
            {
                RpUVAnim *anim;

                /* _rwStreamReadSingleUVAnim(stream, &anim) with RpUVAnimGetDictSchema / RpUVAnimAddRef, static in rpuvanim.c, inlined
                 * in MFL.exe: the animation named in the stream from the current dictionary (reference taken), else a dummy */
                {
                    RwChar name[RP_UVANIM_MAXNAME];
                    RtDict *dict;

                    if (!RwStreamRead(stream, name, RP_UVANIM_MAXNAME))
                    {
                        return ((RwStream *)NULL);
                    }

                    dict = RtDictSchemaGetCurrentDict(&_rpUVAnimDictSchema);
                    if (dict)
                    {
                        anim = RtDictFindNamedEntry(dict, name);

                        if (anim)
                        {
                            ++((_rpUVAnimCustomData *)anim->customData)->refCount;
                        }
                        else
                        {
                            anim = CreateDummyAnim(name);
                        }
                    }
                    else
                    {
                        anim = CreateDummyAnim(name);
                    }

                    if (!anim || !stream)
                    {
                        return ((RwStream *)NULL);
                    }
                }

                {
                    RtAnimInterpolator **interp = &matExt->interp[i];

                    if (!(    (*interp = RtAnimInterpolatorCreate(
                                             RtAnimAnimationGetNumNodes(anim),
                                             anim->interpInfo->interpKeyFrameSize))
                           && (RtAnimInterpolatorSetCurrentAnim(*interp, anim))
                         )
                       )
                    {
                        /* RpUVAnimDestroy(anim) with _rpUVAnimDestruct, inlined in MFL.exe: drop the reference; the last one frees
                         * the custom data and the animation */
                        _rpUVAnimCustomData *data = (_rpUVAnimCustomData *)anim->customData;

                        if (--data->refCount == 0)
                        {
                            RwFreeListFree(_rpUVAnimCustomDataFreeList, data);
                            RtAnimAnimationDestroy(anim);
                        }

                        return ((RwStream *)NULL);
                    }
                }
            }
        }

        if (!_rpUVAnimCreateConsistantMatrices(matExt))
        {
            return ((RwStream *)NULL);
        }
    }

    return (stream);
}
