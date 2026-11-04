/* compile: /G5 */
/* RpHAnimKeyFrameApply (MFL.exe 0x00542fe0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/plugin/hanim/stdkey.c, function RpHAnimKeyFrameApply), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x00542fe0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <math.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef float RwReal;
#define RWALIGN(type, x)   __declspec(align(x)) type
#define rwMATRIXALIGNMENT 16
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define MACRO_START do
#define MACRO_STOP while(0)
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef struct RwV3d RwV3d;
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};
enum RwMatrixType
{
    rwMATRIXTYPENORMAL = 0x00000001,
    rwMATRIXTYPEORTHOGONAL = 0x00000002,
    rwMATRIXTYPEORTHONORMAL = 0x00000003,
    rwMATRIXTYPEMASK = 0x00000003,
    rwMATRIXTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixType RwMatrixType;
enum RwMatrixFlag
{
    rwMATRIXINTERNALIDENTITY = 0x00020000,
    rwMATRIXFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixFlag RwMatrixFlag;
struct RwMatrixTag
{
    /* These are padded to be 16 byte quantities per line */
    RwV3d               right;
    RwUInt32            flags;
    RwV3d               up;
    RwUInt32            pad1;
    RwV3d               at;
    RwUInt32            pad2;
    RwV3d               pos;
    RwUInt32            pad3;
};
typedef struct RwMatrixTag RWALIGN(RwMatrix, rwMATRIXALIGNMENT);
#define rwMatrixSetFlags(m, flagsbit)     ((m)->flags = (flagsbit))
#define RWRETURNVOID() return
#define RWAPIFUNCTION(name)
#define RWASSERT(condition)
typedef struct RtQuat RtQuat;
struct RtQuat
{
    RwV3d               imag;   /**< The imaginary part(s) */
    RwReal              real;   /**< The real part */
};
#define RtQuatUnitConvertToMatrixMacro(qpQuat, mpMatrix)                   \
MACRO_START                                                                \
{                                                                          \
    const RwReal        x = (qpQuat)->imag.x;                              \
    const RwReal        y = (qpQuat)->imag.y;                              \
    const RwReal        z = (qpQuat)->imag.z;                              \
    const RwReal        w = (qpQuat)->real;                                \
    RwV3d               square;                                            \
    RwV3d               cross;                                             \
    RwV3d               wimag;                                             \
                                                                           \
    square.x = x * x;                                                      \
    square.y = y * y;                                                      \
    square.z = z * z;                                                      \
                                                                           \
    cross.x = y * z;                                                       \
    cross.y = z * x;                                                       \
    cross.z = x * y;                                                       \
                                                                           \
    wimag.x = w * x;                                                       \
    wimag.y = w * y;                                                       \
    wimag.z = w * z;                                                       \
                                                                           \
    (mpMatrix)->right.x = 1 - 2 * (square.y + square.z);                   \
    (mpMatrix)->right.y = 2 * (cross.z + wimag.z);                         \
    (mpMatrix)->right.z = 2 * (cross.y - wimag.y);                         \
                                                                           \
    (mpMatrix)->up.x = 2 * (cross.z - wimag.z);                            \
    (mpMatrix)->up.y = 1 - 2 * (square.x + square.z);                      \
    (mpMatrix)->up.z = 2 * (cross.x + wimag.x);                            \
                                                                           \
    (mpMatrix)->at.x = 2 * (cross.y + wimag.y);                            \
    (mpMatrix)->at.y = 2 * (cross.x - wimag.x);                            \
    (mpMatrix)->at.z = (1 - 2 * (square.x + square.y));                    \
                                                                           \
    /* Set position */                                                     \
    (mpMatrix)->pos.x = ((RwReal) 0);                                      \
    (mpMatrix)->pos.y = ((RwReal) 0);                                      \
    (mpMatrix)->pos.z = ((RwReal) 0);                                      \
                                                                           \
    /* Matrix is orthonormal */                                            \
    rwMatrixSetFlags((mpMatrix),                                           \
                      (rwMATRIXTYPEORTHONORMAL  &                          \
                       ~rwMATRIXINTERNALIDENTITY) );                       \
}                                                                          \
MACRO_STOP
#define RtQuatUnitConvertToMatrix(qpQuat, mpMatrix)                        \
    RtQuatUnitConvertToMatrixMacro(qpQuat, mpMatrix)
typedef struct RpHAnimKeyFrame RpHAnimKeyFrame;
struct RpHAnimKeyFrame
{
    RpHAnimKeyFrame    *prevFrame;  /**< Pointer to the previous keyframe */
    RwReal              time;       /**< Time at keyframe */
    RtQuat              q;          /**< Quaternion rotation at keyframe  */
    RwV3d               t;          /**< Translation at keyframe  */
};
typedef struct RpHAnimInterpFrame RpHAnimInterpFrame;
struct RpHAnimInterpFrame
{
    RpHAnimKeyFrame    *keyFrame1;
        /**< Pointer to 1st keyframe of current interpolation pair */
    RpHAnimKeyFrame    *keyFrame2;
        /**< Pointer to 2nd keyframe of current interpolation pair */
    RtQuat              q;          /**< Quaternion rotation */
    RwV3d               t;          /**< Translation */
};
#define RpHAnimKeyFrameToMatrixMacro(_matrix,_voidIFrame)               \
MACRO_START                                                             \
{                                                                       \
    RpHAnimInterpFrame * iFrame = (RpHAnimInterpFrame *)(_voidIFrame);  \
                                                                        \
    /*                                                                  \
     * RpHAnim uses the same types of quaternion as RtQuat              \
     *  hence no conjugate call as in RpSkin                            \
     */                                                                 \
                                                                        \
    RtQuatUnitConvertToMatrix(&iFrame->q,(_matrix));                    \
                                                                        \
    (_matrix)->pos.x = iFrame->t.x;                                     \
    (_matrix)->pos.y = iFrame->t.y;                                     \
    (_matrix)->pos.z = iFrame->t.z;                                     \
}                                                                       \
MACRO_STOP
extern void
RpHAnimKeyFrameApply(void *matrix,
                     void *voidIFrame);

/****************************************************************************
 Local (static) globals
 */
/****************************************************************************
 Local Function Prototypes
 */
/****************************************************************************
 Functions
 */
/**
 *
 * \ingroup rphanim
 * \ref RpHAnimKeyFrameApply
 *  converts a standard keyframe to a matrix
 *
 * \param pMatrix A pointer to the output matrix
 * \param pVoidIFrame A pointer to the input frame
 *
 * \return None
 */
// Its data (PLAN M5): literals, constants and tables where the image has them.
void
RpHAnimKeyFrameApply(void *pMatrix, void *pVoidIFrame)
{
    RWAPIFUNCTION(RWSTRING("RpHAnimKeyFrameApply"));
    RWASSERT(pMatrix);
    RWASSERT(pVoidIFrame);

    RpHAnimKeyFrameToMatrixMacro((RwMatrix *)pMatrix, pVoidIFrame);

    RWRETURNVOID();
}
