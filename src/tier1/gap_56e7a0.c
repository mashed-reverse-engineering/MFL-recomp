/* compile: /G5 */
#include "mfl_tier2.h"
/* Address of the member at +0x40 of a 2D object. (Rwp library, proposed name rwp_obj_0056e7a0) */
void *gap_56e7a0(void *o)
{
  return (char *)o + 0x40;
}
