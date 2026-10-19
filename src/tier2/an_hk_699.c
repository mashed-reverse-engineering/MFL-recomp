#include "mfl_tier2.h"
/*004042c0*/ extern double scene_get_bomb_5fb0a8();
/*004b2b48*/ extern int _ftol();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005de1d0*/ extern float k_f_1_25;
/*00658b34*/ extern undefined4 g_hudpanel_658b34;
/*00658b44*/ extern undefined4 g_hudpanel_658b44;

/* rwMATRIXINTERNALIDENTITY (0x20000) | rwMATRIXTYPEORTHONORMAL (3) */
#define MATRIX_FLAGS_IDENTITY_ORTHONORMAL (0x20003)
/* 8-bit colour channel full scale */
#define COLOUR_LEVEL_MAX (255.0f)
/* alpha byte of the packed colour: fully opaque (inferred) */
#define COLOUR_ALPHA_OPAQUE (0xff)

/* hudpanel_bomb_meter_update */

/* "places the meter frame (-0.48,-0.35) scale 1.25 and sets its fill colour/scale from the bomb
   value" */

void hudpanel_bomb_meter_update(void)

{
  undefined4 meter_colour;
  undefined1 blue_level;
  undefined1 green_level;
  float10 bomb_value;
  struct {
    undefined1 pad_bytes [3];
    undefined1 pad_byte;
    RwV3d meter_scale;
    RwV3d meter_pos;
    RwMatrix meter_matrix;
  } xform;

  /* build the meter's local matrix: identity orthonormal, meter_pos (-0.48,-0.35,1.0) */
  xform.meter_matrix.flags = xform.meter_matrix.flags | MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
  xform.meter_pos.x = -0.48;
  xform.meter_pos.y = -0.35;
  xform.meter_pos.z = 1.0;
  xform.meter_scale.x = 1.0;
  xform.meter_scale.y = 1.0;
  xform.meter_scale.z = 1.0;
  xform.meter_matrix.at.z = 1.0;
  xform.meter_matrix.up.y = 1.0;
  xform.meter_matrix.right.x = 1.0;
  xform.meter_matrix.up.x = 0.0;
  xform.meter_matrix.right.z = 0.0;
  xform.meter_matrix.right.y = 0.0;
  xform.meter_matrix.at.y = 0.0;
  xform.meter_matrix.at.x = 0.0;
  xform.meter_matrix.up.z = 0.0;
  xform.meter_matrix.pos.z = 0.0;
  xform.meter_matrix.pos.y = 0.0;
  xform.meter_matrix.pos.x = 0.0;
  /* place the meter at meter_pos and parent it to the meter frame g_hudpanel_658b44 */
  RwMatrixTranslate(&xform.meter_matrix,&xform.meter_pos,rwCOMBINEREPLACE);
  RwFrameTransform((struct RwFrame *)g_hudpanel_658b44,(RwMatrix *)&xform.meter_matrix,rwCOMBINEREPLACE);
  /* scale the meter frame by 1.25 (k_f_1_25) */
  xform.meter_scale.x = xform.meter_scale.x * k_f_1_25;
  xform.meter_scale.y = xform.meter_scale.y * k_f_1_25;
  xform.meter_scale.z = xform.meter_scale.z * k_f_1_25;
  RwFrameScale((struct RwFrame *)g_hudpanel_658b44,(struct RwV3d *)&xform.meter_scale,rwCOMBINEPRECONCAT);
  /* read the bomb value (0..1 fraction, inferred) and use it as the x scale of the fill */
  bomb_value = (float10)scene_get_bomb_5fb0a8();
  xform.meter_scale.x = (RwReal)bomb_value;
  xform.meter_scale.z = 1.0;
  xform.meter_scale.y = 1.0;
  /* fill colour: green rises and blue falls with the bomb value; the packed colour is
     0xFF000000 | green << 8 | blue (D3DCOLOR byte order, inferred) */
  blue_level = ((int)((1.0f - xform.meter_scale.x) * COLOUR_LEVEL_MAX));
  green_level = ((int)(xform.meter_scale.x * COLOUR_LEVEL_MAX));
  meter_colour = CONCAT13(COLOUR_ALPHA_OPAQUE,CONCAT12(0,CONCAT11(green_level,blue_level)));
  *(undefined4 *)(**(int **)(*(char **)(g_hudpanel_658b34 + 0x18) + 0x20) + 4) = meter_colour;
  /* scale the fill frame (g_hudpanel_658b34 + 4) to (bomb_value, 1, 1) */
  RwFrameScale(*(RwFrame **)(g_hudpanel_658b34 + 4),&xform.meter_scale,rwCOMBINEREPLACE);
  return;
}


