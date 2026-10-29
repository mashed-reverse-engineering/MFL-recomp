#include "mfl_tier2.h"
extern char UNK_004537f0[];
extern char UNK_005df4d8[];
extern char UNK_00453860[];
/* Course setup for the Egypt course (inferred): calls func_0x004801f0 twice, first with the "EgyptPushColumn1" string and UNK_004537f0, then with UNK_005df4d8 and UNK_00453860. Takes no ... */
/* course_egypt_cb12
 * Course setup for the Egypt course (inferred): calls func_0x004801f0 twice,
 * first with the "EgyptPushColumn1" string and UNK_004537f0, then with
 * UNK_005df4d8 and UNK_00453860. Takes no parameters, returns nothing.
 * Callee and its arguments are unnamed in the dossier, so the purpose is a guess.
 */
void course_egypt_cb12(void)
{
  func_0x004801f0("EgyptPushColumn1",&UNK_004537f0);
  func_0x004801f0(&UNK_005df4d8,&UNK_00453860);
  return;
}
