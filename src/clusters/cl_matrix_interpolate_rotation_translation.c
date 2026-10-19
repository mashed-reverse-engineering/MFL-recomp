// cluster-entries: matrix_interpolate_rotation_translation
/* Installed cluster matrix_interpolate_rotation_translation (tools/cluster_install.py from output/cluster_pilot_zy/matrix_interpolate_rotation_translation.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combine);
/*004d4490*/ extern RwMatrix *RwMatrixInvert(RwMatrix *inverse, RwMatrix *source);
/*004d49c0*/ extern RwMatrix *RwMatrixTransform(RwMatrix *matrix, RwMatrix *transform, enum RwOpCombineType combine);
/*004d4b40*/ extern RwMatrix *RwMatrixQueryRotate(RwMatrix *matrix, struct RwV3d *axis, RwReal *angle, struct RwV3d *center);
/*004d43f0*/ extern RwMatrix *RwMatrixRotate(RwMatrix *matrix, struct RwV3d *axis, RwReal angle, enum RwOpCombineType combine);
/*004d3cc0*/ extern RwMatrix *RwMatrixUpdate(RwMatrix *matrix);

/* Interpolates between the frames `from` and `to`: result = from, rotated by (rotation of from^-1 * to) * t_rot and moved by (to.pos - from.pos) * t_pos (both t clamped to 0..1). */
void matrix_interpolate_rotation_translation(RwMatrix *result, RwMatrix *from, RwMatrix *to, float t_rot, float t_pos)
{
  RwReal angle;
  struct RwV3d center;
  struct RwV3d axis;
  struct RwV3d back;
  RwMatrix from_copy;
  RwMatrix to_copy;
  RwMatrix inverse;
  volatile float dz;

  angle = 0.0f;
  if (t_rot <= float_1_0) {
    if (t_rot < k_f_0) {
      t_rot = 0.0f;
    }
  } else {
    t_rot = 1.0f;
  }
  if (t_pos <= float_1_0) {
    if (t_pos < k_f_0) {
      t_pos = 0.0f;
    }
  } else {
    t_pos = 1.0f;
  }
  from_copy = *from;
  center.x = from_copy.pos.x * k_f_n1;
  center.y = from_copy.pos.y * k_f_n1;
  center.z = from_copy.pos.z * k_f_n1;
  RwMatrixTranslate(&from_copy, &center, rwCOMBINEPOSTCONCAT);
  to_copy = *to;
  center.x = to_copy.pos.x * k_f_n1;
  center.y = to_copy.pos.y * k_f_n1;
  center.z = to_copy.pos.z * k_f_n1;
  RwMatrixTranslate(&to_copy, &center, rwCOMBINEPOSTCONCAT);
  RwMatrixInvert(&inverse, &from_copy);
  RwMatrixTransform(&to_copy, &inverse, rwCOMBINEPRECONCAT);
  RwMatrixQueryRotate(&to_copy, &axis, &angle, &center);
  RwMatrixTransform(result, from, rwCOMBINEREPLACE);
  RwMatrixRotate(result, &axis, angle * t_rot, rwCOMBINEPOSTCONCAT);
  dz = to->pos.z - from->pos.z;
  result->pos.x = (to->pos.x - from->pos.x) * t_pos + from->pos.x;
  result->pos.y = (to->pos.y - from->pos.y) * t_pos + from->pos.y;
  result->pos.z = dz * t_pos + from->pos.z;
  RwMatrixUpdate(result);
}

