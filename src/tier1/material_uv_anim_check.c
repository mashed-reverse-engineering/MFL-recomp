#include "mfl_tier2.h"
/*005625f0*/ extern RwBool __cdecl RpMaterialUVAnimExists(struct RpMaterial *p0);


/* material_uv_anim_check */

/* Checks whether a material has UV animation and sets the flag if present. */
RpMaterial * material_uv_anim_check(RpMaterial *material,int *has_uv_anim)
{
  RwBool uv_anim_exists;
  
  uv_anim_exists = RpMaterialUVAnimExists(material);
  if (uv_anim_exists != 0) {
    *has_uv_anim = 1;
  }
  return material;
}

