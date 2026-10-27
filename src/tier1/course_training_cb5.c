#include "mfl_tier2.h"
/*0049c880*/ extern int SUB_0049c880();
/*0049f400*/ extern int fx_ptank_atomic_destroy();
/*00484ad0*/ extern int pool_list_destroy();
/*008bf084*/ extern undefined4 *g_course_training_8bf084;
/* course_training_cb5 */
/* destroys the training pool list; the original ends in a tail jump to pool_list_destroy */
int course_training_cb5(int param_1)
{
  fx_ptank_atomic_destroy();
  SUB_0049c880(0);
  return pool_list_destroy(g_course_training_8bf084);
}
