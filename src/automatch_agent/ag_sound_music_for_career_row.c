#include "mfl_tier2.h"
/*00435f10*/ extern int get_career_row();


/* sound_music_for_career_row */

/* "switch on the career row returning the music id" */

undefined4 sound_music_for_career_row(void)

{
  switch(get_career_row()) {
  case 0:
    return 0x1e;
  case 1:
    return 3;
  case 2:
    return 0x27;
  case 3:
    return 0;
  case 4:
    return 0x26;
  case 5:
    return 0xb;
  case 6:
    return 0x19;
  case 7:
    return 2;
  case 8:
    return 0x24;
  case 9:
    return 0x22;
  case 10:
    return 0x25;
  case 11:
    return 0x21;
  case 12:
    return 0x1a;
  }
  return 0x1e;
}
