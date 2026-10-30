#include "mfl_tier2.h"
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8(float v);
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*004b2b48*/ extern int _ftol();
/*005dd330*/ extern float float_1_0;
/*005dda24*/ extern float k_f_0_75;
/*005de1f8*/ extern float k_f_3_1415927;
/*0076be70*/ extern undefined g_particles_76be70;
/*0076beb0*/ extern undefined1 g_particles_76beb0;
/*0076c4e0*/ extern undefined4 g_particles_76c4e0;
/*0076c4e4*/ extern undefined4 g_fx_slots_76c4e4;
/*0076c4e8*/ extern undefined4 g_fx_slots_76c4e8;
/*0076c4fc*/ extern undefined g_effects_update_76c4fc;
/*0076c504*/ extern undefined4 g_fx_slots_76c504;
/*0076c508*/ extern undefined4 g_particles_76c508;
/*0076c50c*/ extern undefined g_fx_systems_76c50c;
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);
#define PUFF_POOL_SIZE (0x80) /* puffs in the pool; one bit each in g_particles_76beb0 */
#define PUFF_RECORD_SIZE (0x34) /* bytes per puff record from g_particles_76c4e0 (13 words) */
/* fx_smoke_puffs_update */

/* "iterates the active bit mask of the puff pool and updates each puff" */

void fx_smoke_puffs_update(void)

{
  float life_t;
  float scaled_age;
  float life_for_alpha;
  byte mask_byte;
  char alpha_byte;
  int byte_index;
  uint puff_index;
  int record_offset;
  double wobble;
  struct {
    undefined4 colour_rgba;
    float size_w;
    float size_h;
    float pos_x;
    float pos_y;
    float pos_z;
  } frame;
  
  /* Walks the live-puff bitmask (bit per slot, 0x80 slots); for each live puff: advance its age (life_t, written to +0x504/+0x508 words), compute position (velocity*age + base, plus sin(age*pi) sideways wobble), size and fade alpha, and queue it with particles_add; puffs whose age exceeds 1.0 are cleared from the mask. byte_index is reused: mask byte index while scanning, then slot index (inferred). */
  puff_index = 0;
LAB_0049d830:
  frame.size_w = 0.5;
  frame.size_h = 0.5;
  do {
    while( true ) {
      if (PUFF_POOL_SIZE - 1 < (int)puff_index) goto LAB_0049d9f7;
      if (*(int *)((char *)&g_particles_76beb0 + ((int)puff_index >> 5) * 4) != 0) break;
      puff_index = (((int)puff_index >> 5) + 1) * 0x20;
    }
    byte_index = (int)puff_index >> 3;
    mask_byte = (&g_particles_76beb0)[byte_index];
    if (mask_byte != 0) {
      switch(puff_index & 7) {
      case 0:
        puff_index = puff_index + 1;
        if ((mask_byte & 1) != 0) goto LAB_0049d8b5;
        break;
      case 1:
        break;
      case 2:
        goto switchD_0049d87d_caseD_2;
      case 3:
        goto switchD_0049d87d_caseD_3;
      case 4:
        goto switchD_0049d87d_caseD_4;
      case 5:
        goto switchD_0049d87d_caseD_5;
      case 6:
        goto switchD_0049d87d_caseD_6;
      case 7:
        goto switchD_0049d87d_caseD_7;
      default:
        goto switchD_0049d87d_default;
      }
      puff_index = puff_index + 1;
      if ((mask_byte & 2) != 0) break;
switchD_0049d87d_caseD_2:
      puff_index = puff_index + 1;
      if ((mask_byte & 4) != 0) break;
switchD_0049d87d_caseD_3:
      puff_index = puff_index + 1;
      if ((mask_byte & 8) != 0) break;
switchD_0049d87d_caseD_4:
      puff_index = puff_index + 1;
      if ((mask_byte & 0x10) != 0) break;
switchD_0049d87d_caseD_5:
      puff_index = puff_index + 1;
      if ((mask_byte & 0x20) != 0) break;
switchD_0049d87d_caseD_6:
      puff_index = puff_index + 1;
      if ((mask_byte & 0x40) != 0) break;
switchD_0049d87d_caseD_7:
      puff_index = puff_index + 1;
      if ((char)mask_byte < '\0') break;
    }
switchD_0049d87d_default:
    puff_index = byte_index * 8 + 8;
  } while( true );
LAB_0049d8b5:
  if ((int)puff_index < 0) goto LAB_0049d9f7;
  byte_index = puff_index - 1;
  record_offset = byte_index * PUFF_RECORD_SIZE;
  frame.colour_rgba = *(undefined4 *)((char *)&g_particles_76c4e0 + record_offset + 0x18);
  wobble = (double)sin((double)*(float *)&(&g_fx_slots_76c504)[byte_index * 0xd] * (double)k_f_3_1415927);
  life_t = g_tick_seconds + *(float *)&(&g_particles_76c508)[byte_index * 0xd];
  *(float *)&(&g_particles_76c508)[byte_index * 0xd] = life_t;
  *(float *)&(&g_fx_slots_76c504)[byte_index * 0xd] = life_t;
  scaled_age = life_t * *(float *)((char *)&g_fx_systems_76c50c + record_offset);
  frame.pos_x = *(float *)((char *)&g_particles_76c4e0 + record_offset + 0xc) * scaled_age +
            *(float *)&(&g_particles_76c4e0)[byte_index * 0xd];
  frame.pos_z = *(float *)((char *)&g_particles_76c4e0 + record_offset + 0x14) * scaled_age +
            *(float *)&(&g_fx_slots_76c4e8)[byte_index * 0xd];
  frame.pos_y = (float)(wobble * (double)*(float *)((char *)&g_particles_76c4e0 + record_offset + 0x30) +
                   (double)*(float *)&(&g_fx_slots_76c4e4)[byte_index * 0xd]);
  if (!(life_t > float_1_0)) {      /* NaN counts as alive (original: fcomp, test ah,0x41, jne) */
    wobble = wobble * (double)*(float *)((char *)&g_particles_76c4e0 + record_offset + 0x20);
    wobble = wobble + wobble + (double)*(float *)((char *)&g_particles_76c4e0 + record_offset + 0x20);
    frame.size_h = (float)wobble;
    frame.size_w = (float)(wobble * (double)k_f_0_75);
    alpha_byte = ((int)(((life_t * life_t) * life_t) * 255.0f));
    frame.colour_rgba = CONCAT13(-1 - alpha_byte,(undefined3)frame.colour_rgba);
    point_lerp_to_aux_camera(&frame.pos_x,&frame.pos_x,0x3d4ccccd);
    life_t = *(float *)((char *)&g_effects_update_76c4fc + record_offset);
    life_for_alpha = *(float *)&(&g_fx_slots_76c504)[byte_index * 0xd];
    particles_set_position(&frame.pos_x);
    particles_set_color(&frame.colour_rgba);
    particles_set_size(&frame.size_w);
    gfx_set_gfx_6c50c8(life_t * life_for_alpha);
    particles_add(&g_particles_76be70);
  }
  else {
    (&g_particles_76beb0)[byte_index >> 3] =
         (&g_particles_76beb0)[byte_index >> 3] & ~('\x01' << ((byte)byte_index & 7));
    puff_index = puff_index + 1;
  }
  if ((int)puff_index < 1) {
LAB_0049d9f7:
    particles_flush(&g_particles_76be70);
    return;
  }
  goto LAB_0049d830;
}


