#include "mfl_tier2.h"
/*008bf050*/ extern undefined4 g_car_8bf050;


/* course_get_8bf050 */

/* "g_car_8bf050[index]" */

/* Read entry index of the car table at g_car_8bf050. */
undefined4 __cdecl course_get_8bf050(int index)
{
  return (&g_car_8bf050)[index];
}
