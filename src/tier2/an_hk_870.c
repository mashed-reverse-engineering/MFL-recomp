#include "mfl_tier2.h"
/*004220f0*/ extern int vehicle_get_position_158();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd378*/ extern float k_f_0_1;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddb2c*/ extern float k_f_1_6999999;
/*005ddb30*/ extern float k_f_0_083333336;
/*005ddc28*/ extern float k_f_0;
#define OBJ_PATH_OFFSET (0x10)
#define PATH_POINT_PTR_OFFSET (0x4)
#define PATH_POINT_X_OFFSET (0x40)
#define PATH_POINT_Y_OFFSET (0x44)
#define PATH_POINT_Z_OFFSET (0x48)
#define OBJ_DIR_X_OFFSET (0x50)   /* (inferred) direction vector of the object */
#define OBJ_DIR_Y_OFFSET (0x54)
#define OBJ_DIR_Z_OFFSET (0x58)
#define OBJ_FX_LEVEL_OFFSET (0xd8) /* (inferred) float written back to the object: fx level */
#define FX_FAR_LEVEL_BITS (0x3fd99999) /* float bits of 1.6999999f: level used when the vehicle is ahead */
/* fx_distance_check */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "distance to the first camera path point against a threshold (inferred)"
 * Sets the fx level of an object from the vehicle's distance to the object's path point.
 *   param_1: object pointer, passed in a float slot (its bits are read as an int, see `obj`).
 *            It is reused below as a float for the clamped distance factor.
 * Writes OBJ_FX_LEVEL_OFFSET of the object and returns nothing. */

void fx_distance_check(float param_1)

{
  char *path;
  float scaled_distance;
  int obj;
  RwReal offset_length;
  RwV3d *offset;
  struct {
    RwV3d local_1c;
    float local_10;
    float local_c;
    float local_8;
    int pad_top;
  } frame;

  /* vehicle position (frame.local_10..8) minus the object's path point (inferred: path->point at +0x40) */
  offset = &frame.local_1c;
  obj = *(int *)&param_1;
  vehicle_get_position_158(0,&frame.local_10);
  path = *(char **)(*(int *)&param_1 + OBJ_PATH_OFFSET);
  offset->x = frame.local_10 - *(float *)(*(char **)(path + PATH_POINT_PTR_OFFSET) + PATH_POINT_X_OFFSET);
  offset->y = frame.local_c - *(float *)(*(char **)(path + PATH_POINT_PTR_OFFSET) + PATH_POINT_Y_OFFSET);
  offset->z = frame.local_8 - *(float *)(*(char **)(path + PATH_POINT_PTR_OFFSET) + PATH_POINT_Z_OFFSET);
  /* only when the squared distance to the point is above the k_f_1_0en5 threshold */
  if (k_f_1_0en5 < offset->y * offset->y + offset->x * offset->x + offset->z * offset->z) {
    /* distance in units of 1/12 (k_f_0_083333336), then offset becomes a unit direction */
    offset_length = RwV3dLength(offset);
    scaled_distance = offset_length * k_f_0_083333336;
    RwV3dNormalize(offset,offset);
    /* vehicle ahead of the point along the object's direction: far level, done */
    if (k_f_0 < *(float *)(*(int *)&param_1 + OBJ_DIR_Y_OFFSET) * offset->y +
                *(float *)(*(int *)&param_1 + OBJ_DIR_X_OFFSET) * offset->x +
                *(float *)(*(int *)&param_1 + OBJ_DIR_Z_OFFSET) * offset->z) {
      *(undefined4 *)(*(int *)&param_1 + OBJ_FX_LEVEL_OFFSET) = FX_FAR_LEVEL_BITS;
      return;
    }
    /* otherwise clamp the distance factor to 1.0 */
    param_1 = scaled_distance;
    if (float_1_0 < scaled_distance) {
      param_1 = 1.0;
    }
    /* level falls linearly from ~1.8 (factor 0) to 0.1 (factor 1.0) (inferred from the constants) */
    *(float *)(obj + OBJ_FX_LEVEL_OFFSET) = (float_1_0 - param_1) * k_f_1_6999999 + k_f_0_1;
  }
  return;
}


