#include "mfl_tier2.h"
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/* vehicle_get_matrix */

/* (vehicle_index, out_matrix): copies the VEHICLE_MATRIX_FLOATS floats of
   g_vehicle_table[vehicle_index].matrix6 (+0x230) to out_matrix. Returns nothing.
   out_matrix must have room for six floats (inferred). */

/* TIER2: validated 2026-10-18 authority=diffemu */
typedef struct { float f[6]; } M6;
void vehicle_get_matrix(int vehicle_index,float *out_matrix)
{
  char *rec;
  rec = (char *)&g_vehicle_table[vehicle_index];
  *(M6 *)out_matrix = *(M6 *)(rec + 0x230);
}
