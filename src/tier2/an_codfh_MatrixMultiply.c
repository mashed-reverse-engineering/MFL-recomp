/* MatrixMultiply (MFL.exe 004d37b0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamatrix.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define RWASMAPI(TYPE) TYPE
/* ---- end of the generated declarations ---- */

#define RWASMAPI_void RWASMAPI(void)
/* TIER2: unvalidated authority=diffemu */
RWASMAPI_void MatrixMultiply(RwMatrix * dstMat, const RwMatrix * matA, const RwMatrix * matB)
{

    dstMat->right.x = ( (matA->right.x * matB->right.x) +
                        (matA->right.y * matB->up.x) +
                        (matA->right.z * matB->at.x) );
    dstMat->right.y = ( (matA->right.x * matB->right.y) +
                        (matA->right.y * matB->up.y) +
                        (matA->right.z * matB->at.y) );
    dstMat->right.z = (  (matA->right.x * matB->right.z) +
                         (matA->right.y * matB->up.z) +
                         (matA->right.z * matB->at.z) );

    dstMat->up.x = ( (matA->up.x * matB->right.x) +
                     (matA->up.y * matB->up.x) +
                     (matA->up.z * matB->at.x) );
    dstMat->up.y = ( (matA->up.x * matB->right.y) +
                      (matA->up.y * matB->up.y) +
                      (matA->up.z * matB->at.y) );
    dstMat->up.z = ( (matA->up.x * matB->right.z) +
                     (matA->up.y * matB->up.z) +
                     (matA->up.z * matB->at.z) );

    dstMat->at.x = ( (matA->at.x * matB->right.x) +
                     (matA->at.y * matB->up.x) +
                     (matA->at.z * matB->at.x) );
    dstMat->at.y = ( (matA->at.x * matB->right.y) +
                     (matA->at.y * matB->up.y) +
                     (matA->at.z * matB->at.y) );
    dstMat->at.z = ( (matA->at.x * matB->right.z) +
                     (matA->at.y * matB->up.z) +
                     (matA->at.z * matB->at.z) );

    dstMat->pos.x = ( (matA->pos.x * matB->right.x) +
                      (matA->pos.y * matB->up.x) +
                      (matA->pos.z * matB->at.x) +
                      (  matB->pos.x) );
    dstMat->pos.y = ( (matA->pos.x * matB->right.y) +
                      (matA->pos.y * matB->up.y) +
                      (matA->pos.z * matB->at.y) +
                      (  matB->pos.y) );
    dstMat->pos.z = ( (matA->pos.x * matB->right.z) +
                      (matA->pos.y * matB->up.z) +
                      (matA->pos.z * matB->at.z) +
                      (  matB->pos.z) );

    return;
}
