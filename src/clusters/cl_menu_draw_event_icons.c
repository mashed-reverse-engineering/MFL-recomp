// cluster-entries: menu_draw_event_icons
/* Installed cluster menu_draw_event_icons (tools/cluster_install.py from output/cluster_pilot_dtrig/menu_draw_event_icons.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* DECLS-BEGIN */
/*007ef7ec*/ extern float g_frame_seconds;
/*0069cf98*/ extern int g_menu_bot_skill_a;
/*0069cfac*/ extern int g_menu_bot_skill_b;
/*0069cbd0*/ extern int g_menu_current;
/*0069cf94*/ extern int g_menu_markers_enabled;
/*0069c9c0*/ extern int g_menu_panel_state_69c9c0;
/*0069cfb4*/ extern int g_menu_player_team_choice;
/*0069cfb8*/ extern int g_menu_player_team_choice_1;
/*0069cfbc*/ extern int g_menu_player_team_choice_2;
/*0069cf9c*/ extern int g_menu_race_length_choice;
/*0069cfc8*/ extern int g_menu_team_mode_choice;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* DECLS-END */
/*0069d1dc*/ extern float g_menu_draw_69d1dc;   /* seconds of the menu animation clock */
/*00608848*/ extern unsigned int g_menu_608848[];     /* [row][3] career event ids (type in the high word) */
/*006088fc*/ extern unsigned int g_menu_6088fc[];
/*00608938*/ extern unsigned int g_menu_608938[];
/*00608974*/ extern unsigned int g_menu_608974[];
/*0069d270*/ extern int g_menu_draw_69d270[5];        /* random powerup icon shown per slot */
/*00609a18*/ extern int g_menu_draw_609a18[16];       /* powerup ids the random icons draw from */
/*0060a8c0*/ extern char g_menu_page_18_game_mode[];
/*0060a990*/ extern char g_menu_page_24_game_mode[];
/*00608750*/ extern char *PTR_g_data_6086d8_00608750[];  /* texture name per event icon */
/*0069c9c4*/ extern int g_menu_69c9c4;                /* menu fade level 0..255 */
/*0069ca1c*/ extern int g_menu_69ca1c;
/*0069d688*/ extern int g_career_row;
/*0069cbd4*/ extern int g_event_type;
/*0069cfb0*/ extern int g_menu_event_course_index;
/*0069cf90*/ extern int g_menu_powerup_set;
/*0069cfa4*/ extern int g_menu_game_mode_choice;
/*0069d690*/ extern int g_menu_event_column;
/*0069cc40*/ extern int g_team_mode;
/*007eeff8*/ extern struct GameState g_game_state;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];

/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*0043d9e0*/ extern void menu_draw_event_select(int fade, int unused);
/*00435fc0*/ extern int career_event_set_progress(int event);
/*00436400*/ extern int menu_event_type_switch_b(void);
/*00436460*/ extern int menu_event_type_switch_c(void);
/*0040f080*/ extern int **race_get_race_setup(void);
/*00464330*/ extern void *powerup_icon_texture(int powerup);
/*0042fb40*/ extern int course_event_icon_lookup_a(int course);
/*0042fbb0*/ extern int course_event_icon_lookup_b(int course);
/*0047be30*/ extern void career_row_for_event(float *stat_a, float *stat_b, float *stat_c, float *stat_d);
/*0040d0f0*/ extern void *find_texture_dict_c(const char *name);
/*0040d110*/ extern void *find_texture_dict_d(const char *name);
/*0040d130*/ extern void *find_texture_a(const char *name);
/*0042c440*/ extern void ui_rt2d_text_draw(int text_id, float x, float y, unsigned int colour, float scale, int flags);
/*0042c8e0*/ extern float text_width(int text_id, float scale);
/*00481fd0*/ extern double random_range(float low, float high);
/*004824a0*/ extern void draw_rect(float x, float y, float w, float h, unsigned int colour);
/*00483130*/ extern void draw_sprite(void *texture, float x, float y, float w, float h, unsigned int colour, int mode);
/*004832d0*/ extern void draw_sprite_uv(void *texture, float x, float y, float w, float h, unsigned int colour, float u0, float u1, float v0, float v1, int mode, int flag);
extern double sin(double);

/* A packed colour: byte 3 is the alpha, bytes 0..2 the colour channels. */
typedef union UiColour {
    unsigned char b[4];
    unsigned int packed;
} UiColour;

#define ARROW_COLUMN_X 0.484375f            /* x of the option arrows, fraction of the screen width */
#define ARROW_COLUMN_W 0.01875f
#define ARROW_ROW_H 0.025f
#define ROW_STEP 30.0f                      /* distance between two option rows */
#define ROW_STEP_EVENT_PAGE 28.0f           /* ... on the event page (page 0x18) */
#define EVENT_PAGE_FIRST_ROW_SHIFT 58.0f
#define ROW_SHADOW_OFFSET_Y 6.0f
#define LABEL_SCALE_EVENT_PAGE 0.58f
#define LABEL_SCALE 0.52f
#define LABEL_GAP 2.0f
#define PAGE_EVENT MENU_PAGE_GAME_SETUP_SINGLE
#define PAGE_SKILL MENU_PAGE_GAME_SETUP_MULTI
#define SCREEN_WIDTH_UNITS 640              /* the menus are laid out for 640x480 */
#define SPRITE_SCREEN_FRACTION_Y 0.0020833334f   /* 1/480 */
#define SPRITE_SCREEN_FRACTION_X 0.0015625f      /* 1/640 */
#define MENU_FADE_VISIBLE_MIN 0x60

/* Draws the text of one option row at x, then moves x left by the width of the text (plus a gap) so the right arrow
   can be drawn behind it. */
static int menu_count_items_r(char * p0);
extern void menu_count_items(void);
static int menu_item_y_position_r(int p0, int p1);
extern void menu_item_y_position(void);
static void course_flag_30_set_r(unsigned char * p0);
extern void course_flag_30_set(void);
static float option_label_draw(int text_id, float scale, float x, float y, UiColour colour)
{
    int label_width;

    ui_rt2d_text_draw(text_id, (x - 1.0f) / screen_width() * 640.0f, y, colour.packed, scale, 1);
    label_width = (int)text_width(text_id, scale);
    return (x - (float)((int)screen_width() * (label_width + 0x12) / SCREEN_WIDTH_UNITS)) + LABEL_GAP;
}

/* Draws the rows of the event / game mode option pages (arrows and the current value of every option), then the
   powerup icons, statistic bars and the event picture around them.  skip_option_rows: only the second half. */
void menu_draw_event_icons(int skip_option_rows)
{
    int team_label[7] = { 0x82, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81 };   /* label per team choice */
    UiColour tint;          /* colour of the option rows; later white */
    UiColour overlay;       /* colour of the powerup icons and the statistics */
    unsigned int overlay_word;
    char *item_list;
    int alpha;
    int current;
    int page_id;
    int item_count;
    int first_y;
    int text_id;
    int i;
    float row_y;
    float x, y, w, h;
    int slot;

    g_menu_draw_69d1dc = g_frame_seconds + g_menu_draw_69d1dc;
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);

    if (skip_option_rows != 0) {
        overlay.b[3] = 255;
        tint.b[3] = 255;
    } else {
        if (g_menu_panel_state_69c9c0 != 0)
            menu_draw_event_select(g_menu_69c9c4 & 0xff, 0);
        alpha = g_menu_69c9c4;
        if (g_menu_panel_state_69c9c0 == 0) {
            if (alpha < MENU_FADE_VISIBLE_MIN) {
                RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
                RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
                return;
            }
            item_list = g_menu_records[g_menu_current + 1].item_list;
        } else {
            item_list = g_menu_records[g_menu_current].item_list;
        }
        current = g_menu_current;
        item_count = menu_count_items_r(item_list);
        first_y = menu_item_y_position_r(item_count, 30);
        page_id = g_menu_records[current].page_id;
        overlay.b[3] = (unsigned char)alpha;
        tint.b[3] = (unsigned char)alpha;
        row_y = (float)first_y + ROW_STEP;
        text_id = 0;            /* an option value without a label (outside the menu's domain) keeps the id of the previous row */
        if (page_id == PAGE_EVENT) {
            item_count = menu_count_items_r(item_list);
            row_y = ((float)menu_item_y_position_r(item_count, 28) + ROW_STEP_EVENT_PAGE) - EVENT_PAGE_FIRST_ROW_SHIFT;
        }
        if (page_id == PAGE_SKILL)
            row_y = row_y - EVENT_PAGE_FIRST_ROW_SHIFT;
        for (i = 1; i < item_count; i++) {
            current = g_menu_current;
            if (g_menu_records[current].entry_enabled[i] == 0) {
                course_flag_30_set_r(tint.b);
                course_flag_30_set_r(overlay.b);
            }
            if (g_menu_69ca1c != 0 && g_menu_records[current + 1].entry_enabled[i] == 0) {
                course_flag_30_set_r(tint.b);
                course_flag_30_set_r(overlay.b);
            }
            if ((int)tint.b[3] > alpha)
                tint.b[3] = (unsigned char)alpha;
            tint.b[0] = 0;
            tint.b[1] = 0;
            tint.b[2] = 0;
            x = (float)screen_width() * ARROW_COLUMN_X;
            y = (float)screen_height() * (row_y - ROW_SHADOW_OFFSET_Y) * SPRITE_SCREEN_FRACTION_Y;
            w = (float)screen_width() * ARROW_COLUMN_W;
            h = (float)screen_height() * ARROW_ROW_H;
            if (page_id == PAGE_EVENT) {
                if (i != 3 || career_event_set_progress(1) != 0)
                    draw_sprite_uv(find_texture_dict_c("Arrow"), x, y, w, h, tint.packed, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 1);
                switch (i) {
                case 1:
                    text_id = 0x5c;
                    if (g_menu_powerup_set == 1)
                        text_id = 0x5e;
                    else if (g_menu_powerup_set == 2)
                        text_id = 0x5f;
                    break;
                case 2:
                    if (g_menu_bot_skill_b < 1) {
                        g_menu_bot_skill_b = 1;
                        text_id = 0xd7;
                    } else if (g_menu_bot_skill_b > 4) {
                        g_menu_bot_skill_b = 4;
                        text_id = 0xe6;
                    } else if (g_menu_bot_skill_b == 1) {
                        text_id = 0xd7;
                    } else if (g_menu_bot_skill_b == 2) {
                        text_id = 0xd6;
                    } else if (g_menu_bot_skill_b == 3) {
                        text_id = 0xd5;
                    } else if (g_menu_bot_skill_b == 4) {
                        text_id = 0xe6;
                    }
                    break;
                case 3:
                    text_id = course_event_icon_lookup_b(g_menu_event_course_index);
                    break;
                case 4:
                    text_id = team_label[g_menu_player_team_choice];
                    break;
                case 5:
                    text_id = team_label[g_menu_player_team_choice_1];
                    break;
                case 6:
                    text_id = team_label[g_menu_player_team_choice_2];
                    break;
                case 7:
                    text_id = 0x263 - (g_menu_team_mode_choice != 0);
                    break;
                }
                x = option_label_draw(text_id, LABEL_SCALE_EVENT_PAGE, x, row_y, tint);
                if (i != 3 || career_event_set_progress(1) != 0)
                    draw_sprite(find_texture_dict_c("Arrow"), x, y, w, h, tint.packed, 1);
            } else {
                draw_sprite_uv(find_texture_dict_c("Arrow"), x, y, w, h, tint.packed, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 1);
                switch (i) {
                case 1:
                    if (g_menu_powerup_set == 0)
                        text_id = 0x5c;
                    else if (g_menu_powerup_set == 1)
                        text_id = 0x5e;
                    else if (g_menu_powerup_set == 2)
                        text_id = 0x5f;
                    break;
                case 2:
                    if (g_menu_race_length_choice == 0)
                        text_id = 0xe3;
                    else if (g_menu_race_length_choice == 1)
                        text_id = 0xe4;
                    else if (g_menu_race_length_choice == 2)
                        text_id = 0xe5;
                    break;
                case 3:
                    if (g_menu_bot_skill_a == 0)
                        text_id = 0x5c;
                    else if (g_menu_bot_skill_a == 1)
                        text_id = 0xd7;
                    else if (g_menu_bot_skill_a == 2)
                        text_id = 0xd6;
                    else if (g_menu_bot_skill_a == 3)
                        text_id = 0xd5;
                    else if (g_menu_bot_skill_a == 4)
                        text_id = 0xe6;
                    break;
                case 4:
                    text_id = course_event_icon_lookup_b(g_menu_event_course_index);
                    break;
                case 5:
                    text_id = 0x5c + (g_menu_markers_enabled != 0);
                    break;
                case 6:
                    if (g_menu_game_mode_choice == 0)
                        text_id = 0xef;
                    else if (g_menu_game_mode_choice == 1)
                        text_id = 0x250;
                    else if (g_menu_game_mode_choice == 2)
                        text_id = 0x146;
                    break;
                }
                x = option_label_draw(text_id, LABEL_SCALE, x, row_y, tint);
                draw_sprite(find_texture_dict_c("Arrow"), x, y, w, h, tint.packed, 1);
            }
            page_id = g_menu_records[g_menu_current].page_id;
            row_y = row_y + (page_id == PAGE_EVENT ? ROW_STEP_EVENT_PAGE : ROW_STEP);
            alpha = g_menu_69c9c4;
            overlay.b[3] = (unsigned char)alpha;
            tint.b[3] = (unsigned char)alpha;
        }
    }

    /* ---- the pages' extras: game mode captions, powerup icons, statistics, event picture, markers ---- */
    tint.b[0] = 255;
    tint.b[1] = 255;
    tint.b[2] = 255;
    x = (float)((int)screen_width() * 330 / SCREEN_WIDTH_UNITS);
    y = (float)((int)screen_height() * 238 / 480);
    w = (float)screen_width() * 0.05f;
    h = (float)screen_height() * 0.06666667f;
    if (g_event_type >= 6 && g_event_type <= 8) {
        if (g_menu_game_mode_choice == 0) {
            ui_rt2d_text_draw(0xef, 330.0f, 316.0f, tint.packed, 0.6f, 0);
            if (g_player_slots[3].controller != -1 && g_team_mode == 0) {
                ui_rt2d_text_draw(0x129, 330.0f, 334.0f, tint.packed, 0.6f, 0);
                ui_rt2d_text_draw(0x12a, 330.0f, 350.0f, tint.packed, 0.6f, 0);
            } else {
                ui_rt2d_text_draw(0x131, 330.0f, 334.0f, tint.packed, 0.6f, 0);
                ui_rt2d_text_draw(0x132, 330.0f, 350.0f, tint.packed, 0.6f, 0);
            }
        } else if (g_menu_game_mode_choice == 1) {
            ui_rt2d_text_draw(0x250, 330.0f, 316.0f, tint.packed, 0.6f, 0);
            ui_rt2d_text_draw(0x14d, 330.0f, 334.0f, tint.packed, 0.55f, 0);
            ui_rt2d_text_draw(0x14e, 330.0f, 350.0f, tint.packed, 0.55f, 0);
        } else if (g_menu_game_mode_choice == 2) {
            ui_rt2d_text_draw(0x146, 330.0f, 316.0f, tint.packed, 0.6f, 0);
            ui_rt2d_text_draw(0x14f, 330.0f, 334.0f, tint.packed, 0.55f, 0);
            ui_rt2d_text_draw(0x150, 330.0f, 350.0f, tint.packed, 0.55f, 0);
        }
    }

    overlay.b[0] = 255;
    overlay.b[1] = 255;
    overlay.b[2] = 255;
    overlay_word = overlay.packed;
    if (g_menu_powerup_set != 0) {
        int kind = 0;
        unsigned int event_id;

        if (g_event_type == EVENT_CUP_4)
            kind = 1;
        else if (g_event_type == EVENT_CUP_5)
            kind = 2;
        event_id = g_menu_608848[g_career_row * 3 + kind];
        if (g_event_type == EVENT_TURBO_TOURNAMENT)
            event_id = g_menu_6088fc[g_career_row * 3];
        else if (g_event_type == EVENT_ICE_TOURNAMENT)
            event_id = g_menu_608938[g_career_row * 3];
        else if (g_event_type == EVENT_NIGHT_TOURNAMENT)
            event_id = g_menu_608974[g_career_row * 3];
        event_id &= 0xffff0000;
        for (slot = 0; slot < 5; slot++) {
            double base_x = (float)screen_width() * 0.878125f;
            int choice;
            int **race;
            void *icon;
            float scale;

            y = (float)screen_height() * 0.78333336f;
            x = (float)(base_x - (int)screen_width() * slot * 38 / SCREEN_WIDTH_UNITS);
            choice = 0;
            if (g_menu_event_column == menu_event_type_switch_b())
                choice = 1;
            if (g_menu_event_column == menu_event_type_switch_c())
                choice = 2;
            race = race_get_race_setup();
            icon = 0;
            scale = 0.0f;
            if (g_menu_powerup_set == 1) {
                if (g_game_state.records[slot + *race[1 + choice * 15 + g_career_row] * 5] <= -1)
                    continue;
                choice = 0;
                if (g_menu_event_column == menu_event_type_switch_b())
                    choice = 1;
                if (g_menu_event_column == menu_event_type_switch_c())
                    choice = 2;
                race = race_get_race_setup();
                icon = powerup_icon_texture(g_game_state.records[slot + *race[1 + choice * 15 + g_career_row] * 5]);
                if (event_id == 0x70000) {
                    icon = 0;
                    if (slot == 0)
                        icon = powerup_icon_texture(9);
                    else if (slot == 1)
                        icon = powerup_icon_texture(0xb);
                }
                scale = 0.0f;
            } else if (g_menu_powerup_set == 2) {
                /* the icons pulse in turn: a sine wave over the slots, a new random icon when a pulse peaks */
                double wave = sin((double)(slot * 30 - (int)(g_menu_draw_69d1dc * -60.0f)) * 0.16666667f) * 48.0f;
                double pulse;

                if (!(wave >= 0.0f))
                    wave = 0.0f;
                pulse = wave * 0.6666667f;
                scale = (float)pulse;
                if (!(pulse < 31.0f))
                    g_menu_draw_69d270[slot] = (int)(random_range(0, 15.0f) + g_menu_draw_69d270[slot]);
                icon = powerup_icon_texture(g_menu_draw_609a18[g_menu_draw_69d270[slot] & 0xf]);
            } else {
                continue;
            }
            if (icon != 0) {
                double shadow_x = x - 4.0f;
                double shadow_y = y - 4.0f;
                float shadow_scale;
                double shadow_shift;
                double icon_x, icon_y;
                double icon_w, icon_h;

                w = (float)screen_width() * 0.075f;
                h = (float)screen_height() * 0.1f;
                shadow_scale = scale * 1.5f;
                shadow_shift = (double)scale * 1.5f * 0.5f;
                x = (float)(shadow_shift + shadow_x);
                y = (float)(shadow_shift + shadow_y);
                w = w - (float)screen_width() * shadow_scale * SPRITE_SCREEN_FRACTION_X;
                h = h - (float)screen_height() * shadow_scale * SPRITE_SCREEN_FRACTION_Y;
                draw_sprite(find_texture_a("Powerupshadow"), x, y, w, h, overlay_word, 0);

                icon_x = (float)screen_width() * 0.878125f;
                icon_y = (float)screen_height() * 0.78333336f;
                shadow_shift = (double)scale * 0.5f;
                x = (float)(icon_x - (int)screen_width() * slot * 38 / SCREEN_WIDTH_UNITS + shadow_shift);
                y = (float)(shadow_shift + icon_y);
                icon_w = (float)screen_width() * 0.05f;
                icon_h = (float)screen_height() * 0.06666667f;
                w = (float)(icon_w - (float)screen_width() * scale * SPRITE_SCREEN_FRACTION_X);
                h = (float)(icon_h - (float)screen_height() * scale * SPRITE_SCREEN_FRACTION_Y);
                draw_sprite(icon, x, y, w, h, overlay_word, 0);
            }
        }
    }

    /* picture of the selected event next to the game mode pages */
    if (g_menu_records[g_menu_current].item_list == g_menu_page_24_game_mode) {
        void *picture = find_texture_dict_d(PTR_g_data_6086d8_00608750[course_event_icon_lookup_a(g_menu_event_course_index)]);

        draw_sprite_uv(picture, 456.0f, 136.0f, 152.0f, 152.0f, overlay_word, 0.0f, 1.0f, 0.0f, 1.0f, 1, 0);
    }
    if (g_menu_records[g_menu_current].item_list == g_menu_page_24_game_mode ||
        g_menu_records[g_menu_current].item_list == g_menu_page_18_game_mode) {
        float stat[4];

        /* statistics panel: dark backdrop, four bars (outline, then the filled part) and their labels */
        overlay.b[3] = overlay.b[3] >> 1;
        overlay.b[0] = 0;
        overlay.b[1] = 0;
        overlay.b[2] = 0;
        draw_rect(336.0f, 162.0f, 116.0f, 100.0f, overlay.packed);
        draw_rect(342.0f, 180.0f, 104.0f, 4.0f, overlay.packed);
        draw_rect(342.0f, 204.0f, 104.0f, 4.0f, overlay.packed);
        draw_rect(342.0f, 227.0f, 104.0f, 4.0f, overlay.packed);
        draw_rect(342.0f, 252.0f, 104.0f, 4.0f, overlay.packed);
        overlay.b[3] = (unsigned char)(overlay.b[3] << 1);
        overlay.b[0] = 255;
        overlay.b[1] = 255;
        overlay.b[2] = 255;
        overlay_word = overlay.packed;
        ui_rt2d_text_draw(0x313, 340.0f, 170.0f, overlay_word, 0.55f, 0);
        ui_rt2d_text_draw(0x314, 340.0f, 194.0f, overlay_word, 0.55f, 0);
        ui_rt2d_text_draw(0x316, 340.0f, 218.0f, overlay_word, 0.55f, 0);
        ui_rt2d_text_draw(0x315, 340.0f, 242.0f, overlay_word, 0.55f, 0);
        career_row_for_event(&stat[0], &stat[1], &stat[2], &stat[3]);
        draw_rect(342.0f, 180.0f, (stat[1] - 20000.0f) * 0.0034666667f, 4.0f, overlay_word);
        draw_rect(342.0f, 204.0f, (stat[2] - 0.8f) * 208.0f, 4.0f, overlay_word);
        draw_rect(342.0f, 227.0f, (stat[3] - 0.8f) * 80.0f, 4.0f, overlay_word);
        draw_rect(342.0f, 252.0f, (stat[0] - 50.0f) * 1.7333333f, 4.0f, overlay_word);
    }
    if (g_menu_records[g_menu_current].item_list == g_menu_page_18_game_mode) {
        void *picture = find_texture_dict_d(PTR_g_data_6086d8_00608750[course_event_icon_lookup_a(g_menu_event_course_index)]);

        draw_sprite_uv(picture, 456.0f, 144.0f, 152.0f, 152.0f, overlay_word, 0.0f, 1.0f, 0.0f, 1.0f, 1, 0);
    }
    if (g_menu_markers_enabled != 0) {
        double marker_x = (float)screen_width() * 0.515625f;
        double marker_y = (float)screen_height() * 0.78333336f;

        x = (float)(marker_x - 4.0f);
        y = (float)(marker_y - 4.0f);
        w = (float)screen_width() * 0.075f;
        h = (float)screen_height() * 0.1f;
        draw_sprite(find_texture_a("Powerupshadow"), x, y, w, h, overlay_word, 0);
        x = x + 4.0f;
        y = y + 4.0f;
        w = (float)screen_width() * 0.05f;
        h = (float)screen_height() * 0.06666667f;
        draw_sprite(powerup_icon_texture(0x16), x, y, w, h, overlay_word, 0);
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
}

/* menu_count_items_r: C -> original menu_count_items (registers edx) */
__declspec(naked) int menu_count_items_r(char * p0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        call menu_count_items
        ret
    }
}

/* menu_item_y_position_r: C -> original menu_item_y_position (registers eax,ecx) */
__declspec(naked) int menu_item_y_position_r(int p0, int p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call menu_item_y_position
        ret
    }
}

/* course_flag_30_set_r: C -> original course_flag_30_set (registers eax) */
__declspec(naked) void course_flag_30_set_r(unsigned char * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call course_flag_30_set
        ret
    }
}
