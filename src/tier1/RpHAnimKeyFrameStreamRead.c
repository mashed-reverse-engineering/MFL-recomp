/* compile: /G5 */
/* RpHAnimKeyFrameStreamRead (MFL.exe 0x00543920): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/plugin/hanim/stdkey.c, function RpHAnimKeyFrameStreamRead), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x00543920 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <math.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned char RwUInt8;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef struct RwV3d RwV3d;
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};
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
#define RwStreamReadInt(_stream, _ints, _numBytes) \
        RwStreamReadInt32(_stream, _ints, _numBytes)
extern RwStream *RwStreamReadReal(RwStream *stream, RwReal *reals,
                                  RwUInt32 numBytes);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)
#define RWASSERT(condition)
typedef struct RtQuat RtQuat;
struct RtQuat
{
    RwV3d               imag;   /**< The imaginary part(s) */
    RwReal              real;   /**< The real part */
};
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
typedef struct RpHAnimKeyFrame RpHAnimKeyFrame;
struct RpHAnimKeyFrame
{
    RpHAnimKeyFrame    *prevFrame;  /**< Pointer to the previous keyframe */
    RwReal              time;       /**< Time at keyframe */
    RtQuat              q;          /**< Quaternion rotation at keyframe  */
    RwV3d               t;          /**< Translation at keyframe  */
};
extern RtAnimAnimation *
RpHAnimKeyFrameStreamRead(RwStream *stream,
                             RtAnimAnimation *animation);

/**
 *
 * \ingroup rphanim
 * \ref RpHAnimKeyFrameStreamRead reads a hierarchical animation from a stream.
 *
 * \param stream A pointer to the stream to be read from.
 * \param pAnimation A pointer to the animation into which to read the data
 *
 * \return A pointer to the animation, or NULL if an error occurs
 *
 */
RtAnimAnimation *
RpHAnimKeyFrameStreamRead(RwStream * stream, RtAnimAnimation *pAnimation)
{
    RwInt32             i;
    RwInt32             temp;
    RpHAnimKeyFrame  *pFrames;

    RWAPIFUNCTION(RWSTRING("RpHAnimKeyFrameStreamRead"));
    RWASSERT(stream);
    RWASSERT(pAnimation->pFrames);

    pFrames = (RpHAnimKeyFrame *)pAnimation->pFrames;

    for (i = 0; i < pAnimation->numFrames; ++i)
    {
        if (!RwStreamReadReal
            (stream, (RwReal *) & (pFrames[i].time),
             sizeof(RwReal) * 8))
        {
            RWRETURN((RtAnimAnimation *)NULL);
        }

        if (!RwStreamReadInt
            (stream, (RwInt32 *) & temp, sizeof(RwInt32)))
        {
            RWRETURN((RtAnimAnimation *)NULL);
        }

        pFrames[i].prevFrame =
            &pFrames[temp / sizeof(RpHAnimKeyFrame)];
    }

    RWRETURN(pAnimation);
}
