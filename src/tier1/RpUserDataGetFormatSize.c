/* compile: /G5 */
#include "mfl_tier2.h"
/* Element size of a user data format: 4 bytes for int, real and string pointers. */
int RpUserDataGetFormatSize(int f)
{
  switch (f) { case 1: return 4; case 2: return 4; case 3: return 4; }
  return 0;
}
