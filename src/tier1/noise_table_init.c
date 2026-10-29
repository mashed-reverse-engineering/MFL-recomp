#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*00544e00*/ extern undefined __cdecl RpRandomSeed(undefined4 p0);
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;
/*00653298*/ extern undefined4 g_noise_table_653298;
/*008c9ee0*/ extern float g_noise_table_8c9ee0;
/*008c9ee4*/ extern float g_noise_table_8c9ee4;
/*008c9ee8*/ extern float g_noise_table_8c9ee8;
/*008c9eec*/ extern float g_noise_table_8c9eec;
/*008c9ef0*/ extern undefined4 g_noise_table_8c9ef0;
#define NOISE_TABLE_SEED (0xd7a33)
#define NOISE_TABLE_BYTES (0x4000)
#define NOISE_RANDOM_MIN_BITS (0xbe800000) /* -0.25f */
#define NOISE_RANDOM_MAX_BITS (0x3fa00000) /* 1.25f */
#define NOISE_HALF_TURN_BITS (0x43340000) /* 180.0f (inferred from 360.0f in g_noise_table_8c9ee8) */
/*
 * noise_table_init: fills the noise table at g_noise_table_653298 with NOISE_TABLE_BYTES / 4 floats in [0.0, 1.0],
 * drawn from RpRandomSeed(NOISE_TABLE_SEED) and random_range(-0.25f, 1.25f), then sets four float globals.
 * Takes no parameters, returns nothing. The clamp bounds k_f_0 (low) and float_1_0 (high) are not in the dossier, so their values are inferred.
 */
/* "RpRandomSeed(0xD7A33) then fills 4096 random floats at 0x653298" */
void noise_table_init(void)
{
  float *noise_entry;
  
  RpRandomSeed(NOISE_TABLE_SEED);
  noise_entry = (float *)&g_noise_table_653298;
  /* loop over every entry: random sample, below k_f_0 -> 0.0, above float_1_0 -> 1.0 */
  do {
    *noise_entry = (float)random_range(NOISE_RANDOM_MIN_BITS,NOISE_RANDOM_MAX_BITS);
    if (*noise_entry < k_f_0) {
      *noise_entry = 0.0;
    }
    else if (*noise_entry > float_1_0) {
      *noise_entry = 1.0;
    }
    noise_entry = noise_entry + 1;
  } while ((int)noise_entry < (int)&g_noise_table_653298 + NOISE_TABLE_BYTES);
  /* globals after the table, purpose inferred: 360.0f (a full turn in degrees), then 180.0f bits, then 1.0f x3 */
  g_noise_table_8c9ee8 = 360.0f;
  g_noise_table_8c9ef0 = NOISE_HALF_TURN_BITS;
  g_noise_table_8c9eec = 1.0f;
  g_noise_table_8c9ee0 = 1.0f;
  g_noise_table_8c9ee4 = 1.0f;
  return;
}
