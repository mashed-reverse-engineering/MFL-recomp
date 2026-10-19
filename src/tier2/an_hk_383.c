#include "mfl_tier2.h"

/* bytes per queue entry (inferred: the sound table entry copied into it has the same stride) */

/* voice_queue_add */

/* "(line, team, player): adds a voice line to the team queue unless it is already queued / playing
   or the pair is on the same team (max 0x20 entries)" */

typedef struct { undefined4 d[10]; } Dw10;
extern void voice_queue_add(int line,undefined4 team,undefined4 player);


