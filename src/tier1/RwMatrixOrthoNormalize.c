/* compile: /G5 */
#include "mfl_tier2.h"
extern void __cdecl MatrixOrthoNormalize(void *a, void *b);
/* Orthonormalises a matrix (forwards to the matrix routine, tail call). */
void RwMatrixOrthoNormalize(void *a, void *b)
{
  MatrixOrthoNormalize(a, b);
}
