// cluster-entries: menu_draw_event_select
/* Installed cluster menu_draw_event_select (tools/cluster_install.py from output/cluster_pilot_j1/menu_draw_event_select.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* DECLS-BEGIN */
/*007ef7ec*/ extern float g_frame_seconds;
/* DECLS-END */
/*0069cc3c*/ extern float g_menu_draw_69cc3c;           /* animation clock of the event select page (seconds) */
/*0069cbd4*/ extern int g_event_type;
/*0069cc40*/ extern int g_team_mode;
/*0069d688*/ extern int g_career_row;
/*0069d690*/ extern int g_menu_event_column;
/*0069cf98*/ extern int g_menu_bot_skill_a;
/*0069cbd0*/ extern int g_menu_current;
/*0069cfb4*/ extern int g_menu_player_team_choice;
/*0069cfb8*/ extern int g_menu_player_team_choice_1;
/*0069cfbc*/ extern int g_menu_player_team_choice_2;
/*0069cfc8*/ extern int g_menu_team_mode_choice;
/*0069d68c*/ extern int g_menu_69d68c;                  /* scroll offset of the event list */
/*008c8090*/ extern int g_menu_8c8090;                  /* countdown of the "new event" marker */
/*008c8724*/ extern int g_course_8c8724;                /* text id of the message shown when an event unlocks */
/*0069d294*/ extern int g_menu_draw_69d294[];           /* random powerup per icon slot */
/*00609a18*/ extern int g_menu_draw_609a18[];           /* powerup ids that can be drawn */
/*00608848*/ extern unsigned int g_menu_608848[];       /* per career row and event type 3..5: category word (high half) + sub kind (low bits) */
/*006088fc*/ extern unsigned int g_menu_6088fc[];       /* the same for event type 0xe */
/*00608938*/ extern unsigned int g_menu_608938[];       /* ... 0xf */
/*00608974*/ extern unsigned int g_menu_608974[];       /* ... 0x10 */
/*0060a990*/ extern char g_menu_page_24_game_mode[];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007eebc0*/ extern struct GameState g_game_state;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*005de9b4*/ extern char g_frontend_5de9b4[];           /* "%d" */
/*005de9b8*/ extern char g_ui_5de9b8[];                 /* format of a zero padded number */
/*005de9c0*/ extern char g_ui_5de9c0[];
/*005dec10*/ extern char g_frontend_5dec10[];
/*005dec1c*/ extern char g_frontend_5dec1c[];
/*005de9c8*/ extern char g_menu_draw_5de9c8[];          /* "check" */
/*005dec6c*/ extern char g_menu_draw_5dec6c[];          /* "Powerupshadow" */

/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*0043d1e0*/ extern void menu_layout_draw_b(int fade, int skip_list);
/*00435ec0*/ extern int event_type_uses_standard_menu(void);
/*00435040*/ extern int team_color_names(int team);
/*0040d130*/ extern int find_texture_a(const char *name);
/*0040d0f0*/ extern int find_texture_dict_c(const char *name);
/*00435dc0*/ extern int ui_collect_team_ids(int index);
/*00435f20*/ extern int ui_get_menu_69d690(void);
/*00436400*/ extern int menu_event_type_switch_b(void);
/*00436460*/ extern int menu_event_type_switch_c(void);
/*004362c0*/ extern int career_event_has_entry(void);
/*00434290*/ extern int career_flag_enabled(int event_row, int flag);
/*0040f080*/ extern int *race_get_race_setup(void);
/*00464330*/ extern int powerup_icon_texture(int powerup);
/*00481fd0*/ extern double random_range(float low, float high);
/*00431130*/ extern void message_show_now(int text_id, int a, int b, int c, int d, int e);
/*00434030*/ extern void menu_player_select_draw_rotated(int text_id, float x, float y, unsigned int colour, float scale, float angle);
/*0042c440*/ extern void ui_rt2d_text_draw(int text_id, float x, float y, unsigned int colour, float scale, int flags);
/*0042c540*/ extern void text_draw_plain(char *text, float x, float y, unsigned int colour, float scale, int flags);
/*004824a0*/ extern void draw_rect(float x, float y, float w, float h, unsigned int colour);
/*00483130*/ extern void draw_sprite(int texture, float x, float y, float w, float h, unsigned int colour, int mode);
/*004832d0*/ extern void draw_sprite_uv(int texture, float x, float y, float w, float h, unsigned int colour, float u0, float v0, float u1, float v1, int flag_a, int flag_b);
/*004b2a60*/ extern int __cdecl sprintf(char *buffer, const char *format, ...);
extern double sin(double);

/* A player / team icon (flag_b 1 = first icon of a group) and a plain texture. */
#define TEAM_ICON(team, x, y, w, h, first) draw_sprite_uv(team_color_names(team), (x), (y), (w), (h), colour, 0.0f, 1.0f, 0.0f, 1.0f, 1, (first))
#define TEX_UV(tex, x, y, w, h) draw_sprite_uv((tex), (x), (y), (w), (h), colour, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1)

/* event types: enum EventType of mfl_tier2.h */
#define EV_CUP_0 EVENT_CUP_3
#define EV_CUP_1 EVENT_CUP_4
#define EV_CUP_2 EVENT_CUP_5
#define EV_TURBO EVENT_TURBO_TOURNAMENT
#define EV_ICE EVENT_ICE_TOURNAMENT
#define EV_NIGHT EVENT_NIGHT_TOURNAMENT
#define EV_TRAINING EVENT_TRAINING
#define IS_TIME_TRIAL(e) ((e) >= EVENT_TIME_TRIAL_0 && (e) <= EVENT_TIME_TRIAL_2)
#define IS_QUICK(e) ((e) >= EVENT_QUICK_RACE_11 && (e) <= EVENT_QUICK_RACE_13)
#define IS_TOURNAMENT(e) ((e) == EVENT_TURBO_TOURNAMENT || (e) == EVENT_ICE_TOURNAMENT || (e) == EVENT_NIGHT_TOURNAMENT)

/* category (high half of the table word) of a career row */
#define CAT_MASK 0xffff0000u

static int team_choices(void)
{
    int n = g_menu_player_team_choice != 0;
    if (g_menu_player_team_choice_1 != 0) n++;
    if (g_menu_player_team_choice_2 != 0) n++;
    return n;
}

/* word of the category table for the event type (event types 3..5: column 0..2 of the row; 0xe..0x10: own tables) */
static unsigned int category_word(int event)
{
    int col = 0;
    unsigned int word;
    if (event == EV_CUP_1) col = 1;
    else if (event == EV_CUP_2) col = 2;
    word = g_menu_608848[col + g_career_row * 3];
    if (event == EV_TURBO) word = g_menu_6088fc[g_career_row * 3];
    else if (event == EV_ICE) word = g_menu_608938[g_career_row * 3];
    else if (event == EV_NIGHT) word = g_menu_608974[g_career_row * 3];
    return word;
}

/* Draws the event selection panel of the front end (a 576 x 96 px band at y 300..396 of the 640x480 layout with a divider
   at x 319 (not for training), faded by 'fade'). Per event type (cups, tournaments, time trials, quick races, training, or
   the multiplayer setups when event_type_uses_standard_menu() is 0) it shows the player/team icons, the
   category of the career row looked up in the g_menu_* category tables (category_word), the track/powerup
   icons and, for time trials, the best time formatted as mm:ss.cc. skip_list is handed to menu_layout_draw_b.
   Inferred from the draw calls and the tables read; the exact meaning of several g_menu_* tables is not
   confirmed. */
void menu_draw_event_select(int fade, int skip_list)
{
    unsigned int colour, band, word, cat, sub;
    int alpha;
    int event, layout, nplay, draw_icons, tex, ntm;
    int a0, a1, a2, b0, b1, b2, p0, p1, p2, p3, p4;
    float x, y, vw, vh, ys, cw, ch, xs;
    int t1, t2, t3, k, i, sw, shh;
    int minutes, seconds, centi;
    char sec_text[8], centi_text[8], line[16];

    g_menu_draw_69cc3c = g_frame_seconds + g_menu_draw_69cc3c;
    menu_layout_draw_b(fade & 0xff, skip_list);
    band = (unsigned int)(unsigned char)(((fade & 0xff) * 2) / 3) << 24;
    draw_rect(32.0f, 300.0f, 576.0f, 96.0f, band);
    colour = ((unsigned int)(fade & 0xff) << 24) | 0xffffff;
    alpha = fade & 0xff;
    if (g_event_type != EV_TRAINING) draw_rect(319.0f, 308.0f, 2.0f, 80.0f, colour);
    event = g_event_type;
    screen_width();
    screen_height();
    screen_width();
    screen_height();
    layout = 0;
    if (event_type_uses_standard_menu() == 0) {
        /* free-for-all / team icons of the players (multiplayer) */
        nplay = g_player_slots[0].controller != -1;
        if (g_player_slots[1].controller != -1) nplay++;
        if (g_player_slots[2].controller != -1) nplay++;
        if (g_player_slots[3].controller != -1) nplay++;
        if (g_team_mode != 0) {
            if (nplay == 2) {
                TEAM_ICON(g_player_slots[0].team, 40.0f, 292.0f, 112.0f, 112.0f, 1);
                TEAM_ICON(g_player_slots[1].team, 196.0f, 292.0f, 112.0f, 112.0f, 0);
                tex = find_texture_a("vs");
                TEX_UV(tex, 132.0f, 300.0f, 88.0f, 88.0f);
            }
            if (nplay == 3) {
                /* two teams: the odd one out (humans against computer players) against the other two */
                if ((g_player_slots[0].state == 0) == (g_player_slots[1].state == 0)) p0 = 2;
                else p0 = ((g_player_slots[2].state == 0) == (g_player_slots[0].state == 0)) ? 1 : 0;
                p1 = (p0 == 0) ? 1 : 0;
                p2 = (p0 == 2) ? 1 : 2;
                TEAM_ICON(g_player_slots[p0].team, 56.0f, 292.0f, 112.0f, 112.0f, 1);
                TEAM_ICON(g_player_slots[p1].team, 216.0f, 288.0f, 64.0f, 64.0f, 0);
                TEAM_ICON(g_player_slots[p2].team, 216.0f, 340.0f, 64.0f, 64.0f, 0);
                tex = find_texture_a("vs");
                TEX_UV(tex, 144.0f, 304.0f, 88.0f, 88.0f);
            }
            if (nplay == 4) {
                /* a0 a1 a2: the human players (state 0), b0 b1 b2: the others, in slot order (a third entry is overwritten by a fourth) */
                a0 = a1 = a2 = b0 = b1 = b2 = -1;
                if (g_player_slots[0].state == 0) a0 = 0; else b0 = 0;
                if (g_player_slots[1].state == 0) {
                    if (a0 == -1) a0 = 1; else a1 = 1;
                } else if (b0 == -1) b0 = 1;
                else b1 = 1;
                if (g_player_slots[2].state == 0) {
                    if (a0 == -1) a0 = 2; else if (a1 == -1) a1 = 2; else a2 = 2;
                } else if (b0 == -1) b0 = 2;
                else if (b1 == -1) b1 = 2;
                else b2 = 2;
                if (g_player_slots[3].state == 0) {
                    if (a0 == -1) a0 = 3; else if (a1 == -1) a1 = 3; else a2 = 3;
                } else if (b0 == -1) b0 = 3;
                else if (b1 == -1) b1 = 3;
                else b2 = 3;
                p0 = a0;
                p1 = b0;
                p2 = a1;
                p3 = b1;
                p4 = b2;
                if (a2 != -1) {
                    p4 = a2;
                    p0 = b0;
                    p3 = a1;
                    p1 = a0;
                    p2 = b1;
                }
                if (p4 == -1) {
                    TEAM_ICON(g_player_slots[p0].team, 74.0f, 296.0f, 64.0f, 64.0f, 1);
                    TEAM_ICON(g_player_slots[p1].team, 220.0f, 296.0f, 64.0f, 64.0f, 0);
                    TEAM_ICON(g_player_slots[p2].team, 74.0f, 340.0f, 64.0f, 64.0f, 0);
                    TEAM_ICON(g_player_slots[p3].team, 220.0f, 340.0f, 64.0f, 64.0f, 0);
                    tex = find_texture_a("vs");
                    TEX_UV(tex, 127.0f, 300.0f, 96.0f, 96.0f);
                } else {
                    TEAM_ICON(g_player_slots[p0].team, 56.0f, 292.0f, 112.0f, 112.0f, 1);
                    TEAM_ICON(g_player_slots[p1].team, 202.0f, 288.0f, 64.0f, 64.0f, 0);
                    TEAM_ICON(g_player_slots[p3].team, 232.0f, 316.0f, 64.0f, 64.0f, 0);
                    TEAM_ICON(g_player_slots[p4].team, 202.0f, 344.0f, 64.0f, 64.0f, 0);
                    tex = find_texture_a("vs");
                    TEX_UV(tex, 144.0f, 304.0f, 88.0f, 88.0f);
                }
            }
        } else if (nplay == 2) {
            TEAM_ICON(g_player_slots[0].team, 40.0f, 292.0f, 112.0f, 112.0f, 1);
            x = g_menu_bot_skill_a != 0 ? 148.0f : 196.0f;
            TEAM_ICON(g_player_slots[1].team, x, 292.0f, 112.0f, 112.0f, 0);
            x = g_menu_bot_skill_a != 0 ? 108.0f : 132.0f;
            tex = find_texture_a("vs");
            TEX_UV(tex, x, 300.0f, 88.0f, 88.0f);
            if (g_menu_bot_skill_a != 0) {
                tex = find_texture_a("question");
                TEX_UV(tex, 262.0f, 304.0f, 48.0f, 48.0f);
                TEX_UV(tex, 262.0f, 348.0f, 48.0f, 48.0f);
                tex = find_texture_a("vs");
                TEX_UV(tex, 232.0f, 324.0f, 48.0f, 48.0f);
            }
        } else if (nplay == 3) {
            TEAM_ICON(g_player_slots[0].team, 40.0f, 308.0f, 80.0f, 80.0f, 1);
            x = g_menu_bot_skill_a != 0 ? 112.0f : 136.0f;
            TEAM_ICON(g_player_slots[1].team, x, 308.0f, 80.0f, 80.0f, 0);
            x = g_menu_bot_skill_a != 0 ? 184.0f : 232.0f;
            TEAM_ICON(g_player_slots[2].team, x, 308.0f, 80.0f, 80.0f, 0);
            x = g_menu_bot_skill_a != 0 ? 88.0f : 100.0f;
            tex = find_texture_a("vs");
            TEX_UV(tex, x, 316.0f, 64.0f, 64.0f);
            x = g_menu_bot_skill_a != 0 ? 160.0f : 196.0f;
            TEX_UV(tex, x, 316.0f, 64.0f, 64.0f);
            if (g_menu_bot_skill_a != 0) {
                tex = find_texture_a("question");
                TEX_UV(tex, 262.0f, 316.0f, 64.0f, 64.0f);
                tex = find_texture_a("vs");
                TEX_UV(tex, 232.0f, 324.0f, 48.0f, 48.0f);
            }
        } else if (nplay == 4) {
            TEAM_ICON(g_player_slots[0].team, 40.0f, 308.0f, 72.0f, 72.0f, 1);
            TEAM_ICON(g_player_slots[1].team, 104.0f, 308.0f, 72.0f, 72.0f, 0);
            TEAM_ICON(g_player_slots[2].team, 168.0f, 308.0f, 72.0f, 72.0f, 0);
            TEAM_ICON(g_player_slots[3].team, 232.0f, 308.0f, 72.0f, 72.0f, 0);
            tex = find_texture_a("vs");
            TEX_UV(tex, 72.0f, 316.0f, 56.0f, 56.0f);
            TEX_UV(tex, 136.0f, 316.0f, 56.0f, 56.0f);
            TEX_UV(tex, 200.0f, 316.0f, 56.0f, 56.0f);
        }
    } else {
        switch (event) {
        case 0: case 1: case 2:
            ui_rt2d_text_draw(0x25, 330.0f, 316.0f, colour, 0.6f, 0);
            ui_rt2d_text_draw(0x13f, 330.0f, 334.0f, colour, 0.55f, 0);
            minutes = g_game_state.best_minutes[ui_get_menu_69d690() + g_career_row * 3];
            seconds = g_game_state.best_seconds[ui_get_menu_69d690() + g_career_row * 3];
            centi = (int)(g_game_state.best_fraction[ui_get_menu_69d690() + g_career_row * 3] * 100.0f);
            if (seconds < 10) sprintf(sec_text, g_ui_5de9b8, g_ui_5de9c0, seconds);
            else sprintf(sec_text, g_frontend_5de9b4, seconds);
            if (centi < 10) sprintf(centi_text, g_ui_5de9b8, g_ui_5de9c0, centi);
            else sprintf(centi_text, g_frontend_5de9b4, centi);
            sprintf(line, g_frontend_5dec10, minutes, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
            ui_rt2d_text_draw(0x12f, 330.0f, 350.0f, colour, 0.55f, 0);
            text_draw_plain(line, 508.0f, 350.0f, colour, 0.55f, 0);
            layout = 1;
            break;
        case 3: case 4: case 5: case 0xe: case 0xf: case 0x10:
            if (event == EV_TURBO) i = 0x2b3;
            else if (event == EV_ICE) i = 0x2b5;
            else if (event == EV_NIGHT) i = 0x2b4;
            else if (event == EV_CUP_0) i = 0x11a;
            else if (event == EV_CUP_1) i = 0x11b;
            else i = 0x11c;
            if (g_career_row > 9) {
                i = 0x259;
                if (g_career_row == 0xb) i = 0x25c;
                else if (g_career_row == 0xc) i = 0x25f;
                if (event == EV_CUP_1) i = i + 1;
                else if (event == EV_CUP_2) i = i + 2;
            }
            ui_rt2d_text_draw(i, 326.0f, 316.0f, colour, 0.55f, 0);
            word = category_word(event);
            cat = word & CAT_MASK;
            sub = word & 3;
            if (cat == 0 || cat == 0x10000 || cat == 0xc0000) {
                if (sub == 1) {
                    ui_rt2d_text_draw(0xef, 326.0f, 334.0f, colour, 0.6f, 0);
                    ui_rt2d_text_draw(0x11d, 326.0f, 350.0f, colour, 0.55f, 0);
                    if (cat == 0x10000) ui_rt2d_text_draw(0x126, 326.0f, 366.0f, colour, 0.55f, 0);
                } else {
                    if (sub == 2) {
                        if (cat == 0x10000 || cat == 0xc0000) {
                            ui_rt2d_text_draw(0x20b, 326.0f, 334.0f, colour, 0.6f, 0);
                            ui_rt2d_text_draw(0x20c, 326.0f, 350.0f, colour, 0.55f, 0);
                            i = 0x20d;
                        } else {
                            ui_rt2d_text_draw(0x208, 326.0f, 334.0f, colour, 0.6f, 0);
                            ui_rt2d_text_draw(0x209, 326.0f, 350.0f, colour, 0.55f, 0);
                            i = 0x20a;
                        }
                    } else if (cat == 0x10000 || cat == 0xc0000) {
                        ui_rt2d_text_draw(0x20e, 326.0f, 334.0f, colour, 0.6f, 0);
                        ui_rt2d_text_draw(0x20f, 326.0f, 350.0f, colour, 0.55f, 0);
                        i = 0x210;
                    } else {
                        ui_rt2d_text_draw(0x211, 326.0f, 334.0f, colour, 0.6f, 0);
                        ui_rt2d_text_draw(0x212, 326.0f, 350.0f, colour, 0.55f, 0);
                        i = 0x213;
                    }
                    ui_rt2d_text_draw(i, 326.0f, 366.0f, colour, 0.55f, 0);
                }
                layout = 2;
            }
            if (cat == 0x90000 || cat == 0xa0000 || cat == 0xd0000) {
                ui_rt2d_text_draw(0xef, 326.0f, 334.0f, colour, 0.6f, 0);
                i = (sub == 1) ? 0x11d : (sub == 2) ? 0x11e : 0x11f;
                ui_rt2d_text_draw(i, 326.0f, 350.0f, colour, 0.55f, 0);
                if (cat == 0xa0000) ui_rt2d_text_draw(0x126, 326.0f, 366.0f, colour, 0.55f, 0);
                layout = 2;
            }
            if (cat == 0x20000 || cat == 0x30000 || cat == 0xb0000) {
                ui_rt2d_text_draw(0x124, 326.0f, 334.0f, colour, 0.6f, 0);
                ui_rt2d_text_draw(0x125, 326.0f, 350.0f, colour, 0.55f, 0);
                if (cat != 0x20000) ui_rt2d_text_draw(0x126, 326.0f, 366.0f, colour, 0.55f, 0);
                layout = 2;
            }
            if (cat == 0x40000) {
                ui_rt2d_text_draw(0xf5, 326.0f, 334.0f, colour, 0.6f, 0);
                ui_rt2d_text_draw(0xfa, 326.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0xfb, 326.0f, 366.0f, colour, 0.55f, 0);
                layout = 0x40000;
            } else if (cat == 0x50000) {
                ui_rt2d_text_draw(0xf3, 326.0f, 334.0f, colour, 0.6f, 0);
                ui_rt2d_text_draw(0xf8, 326.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0xf9, 326.0f, 366.0f, colour, 0.55f, 0);
                layout = 0x50000;
            } else if (cat == 0x60000) {
                ui_rt2d_text_draw(0xf2, 326.0f, 334.0f, colour, 0.6f, 0);
                ui_rt2d_text_draw(0xfc, 326.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0xfd, 326.0f, 366.0f, colour, 0.55f, 0);
                layout = 2000;
            } else if (cat == 0x70000) {
                ui_rt2d_text_draw(0xf1, 326.0f, 334.0f, colour, 0.6f, 0);
                ui_rt2d_text_draw(0xf6, 326.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0xf7, 326.0f, 366.0f, colour, 0.55f, 0);
                layout = 0x70000;
            } else if (cat == 0x80000) {
                ui_rt2d_text_draw(0xf4, 326.0f, 334.0f, colour, 0.6f, 0);
                ui_rt2d_text_draw(0xfe, 326.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0xff, 326.0f, 366.0f, colour, 0.55f, 0);
                layout = 2;
            }
            if (layout == 0) goto tail;
            break;
        case 0xb: case 0xc: case 0xd:
            sw = screen_width();
            cw = (float)sw * 0.0375f;
            shh = screen_height();
            ch = (float)shh * 0.05f;
            sw = screen_width();
            xs = (float)((sw * 0x142) / 0x280);
            shh = screen_height();
            ys = (float)((shh * 0x142) / 0x1e0);
            ui_rt2d_text_draw(0xe9, 344.0f, 316.0f, colour, 0.6f, 0);
            if (g_menu_records[g_menu_current].item_list == g_menu_page_24_game_mode) {
                ntm = team_choices();
                g_team_mode = g_menu_team_mode_choice ^ 1;
                if (ntm == 3 && g_menu_team_mode_choice != 0) {
                    ui_rt2d_text_draw(0x129, 344.0f, 334.0f, colour, 0.6f, 0);
                    ui_rt2d_text_draw(0x12a, 344.0f, 350.0f, colour, 0.6f, 0);
                } else {
                    ui_rt2d_text_draw(0x131, 344.0f, 334.0f, colour, 0.6f, 0);
                    ui_rt2d_text_draw(0x132, 344.0f, 350.0f, colour, 0.6f, 0);
                }
            } else {
                ui_rt2d_text_draw(0xea, 344.0f, 334.0f, colour, 0.55f, 0);
                tex = find_texture_dict_c(g_menu_draw_5de9c8);
                draw_sprite(tex, xs, ys, cw, ch, colour, 1);
                shh = screen_height();
                y = (float)((shh * 0x10) / 0x1e0) + ys;
                ui_rt2d_text_draw(0xeb, 344.0f, 350.0f, colour, 0.55f, 0);
                tex = find_texture_dict_c(g_menu_draw_5de9c8);
                draw_sprite(tex, xs, y, cw, ch, colour, 1);
                shh = screen_height();
                y = (float)((shh * 0x10) / 0x1e0) + y;
                ui_rt2d_text_draw(0xec, 344.0f, 366.0f, colour, 0.55f, 0);
                tex = find_texture_dict_c(g_menu_draw_5de9c8);
                draw_sprite(tex, xs, y, cw, ch, colour, 1);
                shh = screen_height();
                y = (float)((shh * 0x10) / 0x1e0) + y;
                ui_rt2d_text_draw(0xed, 344.0f, 382.0f, colour, 0.55f, 0);
                tex = find_texture_dict_c(g_menu_draw_5de9c8);
                draw_sprite(tex, xs, y, cw, ch, colour, 1);
            }
            layout = 2;
            break;
        case 0x11:
            switch (g_career_row) {
            case 0:
                ui_rt2d_text_draw(0x2c1, 64.0f, 316.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2c2, 64.0f, 333.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2c3, 64.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2c4, 64.0f, 367.0f, colour, 0.55f, 0);
                break;
            case 1:
                ui_rt2d_text_draw(0x2c5, 64.0f, 316.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2c6, 64.0f, 333.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2c7, 64.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2c8, 64.0f, 367.0f, colour, 0.55f, 0);
                break;
            case 2:
                ui_rt2d_text_draw(0x2c9, 64.0f, 316.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2ca, 64.0f, 333.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2cb, 64.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2cc, 64.0f, 367.0f, colour, 0.55f, 0);
                break;
            case 3:
                ui_rt2d_text_draw(0x2cd, 64.0f, 333.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2ce, 64.0f, 350.0f, colour, 0.55f, 0);
                break;
            case 4:
                ui_rt2d_text_draw(0x2cf, 64.0f, 316.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2d0, 64.0f, 333.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2d1, 64.0f, 350.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2d2, 64.0f, 367.0f, colour, 0.55f, 0);
                break;
            case 5:
                ui_rt2d_text_draw(0x2d3, 64.0f, 325.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2d4, 64.0f, 342.0f, colour, 0.55f, 0);
                ui_rt2d_text_draw(0x2d5, 64.0f, 359.0f, colour, 0.55f, 0);
                break;
            }
            goto tail;
        default:
            goto tail;
        }
        /* the icon of the first player, then the icons of the other teams */
        x = 40.0f;
        y = 292.0f;
        if (layout == 1) x = 128.0f;
        if (IS_QUICK(event) && team_choices() == 0) x = x + 88.0f;
        vw = 112.0f;
        vh = 112.0f;
        TEAM_ICON(g_player_slots[0].team, x, 292.0f, 112.0f, 112.0f, 1);
        if (layout == 2 || layout == 2000) {
            ntm = 0;
            if (IS_QUICK(event)) ntm = g_menu_team_mode_choice;
            if (event >= EV_CUP_0 && event <= EV_CUP_2) {
                cat = category_word(event) & CAT_MASK;
                if (cat == 0x90000 || cat == 0xa0000 || cat == 0xd0000) ntm = 1;
            }
            if (IS_TOURNAMENT(event)) {
                cat = category_word(event) & CAT_MASK;
                if (cat == 0x90000 || cat == 0xa0000 || cat == 0xd0000) goto team_pair;
            }
            if (ntm != 0) goto team_pair;
            /* up to three more teams, laid out by their number */
            k = 1;
            if (ui_collect_team_ids(2) != -1) k = 2;
            if (ui_collect_team_ids(3) != -1) k = 3;
            if (IS_QUICK(event)) k = team_choices();
            if (k == 2 || k == 3) {
                vw = 64.0f;
                vh = 64.0f;
            }
            t1 = ui_collect_team_ids(1);
            if (t1 != -1) {
                if (k == 1) { x = 196.0f; y = 292.0f; }
                else if (k == 2) { x = 216.0f; y = 288.0f; }
                else if (k == 3) { x = 202.0f; y = 288.0f; }
                TEAM_ICON(t1, x, y, vw, vh, 0);
            }
            t2 = ui_collect_team_ids(2);
            if (t2 != -1) {
                if (k == 2) { x = 216.0f; y = 340.0f; }
                else if (k == 3) { x = 232.0f; y = 316.0f; }
                TEAM_ICON(t2, x, y, vw, vh, 0);
            }
            t3 = ui_collect_team_ids(3);
            if (t3 != -1) TEAM_ICON(t3, 202.0f, 344.0f, vw, vh, 0);
            if (k == 0) goto cat_icon;
            tex = find_texture_a("vs");
            TEX_UV(tex, 132.0f, 300.0f, 88.0f, 88.0f);
            if (layout != 2000) goto cat_icon;
            TEX_UV(team_color_names(9), 104.0f, 356.0f, 48.0f, 48.0f);
            goto tail;
team_pair:
            k = ui_collect_team_ids(1) != -1;
            if (ui_collect_team_ids(2) != -1) k = 2;
            if (ui_collect_team_ids(3) != -1) {
                k = 3;
                vw = 56.0f;
                vh = 56.0f;
            } else if (k == 2) {
                vw = 64.0f;
                vh = 64.0f;
            }
            t1 = ui_collect_team_ids(1);
            if (t1 != -1) {
                if (k == 1) { x = 196.0f; y = 292.0f; }
                else if (k == 2) { x = 164.0f; y = 316.0f; }
                else if (k == 3) { x = 142.0f; y = 320.0f; }
                TEAM_ICON(t1, x, y, vw, vh, 0);
            }
            t2 = ui_collect_team_ids(2);
            if (t2 != -1) {
                if (k == 2) { x = 242.0f; y = 316.0f; }
                else if (k == 3) { x = 192.0f; y = 320.0f; }
                TEAM_ICON(t2, x, y, vw, vh, 0);
            }
            t3 = ui_collect_team_ids(3);
            if (t3 != -1) TEAM_ICON(t3, 252.0f, 320.0f, vw, vh, 0);
            if (k == 1) {
                tex = find_texture_a("vs");
                TEX_UV(tex, 132.0f, 300.0f, 88.0f, 88.0f);
            } else if (k == 2) {
                tex = find_texture_a("vs");
                TEX_UV(tex, 128.0f, 320.0f, 56.0f, 56.0f);
                TEX_UV(tex, 206.0f, 320.0f, 56.0f, 56.0f);
            } else if (k == 3) {
                tex = find_texture_a("vs");
                TEX_UV(tex, 116.0f, 324.0f, 48.0f, 48.0f);
                TEX_UV(tex, 166.0f, 324.0f, 48.0f, 48.0f);
                TEX_UV(tex, 216.0f, 324.0f, 48.0f, 48.0f);
            }
        }
cat_icon:
        /* the icon of the category: team_color_names index 6 for category 0x70000 (kill the copter), 7 for 0x40000, 8 for 0x50000 (chase the fugitive) (inferred: these indices are reused as category pictures) */
        if (layout == 0x70000) k = 6;
        else if (layout == 0x40000) k = 7;
        else if (layout == 0x50000) k = 8;
        else goto tail;
        TEAM_ICON(k, 196.0f, 292.0f, vw, vh, 0);
        tex = find_texture_a("vs");
        TEX_UV(tex, 132.0f, 300.0f, 88.0f, 88.0f);
    }
tail:
    event = g_event_type;
    draw_icons = 0;
    if (event_type_uses_standard_menu() != 0) {
        cat = 0;
        if (event >= EV_CUP_0 && event <= EV_CUP_2) cat = category_word(event) & CAT_MASK;
        else if (IS_TOURNAMENT(event)) cat = category_word(event) & CAT_MASK;
        if (cat == 0x10000 || cat == 0xc0000 || cat == 0xd0000 || cat == 0xa0000 || cat == 0x30000 || cat == 0xb0000 || cat == 0x70000 || cat == 0x60000)
            draw_icons = 1;
    }
    if (skip_list == 0 && draw_icons) {
        for (i = 0; i < 5; i++) {
            double base_x, base_y, hs;
            float x0, y0, scale, s15, sx, sy;
            int pu, rec, it;
            short sw1, sw2, sh1;

            sw1 = screen_width();
            base_x = sw1 * 0.8625f;
            sh1 = screen_height();
            y0 = (float)(sh1 * 0.78333336f);
            sw2 = screen_width();
            it = (sw2 * i * 40) / 640;
            x0 = (float)(base_x - it);
            cat = 0;
            word = category_word(event);
            cat = word & CAT_MASK;
            scale = 0.0f;
            if (cat == 0xb0000 || cat == 0xc0000 || cat == 0xd0000) {
                double v;
                int t = (int)(g_menu_draw_69cc3c * -60.0f);
                v = sin((double)((i * 30 - t) * 0.16666667f)) * 48.0f;
                if (v < 0.0) v = 0.0f;
                v = v * 0.6666667f;
                scale = (float)v;
                if (v >= 31.0f)
                    g_menu_draw_69d294[i] = (int)((double)g_menu_draw_69d294[i] + random_range(0.0f, 15.0f));
                pu = powerup_icon_texture(g_menu_draw_609a18[g_menu_draw_69d294[i] & 0xf]);
            } else {
                int *setup, *entry;
                int u = 0;
                if (g_menu_event_column == menu_event_type_switch_b()) u = 1;
                if (g_menu_event_column == menu_event_type_switch_c()) u = 2;
                setup = race_get_race_setup();
                entry = (int *)setup[1 + u * 15 + g_career_row];
                if (g_game_state.records[i + *entry * 5] <= -1) goto next_icon;
                u = 0;
                if (g_menu_event_column == menu_event_type_switch_b()) u = 1;
                if (g_menu_event_column == menu_event_type_switch_c()) u = 2;
                setup = race_get_race_setup();
                entry = (int *)setup[1 + u * 15 + g_career_row];
                rec = g_game_state.records[i + *entry * 5];
                pu = powerup_icon_texture(rec);
                if (cat == 0x70000) {
                    pu = 0;
                    if (i == 0) pu = powerup_icon_texture(9);
                    else if (i == 1) pu = powerup_icon_texture(0xb);
                }
            }
            if (pu != 0) {
                float shx, shy, shw, shh2;
                short a, b, c, d;
                a = screen_width();
                sx = (float)(a * 0.075f);
                b = screen_height();
                sy = (float)(b * 0.1f);
                s15 = (float)(scale * 1.5f);
                hs = (scale * 1.5f) * 0.5f;
                shx = (float)((x0 - 4.0f) + hs);
                shy = (float)((y0 - 4.0f) + hs);
                c = screen_width();
                shw = (float)(sx - c * s15 * 0.0015625f);
                d = screen_height();
                shh2 = (float)(sy - d * s15 * 0.0020833334f);
                draw_sprite(find_texture_a(g_menu_draw_5dec6c), shx, shy, shw, shh2, colour, 0);
                {
                    short e1, e2, e3, e4, e5, e6, e7;
                    double ax, by, hsc, w05, h06;
                    float icon_x, icon_y, icon_w, icon_h;
                    e1 = screen_width();
                    ax = e1 * 0.8625f;
                    e2 = screen_height();
                    by = e2 * 0.78333336f;
                    hsc = scale * 0.5f;
                    e3 = screen_width();
                    it = (e3 * i * 40) / 640;
                    icon_x = (float)((ax - it) + hsc);
                    icon_y = (float)(by + hsc);
                    e4 = screen_width();
                    w05 = e4 * 0.05f;
                    e5 = screen_height();
                    h06 = e5 * 0.06666667f;
                    e6 = screen_width();
                    icon_w = (float)(w05 - e6 * scale * 0.0015625f);
                    e7 = screen_height();
                    icon_h = (float)(h06 - e7 * scale * 0.0020833334f);
                    draw_sprite(pu, icon_x, icon_y, icon_w, icon_h, colour, 0);
                }
            }
next_icon:
            event = g_event_type;
        }
    }
    /* the marker of a new event (inferred: a blinking stamp, sprite 0xee or 0x268 when the career progress is 0, that flies off while g_menu_8c8090 counts down) */
    {
        int ent, st, cnt, row, id;
        float mx, my, mscale, mangle;
        unsigned int mcol;

        if (IS_QUICK(event) || IS_TIME_TRIAL(event)) goto event_row;
        ent = career_event_has_entry();
        row = g_career_row;
        if (career_flag_enabled(row, ent + 1000) == 0 && g_game_state.progress[ent + row * 18] == 1) {
            g_menu_8c8090 = 0;
            goto after_marker;
        }
        st = g_game_state.progress[ent + row * 18];
        if (st != 2 && st != 0) {
            g_menu_8c8090 = 0;
            goto after_marker;
        }
        if (event_type_uses_standard_menu() == 0) {
            g_menu_8c8090 = 0;
            goto after_marker;
        }
        cnt = g_menu_8c8090;
        if (cnt == 0) alpha = (unsigned char)(int)((sin(g_menu_draw_69cc3c * 8.0f) + 1.0f) * 0.5f * (fade & 0xff));
        mangle = 7.5f;
        if (cnt > 0x20) {
            int d = cnt - 0x20;
            alpha = (unsigned char)(int)((64.0f - cnt) * 0.03125f * alpha);
            mscale = (float)(d * 0.5f + 1.2f);
            mangle = (float)(-(float)d * 4.0f + 7.5f);
        } else mscale = 1.2f;
        mscale = (float)(mscale * 0.6896551f);
        mx = 466.0f;
        my = 260.0f;
        mangle = mangle - 7.5f;
        if (cnt > 0x20) {
            int d = cnt - 0x20;
            mx = (float)(466.0f - 4.5625f * d);
            my = (float)(260.0f - d * 0.625f);
        }
        mcol = (unsigned int)alpha << 24;
        if (skip_list != 0) {
            mx = mx - 16.0f;
            my = my - 8.0f;
        }
        id = 0xee;
        if (st == 0) id = 0x268;
        menu_player_select_draw_rotated(id, mx, my, mcol, mscale, mangle);
        mx = mx - 2.0f;
        my = my - 2.0f;
        mcol = ((unsigned int)alpha << 24) | 0xa0e0ff;
        if (st == 0) mcol = ((unsigned int)alpha << 24) | 0x8080ff;
        menu_player_select_draw_rotated(id, mx, my, mcol, mscale, mangle);
        cnt = g_menu_8c8090;
        if (cnt != 0) {
            cnt = cnt - 1;
            g_menu_8c8090 = cnt;
            if (cnt < 0) {
                g_menu_8c8090 = 0;
                goto after_marker;
            }
        }
        if (cnt != 1) goto after_marker;
        if (g_course_8c8724 != 0) message_show_now(g_course_8c8724, 0, 1, 0x2e, 0, 0);
        event = g_event_type;
        row = g_career_row;
        if (event == EV_CUP_0) {
            if (row > 5 && row != 0xe && g_game_state.progress[row * 18 + 0x15] != 0) g_menu_69d68c = g_menu_69d68c - 0x15;
            goto event_row;
        }
        goto after_marker2;
after_marker:
        event = g_event_type;
event_row:
        row = g_career_row;
after_marker2:
        if (event != EV_CUP_0) {
            if (event != EVENT_CUP_4 && event != EVENT_CUP_5 && event != EVENT_TURBO_TOURNAMENT && event != EVENT_ICE_TOURNAMENT && event != EVENT_NIGHT_TOURNAMENT) g_menu_8c8090 = 0;
            if (event != EV_CUP_0 && event != EVENT_CUP_4 && event != EVENT_CUP_5) return;
        }
        if (row >= 0xc) {
            mx = 466.0f;
            my = 240.0f;
            if (skip_list != 0) {
                mx = 450.0f;
                my = 232.0f;
            }
            menu_player_select_draw_rotated(0x258, mx, my, (unsigned int)alpha << 24, 0.82758623f, 0.0f);
            menu_player_select_draw_rotated(0x258, mx - 3.0f, my - 3.0f, ((unsigned int)alpha << 24) | 0xa0e0ff, 0.82758623f, 0.0f);
        }
    }
}

