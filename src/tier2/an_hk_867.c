#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*0049cd60*/ extern undefined fx_spawn_burst(float *p1, int p2, float *p3, undefined4 *p4, undefined4 *p5, float p6);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005de9c4*/ extern float k_f_0_017453292;
extern double sin(double);
#define RAND_DIR_MIN_BITS (0xbf800000) /* float bits of -1.0f, lower bound of a direction component */
#define SPEED_JITTER_MIN_BITS (0x3f733333) /* float bits of 0.95f */
#define SPEED_JITTER_MAX_BITS (0x3f866666) /* float bits of 1.05f */
/* fx_spawn_burst_spread */

/* Spawns burst_count particles at spawn_pos (inferred), each with a random direction jittered around cone_axis.
 * The jitter offset is sin(cone_angle_deg) (inferred); the direction is normalised and the speed is speed
 * times a 0.95..1.05 random factor. fx_ptr_a, fx_ptr_b and fx_scale are passed through to fx_spawn_burst (inferred).
 * Returns nothing.
 * (was: "spawns n burst particles with random directions around a cone of angle unused_5") */

void fx_spawn_burst_spread(char *spawn_pos,int burst_count,float speed,float *cone_axis,float cone_angle_deg,
               undefined4 *fx_ptr_a,undefined4 *fx_ptr_b,float fx_scale)
{
  float cone_sin;
  double scratch;
  /* per-particle scratch: particles left to spawn, direction, then velocity (passed to fx_spawn_burst as float *) */
  struct {
    int remaining;
    RwV3d direction;
    float vel_x;
    float vel_y;
    float vel_z;
  } burst_state;
  
  /* nothing to spawn when burst_count is 0 */
  if (burst_count != 0) {
    /* cone offset scale = sin(angle in radians); k_f_0_017453292 is pi/180 */
    scratch = (double)sin((double)cone_angle_deg * (double)k_f_0_017453292);
    burst_state.remaining = burst_count;
    cone_sin = (float)scratch;
    /* one iteration per particle: random direction, normalise, random speed, spawn */
    do {
      scratch = (double)random_range(RAND_DIR_MIN_BITS,FLOAT_ONE_BITS);
      burst_state.direction.x = (RwReal)scratch;
      scratch = (double)random_range(RAND_DIR_MIN_BITS,FLOAT_ONE_BITS);
      burst_state.direction.y = (RwReal)scratch;
      scratch = (double)random_range(RAND_DIR_MIN_BITS,FLOAT_ONE_BITS);
      burst_state.direction.x = burst_state.direction.x * cone_sin;
      burst_state.direction.y = burst_state.direction.y * cone_sin;
      burst_state.direction.z = (float)(scratch * (double)cone_sin);
      burst_state.direction.x = burst_state.direction.x + *cone_axis;
      burst_state.direction.y = burst_state.direction.y + cone_axis[1];
      burst_state.direction.z = burst_state.direction.z + cone_axis[2];
      RwV3dNormalize(&burst_state.direction,&burst_state.direction);
      scratch = (double)random_range(SPEED_JITTER_MIN_BITS,SPEED_JITTER_MAX_BITS);
      burst_state.vel_x = (float)(scratch * (double)speed * (double)burst_state.direction.x);
      scratch = (double)random_range(SPEED_JITTER_MIN_BITS,SPEED_JITTER_MAX_BITS);
      burst_state.vel_y = (float)(scratch * (double)speed * (double)burst_state.direction.y);
      scratch = (double)random_range(SPEED_JITTER_MIN_BITS,SPEED_JITTER_MAX_BITS);
      burst_state.vel_z = (float)(scratch * (double)speed * (double)burst_state.direction.z);
      fx_spawn_burst((float *)spawn_pos,1,&burst_state.vel_x,fx_ptr_a,fx_ptr_b,fx_scale);
      burst_state.remaining = burst_state.remaining + -1;
    } while (burst_state.remaining != 0);
  }
  return;
}


