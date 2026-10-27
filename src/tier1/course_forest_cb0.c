#include "mfl_tier2.h"
extern char UNK_005df51c[];
/*008c4b08*/ extern undefined4 g_course_forest_8c4b08;
/* course_forest_cb0 */
/* Course-specific setup for the Forest course (course id 34, COURSE_FOREST), slot cb0 (inferred).
 * game_state: pointer-sized base of the game state block (inferred); the value at offset 0xc9d0
 * is passed to func_0x004849e0 together with UNK_005df51c, and its result is stored in
 * g_course_forest_8c4b08. Returns nothing. */
void course_forest_cb0(char *game_state)
{
  func_0x00458e70(game_state);
  /* Look up a value from the state block and keep the result in the global (inferred). */
  g_course_forest_8c4b08 = func_0x004849e0(&UNK_005df51c,*(undefined4 *)(game_state + 0xc9d0));
  return;
}
