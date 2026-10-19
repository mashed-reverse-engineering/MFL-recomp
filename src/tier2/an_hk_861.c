#include "mfl_tier2.h"
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*004b2b48*/ extern int _ftol();
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005e0990*/ extern float k_f_2_3561945;
/*0062ac68*/ extern float g_car_62ac68;
/*0076b830*/ extern undefined g_effects_update_76b830;
/*0076b840*/ extern float g_effects_update_76b840;
/*0076be30*/ extern undefined g_particles_76be30;
/*0076bec0*/ extern undefined4 g_particles_76bec0[0x1000];
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);

/* Floating text slot table: 64 slots of 24 bytes (inferred), age float at +0x10 of each slot */
#define FX_TEXT_TABLE_BYTES (0x600)
#define FX_TEXT_SLOT_FLOATS (6)
#define FX_TEXT_SLOT_V3D (2)
/* Text colour bytes (RGB, alpha is computed per frame) */
#define FX_TEXT_COLOUR (200)
/* Peak alpha of the fade (alpha = 64 - 64 * age^2, inferred) */
#define FX_TEXT_ALPHA_MAX (64)
/* Billboard rotation angle in degrees about the g_car_62ac68 axis */
#define FX_TEXT_ROTATE_DEG (90.0)

/* fx_floating_text_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "advances each active slot, scales with sin, fades alpha and sets the particle matrix/colour" */

/* Per-frame update of the floating text pool. Takes no parameters, returns nothing.
 * Slot i (24 bytes, inferred): position RwV3d at g_effects_update_76b830 + 24*i,
 * age float at slot + 0x10, pulse amplitude float at slot + 0x14 (translation[1].z).
 * Active slots (g_particles_76bec0[i] != 0) age by g_tick_seconds; a slot older than
 * float_1_0 is deactivated, otherwise it is drawn with a sin() pulse and a fading alpha.
 */
void fx_floating_text_update(void)

{
  float10 pulse_scale;
  float *slot_age;
  RwV3d *slot_pos;
  int *slot_active;
  struct {
    float age;
    undefined1 colour_r;
    undefined1 colour_g;
    undefined1 colour_b;
    char alpha;
    RwV3d scale;
    RwMatrix matrix;
  } frame;

  frame.colour_r = FX_TEXT_COLOUR;
  frame.colour_g = FX_TEXT_COLOUR;
  frame.colour_b = FX_TEXT_COLOUR;
  frame.alpha = -0x38;
  slot_active = g_particles_76bec0;
  slot_pos = (RwV3d *)&g_effects_update_76b830;
  slot_age = (float *)&g_effects_update_76b840;
  /* Walk every slot of the table, one step per slot */
  do {
    if (*slot_active != 0) {
      frame.scale.x = 1.0;
      frame.age = g_tick_seconds + *slot_age;
      frame.scale.y = 1.0;
      frame.scale.z = 1.0;
      *slot_age = frame.age;
      slot_pos[1].x = frame.age;
      /* Still alive: draw with pulse scale and fade */
      if (frame.age <= float_1_0) {
        pulse_scale = sin((float10)frame.age * (float10)k_f_2_3561945) * slot_pos[1].z;
        frame.alpha = ((int)((frame.age * frame.age) * 64.0f));
        frame.scale.x = (RwReal)pulse_scale;
        frame.scale.y = (RwReal)pulse_scale;
        frame.alpha = FX_TEXT_ALPHA_MAX - frame.alpha;
        frame.matrix.flags = frame.matrix.flags | 0x20003;
        frame.matrix.at.z = 1.0;
        frame.matrix.up.y = 1.0;
        frame.matrix.right.x = 1.0;
        frame.matrix.up.x = 0.0;
        frame.matrix.right.z = 0.0;
        frame.matrix.right.y = 0.0;
        frame.matrix.at.y = 0.0;
        frame.matrix.at.x = 0.0;
        frame.matrix.up.z = 0.0;
        frame.matrix.pos.z = 0.0;
        frame.matrix.pos.y = 0.0;
        frame.matrix.pos.x = 0.0;
        RwMatrixTranslate(&frame.matrix,slot_pos,rwCOMBINEREPLACE);
        RwMatrixRotate(&frame.matrix,(RwV3d *)&g_car_62ac68,FX_TEXT_ROTATE_DEG,rwCOMBINEPRECONCAT);
        RwMatrixScale(&frame.matrix,&frame.scale,rwCOMBINEPRECONCAT);
        gfx_set_fx_matrix(&frame.matrix);
        particles_set_color(&frame.colour_r);
        particles_add(&g_particles_76be30);
      }
      /* Expired: free the slot */
      else {
        *slot_active = 0;
      }
    }
    slot_age = slot_age + FX_TEXT_SLOT_FLOATS;
    slot_pos = slot_pos + FX_TEXT_SLOT_V3D;
    slot_active = slot_active + 1;
  } while ((int)slot_age < (int)&g_effects_update_76b840 + FX_TEXT_TABLE_BYTES);
  particles_flush(&g_particles_76be30);
  return;
}


