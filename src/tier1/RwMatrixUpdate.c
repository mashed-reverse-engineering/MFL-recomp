/* compile: /G5 */
#include "mfl_tier2.h"
/* Clears the identity and orthonormal type bits of the matrix flags; returns the matrix. */
RwMatrix *RwMatrixUpdate(RwMatrix *m)
{
  m->flags &= 0xfffdfffc;
  return m;
}
