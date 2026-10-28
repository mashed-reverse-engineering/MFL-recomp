#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*005dd628*/ extern float k_f_0_25;
/*005dd9c4*/ extern float k_f_0_15;
/*005dda24*/ extern float k_f_0_75;
/*005dfd78*/ extern float k_f_1_05;
/* course_storm_cb1_sub_4aaea0 */

/* single caller course_storm_cb1_sub_4ab160 */

/* Spawn one particle into the first free slot (of 512, 32 bytes each) of the particles array, with randomised velocity/size scaled by scale (inferred). */
void course_storm_cb1_sub_4aaea0(float *emitter,float scale,short *particles)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fbits4;
  int index;
  double speed;
  
  speed = -(double)random_range((fbits1 = scale * k_f_0_15, *(int *)&fbits1),(fbits2 = scale * k_f_0_25, *(int *)&fbits2));
  index = 0;
  do {
    if (*particles == 0) {
      *particles = 1;
      particles[1] = 0;
      *(float *)(particles + 8) = (float)(speed * (double)*emitter);
      *(float *)(particles + 0xc) = (float)(speed * (double)emitter[2]);
      speed = (double)random_range((fbits3 = scale * k_f_0_75, *(int *)&fbits3),(fbits4 = scale * k_f_1_05, *(int *)&fbits4));
      *(float *)(particles + 10) = (float)speed;
      *(float *)(particles + 2) = emitter[0xc];
      *(float *)(particles + 4) = emitter[0xd];
      *(float *)(particles + 6) = emitter[0xe];
      return;
    }
    particles = particles + 0x10;
    index = index + 1;
  } while (index < 0x200);
  return;
}


