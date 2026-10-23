// cluster-entries: ui_draw_curved_wipe
/* Installed cluster ui_draw_curved_wipe (tools/cluster_install.py from output/cluster_pilot_dtrig/ui_draw_curved_wipe.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*0089ddfc*/ extern int g_gfx_89ddfc;                  /* target alpha of the wipe (0..255) */
/*0089de00*/ extern int g_gfx_89de00;                  /* current alpha, follows the target by 2 per frame */
/*007ef7f8*/ extern float g_menu_wave_time;            /* animation clock of the wave */
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*004d5410*/ extern float _rwSqrt(float x);
/*00482aa0*/ extern void gfx_draw_rect_2d_gradient_a(float x, float y, float w, float h, unsigned int colour_left, unsigned int colour_right);
/*00482c40*/ extern void gfx_draw_rect_2d_gradient_b(float x, float y, float w, float h, unsigned int colour_top, unsigned int colour_bottom);
extern double sin(double);

/* A packed colour: byte 3 is the alpha, bytes 0..2 the colour channels. */
typedef union UiColour {
    unsigned char b[4];
    unsigned int packed;
} UiColour;

#define WIPE_BAR_WIDTH 128.0f               /* the two black bars at the screen edges */
#define WIPE_BAR_HEIGHT 64.0f
#define WIPE_BOTTOM_BAR_Y 416.0f
#define WIPE_RADIUS 90                      /* the edge is a circle of this radius, drawn as strips */
#define WIPE_STRIP_STEP 12
#define WIPE_STRIP_HEIGHT 34.133335f
#define WIPE_STRIP_Y_SCALE 2.8444445f       /* strip row -> screen y (480 / 168.75) */
#define WIPE_ALPHA_PER_STEP 2               /* the wipe alpha follows its target this fast per frame */
#define WAVE_COLUMNS 3                      /* wavy grid: columns x rows of quads, in two passes (top and bottom band) */
#define WAVE_ROWS 7
#define WAVE_AMPLITUDE_X 4.0                /* horizontal wave of the points */
#define WAVE_AMPLITUDE_Y 2.0
#define WAVE_CELL 21.0f                     /* quad size */
#define WAVE_BOTTOM_BAND_Y 416.0f
#define WAVE_FIXED_EDGE_Y 64.0f
#define SCREEN_FRACTION_X 0.0015625f        /* 1/640 */
#define SCREEN_FRACTION_Y 0.0020833334f     /* 1/480 */

/* Draws the screen wipe: two black bars sliding by `position`, a strip-wise circular edge fading with the wipe alpha, and
   a wavy grid of grey quads over it.  unused0 / unused1 are not read. */
void ui_draw_curved_wipe(int unused0, int unused1, float position)
{
    UiColour black;
    UiColour white;
    UiColour white_boosted;
    UiColour grey;
    UiColour clear;
    UiColour vertex_colour;
    int i;
    int row;
    int pass, column, cell;
    int n;

    clear.packed = 0;
    (void)screen_width();
    (void)screen_height();
    (void)screen_width();
    (void)screen_height();
    black.b[0] = 0;
    black.b[1] = 0;
    black.b[2] = 0;
    black.b[3] = 0xff;
    gfx_draw_rect_2d_gradient_b(position - WIPE_BAR_WIDTH, 0.0f, WIPE_BAR_WIDTH, WIPE_BAR_HEIGHT, black.packed, clear.packed);
    gfx_draw_rect_2d_gradient_b(position - WIPE_BAR_WIDTH, WIPE_BOTTOM_BAR_Y, WIPE_BAR_WIDTH, WIPE_BAR_HEIGHT, black.packed, clear.packed);

    if (g_gfx_89ddfc > 0xff)
        g_gfx_89ddfc = 0xff;
    for (n = 0; n < WIPE_ALPHA_PER_STEP; n++) {
        if (g_gfx_89de00 < g_gfx_89ddfc)
            g_gfx_89de00 = g_gfx_89de00 + 1;
        if (g_gfx_89ddfc < g_gfx_89de00)
            g_gfx_89de00 = g_gfx_89de00 - 1;
    }

    /* the curved edge: strips of the circle, every strip a gradient from the bar towards the edge */
    white.b[0] = 0xff;
    white.b[1] = 0xff;
    white.b[2] = 0xff;
    row = 0;
    for (i = WIPE_RADIUS; i > -WIPE_RADIUS; i -= WIPE_STRIP_STEP, row += WIPE_STRIP_STEP) {
        float edge = _rwSqrt(8100.0f - (float)i * (float)i);
        float x, y;

        white.b[3] = (unsigned char)(g_gfx_89de00 * 0x60 / 0xff);
        x = (position - edge) - WIPE_BAR_WIDTH;
        y = (float)row * WIPE_STRIP_Y_SCALE;
        gfx_draw_rect_2d_gradient_a(x, y, WIPE_BAR_WIDTH, WIPE_STRIP_HEIGHT, white.packed, 0);
        white_boosted = white;
        white_boosted.b[3] = (unsigned char)(int)((float)white.b[3] * 1.25f);
        gfx_draw_rect_2d_gradient_a(0.0f, y, x, WIPE_STRIP_HEIGHT, white_boosted.packed, white.packed);
    }

    /* the wavy grid: the corners of every quad move with sine waves of the wave clock */
    grey.b[0] = 0x80;
    grey.b[1] = 0x80;
    grey.b[2] = 0x80;
    grey.b[3] = 0xff;
    for (pass = 0; pass < 2; pass++) {
        for (column = 0; column < WAVE_COLUMNS; column++) {
            for (cell = 0; cell < WAVE_ROWS; cell++) {
                int origin = (column & 1) * 21 + (cell * 5 + 0x2d) * 8;
                float column_y = (float)(column * 21);
                float origin_f = (float)origin;
                double band_y = column_y;
                float wave_time;
                float s1;
                float x0, x1, x2, x3, y0, y1, y2, y3;
                float below;
                float x_right;

                if (pass != 0)
                    band_y = column_y + WAVE_BOTTOM_BAND_Y;
                below = (float)(band_y + WAVE_CELL);
                x_right = (float)(origin_f + WAVE_CELL);
                wave_time = g_menu_wave_time + g_menu_wave_time;
                x0 = (float)(sin((double)(column + cell) + wave_time) * WAVE_AMPLITUDE_X + origin_f);
                s1 = (float)(sin((double)(column + 1 + cell) + wave_time) * WAVE_AMPLITUDE_X);
                x1 = (float)(sin((double)(column + 1 + cell) + wave_time) * WAVE_AMPLITUDE_X + (origin_f + WAVE_CELL));
                x2 = s1 + origin_f;
                x3 = (float)(sin((double)(column + 2 + cell) + wave_time) * WAVE_AMPLITUDE_X + x_right);
                y0 = (float)(sin((double)(cell + column * 2) + wave_time) * WAVE_AMPLITUDE_Y + band_y);
                y1 = (float)(sin((double)(cell + 1 + column * 2) + wave_time) * WAVE_AMPLITUDE_Y + band_y);
                y2 = (float)(sin((double)(cell + 2 + column * 2) + wave_time) * WAVE_AMPLITUDE_Y + below);
                y3 = (float)(sin((double)(cell + 3 + column * 2) + wave_time) * WAVE_AMPLITUDE_Y + below);
                if (pass == 0) {
                    if (column == 2) {
                        y3 = WAVE_FIXED_EDGE_Y;
                        y2 = WAVE_FIXED_EDGE_Y;
                    }
                } else if (column == 0) {
                    y1 = WAVE_BOTTOM_BAND_Y;
                    y0 = WAVE_BOTTOM_BAND_Y;
                }
                vertex_colour = grey;
                if (cell == 0 && (column & 1) == 0)
                    vertex_colour = clear;
                x0 = (float)screen_width() * x0 * SCREEN_FRACTION_X;
                y0 = (float)screen_height() * y0 * SCREEN_FRACTION_Y;
                x1 = (float)screen_width() * x1 * SCREEN_FRACTION_X;
                y1 = (float)screen_height() * y1 * SCREEN_FRACTION_Y;
                x2 = (float)screen_width() * x2 * SCREEN_FRACTION_X;
                y2 = (float)screen_height() * y2 * SCREEN_FRACTION_Y;
                x3 = (float)screen_width() * x3 * SCREEN_FRACTION_X;
                g_im2d_vertices[0].x = x0;
                g_im2d_vertices[0].y = y0;
                g_im2d_vertices[1].x = x1;
                g_im2d_vertices[1].y = y1;
                g_im2d_vertices[2].x = x2;
                g_im2d_vertices[2].y = y2;
                g_im2d_vertices[3].x = x3;
                g_im2d_vertices[3].y = (float)screen_height() * y3 * SCREEN_FRACTION_Y;
                g_im2d_vertices[0].emissiveColor = ((unsigned int)vertex_colour.b[3] << 24) | ((unsigned int)vertex_colour.b[0] << 16) |
                                                   ((unsigned int)vertex_colour.b[1] << 8) | vertex_colour.b[2];
                g_im2d_vertices[1].emissiveColor = 0xff808080;
                g_im2d_vertices[2].emissiveColor = g_im2d_vertices[0].emissiveColor;
                g_im2d_vertices[3].emissiveColor = 0xff808080;
                for (i = 0; i < 4; i++) {
                    g_im2d_vertices[i].z = RwIm2DGetNearScreenZ();
                    g_im2d_vertices[i].rhw = 1.0f;
                }
                RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
                RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
                RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)5);
                RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)6);
                RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, g_im2d_vertices, 4);
            }
        }
    }
}

