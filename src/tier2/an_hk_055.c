#include "mfl_tier2.h"
/*005501d0*/ extern struct RpMaterial * RpMatFXMaterialSetEffects(struct RpMaterial *p0, enum RpMatFXMaterialFlags p1);
/*00550350*/ extern struct RpMaterial * RpMatFXMaterialSetupEnvMap(struct RpMaterial *p0, struct RwTexture *p1, struct RwFrame *p2, RwBool p3, RwReal p4);
/* material_set_envmap */

/* "RpMatFX env map effect for materials with a texture, else no effect" */

/* Applies the RpMatFX env map effect to a material. If the material has a texture, the
 * effect is enabled and set up; otherwise the effect is cleared (rpMATFXEFFECTNULL).
 * material: material to modify.
 * envmap_params: float array read as { [0] = env map coefficient (RwReal, inferred),
 *   [1] = RwTexture * env map texture, [2] = RwFrame * frame }.
 * Returns material. */
RpMaterial * material_set_envmap(RpMaterial *material,RwReal *envmap_params)

{
  /* material has a texture: enable the env map effect and set it up from envmap_params */
  if (RpMaterial_texture(material) != (RwTexture *)0x0) {
    RpMatFXMaterialSetEffects(material,rpMATFXEFFECTENVMAP);
    RpMatFXMaterialSetupEnvMap(material,*(RwTexture **)&envmap_params[1],*(RwFrame **)&envmap_params[2],1,*envmap_params);
    return material;
  }
  RpMatFXMaterialSetEffects(material,rpMATFXEFFECTNULL);
  return material;
}


