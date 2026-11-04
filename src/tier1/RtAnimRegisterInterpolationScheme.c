/* compile: /G5 */
/* RtAnimRegisterInterpolationScheme (MFL.exe 0x0053ded0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/tool/anim/rtanim.c, function RtAnimRegisterInterpolationScheme), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x0053ded0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <math.h>
#define rwID_ANIMTOOLKIT       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb7)
#define rwPLUGIN_ID      rwID_ANIMTOOLKIT
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned char RwUInt8;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define FALSE 0
#define TRUE !FALSE
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define MAKECHUNKID(vendorID, chunkID) (((vendorID & 0xFFFFFF) << 8) | (chunkID & 0xFF))
enum RwPluginVendor
{
    rwVENDORID_CORE             = 0x000000L,
    rwVENDORID_CRITERIONTK      = 0x000001L,
    rwVENDORID_REDLINERACER     = 0x000002L,
    rwVENDORID_CSLRD            = 0x000003L,
    rwVENDORID_CRITERIONINT     = 0x000004L,
    rwVENDORID_CRITERIONWORLD   = 0x000005L,
    rwVENDORID_BETA             = 0x000006L,
    rwVENDORID_CRITERIONRM      = 0x000007L,
    rwVENDORID_CRITERIONRWA     = 0x000008L, /* RenderWare Audio */
    rwVENDORID_CRITERIONRWP     = 0x000009L, /* RenderWare Physics */
    rwPLUGINVENDORFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPluginVendor RwPluginVendor;
enum RwStreamType
{
    rwNASTREAM = 0,     /**<Invalid stream type */
    rwSTREAMFILE,       /**<File */
    rwSTREAMFILENAME,   /**<File name */
    rwSTREAMMEMORY,     /**<Memory*/
    rwSTREAMCUSTOM,     /**<Custom */
    rwSTREAMTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamType RwStreamType;
enum RwStreamAccessType
{
    rwNASTREAMACCESS = 0,   /**<Invalid stream access */
    rwSTREAMREAD,           /**<Read */
    rwSTREAMWRITE,          /**<Write */
    rwSTREAMAPPEND,         /**<Append */
    rwSTREAMACCESSTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamAccessType RwStreamAccessType;
typedef struct RwStreamMemory RwStreamMemory;
struct RwStreamMemory
{
    RwUInt32            position; /* Current 'memory' position 0 is first byte */
    RwUInt32            nSize;  /* Space allocated currently */
    RwUInt8            *memBlock; /* Current memory block pointer */
};
typedef union RwStreamFile RwStreamFile;
union RwStreamFile
{
    void    *fpFile;               /**< file pointer */
    const void    *constfpFile;    /**< const file pointer */
};
typedef             RwBool(*rwCustomStreamFnClose) (void *data);
typedef             RwUInt32(*rwCustomStreamFnRead) (void *data, void *buffer,
                                                     RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnWrite) (void *data,
                                                    const void *buffer,

                                                    RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnSkip) (void *data,

                                                   RwUInt32 offset);
typedef struct RwStreamCustom RwStreamCustom;
struct RwStreamCustom
{
    rwCustomStreamFnClose sfnclose;
    rwCustomStreamFnRead sfnread;
    rwCustomStreamFnWrite sfnwrite;
    rwCustomStreamFnSkip sfnskip;
    void               *data;
};
typedef union RwStreamUnion RwStreamUnion;
union RwStreamUnion
{
    RwStreamMemory      memory; /**< memory */
    RwStreamFile        file; /**< file */
    RwStreamCustom      custom; /**< custom */
};
typedef struct RwStream RwStream;
struct RwStream
{
    RwStreamType        type;
    RwStreamAccessType  accessType;
    RwInt32             position;
    RwStreamUnion       Type;
    RwBool              rwOwned;
};
typedef struct RwError RwError;
struct RwError
{
    RwInt32     pluginID;  /**< Internal Use */
    RwInt32     errorCode; /**< Internal Use */
};
extern RwError *RwErrorSet(RwError *code);
extern RwInt32 _rwerror(RwInt32 code, ...);
#define RWRETURN(value) return(value)
#define RWERROR(errorcode)                              \
do                                                      \
{                                                       \
    RwError _rwErrorCode;                               \
                                                        \
    _rwErrorCode.pluginID = rwPLUGIN_ID;                \
    _rwErrorCode.errorCode = _rwerror errorcode;        \
                                                        \
    RwErrorSet(&_rwErrorCode);                          \
}                                                       \
while (0)
#define RWAPIFUNCTION(name)
#define RWASSERT(condition)
enum e_rwdb_CriterionAnim
{

E_RT_ANIM_INTERP_IDINUSE,
E_RT_ANIM_INTERP_BLOCKFULL,
E_RT_ANIM_INTERP_IDUNKNOWN,

    e_rwdb_CriterionAnimLAST = RWFORCEENUMSIZEINT
};
typedef enum e_rwdb_CriterionAnim e_rwdb_CriterionAnim;
typedef struct RtAnimAnimation RtAnimAnimation;
typedef void (*RtAnimKeyFrameApplyCallBack) (void *result, void *voidIFrame);
typedef void (*RtAnimKeyFrameBlendCallBack) (void *voidOut, void *voidIn1,
                                             void *voidIn2, RwReal alpha);
typedef void (*RtAnimKeyFrameInterpolateCallBack) (void *voidOut, void *voidIn1,
                                                   void *voidIn2, RwReal time,
                                                   void *customData);
typedef void (*RtAnimKeyFrameAddCallBack) (void *voidOut, void *voidIn1,
                                                    void *voidIn2);
typedef void (*RtAnimKeyFrameMulRecipCallBack)
                                            (void *voidFrame, void *voidStart);
typedef RtAnimAnimation * (*RtAnimKeyFrameStreamReadCallBack)
                                (RwStream *stream, RtAnimAnimation *animation);
typedef RwBool (*RtAnimKeyFrameStreamWriteCallBack)
                       (const RtAnimAnimation *animation, RwStream *stream);
typedef RwInt32 (*RtAnimKeyFrameStreamGetSizeCallBack)
                                        (const RtAnimAnimation *animation);
typedef struct RtAnimInterpolatorInfo RtAnimInterpolatorInfo;
struct RtAnimInterpolatorInfo
{
    RwInt32                             typeID;
            /**< The ID of the interpolation scheme. */
    RwInt32                             interpKeyFrameSize;
            /**< Size, in bytes, of the interpolated keyframe structure. */
    RwInt32                             animKeyFrameSize;
            /**< Size, in bytes, of the animation keyframe structure. */
    RtAnimKeyFrameApplyCallBack         keyFrameApplyCB;
            /**< Pointer to a function that converts a keyframe to the needed
             * format. This function is never called from the \ref rtanim
             * toolkit (as the toolit doesn't know the data layout of the
             * results) but is stored to ease the creation of overloaded
             * interpolators. */
    RtAnimKeyFrameBlendCallBack         keyFrameBlendCB;
            /**< Pointer to a function that blends between two
             *   interpolated keyframes, for the purpose of blending
             *   between the states of two \ref RtAnimInterpolator objects. */
    RtAnimKeyFrameInterpolateCallBack   keyFrameInterpolateCB;
            /**< Pointer to a function that interpolates between two keyframes
             * for a given time in between. */
    RtAnimKeyFrameAddCallBack           keyFrameAddCB;
            /**< Pointer to a function that adds two interpolated keyframes
             *   for the purpose of adding the states of two
             *   \ref RtAnimInterpolator objects. */
    RtAnimKeyFrameMulRecipCallBack      keyFrameMulRecipCB;
            /**< Pointer to a function that multiplies a keyframe by the
             * reciprocal of another. */
    RtAnimKeyFrameStreamReadCallBack    keyFrameStreamReadCB;
            /**< Pointer to a function that reads the keyframes from a stream
             * for a given animation. */
    RtAnimKeyFrameStreamWriteCallBack   keyFrameStreamWriteCB;
            /**< Pointer to a function that writes the keyframes to a stream for
             *  a given animation. */
    RtAnimKeyFrameStreamGetSizeCallBack keyFrameStreamGetSizeCB;
            /**< Pointer to a function that returns the binary stream size of
             * the keyframes for a given animation. */
    RwInt32                             customDataSize;
            /**< Amount of custom data required per animation. */
};
struct RtAnimAnimation
{
    RtAnimInterpolatorInfo              *interpInfo;
            /**< Pointer to interpolation scheme information */
    RwInt32                             numFrames;
            /**< Number of keyframes in the animation  */
    RwInt32                             flags;
            /**< Specifies details about animation,
             * relative translation modes etc. */
    RwReal                              duration;
            /**< Duration of animation in seconds */
    void                                *pFrames;
            /**< Pointer to the animation keyframes  */
    void                                *customData;
            /**< Pointer to custom data for this animation */
};
extern RwBool
RtAnimRegisterInterpolationScheme(RtAnimInterpolatorInfo *interpolatorInfo);
#define RTANIMINTERPOLATORINFOBLOCKMAXENTRIES 16
extern RwInt32 RtAnimInterpolatorInfoBlockNumEntries;
extern RtAnimInterpolatorInfo RtAnimInterpolatorInfoBlock[RTANIMINTERPOLATORINFOBLOCKMAXENTRIES];

/**
 * \ingroup rtanim
 * \ref RtAnimRegisterInterpolationScheme registers a keyframe interpolation
 * scheme. The function must be called before any animations using the scheme
 * are created or read from a stream.
 *
 * An \ref RtAnimInterpolatorInfo structure should be filled with the type ID
 * of the scheme, the size of the keyframe structure and a list of
 * callbacks for processing keyframes, before being passed to this function.
 *
 * The type ID must be unique and it is recommended that values in the
 * range 1 to 255 are reserved for internal use by RenderWare Graphics. It is
 * suggested that developers construct unique IDs using
 * MAKECHUNKID(vendorID, typeID).
 * An animation using the scheme may be created by passing in the type ID.
 *
 * The keyframe structure should always begin with a standard header
 * (\ref RtAnimKeyFrameHeader), i.e. the first 4 bytes hold a pointer to the
 * previous keyframe for the particular node, and the second 4
 * bytes hold the time of the keyframe. This allows the keyframe type to
 * function with \ref RtAnimInterpolatorAddAnimTime etc. The header size
 * should be included in the \e animKeyFrameSize entry. For an interpolated
 * keyframe, a different header is used (\ref RtAnimInterpFrameHeader)
 * which has the same size and should be included in the \e interpKeyFrameSize
 * entry.
 *
 * See \ref RtAnimInterpolatorInfo for information about the callbacks.
 *
 * \param interpInfo    Pointer to the \ref RtAnimInterpolatorInfo structure
 *                      containing the interpolator information. This information
 *                      will be copied to an internal data block.
 *
 * \return \ref RwBool, TRUE if successful, or FALSE if an error occurred.
 *
 * \see RtAnimGetInterpolatorInfo
 */
// Its data (PLAN M5): literals, constants and tables where the image has them.
RwBool
RtAnimRegisterInterpolationScheme(RtAnimInterpolatorInfo *interpInfo)
{
    RWAPIFUNCTION(RWSTRING("RtAnimRegisterInterpolationScheme"));
    RWASSERT(interpInfo);
    RWASSERT(interpInfo->animKeyFrameSize >= 0);
    RWASSERT(interpInfo->interpKeyFrameSize >= 0);

    if (RtAnimInterpolatorInfoBlockNumEntries <
            RTANIMINTERPOLATORINFOBLOCKMAXENTRIES)
    {
        RwInt32    i;

        /* Check that the ID isn't already in use */
        for (i=0; i<RtAnimInterpolatorInfoBlockNumEntries; i++)
        {
            if (RtAnimInterpolatorInfoBlock[i].typeID == interpInfo->typeID)
            {
                RWERROR((E_RT_ANIM_INTERP_IDINUSE));
                RWRETURN(FALSE);
            }
        }

        RtAnimInterpolatorInfoBlock[RtAnimInterpolatorInfoBlockNumEntries]
            = *interpInfo;
        RtAnimInterpolatorInfoBlockNumEntries++;

        RWRETURN(TRUE);
    }

    RWERROR((E_RT_ANIM_INTERP_BLOCKFULL));
    RWRETURN(FALSE);
}
