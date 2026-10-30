#include "mfl_tier2.h"
/*00409f30*/ extern double player_progress_b();
/*0040c820*/ extern int player_add_points();
/*00426b40*/ extern int car_kill();
/*005dd62c*/ extern float k_f_100;
/*005dd7f4*/ extern float k_f_80;
/*005ddea0*/ extern float k_f_20;
/* player_progress_difference */

/* Scores the race order of three players by their progress (player_progress_b).
 * player_a, player_b, player_c: player ids. points: score step.
 * First place: +points * 2, no car_kill. Second: car_kill + points.
 * Last: car_kill + -points. Progress is compared with wrap around (see the
 * blocks below), so the order is decided on the wrapped values. Returns nothing.
 * (was: "difference of two player_progress_b values with wrap around at 100 (inferred)")
 */

void __cdecl player_progress_difference(undefined4 player_a,undefined4 player_b,undefined4 player_c,int points)

{
  double progress_left;
  double progress_right;
  float left_f;

  /* wrap-aware compare: if one value is above 80 and the other below 20,
     the one above 80 is shifted down by 100 (the 80/20/100 values are inferred from the names) */
  left_f = (float)player_progress_b(player_a);      /* the original spills the first value as a float (fstp dword), the second stays unrounded in st(0) */
  progress_right = (double)player_progress_b(player_b);
  progress_left = (double)left_f;
  if (!(progress_left > (double)k_f_80) || !(progress_right < (double)k_f_20)) {      /* NaN takes the first branch (original: jne / jp) */
    if (((double)k_f_80 < progress_right) && (progress_left < (double)k_f_20)) {
      progress_right = progress_right - (double)k_f_100;
    }
  }
  else {
    progress_left = progress_left - (double)k_f_100;
  }
  /* player_a <= player_b: check player_c against player_a */
  if (!(progress_left > progress_right)) {      /* NaN takes this branch (original: test ah,0x41; jne) */
    left_f = (float)player_progress_b(player_c);      /* the original spills the first value as a float (fstp dword), the second stays unrounded in st(0) */
    progress_right = (double)player_progress_b(player_a);
    progress_left = (double)left_f;
    if (!(progress_left > (double)k_f_80) || !(progress_right < (double)k_f_20)) {      /* NaN takes the first branch (original: jne / jp) */
      if (((double)k_f_80 < progress_right) && (progress_left < (double)k_f_20)) {
        progress_right = progress_right - (double)k_f_100;
      }
    }
    else {
      progress_left = progress_left - (double)k_f_100;
    }
    /* player_a < player_c: player_a is last of a and c, then order player_b vs player_c */
    if (progress_right < progress_left) {
      car_kill(player_a);
      player_add_points(player_a,-points);
      left_f = (float)player_progress_b(player_b);      /* the original spills the first value as a float (fstp dword), the second stays unrounded in st(0) */
      progress_right = (double)player_progress_b(player_c);
      progress_left = (double)left_f;
      if (!(progress_left > (double)k_f_80) || !(progress_right < (double)k_f_20)) {      /* NaN takes the first branch (original: jne / jp) */
        if (((double)k_f_80 < progress_right) && (progress_left < (double)k_f_20)) {
          progress_right = progress_right - (double)k_f_100;
        }
      }
      else {
        progress_left = progress_left - (double)k_f_100;
      }
      /* player_c < player_b: player_c is second, player_b is first */
      if (progress_right < progress_left) {
        car_kill(player_c);
        player_add_points(player_c,points);
        player_add_points(player_b,points * 2);
        return;
      }
      car_kill(player_b);
      player_add_points(player_b,points);
      player_add_points(player_c,points * 2);
      return;
    }
    /* player_c <= player_a: player_c is last, player_a second, player_b first */
    car_kill(player_c);
    player_add_points(player_c,-points);
    car_kill(player_a);
    player_add_points(player_a,points);
    player_add_points(player_b,points * 2);
    return;
  }
  /* player_b < player_a: compare player_c against player_b */
  left_f = (float)player_progress_b(player_c);      /* the original spills the first value as a float (fstp dword), the second stays unrounded in st(0) */
  progress_right = (double)player_progress_b(player_b);
  progress_left = (double)left_f;
  if (!(progress_left > (double)k_f_80) || !(progress_right < (double)k_f_20)) {      /* NaN takes the first branch (original: jne / jp) */
    if (((double)k_f_80 < progress_right) && (progress_left < (double)k_f_20)) {
      progress_right = progress_right - (double)k_f_100;
    }
  }
  else {
    progress_left = progress_left - (double)k_f_100;
  }
  /* player_c <= player_b: player_c is last, player_b second, player_a first */
  if (!(progress_left > progress_right)) {      /* NaN takes this branch (original: test ah,0x41; jne) */
    car_kill(player_c);
    player_add_points(player_c,-points);
    car_kill(player_b);
    player_add_points(player_b,points);
    player_add_points(player_a,points * 2);
    return;
  }
  /* player_b is last of b and c: compare player_a against player_c for second/first */
  car_kill(player_b);
  player_add_points(player_b,-points);
  left_f = (float)player_progress_b(player_a);      /* the original spills the first value as a float (fstp dword), the second stays unrounded in st(0) */
  progress_right = (double)player_progress_b(player_c);
  progress_left = (double)left_f;
  if (!(progress_left > (double)k_f_80) || !(progress_right < (double)k_f_20)) {      /* NaN takes the first branch (original: jne / jp) */
    if (((double)k_f_80 < progress_right) && (progress_left < (double)k_f_20)) {
      progress_right = progress_right - (double)k_f_100;
    }
  }
  else {
    progress_left = progress_left - (double)k_f_100;
  }
  if (progress_right < progress_left) {
    car_kill(player_c);
    player_add_points(player_c,points);
    player_add_points(player_a,points * 2);
    return;
  }
  car_kill(player_a);
  player_add_points(player_a,points);
  player_add_points(player_c,points * 2);
  return;
}


