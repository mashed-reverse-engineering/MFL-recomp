// cluster-entries: hud_label_draw_text
/* Installed cluster hud_label_draw_text (tools/cluster_install.py from output/cluster_pilot8/hud_label_draw_text.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster hud_label_draw_text: draws the HUD text labels (8 label slots of 0xb0 bytes) with the Rt2d font: the 640x480 position becomes
   the Rt2d text space (1.333 wide, 1 high, v flipped; height = scale / 480), alignment 1 centres the text (kept inside 0 .. 1.33 horizontally);
   alignment 2 moves the start of the text RIGHT by its width (x + width; not a right-aligned text ending at x).  The four colour arguments are RwRGBA (4 byte) pointers:
   the label passes its +0xa4 colour as c0 / c1 and its +0xa0 colour as c2 / c3. */
typedef struct HudLabel {
    unsigned short text[0x40];   /* 0x00: wide string */
    float world[3];              /* 0x80 */
    float x;                     /* 0x8c: screen position in 640x480 */
    float y;                     /* 0x90 */
    float delta_x;               /* 0x94 */
    float delta_y;               /* 0x98 */
    int flags;                   /* 0x9c: nonzero = in use */
    float colour_a0;             /* 0xa0: RGBA bytes (the brush corners c2 / c3) */
    float colour_a4;             /* 0xa4: RGBA bytes (the brush corners c0 / c1) */
    float scale;                 /* 0xa8: text height factor */
    int param_ac;                /* 0xac */
} HudLabel;
typedef struct TextState {
    int unknown_00;
    void *texture;               /* 0x04 */
    void *font;                  /* 0x08 */
    void *brush;                 /* 0x0c */
} TextState;

extern HudLabel g_car_6c2958[8];                     /* the label slots */
extern TextState *g_text_state;
extern struct RwGlobalsRelease *RwEngineInstance;
extern float k_f_0_5;
extern float k_f_0_0020833334;
extern float k_f_1_333;
extern float k_f_1_33;
extern void **FUN_004c4280(void *texture);                /* raster of the texture */
extern void FUN_00567780(void *texture);
extern void __cdecl Rt2dCTMPush(void);
extern void __cdecl Rt2dCTMSetIdentity(void);
extern int __cdecl Rt2dCTMRotate(float angle);
extern void __cdecl Rt2dCTMTranslate(float x, float y);
extern void __cdecl Rt2dBrushSetRGBA(void *brush, const float *c0, const float *c1, const float *c2, const float *c3);
extern float __cdecl Rt2dFontGetStringWidth(void *font, const void *text, float height);
extern void __cdecl FUN_00567760(void *font, const void *text, float height, const float *origin, void *brush);   /* Rt2dFontShow */
extern void __cdecl FUN_00563340(void);                                                                           /* Rt2dCTMPop */

static void rt2d_text_pixel_to_uv_r(float * p0, float p1, float p2);
extern void rt2d_text_pixel_to_uv(void);
/* was: EDI = text, then the stack arguments */
void hud_label_draw_text_n(const void *text, float x, float y, const float *c0, const float *c1, const float *c2, const float *c3, float scale, int align)
{
    float origin[2];
    float uv[2];
    float height, width;
    void **raster;

    origin[0] = 0.0f;
    origin[1] = 0.0f;
    raster = FUN_004c4280(g_text_state->texture);
    FUN_00567780(g_text_state->texture);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, *raster);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)2);
    height = scale * k_f_0_0020833334;
    rt2d_text_pixel_to_uv_r(uv, x, y);
    Rt2dCTMPush();
    Rt2dCTMSetIdentity();
    Rt2dCTMTranslate(uv[0], uv[1]);
    Rt2dCTMRotate(0.0f);
    Rt2dCTMTranslate(-uv[0], -uv[1]);
    Rt2dBrushSetRGBA(g_text_state->brush, c0, c1, c2, c3);
    width = Rt2dFontGetStringWidth(g_text_state->font, text, height);
    if (align == 1) {
        uv[0] = uv[0] - width * k_f_0_5;
        uv[1] = uv[1] - height * k_f_0_5;
        if (uv[0] < 0.0f)
            uv[0] = 0.0f;
        if (k_f_1_333 < width + uv[0])
            uv[0] = k_f_1_33 - width;
    } else if (align == 2) {
        uv[0] = width + uv[0];
    }
    Rt2dCTMTranslate(uv[0], uv[1]);
    FUN_00567760(g_text_state->font, text, height, origin, g_text_state->brush);
    FUN_00563340();
}

/* rt2d_text_pixel_to_uv_r: C -> original rt2d_text_pixel_to_uv (registers eax,s0,s1) */
__declspec(naked) void rt2d_text_pixel_to_uv_r(float * p0, float p1, float p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call rt2d_text_pixel_to_uv
        add esp, 8
        ret
    }
}

/* entry thunk at the original address of hud_label_draw_text: register inputs (edi,s0,s1,s2,s3,s4,s5,s6,s7) -> natural C hud_label_draw_text_n; ret 0 */
__declspec(naked) void hud_label_draw_text(void)
{
    __asm {
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push edi
        call hud_label_draw_text_n
        add esp, 36
        ret
    }
}
