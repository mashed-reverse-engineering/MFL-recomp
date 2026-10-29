/* compile: /G5 */
#include "mfl_tier2.h"
/* Address of the bounding sphere inside a DMorph target. */
void *RpDMorphTargetGetBoundingSphere(void *o)
{
  return (char *)o + 0x30;
}
