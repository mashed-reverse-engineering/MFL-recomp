/* functions of batch27.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch27.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_types.h"
extern int g_sky_atomic;
extern void RpAtomicDestroy(int);
extern void RwFrameDestroy(int);
extern int ui_get_menu_69d1ec(void);
extern void ui_get_8c8730(void);
extern int g_car_8c5928;
extern void menu_scroll_clamp(void);
extern void ai_debug_draw_lines(void);
extern struct PlayerSlot g_player_slots[4];
extern int viewport_get_camera(int);
extern int get_viewport_camera_matrix(int);
extern void RwFrameTransform(int, int, int);
extern int g_course_6a07c4, g_course_6a07c8, g_course_6a07cc, g_course_6a07d0;
extern void object_destroy_frames(int);
extern void pool_list_destroy(int);
extern int g_course_keys_8c5724, g_course_nt1_8c5720;
extern void course_sky_atomic_destroy(void);
extern void course_object_destroy_frames_6a1f28(int);
extern void course_clumps_destroy(int);
extern void course_sandsnow_destroy_frames(void);
extern void ptank_atomic_destroy(void);
extern void rwp_world_current_activate_b(void);
extern int g_course_neustein_6a1d88, g_course_neustein_6a1d8c, g_course_neustein_6a1d90;
extern void nop_stub(void);
extern void course_sky_clumps_update(int);
extern void course_sky_clumps_render(void);
extern void camera_records_update_all(void *);
extern char g_car_8c7460[];
extern int g_car_8c747c, g_camera_records_8c7480;
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00452b90
void viewport_camera_sync_to_main(void)
{
    int cam = viewport_get_camera(0);
    int frame = *(int *)(cam + 4);
    int combine = 0;
    int m = get_viewport_camera_matrix(0);
    RwFrameTransform(frame, m, combine);
}

// FUNCTION: MFL 0x00451c50
void camera_records_warmup(int a, int b)
{
    int n = 100;
    do {
        camera_records_update_all(g_car_8c7460);
        n--;
    } while (n != 0);
    g_car_8c747c = a;
    g_camera_records_8c7480 = b;
}
