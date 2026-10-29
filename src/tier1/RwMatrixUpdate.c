/* compile: /G5 */
#include "mfl_tier2.h"
/* Clears the matrix type bits (0-1) and the internal identity bit (17) of the flags; returns the matrix. */
RwMatrix *RwMatrixUpdate(RwMatrix *m)
{
  m->flags &= 0xfffdfffc;
  return m;
}
