#include "mfl_tier2.h"
/* sound_surface_ambience_lookup */
/* "(course id, surface colour, out a, out b): maps the colour under the car to the two ambience indices of its course" */
/* Every course has a default pair of indices (result 0); a few surface colours have their own pair.  The courses fall into 16 groups with identical tables. */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
int sound_surface_ambience_lookup(unsigned int course, unsigned int colour, int *out_a, int *out_b)
{
  switch (course) {
  case 0:
    /* courses 0 */
    switch (colour) {
    case 0xff646464:
      *out_a = 0;
      *out_b = 1;
      return 1;
    case 0xff968080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 0;
    *out_b = 1;
    return 0;
  case 1:
  case 5:
  case 20:
  case 39:
    /* courses 1, 5, 20, 39 */
    switch (colour) {
    case 0xff961e5a:
      *out_a = 0;
      *out_b = 0;
      return 1;
    case 0xffb4b4b4:
      *out_a = 3;
      *out_b = 2;
      return 1;
    case 0xffffffff:
      *out_a = 3;
      *out_b = 3;
      return 1;
    }
    *out_a = 3;
    *out_b = 3;
    return 0;
  case 2:
  case 6:
    /* courses 2, 6 */
    switch (colour) {
    case 0xffb48080:
      *out_a = 2;
      *out_b = 1;
      return 1;
    case 0xffc88080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    case 0xffdc8080:
      *out_a = 2;
      *out_b = 2;
      return 1;
    }
    *out_a = 2;
    *out_b = 2;
    return 0;
  case 3:
  case 4:
    /* courses 3, 4 */
    switch (colour) {
    case 0xff804000:
      *out_a = 1;
      *out_b = 0;
      return 0;
    case 0xffdc8080:
      *out_a = 2;
      *out_b = 2;
      return 1;
    }
    *out_a = 3;
    *out_b = 3;
    return 0;
  case 7:
  case 10:
  case 14:
  case 16:
  case 19:
  case 21:
  case 27:
  case 28:
  case 29:
  case 31:
  case 32:
  case 35:
  default:
    /* courses 7, 10, 14, 16, 19, 21, 27, 28, 29, 31, 32, 35 */
    *out_a = 3;
    *out_b = 3;
    return 0;
  case 8:
  case 25:
    /* courses 8, 25 */
    switch (colour) {
    case 0xff1e80b4:
      *out_a = 1;
      *out_b = 0;
      return 1;
    case 0xff961e5a:
      *out_a = 0;
      *out_b = 0;
      return 1;
    }
    *out_a = 0;
    *out_b = 0;
    return 0;
  case 9:
  case 34:
    /* courses 9, 34 */
    switch (colour) {
    case 0xff005ab4:
      *out_a = 3;
      *out_b = 2;
      return 1;
    case 0xff0064b4:
      *out_a = 3;
      *out_b = 3;
      return 1;
    case 0xff968080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 2;
    *out_b = 1;
    return 0;
  case 11:
    /* courses 11 */
    switch (colour) {
    case 0xff0064b4:
      *out_a = 3;
      *out_b = 3;
      return 1;
    case 0xff80c800:
      *out_a = 2;
      *out_b = 2;
      return 1;
    case 0xff968080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 0;
    *out_b = 1;
    return 0;
  case 12:
  case 18:
  case 22:
  case 23:
  case 24:
  case 38:
    /* courses 12, 18, 22, 23, 24, 38 */
    switch (colour) {
    case 0xff968080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 3;
    *out_b = 3;
    return 0;
  case 13:
    /* courses 13 */
    switch (colour) {
    case 0xff0064b4:
      *out_a = 3;
      *out_b = 3;
      return 1;
    case 0xffb4b4b4:
      *out_a = 0;
      *out_b = 1;
      return 1;
    case 0xffc8c8c8:
      *out_a = 2;
      *out_b = 2;
      return 1;
    }
    *out_a = 0;
    *out_b = 1;
    return 0;
  case 15:
  case 36:
    /* courses 15, 36 */
    switch (colour) {
    case 0xff646464:
      *out_a = 0;
      *out_b = 2;
      return 1;
    case 0xffb48080:
      *out_a = 1;
      *out_b = 2;
      return 1;
    case 0xffdc8080:
      *out_a = 1;
      *out_b = 1;
      return 0;
    }
    *out_a = 0;
    *out_b = 2;
    return 0;
  case 17:
    /* courses 17 */
    switch (colour) {
    case 0xff804000:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 3;
    *out_b = 3;
    return 0;
  case 26:
    /* courses 26 */
    switch (colour) {
    case 0xff80c800:
      *out_a = 1;
      *out_b = 1;
      return 1;
    case 0xff968080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    case 0xffc88080:
      *out_a = 2;
      *out_b = 0;
      return 1;
    }
    *out_a = 0;
    *out_b = 0;
    return 0;
  case 30:
    /* courses 30 */
    switch (colour) {
    case 0xffb48080:
      *out_a = 2;
      *out_b = 1;
      return 1;
    case 0xffc88080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 3;
    *out_b = 3;
    return 0;
  case 33:
    /* courses 33 */
    switch (colour) {
    case 0xff008000:
      *out_a = 1;
      *out_b = 0;
      return 1;
    case 0xff0080c8:
      *out_a = 0;
      *out_b = 3;
      return 1;
    case 0xff804040:
      *out_a = 0;
      *out_b = 1;
      return 1;
    case 0xff968080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 0;
    *out_b = 1;
    return 0;
  case 37:
    /* courses 37 */
    switch (colour) {
    case 0xff008000:
      *out_a = 1;
      *out_b = 0;
      return 1;
    case 0xff14508c:
      *out_a = 1;
      *out_b = 2;
      return 1;
    case 0xff80c800:
      *out_a = 1;
      *out_b = 1;
      return 1;
    case 0xff968080:
      *out_a = 0;
      *out_b = 1;
      return 1;
    }
    *out_a = 0;
    *out_b = 0;
    return 0;
  }
}
