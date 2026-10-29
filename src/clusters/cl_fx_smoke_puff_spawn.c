// cluster-entries: fx_smoke_puff_spawn
/* Installed cluster fx_smoke_puff_spawn (tools/cluster_install.py from output/cluster_pilot_kd/fx_smoke_puff_spawn.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*00486050*/ extern undefined __cdecl random_vector_in_cone(struct RwV3d *p0, float *p1, float p2);
/*005e64a4*/ extern float k_f_0;
/*0062ac74*/ extern float g_car_62ac74;
/*00632ae8*/ extern undefined4 one;
/*0076beb0*/ extern undefined1 g_particles_76beb0;
/*0076c4e0*/ extern undefined4 g_particles_76c4e0;
/*0076c53c*/ extern undefined4 g_particles_76c53c;
/*0076c090*/ extern char g_fx_systems_76c090[];
/* fx_smoke_puff_spawn */

/* "finds a free entry in the 128 bit mask pool and fills one smoke puff" */

void fx_smoke_puff_spawn(undefined4 *pos, undefined4 *size_ptr, undefined4 puff_f8, undefined4 puff_f11, undefined4 puff_f12, char *unused_6, char *unused_7, char *unused_8, char *unused_9)

{
  int scan_end;
  float oldest_age;
  byte mask_byte;
  uint bit_index;
  byte free_bits;
  float *age_ptr;
  int oldest_index;
  int byte_index;
  undefined4 *puff;
  float10 rnd;
  
  oldest_age = k_f_0;
  bit_index = 0;
  oldest_index = 0;
  puff = (undefined4 *)0x0;
LAB_0049d3e3:
  do {
    while( true ) {
      if (0x7f < (int)bit_index) goto LAB_0049d491;
      if (*(int *)((char *)&g_particles_76beb0 + ((int)bit_index >> 5) * 4) != -1) break;
      bit_index = (((int)bit_index >> 5) + 1) * 0x20;
    }
    byte_index = (int)bit_index >> 3;
    mask_byte = (&g_particles_76beb0)[byte_index];
    if (mask_byte == 0xff) goto switchD_0049d420_default;
    free_bits = ~mask_byte;
    switch(bit_index & 7) {
    case 0:
      bit_index = bit_index + 1;
      if ((free_bits & 1) == 0) goto switchD_0049d420_caseD_1;
      break;
    case 1:
switchD_0049d420_caseD_1:
      bit_index = bit_index + 1;
      if ((free_bits & 2) == 0) goto switchD_0049d420_caseD_2;
      break;
    case 2:
switchD_0049d420_caseD_2:
      bit_index = bit_index + 1;
      if ((free_bits & 4) == 0) goto switchD_0049d420_caseD_3;
      break;
    case 3:
switchD_0049d420_caseD_3:
      bit_index = bit_index + 1;
      if ((free_bits & 8) == 0) goto switchD_0049d420_caseD_4;
      break;
    case 4:
switchD_0049d420_caseD_4:
      bit_index = bit_index + 1;
      if ((free_bits & 0x10) == 0) goto switchD_0049d420_caseD_5;
      break;
    case 5:
switchD_0049d420_caseD_5:
      bit_index = bit_index + 1;
      if ((free_bits & 0x20) == 0) goto switchD_0049d420_caseD_6;
      break;
    case 6:
switchD_0049d420_caseD_6:
      bit_index = bit_index + 1;
      if ((free_bits & 0x40) == 0) goto switchD_0049d420_caseD_7;
      break;
    case 7:
switchD_0049d420_caseD_7:
      bit_index = bit_index + 1;
      if (-1 < (char)mask_byte) break;
    default:
switchD_0049d420_default:
      bit_index = byte_index * 8 + 8;
      goto LAB_0049d3e3;
    }
    if ((int)bit_index < 0) goto LAB_0049d491;
    puff = (undefined4 *)(bit_index * 0x34 + ((int)&g_fx_systems_76c090 + 0x41c));
    (&g_particles_76beb0)[(int)(bit_index - 1) >> 3] =
         (&g_particles_76beb0)[(int)(bit_index - 1) >> 3] | '\x01' << ((byte)(bit_index - 1) & 7);
    if ((int)bit_index < 1) {
LAB_0049d491:
      if (puff == (undefined4 *)0x0) {
        age_ptr = (float *)&g_particles_76c53c;
        byte_index = 2;
        do {
          if (oldest_age < age_ptr[-0xd]) {
            oldest_index = byte_index + -2;
            oldest_age = age_ptr[-0xd];
          }
          if (oldest_age < *age_ptr) {
            oldest_index = byte_index + -1;
            oldest_age = *age_ptr;
          }
          if (oldest_age < age_ptr[0xd]) {
            oldest_age = age_ptr[0xd];
            oldest_index = byte_index;
          }
          if (oldest_age < age_ptr[0x1a]) {
            oldest_index = byte_index + 1;
            oldest_age = age_ptr[0x1a];
          }
          if (oldest_age < age_ptr[0x27]) {
            oldest_index = byte_index + 2;
            oldest_age = age_ptr[0x27];
          }
          if (oldest_age < age_ptr[0x34]) {
            oldest_index = byte_index + 3;
            oldest_age = age_ptr[0x34];
          }
          if (oldest_age < age_ptr[0x41]) {
            oldest_index = byte_index + 4;
            oldest_age = age_ptr[0x41];
          }
          if (oldest_age < age_ptr[0x4e]) {
            oldest_index = byte_index + 5;
            oldest_age = age_ptr[0x4e];
          }
          scan_end = byte_index + 6;
          age_ptr = age_ptr + 0x68;
          byte_index = byte_index + 8;
        } while (scan_end < 0x80);
        puff = &g_particles_76c4e0 + oldest_index * 0xd;
        (&g_particles_76beb0)[oldest_index >> 3] =
             (&g_particles_76beb0)[oldest_index >> 3] | '\x01' << ((byte)oldest_index & 7);
      }
      puff[10] = 0;
      puff[9] = 0;
      rnd = (float10)random_range(0xc0c90fdb,0x40c90fdb);
      *(float *)&puff[7] = (float)rnd;
      puff[8] = puff_f8;
      puff[0xc] = puff_f12;
      puff[0xb] = puff_f11;
      puff[6] = *size_ptr;
      random_vector_in_cone((RwV3d *)(puff + 3),(float *)&g_car_62ac74,60.0);
      *puff = *pos;
      puff[1] = pos[1];
      puff[2] = pos[2];
      return;
    }
  } while( true );
}

