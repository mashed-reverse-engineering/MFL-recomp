/* RtCharsetBufferFlush (MFL.exe 00568b20). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/charse/chrprint.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwCullMode
{
    rwCULLMODENACULLMODE = 0,

    rwCULLMODECULLNONE,

    rwCULLMODECULLBACK,

    rwCULLMODECULLFRONT,

    rwCULLMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCullMode RwCullMode;
#define RwRenderStateGetMacro(_state,_value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateGet(_state, _value))
#define RwRenderStateSetMacro(_state,_value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateSet(_state, _value))
#define RwIm2DRenderIndexedPrimitiveMacro(_primType,_vertices,_numVertices,_indices,_numIndices) (RWSRCGLOBAL(dOpenDevice).fpIm2DRenderIndexedPrimitive(_primType, _vertices, _numVertices, _indices, _numIndices))
#define RwRenderStateGet(_state,_value) RwRenderStateGetMacro(_state, _value)
#define RwRenderStateSet(_state,_value) RwRenderStateSetMacro(_state, _value)
#define RwIm2DRenderIndexedPrimitive(_primType,_vertices,_numVertices,_indices,_numIndices) RwIm2DRenderIndexedPrimitiveMacro(_primType, _vertices, _numVertices, _indices, _numIndices)
/* ---- end of the generated declarations ---- */

struct _RtCharsetBuffer
{
    RwBool              initialised;
    RtCharset          *charSet;
    RwUInt32            numChars;
    RwIm2DVertex       *vertices;
    RwImVertexIndex    *indices;
};
typedef struct _RtCharsetBuffer RtCharsetBuffer;
extern RtCharsetBuffer _rtgBuffer;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(struct _RtCharsetBuffer, numChars) == 8 && RW_OFFSETOF(struct _RtCharsetBuffer, vertices) == 0xc && RW_OFFSETOF(struct _RtCharsetBuffer, indices) == 0x10);      /* MFL.exe 0x568b20 / 0x568c0f / 0x568c03: numChars 0x7da9f4, vertices 0x7da9f8, indices 0x7da9fc; _rtgBuffer is at 0x7da9ec */
/* TIER2: unvalidated authority=diffemu */
RwBool RtCharsetBufferFlush(void)
{

    if (_rtgBuffer.numChars > 0)
    {
        RwBool              zTestEnable, zWriteEnable;
        RwInt32             srcBlend, dstBlend;
        RwRaster           *txtRaster;
        RwInt32             txtFilter;
        RwCullMode          cullMode;

        RwRenderStateGet(rwRENDERSTATEZTESTENABLE,
                         (void *) &zTestEnable);
        RwRenderStateGet(rwRENDERSTATEZWRITEENABLE,
                         (void *) &zWriteEnable);

        RwRenderStateGet(rwRENDERSTATESRCBLEND, (void *) &srcBlend);
        RwRenderStateGet(rwRENDERSTATEDESTBLEND, (void *) &dstBlend);

        RwRenderStateGet(rwRENDERSTATETEXTURERASTER,
                         (void *) &txtRaster);
        RwRenderStateGet(rwRENDERSTATETEXTUREFILTER,
                         (void *) &txtFilter);

        RwRenderStateGet(rwRENDERSTATECULLMODE, (void *) &cullMode);

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) (mfl_uptr) FALSE);

        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *) (mfl_uptr) FALSE);

        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,
                         (void *) (mfl_uptr) rwFILTERNEAREST);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,
                         (void *) _rtgBuffer.charSet);

        RwRenderStateSet(rwRENDERSTATESRCBLEND,
                         (void *) (mfl_uptr) rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,
                         (void *) (mfl_uptr) rwBLENDINVSRCALPHA);

        RwRenderStateSet(rwRENDERSTATECULLMODE,
                         (void *) (mfl_uptr) rwCULLMODECULLNONE);

        RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST,
                                     _rtgBuffer.vertices,
                                     4 *
                                     _rtgBuffer.numChars,
                                     _rtgBuffer.indices,
                                     6 * _rtgBuffer.numChars);

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,
                         (void *) (mfl_uptr) zTestEnable);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,
                         (void *) (mfl_uptr) zWriteEnable);

        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) (mfl_uptr) srcBlend);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) (mfl_uptr) dstBlend);

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,
                         (void *) txtRaster);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,
                         (void *) (mfl_uptr) txtFilter);

        RwRenderStateSet(rwRENDERSTATECULLMODE, (void *) (mfl_uptr) cullMode);
    }

    _rtgBuffer.numChars = 0;

    _rtgBuffer.charSet = (RwRaster *) NULL;

    return (TRUE);
}
