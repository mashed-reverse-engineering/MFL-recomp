// cluster-entries: hud_pickup_icons_update_render
/* Installed cluster hud_pickup_icons_update_render (tools/cluster_install.py from output/cluster_pilot_dtrig/hud_pickup_icons_update_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d53a0*/ extern RwReal RwV3dLength(RwV3d *v);
extern double sin(double);
extern double cos(double);
/*004b2c30*/ extern double __cdecl _pow(double x, double y);   /* the CRT function (cl would expand pow() to _CIpow) */
/*00475730*/ extern char *get_viewport_camera(int viewport);
/*004757a0*/ extern RwMatrix *get_viewport_camera_matrix(int viewport);
/*0047b520*/ extern void car_current_matrix(RwMatrix **matrix_out, int car);
/*004867f0*/ extern void particles_set_position(float *xyz);
/*00486820*/ extern void particles_set_size(float *size_xy);
/*00486840*/ extern void particles_set_color(unsigned char *rgba);
/*00486880*/ extern void gfx_set_gfx_6c50c8(float rotation);
/*00486890*/ extern void gfx_set_params_6c5188(float *uv_rect);
/*00486b50*/ extern void particles_add(void *particle_batch);
/*00486c40*/ extern void particles_flush(void *particle_batch);
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef804*/ extern int g_tick_number;

/* One pickup icon of one player: fade in [0, 1] (0 = hidden), direction of the fade. */
typedef struct PickupIconSlot {
    float fade;
    int reserved;
    int fading_in;   /* 0: the icon fades out, else in */
} PickupIconSlot;

/*00653168*/ extern PickupIconSlot g_particles_653168[4][5];
/*00653258*/ extern char g_trail_653258[];

#define PICKUP_PLAYERS 4
#define PICKUP_ICON_KINDS 5
#define PICKUP_ICON_BOOST 3                 /* the icon kind that is drawn larger, brighter and rotated */
#define PICKUP_FADE_SPEED 2.0f              /* fade units per tick second */
#define PICKUP_ALPHA_SCALE 128.0f           /* fully faded in = alpha 128 */
#define PICKUP_ALPHA_MAX 255.0f
#define PICKUP_HEIGHT_OFFSET 0.3f           /* the icon floats above the car */
#define PICKUP_WOBBLE_SPEED 0.06f           /* radians per game tick */
#define PICKUP_ROTATION_SWING 0.62831855f
#define PICKUP_ROTATION_BASE 0.7853982f
#define PICKUP_BOOST_ROTATION 0.7854f
#define PICKUP_BOOST_SIZE_SCALE 1.25f
#define PICKUP_SIZE_PER_DISTANCE 0.25f      /* icon size grows with the root of the camera distance, times the projection scale */
#define PICKUP_SIZE_WOBBLE 0.3f
#define VIEWPORT_CAMERA_SCALE(camera) (*(float *)((camera) + 0x68))   /* projection scale of the viewport camera */

/* Fades the pickup icons of every player in or out and draws the visible ones as billboards above the cars, with a
   sine/cosine wobble of rotation and size. */
static int hud_icon_uv_rect_get_r(int p0, float * p1);
extern void hud_icon_uv_rect_get(void);
void hud_pickup_icons_update_render(void)
{
    unsigned char colour[4] = { 255, 255, 255, 128 };
    float size[2] = { 0.5f, 0.5f };
    float position[3];
    float uv_rect[4];  /* u0, v0, u1, v1 */
    RwV3d to_camera;
    RwMatrix *car_matrix;
    RwMatrix *view_matrix;
    char *camera;
    int player;
    int kind;

    camera = get_viewport_camera(0);
    for (player = 0; player < PICKUP_PLAYERS; player++) {
        for (kind = 0; kind < PICKUP_ICON_KINDS; kind++) {
            PickupIconSlot *slot = &g_particles_653168[player][kind];
            float distance;
            float wobble_sin;
            float wobble_cos;
            float rotation;
            double scale;

            if (!(slot->fade > 0.0f))
                continue;
            if (slot->fading_in == 0)
                slot->fade = slot->fade - (g_tick_seconds + g_tick_seconds);
            else
                slot->fade = g_tick_seconds + g_tick_seconds + slot->fade;
            if (slot->fade < 0.0f) {
                slot->fade = 0.0f;
                continue;
            }
            if (slot->fade > 1.0f)
                slot->fade = 1.0f;
            colour[3] = (unsigned char)(int)(slot->fade * PICKUP_ALPHA_SCALE);

            car_current_matrix(&car_matrix, player);
            position[0] = car_matrix->pos.x;
            position[1] = car_matrix->pos.y + PICKUP_HEIGHT_OFFSET;
            position[2] = car_matrix->pos.z;
            view_matrix = get_viewport_camera_matrix(0);
            to_camera.x = position[0] - view_matrix->pos.x;
            to_camera.y = position[1] - view_matrix->pos.y;
            to_camera.z = position[2] - view_matrix->pos.z;
            distance = RwV3dLength(&to_camera);
            hud_icon_uv_rect_get_r(kind, uv_rect);

            wobble_sin = (float)sin(g_tick_number * PICKUP_WOBBLE_SPEED);
            wobble_cos = (float)cos(g_tick_number * PICKUP_WOBBLE_SPEED);
            rotation = wobble_sin * PICKUP_ROTATION_SWING - PICKUP_ROTATION_BASE;
            scale = _pow(distance, 0.5);
            if (kind == PICKUP_ICON_BOOST) {
                double alpha = colour[3] + colour[3];

                if (alpha > PICKUP_ALPHA_MAX)
                    alpha = PICKUP_ALPHA_MAX;
                colour[3] = (unsigned char)(int)alpha;
                rotation = rotation + PICKUP_BOOST_ROTATION;
                scale = scale * PICKUP_BOOST_SIZE_SCALE;
            }
            size[0] = (float)(scale * VIEWPORT_CAMERA_SCALE(camera) * PICKUP_SIZE_PER_DISTANCE
                              + wobble_cos * wobble_sin * PICKUP_SIZE_WOBBLE);
            size[1] = size[0];

            particles_set_position(position);
            particles_set_color(colour);
            particles_set_size(size);
            gfx_set_gfx_6c50c8(rotation);
            gfx_set_params_6c5188(uv_rect);
            particles_add(g_trail_653258);
        }
    }
    particles_flush(g_trail_653258);
}

/* hud_icon_uv_rect_get_r: C -> original hud_icon_uv_rect_get (registers esi,ecx) */
__declspec(naked) int hud_icon_uv_rect_get_r(int p0, float * p1)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        call hud_icon_uv_rect_get
        pop esi
        ret
    }
}
