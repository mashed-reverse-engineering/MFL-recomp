#include "mfl_tier2.h"
extern char UNK_005df51c[];
/*0042a430*/ extern int course_get_ambient_light();
/*0042a440*/ extern int course_get_directional_light();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*0050b5e0*/ extern int nop_stub();
/*006a1f2c*/ extern undefined4 g_course_nt1_6a1f2c;
/*008c5720*/ extern undefined4 g_course_nt1_8c5720;
/* course_nt1_cb0: course NT1 per-frame update (inferred). Runs the nop stub and
   func_0x00455770 on game_state, stores a value derived from game_state + 0xc9d0 into
   g_course_nt1_8c5720, then stores into g_course_nt1_6a1f2c the result of func_0x004a5d50
   over the first atomic of the clump at game_state + 0xc120 and the light values.
   Parameter: game_state (inferred), the course state pointer. No return value. */

void course_nt1_cb0(char *game_state)

{
  undefined4 first_atomic;
  undefined4 light_value;

  nop_stub(game_state);
  g_course_nt1_8c5720 = func_0x004849e0(&UNK_005df51c,*(undefined4 *)(game_state + 0xc9d0));
  func_0x00455770(game_state);
  /* Lighting: directional light first, then ambient light fed the directional value. */
  first_atomic = clump_first_atomic(*(undefined4 *)(game_state + 0xc120));
  light_value = course_get_directional_light();
  light_value = course_get_ambient_light(light_value);
  g_course_nt1_6a1f2c =
       func_0x004a5d50(first_atomic,*(undefined4 *)(game_state + 0xc004),game_state + 0xc00,light_value);
  return;
}
