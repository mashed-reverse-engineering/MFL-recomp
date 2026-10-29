/* compile: /G5 */
#include "mfl_tier2.h"
/* x */
unsigned png_get_uint_32(unsigned char *buf)
{
  return (((((unsigned)buf[0] << 8) + buf[1]) << 8) + buf[2] << 8) + buf[3];
}
