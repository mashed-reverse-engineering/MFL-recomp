/* RwMatrixStreamRead (MFL.exe 0x004df060): RenderWare Graphics 3.7 (Criterion Software), reads a matrix struct chunk from a stream
 * into the given matrix (or a new one).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/babinmtx.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 / RwMemFloat32ToReal are no-ops and gone), RWAPIFUNCTION / RWASSERT /
 * RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet), memset as a zero initialiser,
 * rwMatrixInitialize / rwMatrixSetFlags written out, the structs declared here (codfh babinmtx.h / bamatrix.h / baerr.h) with
 * compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct rwStreamMatrix {
    RwV3d right;
    RwV3d up;
    RwV3d at;
    RwV3d pos;
    RwInt32 type;
} rwStreamMatrix;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_stream_matrix, sizeof(rwStreamMatrix) == 0x34);
RW_LAYOUT_ASSERT(rw_assert_matrix_flags, RW_OFFSETOF(RwMatrix, flags) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_matrix_pos, RW_OFFSETOF(RwMatrix, pos) == 0x30);
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);

#define rwID_STRUCT 0x1
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rwMATRIXTYPEMASK 0x3
#define rwMATRIXTYPEORTHONORMAL 0x3
#define rwID_COREPLUGIN 0x01
#define E_RW_BADVERSION ((RwInt32)0x80000004)
#define E_RW_READ ((RwInt32)0x8000001a)

/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004d4e70*/ extern RwMatrix *RwMatrixCreate(void);
/*004d3d40*/ extern RwMatrix *RwMatrixOrthoNormalize(RwMatrix *matrixOut, const RwMatrix *matrixIn);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RWERROR_SET(code) { RwError error; error.pluginID = rwID_COREPLUGIN; error.errorCode = _rwerror(code); RwErrorSet(&error); }

/* TIER2: validated 2026-11-10 authority=diffemu */
RwMatrix *RwMatrixStreamRead(RwStream *stream, RwMatrix *matrix)
{
    RwUInt32 size;
    RwUInt32 version;

    if (RwStreamFindChunk(stream, rwID_STRUCT, &size, &version))
    {
        if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
        {
            rwStreamMatrix mat = { 0 };

            if (size == RwStreamRead(stream, &mat, size))
            {
                if (!matrix)
                {
                    /* need to create it */
                    matrix = RwMatrixCreate();
                }
                if (matrix)
                {
                    /* Copy over the type and flags */
                    const RwUInt32 flags = (mat.type & (RwInt32)rwMATRIXTYPEMASK);

                    matrix->flags = 0;      /* rwMatrixInitialize(matrix, 0) */
                    matrix->right = mat.right;
                    matrix->up = mat.up;
                    matrix->at = mat.at;
                    matrix->pos = mat.pos;
                    matrix->flags = flags;  /* rwMatrixSetFlags */

                    if (rwMATRIXTYPEORTHONORMAL == (rwMATRIXTYPEORTHONORMAL & flags))
                    {
                        RwMatrixOrthoNormalize(matrix, matrix);
                    }
                }
            }
            else
            {
                RWERROR_SET(E_RW_READ);
                return NULL;
            }
        }
        else
        {
            RWERROR_SET(E_RW_BADVERSION);
            matrix = NULL;
        }
    }
    else
    {
        RWERROR_SET(E_RW_READ);
        matrix = NULL;
    }
    return matrix;
}
