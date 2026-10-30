/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpSolverParams {
  int mode, iterations, f08, f0c, f10, f14, f18, limit_a, limit_b, f24, f28, f2c, f30, f34, f38, f3c;
} RwpSolverParams;
/* Fills the solver parameter block with its default limits. (Rwp library) */
void rwp_trilist_00573f70(RwpSolverParams *p)
{
  p->f10 = 0;
  p->f14 = 0;
  p->f18 = 0;
  p->f08 = 0;
  p->f0c = 0;
  p->limit_a = 100;
  p->limit_b = 100;
  p->f34 = 100;
  p->f30 = 1000;
  p->f38 = 1000;
  p->mode = -1;
  p->iterations = 3;
  p->f24 = 10;
  p->f3c = 40;
  p->f28 = 400;
  p->f2c = 400;
}
