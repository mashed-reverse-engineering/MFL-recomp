#include "mfl_tier2.h"
/*004a39a0*/ extern int movie_stop(int p0);
/* Duplicate entry point that tail-calls movie_stop. */
int movie_stop_b(int p0)
{
  return movie_stop(p0);
}
