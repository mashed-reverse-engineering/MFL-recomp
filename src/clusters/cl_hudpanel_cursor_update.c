// cluster-entries: hudpanel_cursor_update
/* Installed cluster hudpanel_cursor_update (tools/cluster_install.py from output/cluster_pilot_tr/hudpanel_cursor_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster hudpanel_cursor_update: the HUD panel cursor of one player (4 slots of 0x6c bytes). State 1 places the cursor at the
   start position of its slot, state 2 moves it with the analog stick (mirror aware, clamped to the unit square), converts it to
   screen pixels and tests the cursor ray against the four car spheres (hover for 0.9 s, then a fire press selects it), state 3
   places the marker in front of the camera. */
typedef struct HudViewport {
    char unknown_00[0xc];
    int width;             /* 0x0c */
    int height;            /* 0x10 */
} HudViewport;
typedef struct HudCamera {
    char unknown_00[0x60];
    HudViewport *viewport; /* 0x60 */
} HudCamera;
typedef struct HudCursorTarget {
    char unknown_00[0x44];
    int disabled;          /* 0x44: nonzero = cursor not usable */
} HudCursorTarget;
typedef struct HudCursor {
    float x;               /* 0x00: cursor position in the unit square */
    float y;               /* 0x04 */
    char unknown_08[0xc];
    float screen_x;        /* 0x14: x * viewport width */
    float screen_y;        /* 0x18 */
    float ray[6];          /* 0x1c: cursor ray (start, end) */
    int state;             /* 0x34: 1 = start, 2 = move, 3 = place marker */
    char unknown_38[0x10];
    int select_progress;   /* 0x48 */
    int unknown_4c;
    int start_slot;        /* 0x50: index of the start position */
    int player;            /* 0x54: player input index */
    int hit;               /* 0x58: ray hits a car sphere */
    float hover_time;      /* 0x5c */
    int hovering;          /* 0x60: hovered for 0.9 s */
    int hit_car;           /* 0x64: index of the car under the cursor or -1 */
    HudCursorTarget *target; /* 0x68 */
} HudCursor;

extern float float_1_0;
extern float k_f_0_01;
extern float k_f_0_9;
extern float k_f_1_125;
extern float g_tick_seconds;
extern float g_hudpanel_603e80[][2];     /* start positions of the cursors */
extern struct GameState g_game_state;
extern struct PlayerInput g_player_inputs[12];
extern struct PlayerInput g_player_inputs_previous[12];
extern HudCamera *get_viewport_camera(int viewport);
extern int __cdecl race_setup_slot_used(int car);
extern void vehicle_get_position_158(int car, float *out);
extern int __cdecl RtLineSphereIntersectionTest(void *line, void *sphere, void *distance);

static void hudpanel_marker_place_in_front_of_camera_r(int p0, HudCursorTarget * p1);
extern void hudpanel_marker_place_in_front_of_camera(void);
/* was: ESI = cursor */
void hudpanel_cursor_update_n(HudCursor *cursor)
{
    struct {
        float stick_x;
        float scratch_x;     /* stick y, then the screen x, then the ray distance result */
        float scratch_y;     /* screen y */
        float sphere[4];     /* car position and radius */
    } frame;
    HudViewport *viewport;
    int car;

    if (cursor->state == 1) {
        cursor->x = g_hudpanel_603e80[cursor->start_slot][0];
        cursor->y = g_hudpanel_603e80[cursor->start_slot][1];
        if (cursor->target->disabled == 0)
            cursor->state = 2;
        return;
    }
    if (cursor->state == 2) {
        frame.scratch_x = g_player_inputs[cursor->player].analog_y;
        if (g_game_state.mirror_mode == 0)
            frame.stick_x = g_player_inputs[cursor->player].analog_x;
        else
            frame.stick_x = -g_player_inputs[cursor->player].analog_x;
        viewport = get_viewport_camera(-1)->viewport;
        cursor->x = frame.stick_x * k_f_0_01 + cursor->x;
        cursor->y = frame.scratch_x * k_f_0_01 + cursor->y;
        if (cursor->x < 0.0f)
            cursor->x = 0.0f;
        if (cursor->y < 0.0f)
            cursor->y = 0.0f;
        if (float_1_0 < cursor->x)
            cursor->x = 1.0f;
        if (float_1_0 < cursor->y)
            cursor->y = 1.0f;
        frame.scratch_x = (float)viewport->width * cursor->x;
        cursor->screen_x = frame.scratch_x;
        cursor->hit = 0;
        frame.scratch_y = (float)viewport->height * cursor->y;
        cursor->screen_y = frame.scratch_y;
        for (car = 0; car < 4; car++) {
            if (race_setup_slot_used(car) == 0)
                continue;
            vehicle_get_position_158(car, frame.sphere);
            frame.sphere[3] = frame.sphere[3] * k_f_1_125;
            if (RtLineSphereIntersectionTest(cursor->ray, frame.sphere, &frame.scratch_x) != 0) {
                cursor->hit = 1;
                if (cursor->hit_car != car) {
                    cursor->hit_car = car;
                    cursor->hover_time = 0.0f;
                }
                break;
            }
        }
        if (cursor->hit == 0) {
            cursor->hit_car = -1;
            cursor->hover_time = 0.0f;
        } else {
            cursor->hover_time = g_tick_seconds + cursor->hover_time;
        }
        if (cursor->hover_time >= k_f_0_9)
            cursor->hovering = 1;
        else
            cursor->hovering = 0;
        if (cursor->hit != 0 && cursor->hovering != 0 && g_player_inputs_previous[cursor->player].fire_copy == 0 &&
            g_player_inputs[cursor->player].fire_copy != 0) {
            cursor->select_progress = 0;
            cursor->hover_time = 0.0f;
            cursor->state = 3;
        }
        return;
    }
    if (cursor->state == 3) {
        hudpanel_marker_place_in_front_of_camera_r(cursor->hit_car, cursor->target);
        cursor->state = 1;
        cursor->unknown_4c = 0;
        cursor->hit_car = -1;
    }
}

/* hudpanel_marker_place_in_front_of_camera_r: C -> original hudpanel_marker_place_in_front_of_camera (registers eax,edi) */
__declspec(naked) void hudpanel_marker_place_in_front_of_camera_r(int p0, HudCursorTarget * p1)
{
    __asm {
        push edi
        mov eax, dword ptr [esp + 8]
        mov edi, dword ptr [esp + 12]
        call hudpanel_marker_place_in_front_of_camera
        pop edi
        ret
    }
}

/* entry thunk at the original address of hudpanel_cursor_update: register inputs (esi) -> natural C hudpanel_cursor_update_n; ret 0 */
__declspec(naked) void hudpanel_cursor_update(void)
{
    __asm {
        push esi
        call hudpanel_cursor_update_n
        add esp, 4
        ret
    }
}
