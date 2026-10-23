#include "mfl_tier2.h"
extern int g_course_8c53e0;
extern int g_load_shadow_8c53f0;
extern int g_load_8c53e4;
extern int g_load_8c53e8;
extern int g_load_shadow_8c5454;
extern int g_load_shadow_8c5458;
extern int g_load_shadow_8c545c;
extern int g_load_shadow_8c5460;
extern int g_load_shadow_8c5464;
extern int g_load_shadow_8c5468;
extern int g_load_shadow_8c546c;
extern int g_load_shadow_8c5470;
extern int g_load_shadow_8c5474;
extern int g_load_shadow_8c5478;
extern int g_load_shadow_8c547c;
extern int g_load_shadow_8c5480;
extern int g_load_shadow_8c5484;
extern int g_load_shadow_8c54c8;
extern int g_load_shadow_8c54cc;
extern int g_load_shadow_8c54d0;
extern int g_load_shadow_8c54d4;
extern int g_load_shadow_8c54d8;
extern int g_load_shadow_8c54dc;
extern int g_load_shadow_8c54e0;
extern int g_load_shadow_8c54e4;
extern int g_load_shadow_8c54e8;
extern int g_load_shadow_8c54ec;
extern int g_load_shadow_8c54f0;
extern int g_load_shadow_8c54f4;
extern int g_load_shadow_8c54f8;
extern void g_load_shadow_456540();
extern char s_barrel_shad_005df5b4[];
extern char s_crane_shad_005df5a8[];
/*004f6810*/ extern void RpClumpForAllAtomics();
/*004d5d90*/ extern int RwTexDictionaryFindNamedTexture();
/*004d1640*/ extern void RwFrameRotate();

/* IEEE-754 bits of 180.0f, the angle passed to RwFrameRotate */
#define ROTATE_ANGLE_180_DEG (0x43340000)
/* RwFrameRotate combine mode (inferred: rwCOMBINEPOSTCONCAT) */
#define RW_COMBINE_POSTCONCAT (2)

/*00457620*/
/* TIER2: unvalidated 2026-10-12 authority=diffemu */
/*
 * Sets up the shadow-texture globals for the course's models: clears the
 * course/load globals, runs g_load_shadow_456540 over the clump's atomics,
 * rotates the course frame 180 degrees about Y, and fills in the parameters
 * of the two shadow entries ("barrel" and "crane") from their named textures.
 * game_state (inferred): pointer to the game context; the clump is read at
 * +0xc114 and the texture dictionary at +0xc9d0.
 */
/* TIER2: validated 2026-10-22 authority=diffemu */
void load_shadow_models(char *game_state)
{
  float rotate_axis[3];

  g_course_8c53e0 = 0;
  g_load_shadow_8c53f0 = 0;
  g_load_8c53e4 = 0;
  g_load_8c53e8 = 0;
  RpClumpForAllAtomics(*(int *)(game_state + 0xc114), (int)g_load_shadow_456540, (char *)&g_course_8c53e0);
  rotate_axis[0] = 0.0f;
  rotate_axis[1] = 1.0f;
  rotate_axis[2] = 0.0f;
  /* barrel shadow parameters */
  g_load_shadow_8c5454 = (int)0xbe99999a;
  g_load_shadow_8c5458 = 0;
  g_load_shadow_8c545c = (int)0xbea66666;
  g_load_shadow_8c5460 = (int)0x3e99999a;
  g_load_shadow_8c5464 = 0;
  g_load_shadow_8c5468 = (int)0xbea66666;
  g_load_shadow_8c546c = (int)0xbe99999a;
  g_load_shadow_8c5470 = 0;
  g_load_shadow_8c5474 = (int)0x3ea66666;
  g_load_shadow_8c5478 = (int)0x3e99999a;
  g_load_shadow_8c547c = 0;
  g_load_shadow_8c5480 = (int)0x3ea66666;
  g_load_shadow_8c5484 = RwTexDictionaryFindNamedTexture(*(int *)(game_state + 0xc9d0), (int)s_barrel_shad_005df5b4);
  RwFrameRotate(*(int *)(g_course_8c53e0 + 4), (char *)rotate_axis, ROTATE_ANGLE_180_DEG, RW_COMBINE_POSTCONCAT);
  /* crane shadow parameters */
  g_load_shadow_8c54c8 = (int)0xbf400000;
  g_load_shadow_8c54cc = 0;
  g_load_shadow_8c54d0 = (int)0x40000000;
  g_load_shadow_8c54d4 = (int)0x3f400000;
  g_load_shadow_8c54d8 = 0;
  g_load_shadow_8c54dc = (int)0x40000000;
  g_load_shadow_8c54e0 = (int)0xbf400000;
  g_load_shadow_8c54e4 = 0;
  g_load_shadow_8c54e8 = (int)0x40a00000;
  g_load_shadow_8c54ec = (int)0x3f400000;
  g_load_shadow_8c54f0 = 0;
  g_load_shadow_8c54f4 = (int)0x40a00000;
  g_load_shadow_8c54f8 = RwTexDictionaryFindNamedTexture(*(int *)(game_state + 0xc9d0), (int)s_crane_shad_005df5a8);
}
