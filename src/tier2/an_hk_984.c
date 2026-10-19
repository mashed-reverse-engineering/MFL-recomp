#include "mfl_tier2.h"
extern char UNK_005df51c[];
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00449700*/ extern undefined __cdecl course_sky_atomic_create(struct RwTexture *p0, int p1, undefined4 p2);
/*00481fd0*/ extern double random_range();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*006a06e0*/ extern undefined4 g_sky_atomic;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*008c5724*/ extern undefined4 g_course_keys_8c5724;
/* course_keys_cb0 */

void course_keys_cb0(int param_1)

{
  RwTexture *texture_dict;
  
  g_course_keys_8c5724 = func_0x004849e0(&UNK_005df51c,*(undefined4 *)(param_1 + 0xc9d0));
  texture_dict = (RwTexture *)find_texture_dict_b("shine");
  course_sky_atomic_create(texture_dict,0x3f800000,0xc0000000);
  func_0x00449b50();      /* tail call: fills the random sky direction table at 0x69f6e0 and sets the sky atomic flags */
  return;
}
