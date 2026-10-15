/* functions of batch22.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch22.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_types.h"
extern struct Car player_car[16];
extern struct PlayerSlot g_player_slots[4];
extern int g_sound_player_count;
extern unsigned char g_car_6af5c0[32];
extern int g_course_object_count, g_course_object_pair_count;
extern int g_car_6c2658[];
extern char *RwEngineInstance;
extern int car_get_mode(unsigned int, unsigned int *, unsigned int *);
extern int car_is_active(unsigned int);
extern void object_render(int);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00468a20
int car_is_playable(int car)
{
    unsigned int mode, mode2;
    car_get_mode(car, &mode, &mode2);
    if (mode != 0)
        return 0;
    return car_is_active(car) == 1;
}
