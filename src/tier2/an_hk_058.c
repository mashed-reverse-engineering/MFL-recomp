#include "mfl_tier2.h"
/*00408c40*/ extern double fx_current_object_get_float();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005de1d0*/ extern float k_f_1_25;
/*006592c8*/ extern char *g_hudpanel_6592c8;
/*006592d8*/ extern undefined4 g_hudpanel_6592d8;
/* hudpanel_fuel_meter_update */

/* "places the panel frame (-0.48,-0.35) scale 1.25 and scales the bar by 1 - fx value" */

/* Per-frame update of the fuel meter HUD panel. Takes no parameters, returns nothing.
   Uses the global frame g_hudpanel_6592d8 for the panel and g_hudpanel_6592c8 + 4 for the bar frame. */
void hudpanel_fuel_meter_update(void)

{
  double fx_value;
  RwV3d bar_scale;
  RwV3d panel_offset;
  RwMatrix panel_matrix;

  /* panel transform: identity-style matrix (flags 0x20003, inferred) with translation (-0.48, -0.35, 1.0), applied to the panel frame */
  panel_matrix.flags = panel_matrix.flags | 0x20003;
  panel_offset.x = -0.48;
  panel_offset.y = -0.35;
  panel_offset.z = 1.0;
  bar_scale.x = 1.0;
  bar_scale.y = 1.0;
  bar_scale.z = 1.0;
  panel_matrix.at.z = 1.0;
  panel_matrix.up.y = 1.0;
  panel_matrix.right.x = 1.0;
  panel_matrix.up.x = 0.0;
  panel_matrix.right.z = 0.0;
  panel_matrix.right.y = 0.0;
  panel_matrix.at.y = 0.0;
  panel_matrix.at.x = 0.0;
  panel_matrix.up.z = 0.0;
  panel_matrix.pos.z = 0.0;
  panel_matrix.pos.y = 0.0;
  panel_matrix.pos.x = 0.0;
  RwMatrixTranslate(&panel_matrix,&panel_offset,rwCOMBINEREPLACE);
  RwFrameTransform(g_hudpanel_6592d8,&panel_matrix,rwCOMBINEREPLACE);
  bar_scale.x = bar_scale.x * k_f_1_25;
  bar_scale.y = bar_scale.y * k_f_1_25;
  bar_scale.z = bar_scale.z * k_f_1_25;
  RwFrameScale(g_hudpanel_6592d8,&bar_scale,rwCOMBINEPRECONCAT);
  /* bar: x scale = 1 - fx value (inferred: fx value is the fraction used), y/z stay 1.0 */
  fx_value = (double)fx_current_object_get_float();
  bar_scale.x = (RwReal)((double)float_1_0 - fx_value);
  bar_scale.z = 1.0;
  bar_scale.y = 1.0;
  RwFrameScale(*(RwFrame **)(g_hudpanel_6592c8 + 4),&bar_scale,rwCOMBINEREPLACE);
  return;
}


