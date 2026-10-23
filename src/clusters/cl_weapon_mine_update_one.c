// cluster-entries: weapon_mine_update_one
/* Installed cluster weapon_mine_update_one (tools/cluster_install.py from output/cluster_pilot_dtrig/weapon_mine_update_one.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
extern double sin(double);
/*004d0ff0*/ extern RwMatrix *RwFrameGetLTM(struct RwFrame *frame);
/*004f4920*/ extern struct RpWorld *FUN_004f4920(struct RpClump *clump);        /* the world a clump is in (0 = none) */
/*004f46d0*/ extern struct RpWorld *RpWorldRemoveClump(struct RpWorld *world, struct RpClump *clump);
/*004d46e0*/ extern RwMatrix *RwMatrixScale(RwMatrix *matrix, RwV3d *scale, int combine);
/*004867f0*/ extern void particles_set_position(float *xyz);
/*00486820*/ extern void particles_set_size(float *size_xy);
/*00486840*/ extern void particles_set_color(unsigned char *rgba);
/*00486b50*/ extern void particles_add(void *particle_batch);
/*00496850*/ extern void trigger_sphere_register(void *sphere);
/*004c45e0*/ extern float FUN_004c45e0(float low, float high);                  /* uniform random float */
/*004866d0*/ extern void fx_emitter_spawn_b_default(float *position, void *style, float speed, float spread);
/*00485660*/ extern void particle_pool_find_oldest_or_free(int pool, RwMatrix *matrix, int flags);
/*00499100*/ extern void fx_spawn_from_matrix_2(void *owner, float scale);
/*00487670*/ extern void effect_sprite_spawn(int kind, void *owner, unsigned char *rgba, float width, float height);
/*0045f5a0*/ extern void explosion_queue(float *position, float size, float radius, float power, int kind);
/*007ef7f4*/ extern float g_tick_seconds;
/*0060da5c*/ extern unsigned char g_weapon_mines_60da5c[4];     /* glow colour while arming */
/*0060da60*/ extern unsigned char g_weapon_mines_60da60[4];     /* glow colour when armed */
/*006a8ab8*/ extern char g_weapon_6a8ab8[];                     /* particle batch of the mines */
/*0062ac74*/ extern char g_car_62ac74[];                        /* emitter style of the explosion sparks */
/*00463a50*/ extern char LAB_00463a50[];                        /* the trigger callback of a mine */

/* RenderWare object header: only the parent frame is read */
typedef struct RwObjectView {
    unsigned char type, sub_type, flags, private_flags;
    struct RwFrame *parent;
} RwObjectView;

/* One placed mine (0x6c bytes). */
typedef struct WeaponMine {
    RwMatrix transform;
    float sphere[4];                /* trigger sphere: centre xyz and radius, also the explosion position */
    int reserved50;
    RwObjectView *light_object;     /* the glowing light on top; its frame positions the particles */
    struct RpClump *model;          /* the mine model, taken out of the world when it blows up */
    int reserved5c;
    int state;                      /* MINE_* */
    float timer;                    /* seconds in the current state */
    int frames;                     /* frames spent arming */
} WeaponMine;

enum {
    MINE_UNUSED = 0,
    MINE_PLACED = 1,                /* only moved with its owner, nothing to do here */
    MINE_ARMING = 2,                /* blinks until the timer reaches MINE_ARMING_SECONDS */
    MINE_ARMED = 3,                 /* registers its trigger sphere every frame, blinks faster */
    MINE_EXPLODING = 4,             /* the sphere was touched: sparks, debris, flash */
    MINE_EXPLOSION_QUEUED = 5,
    MINE_REMOVED = 6                /* free the slot */
};

#define MINE_ARMING_SECONDS 0.75f
#define MINE_BLINK_SPEED 20.0f          /* radians per second of the glow size */
#define MINE_GLOW_AMPLITUDE 0.15f
#define MINE_GLOW_SIZE 0.2f
#define MINE_TRIGGER_KIND 0xd           /* trigger sphere owner kind */
#define MINE_SPHERE_RADIUS_AFTER_BLAST 0.1f

static void weapon_fx_slot_clear_b_r(void * p0);
extern void weapon_fx_slot_clear_b(void);
/* Blinking glow on top of the mine: size pulses with a sine of the timer. */
static void mine_glow_draw(WeaponMine *mine, double timer, unsigned char *colour)
{
    float size[2];

    size[1] = size[0] = (float)(sin(timer * MINE_BLINK_SPEED) * MINE_GLOW_AMPLITUDE + MINE_GLOW_SIZE);
    particles_set_position(&RwFrameGetLTM(mine->light_object->parent)->pos.x);
    particles_set_size(size);
    particles_set_color(colour);
    particles_add(g_weapon_6a8ab8);
}

/* Steps one mine through its life: arming blink, armed trigger sphere, explosion effects, removal. */
void weapon_mine_update_one(WeaponMine *mine)
{
    double t;

    if (mine->state == MINE_UNUSED || mine->state == MINE_PLACED)
        return;
    if (mine->state == MINE_ARMING) {
        mine->frames = mine->frames + 1;
        t = g_tick_seconds + mine->timer;
        mine->timer = (float)t;
        mine_glow_draw(mine, t, g_weapon_mines_60da5c);
        if (!(mine->timer < MINE_ARMING_SECONDS))
            mine->state = MINE_ARMED;
        return;
    }
    if (mine->state == MINE_ARMED) {
        struct {
            float sphere[4];
            int kind;
            void *owner;
            int zero;
            void *callback;
        } trigger;

        trigger.sphere[0] = mine->sphere[0];
        trigger.sphere[1] = mine->sphere[1];
        trigger.sphere[2] = mine->sphere[2];
        trigger.sphere[3] = mine->sphere[3];
        trigger.kind = MINE_TRIGGER_KIND;
        trigger.owner = mine;
        trigger.zero = 0;
        trigger.callback = LAB_00463a50;
        trigger_sphere_register(&trigger);
        t = g_tick_seconds + mine->timer;
        mine->timer = (float)t;
        mine_glow_draw(mine, t, g_weapon_mines_60da60);
        return;
    }
    if (mine->state == MINE_EXPLODING) {
        unsigned char flash_colour[4] = { 0xff, 0x80, 0x00, 0x40 };
        RwMatrix debris;
        RwV3d debris_scale;
        struct RpWorld *world;

        mine->state = MINE_EXPLOSION_QUEUED;
        fx_emitter_spawn_b_default(mine->sphere, g_car_62ac74, 90.0f, FUN_004c45e0(1.25f, 1.75f));
        mine->sphere[3] = MINE_SPHERE_RADIUS_AFTER_BLAST;
        world = FUN_004f4920(mine->model);
        if (world != 0)
            RpWorldRemoveClump(world, mine->model);
        debris = mine->transform;
        debris_scale.x = debris_scale.y = FUN_004c45e0(0.5f, 1.0f);
        debris_scale.z = 1.0f;
        RwMatrixScale(&debris, &debris_scale, 1);
        particle_pool_find_oldest_or_free(0, &debris, 0);
        fx_spawn_from_matrix_2(mine, FUN_004c45e0(0.4f, 0.6f));
        effect_sprite_spawn(0, mine, flash_colour, 3.0f, 3.0f);
        return;
    }
    if (mine->state == MINE_EXPLOSION_QUEUED) {
        float blast[4];

        blast[0] = mine->sphere[0];
        blast[1] = mine->sphere[1];
        blast[2] = mine->sphere[2];
        blast[3] = 1.5f;
        mine->state = MINE_REMOVED;
        explosion_queue(blast, FUN_004c45e0(20.0f, 30.0f), FUN_004c45e0(150.0f, 250.0f), FUN_004c45e0(250.0f, 350.0f), 2);
        return;
    }
    if (mine->state == MINE_REMOVED) {
        mine->state = MINE_UNUSED;
        weapon_fx_slot_clear_b_r(mine);
    }
}

/* weapon_fx_slot_clear_b_r: C -> original weapon_fx_slot_clear_b (registers esi) */
__declspec(naked) void weapon_fx_slot_clear_b_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_slot_clear_b
        pop esi
        ret
    }
}
