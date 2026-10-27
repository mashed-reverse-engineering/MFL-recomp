#include "mfl_tier2.h"
/*004a5930*/ extern int joypads_poll_frame();


/* input_update */

/* calls input_poll */

undefined4 input_update(void)
{
  joypads_poll_frame();
  return 1;
}
