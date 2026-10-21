#include "mfl_tier2.h"
/*00481ed0*/ extern int __cdecl table_get_6c3f38(int index);
typedef struct SurfaceRecord {
  int unused;
  unsigned int colour;
} SurfaceRecord;
/* course_surface_id_is_active_check */
/* "maps the surface colour code to the table_get_6c3f38 flags: 0xff0N3cff = flag N-1 set, 0xff0Nffff = flag N-1 clear (N = 1..10); 0xff010101 and 0xffff0080 are always active" */
unsigned int __cdecl course_surface_id_is_active_check(SurfaceRecord *surface)
{
  unsigned int colour;
  unsigned int number;

  colour = surface->colour;
  if (colour == 0xff010101 || colour == 0xffff0080) {
    return 1;
  }
  number = (colour >> 16) & 0xff;
  if ((colour >> 24) == 0xff && number >= 1 && number <= 10) {
    if ((colour & 0xffff) == 0x3cff) {
      return table_get_6c3f38(number - 1);
    }
    if ((colour & 0xffff) == 0xffff) {
      return table_get_6c3f38(number - 1) == 0;
    }
  }
  return 0;
}
