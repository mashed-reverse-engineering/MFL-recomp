#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*00659b30*/ extern undefined4 g_load_659b30;
/*00659cdc*/ extern undefined4 g_game_659cdc;
/*00659c38*/ extern char g_load_659c38[];

/* atomics per row of the table (0xb dwords) */
#define VEHICLE_ATOMICS_PER_ROW 0xb
/* end address of the table (rows at 0x659b30, 0x659b30 + 0x2c, ...) */
#define VEHICLE_ATOMIC_TABLE_END ((int)&g_load_659c38)


/* vehicle_atomics_destroy_659b30 */

/* "destroys the atomics of the 0x659B30 table" */

/* Destroys every atomic in the table at g_load_659b30 (rows of 11 atomics, each atomic's
   frame is detached and destroyed too), then destroys the clump g_game_659cdc. */
void vehicle_atomics_destroy_659b30(void)
{
  RwFrame *frame;
  int column;
  undefined4 *row;

  row = &g_load_659b30;
  do {
    column = 0;
    do {
      frame = RpAtomic_object_object_parent(row[column]);
      RpAtomicSetFrame((RpAtomic *)row[column],(RwFrame *)0x0);
      RpAtomicDestroy((RpAtomic *)row[column]);
      RwFrameDestroy(frame);
      column = column + 1;
    } while (column < VEHICLE_ATOMICS_PER_ROW);
    row = row + VEHICLE_ATOMICS_PER_ROW;
  } while ((int)row < VEHICLE_ATOMIC_TABLE_END);
  RpClumpDestroy(g_game_659cdc);
  return;
}
