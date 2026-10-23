// cluster-entries: menu_draw_event_descriptions
/* Installed cluster menu_draw_event_descriptions (tools/cluster_install.py from output/cluster_pilot_dtrig/menu_draw_event_descriptions.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* DECLS-BEGIN */
/*007ef7ec*/ extern float g_frame_seconds;
/* DECLS-END */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0069d28c*/ extern float g_menu_draw_69d28c;         /* seconds of the animation clock of the description pages */
/*0069c98c*/ extern unsigned char g_menu_69c98c;      /* fade level of the page (0xff = fully visible) */
/*0069d6b4*/ extern int g_menu_69d6b4;
/*0069d68c*/ extern int g_menu_69d68c;                /* scroll offset of the event list */
/*00608428*/ extern int g_menu_608428[];              /* 12 ints per event row, -1 = no third column */
/*0069d688*/ extern int g_career_row;
/*0069cbd4*/ extern int g_event_type;
/*0069cc40*/ extern int g_team_mode;
/*0069d690*/ extern int g_menu_event_column;
/*007eebc0*/ extern struct GameState g_game_state;

/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*0043d9e0*/ extern void menu_draw_event_select(int fade, int skip_list);
/*00435ec0*/ extern int event_type_uses_standard_menu(void);
/*00434a10*/ extern int is_team_mode(void);
/*00433f30*/ extern int career_progress_event_unlocked(int event_row);
/*0043d7b0*/ extern int menu_event_is_selectable(int event_row);
/*00434580*/ extern int career_row_title_string_id(int event_row);
/*00434290*/ extern int career_flag_enabled(int event_row, int flag);
/*00436380*/ extern int menu_event_type_switch_a(void);
/*00436400*/ extern int menu_event_type_switch_b(void);
/*00436460*/ extern int menu_event_type_switch_c(void);
/*00434150*/ extern void *medal_asset_names(int medal);
/*004340c0*/ extern void *menu_icon_texture_dict_lookup(int progress);
/*0040d0f0*/ extern void *find_texture_dict_c(const char *name);
/*0042c440*/ extern void ui_rt2d_text_draw(int text_id, float x, float y, unsigned int colour, float scale, int flags);
/*004824a0*/ extern void draw_rect(float x, float y, float w, float h, unsigned int colour);
/*00483130*/ extern void draw_sprite(void *texture, float x, float y, float w, float h, unsigned int colour, int mode);
/*00482610*/ extern void gfx_draw_triangle_2d(float x0, float y0, float x1, float y1, float x2, float y2, unsigned int colour);
extern double sin(double);

/* A packed colour: byte 3 is the alpha, bytes 0..2 the colour channels. */
typedef union UiColour {
    unsigned char b[4];
    unsigned int packed;
} UiColour;

typedef struct SpriteBox {
    float x, y, w, h;
} SpriteBox;

#define SCREEN_WIDTH_UNITS 640                  /* the menus are laid out for 640x480 */
#define SCREEN_HEIGHT_UNITS 480
#define SPRITE_FRACTION_X 0.0015625f            /* 1/640 */
#define SPRITE_FRACTION_Y 0.0020833334f         /* 1/480 */
#define EVENT_ROW_HEIGHT 22                     /* pixels between two rows of the event list */
#define EVENT_ROW_HEIGHT_F 22.0f
#define EVENT_LIST_HEIGHT_COMPACT 7             /* up to this many rows the list is centred */
#define EVENT_LIST_TOP 118.0f
#define EVENT_LIST_FADE_TOP 118                 /* rows above / below this window fade out over FADE_SPAN pixels */
#define EVENT_LIST_FADE_BOTTOM 250
#define EVENT_LIST_FADE_SPAN 22
#define EVENT_LIST_SCROLL_MAX 0x84
#define EVENT_LIST_MEDAL_ROWS_FROM 0x108        /* y offset from which the rows have a medal (third column) */
#define EVENT_LIST_MEDAL_BOUNCE_FROM 0xf2
#define COLUMN_STEP_UNITS 30                    /* x distance of the three status icons */
#define COLUMN_COUNT 3
#define EVENT_TYPE_RACE_FIRST 0xe               /* the event types 0xe..0x11 have a short list of their own */
#define EVENT_TYPE_LAST_SHORT 0x11

static void * menu_find_texture_by_slot_r(int p0);
extern void menu_find_texture_by_slot(void);
/* Fade of a list row near the top / bottom edge of the list window: 256 inside, falling to 0 over EVENT_LIST_FADE_SPAN. */
static int row_edge_fade(int y)
{
    int fade = 256;
    int d;

    if (y < EVENT_LIST_FADE_TOP) {
        d = EVENT_LIST_FADE_TOP - y;
        if (d > EVENT_LIST_FADE_SPAN)
            d = EVENT_LIST_FADE_SPAN;
        fade = ((EVENT_LIST_FADE_SPAN - d) * 256) / EVENT_LIST_FADE_SPAN;
    }
    if (y > EVENT_LIST_FADE_BOTTOM) {
        d = y - EVENT_LIST_FADE_BOTTOM;
        if (d > EVENT_LIST_FADE_SPAN)
            d = EVENT_LIST_FADE_SPAN;
        fade = ((EVENT_LIST_FADE_SPAN - d) * 256) / EVENT_LIST_FADE_SPAN;
    }
    return fade;
}

/* Everything the three status icons of one list row are drawn from. */
typedef struct EventRowDraw {
    int row;
    int state;                  /* 0 locked, 1 not selectable, 2 normal */
    float icon_x, icon_y;       /* icon position, moves right per column */
    float icon_w, icon_h;       /* plain icon size */
    float frame_w, frame_h;     /* size of the icon on the selected row (two icon sizes) */
    SpriteBox small_box;        /* frame of the selected row, normal */
    SpriteBox big_box;          /* ... pulsing, drawn where the cursor column is */
    UiColour light;
    UiColour dark;
} EventRowDraw;

/* One status icon of a row (medal 0..2 = column): the selected row gets a framed icon (pulsing when the cursor is on
   this column) with the progress mark on top, the other rows a plain icon. */
static void draw_row_icon(EventRowDraw *d, int medal, int column)
{
    int progress_index = column + d->row * 18;

    if (d->row == g_career_row) {
        void *texture = medal_asset_names(medal);
        SpriteBox *box;
        unsigned int progress;

        if (medal != 0 && g_game_state.progress[progress_index] == 0)
            d->light.b[3] = d->light.b[3] >> 1;
        box = g_menu_event_column == column ? &d->big_box : &d->small_box;
        draw_sprite(texture, box->x, box->y, box->w, box->h, d->light.packed, 1);
        progress = g_game_state.progress[progress_index];
        if (medal != 0 && progress == 0)
            d->light.b[3] = (unsigned char)(d->light.b[3] << 1);
        if (career_flag_enabled(d->row, column + 1000) != 0 || progress != 1) {
            void *mark = menu_find_texture_by_slot_r(progress);

            if (mark != 0)
                draw_sprite(mark, d->icon_x, d->icon_y, d->frame_w, d->frame_h, d->light.packed, 1);
        }
    } else if (d->state != 1) {
        draw_sprite(menu_icon_texture_dict_lookup(g_game_state.progress[progress_index]), d->icon_x, d->icon_y,
                    d->icon_w, d->icon_h, d->dark.packed, 1);
    }
}

/* x moved one column to the right */
static float column_shift(float x)
{
    return (float)((int)screen_width() * COLUMN_STEP_UNITS / SCREEN_WIDTH_UNITS) + x;
}

/* Draws the event description page: the game mode checklist (race pages), the scrolling list of events with their
   unlock / medal marks, and the three status icons per row.  skip_list: only the page background pieces. */
void menu_draw_event_descriptions(int skip_list)
{
    UiColour dark;              /* black, row alpha */
    UiColour light;             /* white */
    UiColour triangle;          /* colour of the scroll arrows: black, alpha follows the fade */
    int rows = 15;
    int fade;
    int visible;
    int i;
    float bob;                  /* the selected icon pulses by up to 24 pixels */
    float list_top;
    float box_x, box_y;
    float icon_w, icon_h;

    triangle.b[0] = 0;
    triangle.b[1] = 0;
    triangle.b[2] = 0;
    if (g_event_type == EVENT_TURBO_TOURNAMENT || g_event_type == EVENT_ICE_TOURNAMENT || g_event_type == EVENT_NIGHT_TOURNAMENT)
        rows = 5;
    if (g_event_type == EVENT_TRAINING)
        rows = 6;
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
    fade = g_menu_69c98c;
    if (skip_list != 0)
        fade = 0xff;
    if (skip_list != 0 || fade == 0xff)
        g_menu_69d6b4 = 0;
    {
        double clock = g_frame_seconds + g_menu_draw_69d28c;

        g_menu_draw_69d28c = (float)clock;
        triangle.b[3] = (unsigned char)(fade * 160 / 255);
        bob = (float)((sin(clock * 10.0f) + 1.0f) * 12.0f);
    }
    menu_draw_event_select(g_menu_69d6b4 != 0 ? 0xff : fade, skip_list);
    dark.b[0] = 0;
    dark.b[1] = 0;
    dark.b[2] = 0;
    light.b[0] = 255;
    light.b[1] = 255;
    light.b[2] = 255;
    dark.b[3] = (unsigned char)fade;
    light.b[3] = (unsigned char)fade;

    (void)((int)screen_width() * 330 / SCREEN_WIDTH_UNITS);
    (void)((int)screen_height() * 238 / SCREEN_HEIGHT_UNITS);
    icon_w = (float)screen_width() * 0.0375f;
    icon_h = (float)screen_height() * 0.05f;
    box_x = (float)((int)screen_width() * 520 / SCREEN_WIDTH_UNITS);
    box_y = (float)((int)screen_height() * 320 / SCREEN_HEIGHT_UNITS);

    /* race pages: checklist of the game modes with a check / lock mark each */
    if (skip_list == 0 && event_type_uses_standard_menu() == 0 && g_event_type >= 6 && g_event_type <= 8) {
        ui_rt2d_text_draw(g_team_mode != 0 ? 0x145 : 0x23, 330.0f, 316.0f, light.packed, 0.6f, 0);
        ui_rt2d_text_draw(0x59, 330.0f, 334.0f, light.packed, 0.6f, 0);
        draw_sprite(find_texture_dict_c("check"), box_x, box_y, icon_w, icon_h, light.packed, 1);
        ui_rt2d_text_draw(0x250, 330.0f, 350.0f, light.packed, 0.6f, 0);
        box_y = (float)((int)screen_height() * 16 / SCREEN_HEIGHT_UNITS) + box_y;
        draw_sprite(find_texture_dict_c(g_game_state.progress[g_career_row * 18 + 9] != 0 ? "check" : "lock"),
                    box_x, box_y, icon_w, icon_h, light.packed, 1);
        ui_rt2d_text_draw(0x146, 330.0f, 366.0f, light.packed, 0.6f, 0);
        box_y = (float)((int)screen_height() * 16 / SCREEN_HEIGHT_UNITS) + box_y;
        draw_sprite(find_texture_dict_c(g_game_state.progress[g_career_row * 18 + 10] != 0 && is_team_mode() == 0
                                            ? "check" : "lock"),
                    box_x, box_y, icon_w, icon_h, light.packed, 1);
    }

    /* number of rows the list shows */
    visible = 0;
    if (g_event_type == EVENT_TURBO_TOURNAMENT || g_event_type == EVENT_ICE_TOURNAMENT || g_event_type == EVENT_NIGHT_TOURNAMENT || g_event_type == EVENT_TRAINING) {
        int n = g_event_type == EVENT_TRAINING ? 6 : 5;

        for (i = 0; i < n; i++)
            if (i < 1 || menu_event_is_selectable(i) != 0)
                visible++;
    } else {
        for (i = 0; i < 15; i++) {
            if (i < 12) {
                if (i <= 0 || menu_event_is_selectable(i) != 0)
                    visible++;
            } else if (career_progress_event_unlocked(i) != 0) {
                visible++;
            }
        }
    }
    {
        float top = EVENT_LIST_TOP;
        int scrolled;

        if (visible < EVENT_LIST_HEIGHT_COMPACT)
            top = (float)((EVENT_LIST_HEIGHT_COMPACT - visible) * 20 / 2) + EVENT_LIST_TOP;
        list_top = (float)g_menu_69d68c + top;
        scrolled = g_career_row * EVENT_ROW_HEIGHT + g_menu_69d68c;
        if (scrolled > EVENT_LIST_SCROLL_MAX)
            g_menu_69d68c = g_menu_69d68c - 3;
        if (scrolled < 0)
            g_menu_69d68c = g_menu_69d68c + 3;
    }

    if (skip_list == 0) {
        UiColour band;
        float band_top = list_top - 11.0f;
        float band_y = (float)g_career_row * EVENT_ROW_HEIGHT_F + band_top;
        int row;
        int pass;

        /* the highlight band behind the selected row, with a border */
        if (g_career_row <= 0xb) {
            band.b[0] = 0xf7; band.b[1] = 0x94; band.b[2] = 0x1d;
        } else {
            band.b[0] = 0x1d; band.b[1] = 0x94; band.b[2] = 0xf7;
        }
        band.b[3] = (unsigned char)(fade >> 1);
        draw_rect(0.0f, band_y, 310.0f, 22.0f, band.packed);
        if (g_career_row <= 0xb) {
            band.b[0] = 0xb4; band.b[1] = 0x64; band.b[2] = 0x0f;
        } else {
            band.b[0] = 0x0f; band.b[1] = 0x64; band.b[2] = 0xb4;
        }
        band.b[3] = (unsigned char)fade;
        draw_rect(0.0f, band_y, 310.0f, 2.0f, band.packed);
        band_y = band_y + 20.0f;
        draw_rect(0.0f, band_y, 310.0f, 2.0f, band.packed);
        draw_rect(310.0f, band_y - 20.0f, 2.0f, 22.0f, band.packed);

        /* scroll arrows */
        if (g_career_row != 0)
            gfx_draw_triangle_2d(152.0f, 96.0f, 200.0f, 96.0f, 176.0f, 72.0f, triangle.packed);
        if (g_career_row < visible - 1)
            gfx_draw_triangle_2d(152.0f, 272.0f, 200.0f, 272.0f, 176.0f, 296.0f, triangle.packed);

        /* the titles of the events, with new / medal marks */
        {
            int event_type = g_event_type;
            int y_off = 0;

            for (row = 0; row < rows; row++, y_off += EVENT_ROW_HEIGHT) {
                float y_off_f = (float)y_off;
                float row_y = y_off_f + list_top;
                int fade_row = row_edge_fade((int)row_y);
                unsigned char alpha_row;
                int state = 2;
                float scale;
                int kind;

                alpha_row = (unsigned char)(fade_row * fade / 256);
                dark.b[0] = 0;
                dark.b[1] = 0;
                dark.b[2] = 0;
                dark.b[3] = alpha_row;
                if (y_off < EVENT_LIST_MEDAL_ROWS_FROM || event_type == 0xe || event_type == 0xf ||
                    event_type == 0x10 || event_type == 0x11) {
                    if (y_off > 0 && menu_event_is_selectable(row) == 0)
                        state = 1;
                } else if (career_progress_event_unlocked(row) == 0) {
                    state = 0;
                }
                scale = 0.6f;
                if (row != g_career_row)
                    scale = 0.58f;
                if (event_type == 0x11)
                    scale = 0.52f;
                if (state != 2)
                    continue;
                ui_rt2d_text_draw(career_row_title_string_id(row), 64.0f, row_y, dark.packed, scale, 0);
                event_type = g_event_type;
                kind = event_type;
                if (event_type == 4 || event_type == 5)
                    kind = 3;
                if (career_flag_enabled(row, kind) != 0 && y_off < EVENT_LIST_MEDAL_ROWS_FROM) {
                    /* "new" marker, blinking */
                    void *texture;
                    float x = 32.0f, y, w, h;

                    dark.b[0] = 255;
                    dark.b[1] = 255;
                    dark.b[2] = 255;
                    dark.b[3] = (unsigned char)((unsigned char)(int)(((sin(g_menu_draw_69d28c * 8.0f) + 1.0f) * 0.33333334f + 0.333f) * fade) *
                                                fade_row / 256);
                    texture = menu_find_texture_by_slot_r(3);
                    y = (float)screen_height() * ((list_top - 20.0f) + y_off_f) * SPRITE_FRACTION_Y;
                    w = (float)screen_width() * 0.05f;
                    h = (float)screen_height() * 0.06666667f;
                    if (texture != 0)
                        draw_sprite(texture, x, y, w, h, dark.packed, 1);
                    event_type = g_event_type;
                }
                if (kind == 3 && y_off > EVENT_LIST_MEDAL_BOUNCE_FROM) {
                    /* medal of the event, bouncing on the selected row */
                    void *texture;
                    float pulse = (float)((sin(g_menu_draw_69d28c * 8.0f) + 1.0f) * 12.0f);
                    double top, half, grow;
                    float x, y, w, h;

                    if (row != g_career_row)
                        pulse = 0.0f;
                    dark.b[0] = 255;
                    dark.b[1] = 255;
                    dark.b[2] = 255;
                    dark.b[3] = alpha_row;
                    texture = medal_asset_names(row + 0x3dc);
                    top = (float)screen_height() * ((list_top - 12.0f) + y_off_f) * SPRITE_FRACTION_Y;
                    half = pulse * 0.5f;
                    x = (float)(36.0f - half);
                    y = (float)(top - half);
                    grow = pulse + 20.0f;
                    w = (float)((float)screen_width() * grow * SPRITE_FRACTION_X);
                    h = (float)((float)screen_height() * grow * SPRITE_FRACTION_Y);
                    if (texture != 0)
                        draw_sprite(texture, x, y, w, h, dark.packed, 1);
                    event_type = g_event_type;
                }
            }
        }

        /* the three status icons of every row; the selected column is drawn last (second pass) so it lies on top */
        {
            EventRowDraw d;
            float icon_w2 = 0.0f, icon_h2 = 0.0f;
            int selected_column;

            icon_w2 = (float)screen_width() * 0.034375f;
            icon_h2 = (float)screen_height() * 0.045833334f;
            d.dark.b[0] = 0;
            d.dark.b[1] = 0;
            d.dark.b[2] = 0;
            d.dark.b[3] = (unsigned char)fade;
            d.light.b[0] = 255;
            d.light.b[1] = 255;
            d.light.b[2] = 255;
            d.light.b[3] = (unsigned char)fade;
            selected_column = 0;
            if (g_menu_event_column == menu_event_type_switch_b())
                selected_column = 1;
            if (g_menu_event_column == menu_event_type_switch_c())
                selected_column = 2;
            for (row = 0; row < rows; row++) {
                int event_type = g_event_type;
                int fade_row;
                int alpha;
                float icon_y_f, half_w, half_h;

                if (!(row < 12 || event_type == 0xe || event_type == 0xf || event_type == 0x10 || event_type == 0x11)) {
                    if (career_progress_event_unlocked(row) == 0)
                        continue;
                    event_type = g_event_type;
                }
                d.state = 2;
                if (row < 12 || event_type == 0xe || event_type == 0xf || event_type == 0x10 || event_type == 0x11) {
                    if (row > 0 && menu_event_is_selectable(row) == 0)
                        d.state = 1;
                } else if (career_progress_event_unlocked(row) == 0) {
                    d.state = 0;
                }
                fade_row = row_edge_fade((int)((float)(row * EVENT_ROW_HEIGHT) + list_top));
                switch (g_event_type) {
                case EVENT_TIME_TRIAL_0: case EVENT_TIME_TRIAL_1: case EVENT_TIME_TRIAL_2: case EVENT_CUP_3: case EVENT_CUP_4: case EVENT_CUP_5: case EVENT_MULTIPLAYER_6: case EVENT_MULTIPLAYER_7: case EVENT_MULTIPLAYER_8:
                case EVENT_QUICK_RACE_11: case EVENT_QUICK_RACE_12: case EVENT_QUICK_RACE_13: case EVENT_TURBO_TOURNAMENT: case EVENT_ICE_TOURNAMENT: case EVENT_NIGHT_TOURNAMENT: case EVENT_TRAINING:
                    break;
                default:
                    continue;
                }
                alpha = fade >> 2;
                if (menu_event_is_selectable(row) == 0 && alpha > 0x20)
                    alpha = 0x20;
                d.dark.b[0] = 0;
                d.dark.b[1] = 0;
                d.dark.b[2] = 0;
                d.dark.b[3] = (unsigned char)(alpha * fade_row / 256);
                icon_y_f = (float)row * EVENT_ROW_HEIGHT_F + band_top;
                half_w = icon_w2 * 0.5f;
                half_h = icon_h2 * 0.5f;
                d.row = row;
                d.icon_w = icon_w2;
                d.icon_h = icon_h2;
                d.frame_w = icon_w2 + icon_w2;
                d.frame_h = icon_h2 + icon_h2;
                for (pass = 0; pass < 2; pass++) {
                    float x_icon = (float)((int)screen_width() * 220 / SCREEN_WIDTH_UNITS);
                    float y_icon = (float)screen_height() * icon_y_f * SPRITE_FRACTION_Y;
                    double frame_x = x_icon - half_w;
                    double frame_y = y_icon - half_h;
                    int column;

                    d.small_box.x = (float)frame_x;
                    d.small_box.y = (float)(y_icon - half_h);
                    d.small_box.w = d.frame_w;
                    d.small_box.h = d.frame_h;
                    d.big_box.w = (float)((float)screen_width() * bob * SPRITE_FRACTION_X + d.frame_w);
                    d.big_box.h = (float)((float)screen_height() * bob * SPRITE_FRACTION_Y + d.frame_h);
                    d.big_box.x = (float)(frame_x - (float)screen_width() * bob * 0.00078125f);
                    d.big_box.y = (float)(frame_y - (float)screen_height() * bob * 0.0010416667f);
                    if (g_event_type == EVENT_TRAINING) {
                        d.big_box.x = (float)screen_width() * 0.0875f + d.big_box.x;
                        x_icon = (float)screen_width() * 0.0875f + x_icon;
                    }
                    d.icon_x = x_icon;
                    d.icon_y = y_icon;

                    if (pass == 0 ? selected_column != 0 : selected_column == 0)
                        draw_row_icon(&d, 0, menu_event_type_switch_a());
                    d.icon_x = column_shift(d.icon_x);
                    d.small_box.x = column_shift(d.small_box.x);
                    d.big_box.x = column_shift(d.big_box.x);

                    if (pass == 0 ? selected_column != 1 : selected_column == 1) {
                        column = menu_event_type_switch_b();
                        if (column != 0)
                            draw_row_icon(&d, 1, column);
                    }
                    d.small_box.x = column_shift(d.small_box.x);
                    d.big_box.x = column_shift(d.big_box.x);
                    d.icon_x = column_shift(d.icon_x);

                    if (pass == 0 ? selected_column != 2 : selected_column == 2) {
                        if (g_event_type == EVENT_CUP_3 || g_event_type == EVENT_CUP_4 || g_event_type == EVENT_CUP_5 || g_menu_608428[row * 12] != -1) {
                            column = menu_event_type_switch_c();
                            if (column != 0)
                                draw_row_icon(&d, 2, column);
                        }
                    }
                }
            }
        }
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
}

/* menu_find_texture_by_slot_r: C -> original menu_find_texture_by_slot (registers eax) */
__declspec(naked) void * menu_find_texture_by_slot_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call menu_find_texture_by_slot
        ret
    }
}
