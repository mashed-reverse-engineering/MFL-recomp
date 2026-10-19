#include "mfl_tier2.h"
/*00464330*/ extern void * powerup_icon_texture();
/*004c4050*/ extern void * FUN_004c4050(undefined4 p0);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
#define RECORD_POWERUP_TYPE_SLOT (9)
/* weapon_powerup_set_icon_texture */

/* "sets the powerup icon texture (type at +0x24) on the material of the record" */

void weapon_powerup_set_icon_texture(undefined4 *record)

{
  RwTexture *texture;
  RpMaterial *material;

  /* record[0] holds the material handle, record[RECORD_POWERUP_TYPE_SLOT] the powerup type (inferred) */
  texture = (RwTexture *)powerup_icon_texture(record[RECORD_POWERUP_TYPE_SLOT]);
  material = (RpMaterial *)FUN_004c4050(*record);
  RpMaterialSetTexture(material,texture);
  return;
}


