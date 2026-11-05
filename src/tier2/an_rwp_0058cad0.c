/* rwp_object_sync_matrix (0x0058cad0): copy the simulated matrix of an Rwp object back to the matrix it follows */
#include "mfl_tier2.h"

/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, RwV3d *translation, RwOpCombineType combineOp);
/*004d3cc0*/ extern RwMatrix *RwMatrixUpdate(RwMatrix *matrix);

typedef struct MflRwpSimSet {
  unsigned char pad00[0x10];
  RwMatrix **matrices;   /* 0x10: [0] = the simulated matrices */
} MflRwpSimSet;

typedef struct MflRwpSim {
  MflRwpSimSet *set;   /* 0x0 */
  int index;   /* 0x4 */
  RwV3d com;   /* 0x8: centre of mass in object space */
} MflRwpSim;

typedef struct MflRwpState {
  void *shape;   /* 0x0 */
  MflRwpSim *sim;   /* 0x4 */
  RwMatrix *matrix;   /* 0x8 */
} MflRwpState;

typedef struct MflRwpObject {
  unsigned char pad00[0x10];
  MflRwpState *state;   /* 0x10 */
} MflRwpObject;

/* The simulation moves the body about its centre of mass: the matrix of the body is copied into the followed matrix, which is then moved
   back by the centre of mass offset (pre-concatenated) and updated.  Returns o. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflRwpObject *rwp_object_sync_matrix(MflRwpObject *o)
{
  MflRwpSim *sim = o->state->sim;
  RwMatrix *m;
  RwV3d back;

  back.x = -sim->com.x;
  back.y = -sim->com.y;
  back.z = -sim->com.z;
  m = o->state->matrix;
  *m = sim->set->matrices[0][sim->index];
  RwMatrixTranslate(m, &back, rwCOMBINEPRECONCAT);
  RwMatrixUpdate(m);
  return o;
}
