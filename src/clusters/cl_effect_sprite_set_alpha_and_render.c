// cluster-entries: effect_sprite_set_alpha_and_render effect_sprites_render_all
/* Installed cluster effect_sprite_set_alpha_and_render (tools/cluster_install.py from output/cluster_pilot6/effect_sprite_set_alpha_and_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster effect_sprite_set_alpha_and_render: the two sets of effect sprites (16 slots of 0x60 bytes each, visible bits in a flag word per set).
   A visible sprite with a non-zero fade value gets alpha = (1 - fade / fade_max) * base_alpha written into the colour of its patch mesh material,
   then its atomic is rendered through its render callback. */
extern float k_f_0;
extern float float_1_0;
extern int g_gfx_6c51a8;
extern char g_gfx_6c51b0[];
extern int g_gfx_6c57b0;
extern char g_gfx_6c57f8[];
extern char g_gfx_6c5df8[];
extern int g_gfx_6c5e10;
extern struct RwGlobalsRelease *RwEngineInstance;
extern void *FUN_00559a00(void *atomic);
extern unsigned *RpPatchMeshGetMaterial(void *patch, int index);

typedef struct EffectSprite {
    char unknown_00[0x40];
    unsigned colour;       /* 0x40: rgb, the alpha byte (0x43) is the base alpha */
    void *atomic;          /* 0x44: patch mesh atomic, render callback at +0x48 of it */
    float fade;            /* 0x48: current fade value, 0 = not fading */
    float fade_max;        /* 0x4c */
} EffectSprite;
#define SPRITE_SIZE 0x60

/* was: EAX = sprite */
void effect_sprite_set_alpha_and_render_n(EffectSprite *sprite)
{
    float fade;
    unsigned *material;
    unsigned colour;

    if (sprite->fade != k_f_0) {
        fade = sprite->fade / sprite->fade_max;
        material = RpPatchMeshGetMaterial(FUN_00559a00(sprite->atomic), 0);
        colour = sprite->colour;
        ((unsigned char *)&colour)[3] = (unsigned char)(int)((float_1_0 - fade) * (int)((unsigned char *)&sprite->colour)[3]);
        material[1] = colour;
        (*(void (**)(void *))((char *)sprite->atomic + 0x48))(sprite->atomic);
    }
}

void effect_sprites_render_all(void)
{
    int slot;
    EffectSprite *sprite;
    void *saved_src_blend;
    void *saved_dest_blend;

    if (RwCameraGetCurrentCamera() == (void *)0x0) {
        g_gfx_6c5e10 = 0;
    }
    else {
        g_gfx_6c5e10 = *(int *)((char *)RwCameraGetCurrentCamera() + 4) + 0x40;
    }
    if ((g_gfx_6c57b0 != 0) || (g_gfx_6c51a8 != 0)) {
        RwRenderStateGet(rwRENDERSTATESRCBLEND, &saved_src_blend);
        RwRenderStateGet(rwRENDERSTATEDESTBLEND, &saved_dest_blend);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
        slot = 0;
        sprite = (EffectSprite *)g_gfx_6c57f8;
        do {
            if ((*((unsigned char *)&g_gfx_6c57b0 + (slot >> 3)) & (unsigned char)(1 << ((unsigned char)slot & 7))) != 0) {
                effect_sprite_set_alpha_and_render_n(sprite);
            }
            sprite = (EffectSprite *)((char *)sprite + SPRITE_SIZE);
            slot = slot + 1;
        } while ((char *)sprite < g_gfx_6c5df8);
        slot = 0;
        sprite = (EffectSprite *)g_gfx_6c51b0;
        do {
            if ((*((unsigned char *)&g_gfx_6c51a8 + (slot >> 3)) & (unsigned char)(1 << ((unsigned char)slot & 7))) != 0) {
                effect_sprite_set_alpha_and_render_n(sprite);
            }
            sprite = (EffectSprite *)((char *)sprite + SPRITE_SIZE);
            slot = slot + 1;
        } while ((char *)sprite < (char *)&g_gfx_6c57b0);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, saved_src_blend);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, saved_dest_blend);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
    }
}

/* entry thunk at the original address of effect_sprite_set_alpha_and_render: register inputs (eax) -> natural C effect_sprite_set_alpha_and_render_n; ret 0 */
__declspec(naked) void effect_sprite_set_alpha_and_render(void)
{
    __asm {
        push eax
        call effect_sprite_set_alpha_and_render_n
        add esp, 4
        ret
    }
}
