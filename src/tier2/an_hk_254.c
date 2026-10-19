#include "mfl_tier2.h"
extern char PTR_g_data_6c696f_005df7f0[];
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/*006a93d4*/ extern undefined4 g_powerup_icons_txd;
#define WEAPON_MINE (12)
#define WEAPON_MORTAR (7)
#define WEAPON_MACHINEGUN (9)
#define WEAPON_DEPTHCHARGE (10)
#define WEAPON_MISSILE (11)
#define WEAPON_FLAMETHROWER (16)
#define WEAPON_SHOTGUN (17)
#define WEAPON_FLARE (18)
#define WEAPON_CHAOS (21)
#define WEAPON_AIRSTRIKE (22)
/* powerup_icon_texture */

/* Returns the HUD icon texture for a powerup type id, looked up by name in g_powerup_icons_txd.
   powerup_type: the type id (WEAPON_* values above). Returns NULL for ids with no icon.
   "icon texture for a type id (2 flamethrower, 6/0xC/0xD mine, 7 mortar, 9 machinegun, 0xA
   depthcharge, 0xB missile, 0x10 flamethrower, 0x11 shotgun, 0x12 flare, 0x13 oil, 0x15 chaos, 0x16
   airstrike)" */

RwTexture * powerup_icon_texture(undefined4 powerup_type)

{
  RwTexture *texture;

  /* one case per powerup type: find its icon texture by name (0x13 oil passes a data pointer, not a literal) */
  switch(powerup_type) {
  case 2:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"flamethrower");
    return texture;
  default:
    return (RwTexture *)0x0;
  case 6:
  case WEAPON_MINE:
  case 0xd:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"mine");
    return texture;
  case WEAPON_MORTAR:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"mortar");
    return texture;
  case WEAPON_MACHINEGUN:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"machinegun");
    return texture;
  case WEAPON_DEPTHCHARGE:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"depthcharge");
    return texture;
  case WEAPON_MISSILE:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"missile");
    return texture;
  case WEAPON_FLAMETHROWER:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"flamethrower");
    return texture;
  case WEAPON_SHOTGUN:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"shotgun");
    return texture;
  case WEAPON_FLARE:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"flare");
    return texture;
  case 0x13:
    texture = RwTexDictionaryFindNamedTexture
                       (g_powerup_icons_txd,(RwChar *)&PTR_g_data_6c696f_005df7f0);
    return texture;
  case WEAPON_CHAOS:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"chaos");
    return texture;
  case WEAPON_AIRSTRIKE:
    texture = RwTexDictionaryFindNamedTexture(g_powerup_icons_txd,"airstrike");
    return texture;
  }
}


