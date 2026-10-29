/* compile: /G5 */
#include "mfl_tier2.h"
/*004ddd30*/ extern void __cdecl ImagePathForAllFullNames(void *name, int n, void *cb, void **out);
extern void __cdecl ImageDetermineExtender(void);
/* Finds the image file type of a name through the registered extension callbacks. */
void *RwImageFindFileType(void *name)
{
  void *r = 0;
  ImagePathForAllFullNames(name, 0x14, ImageDetermineExtender, &r);
  return r;
}
