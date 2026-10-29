// cluster-entries: menu_draw_controller_select
/* Installed cluster menu_draw_controller_select (tools/cluster_install.py from output/cluster_pilot_dtrig/menu_draw_controller_select.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* DECLS-BEGIN */
/* DECLS-END */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0069c97c*/ extern unsigned char g_menu_69c97c;       /* fade level of the panel */
/*0069d26c*/ extern float g_menu_69d26c;               /* animation clock of the flashing */
/*008c86bc*/ extern int g_hud_8c86bc;
/*0069d66c*/ extern float g_hud_69d66c;                /* position of the player 0 controller card */
/*0069d670*/ extern float g_hud_69d670;
/*0069d668*/ extern int g_menu_player0_team_choice;
/*007ef7d0*/ extern int g_menu_7ef7d0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];

/* One controller card on the team select screen: the team picked with that controller (-1 = no controller) and the
   position of the card. */
typedef struct ControllerCard {
    int team_choice;
    float x, y;
} ControllerCard;

/*0069d028*/ extern ControllerCard g_controller_cards[12];

/*00430ab0*/ extern short screen_width(void);
/*00435ec0*/ extern int event_type_uses_standard_menu(void);
/*00435040*/ extern void *team_color_names(int team);
/*00430eb0*/ extern void ui_keyboard_icon_find(int controller, float x, float y, unsigned int colour, int is_any_team);
/*00430fe0*/ extern void menu_draw_selection_frame(int y, int fade);
/*0042c8e0*/ extern float text_width(int text_id, float scale);
/*0042c540*/ extern void text_draw_plain(char *text, float x, float y, unsigned int colour, float scale, int flags);
/*0042c110*/ extern void text_draw_box(int text_id, float x, float y, float w, float h, unsigned int colour, float scale);
/*0042c440*/ extern void ui_rt2d_text_draw(int text_id, float x, float y, unsigned int colour, float scale, int flags);
/*004824a0*/ extern void draw_rect(float x, float y, float w, float h, unsigned int colour);
/*004832d0*/ extern void draw_sprite_uv(void *texture, float x, float y, float w, float h, unsigned int colour, float u0, float u1, float v0, float v1, int mode, int flag);
extern double sin(double);

/* A packed colour: byte 3 is the alpha, bytes 0..2 the colour channels. */
typedef union UiColour {
    unsigned char b[4];
    unsigned int packed;
} UiColour;

#define TEAM_COUNT 6
#define CARD_COUNT 12
#define CARD_GROUP 6                /* the cards come in two groups of six (inferred: when g_menu_7ef7d0 != 0 the card number shown wraps modulo 6) */
#define TEAM_ANY 6                  /* team choice 6: the player takes any team */
#define TEAM_CHIP_FIRST_X 146.0f    /* the six team chips: colour bar behind a 64x64 team picture, 70 units apart */
#define TEAM_BAR_FIRST_X 143.0f
#define TEAM_CHIP_STEP 70.0f
#define TEAM_CHIP_SIZE 64.0f
#define TEAM_BAR_WIDTH 69.0f
#define TEAM_BAR_HEIGHT 20.0f
#define CARD_PICTURE_UP 41.0f       /* the chips sit above the card position */
#define CARD_PICTURE_UP_MORE 40.0f
#define CARD_BAR_UP 38.0f
#define KEYBOARD_ICON_LIFT 1.0f
#define LABEL_SCALE 0.8f
#define LABEL_TEXT_ID 0x37
#define LABEL_GAP 15.0f
#define NUMBER_SCALE 0.6f
#define FLASH_PERIOD 7.0
#define FLASH_AMPLITUDE 20.0

/* Draws the team colour chips above a card. */
static void team_chips_draw(float card_y, UiColour white, UiColour *team_colour)
{
    float picture_x = TEAM_CHIP_FIRST_X;
    float bar_x = TEAM_BAR_FIRST_X;
    float picture_y = (card_y - CARD_PICTURE_UP) - CARD_PICTURE_UP_MORE;
    float bar_y = card_y - CARD_BAR_UP;
    int i;

    for (i = 0; i < TEAM_COUNT; i++) {
        draw_rect(bar_x, bar_y, TEAM_BAR_WIDTH, TEAM_BAR_HEIGHT, team_colour[i + 1].packed);
        draw_sprite_uv(team_color_names(i), picture_x, picture_y, TEAM_CHIP_SIZE, TEAM_CHIP_SIZE, white.packed,
                       0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        picture_x = picture_x + TEAM_CHIP_STEP;
        bar_x = bar_x + TEAM_CHIP_STEP;
    }
}

/* Team select panel: one card per controller with its team colour, a keyboard / pad icon and the card number.  Without the
   standard menu every active card is drawn on the screen; with it only the card of player 0. */
void menu_draw_controller_select(void)
{
    UiColour team_colour[7];    /* 0 grey (default), 1..6 = team_color_rgba(0..5) in order (b[0] is the RED byte: draw_rect / draw_sprite_uv repack bytes R,G,B,A into the D3DCOLOR; so red, steel blue, olive green, yellow, pink, black) */
    UiColour white, black, colour, any_colour;
    unsigned char fade = g_menu_69c97c;
    float flash;
    char label[4];
    int adv;
    int i;

    team_colour[0].b[0] = 0xe0; team_colour[0].b[1] = 0xe0; team_colour[0].b[2] = 0xe0;
    team_colour[1].b[0] = 0x98; team_colour[1].b[1] = 0x3a; team_colour[1].b[2] = 0x3d;
    team_colour[2].b[0] = 0x4e; team_colour[2].b[1] = 0x89; team_colour[2].b[2] = 0xae;
    team_colour[3].b[0] = 0x61; team_colour[3].b[1] = 0x76; team_colour[3].b[2] = 0x56;
    team_colour[4].b[0] = 0xdb; team_colour[4].b[1] = 0xc3; team_colour[4].b[2] = 0x62;
    team_colour[5].b[0] = 0xeb; team_colour[5].b[1] = 0xa7; team_colour[5].b[2] = 0xa7;
    team_colour[6].b[0] = 0;    team_colour[6].b[1] = 0;    team_colour[6].b[2] = 0;
    any_colour.b[0] = 0xd7; any_colour.b[1] = 0xd7; any_colour.b[2] = 0xd7;
    for (i = 0; i < 7; i++)
        team_colour[i].b[3] = fade;
    any_colour.b[3] = fade;
    white.b[0] = 0xff;
    white.b[1] = 0xff;
    white.b[2] = 0xff;
    g_hud_8c86bc = 0;
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
    flash = (float)(sin(g_menu_69d26c * FLASH_PERIOD) * FLASH_AMPLITUDE);

    if (event_type_uses_standard_menu() != 0) {
        /* the card of player 0 */
        UiColour bar, border;
        float frame_y;
        float frame_width;
        float lower_y;
        float lower_offset;

        white.b[3] = fade;
        team_chips_draw(g_hud_69d670, white, team_colour);
        frame_y = g_hud_69d670 - 13.0f;
        bar.b[0] = 0xf0; bar.b[1] = 0x6e; bar.b[2] = 0x14; bar.b[3] = fade >> 1;
        draw_rect(48.0f, frame_y, 534.0f, 26.0f, bar.packed);
        border.b[0] = 0xb4; border.b[1] = 0x50; border.b[2] = 0x10; border.b[3] = fade;
        draw_rect(46.0f, frame_y, 2.0f, 26.0f, border.packed);
        frame_width = 534.0f + 4.0f;
        draw_rect(46.0f, frame_y, frame_width, 2.0f, border.packed);
        lower_offset = 26.0f - 2.0f;
        lower_y = lower_offset + frame_y;
        draw_rect(46.0f, lower_y, frame_width, 2.0f, border.packed);
        draw_rect(534.0f + 48.0f, lower_y - lower_offset, 2.0f, 26.0f, border.packed);
        if (g_menu_player0_team_choice != TEAM_ANY)
            colour = team_colour[g_menu_player0_team_choice];
        else
            colour = any_colour;
        ui_keyboard_icon_find(g_player_slots[0].controller, g_hud_69d66c, g_hud_69d670 - KEYBOARD_ICON_LIFT, colour.packed,
                              g_menu_player0_team_choice == TEAM_ANY);
        adv = (int)text_width(LABEL_TEXT_ID, LABEL_SCALE);
        adv = adv / 2 + 4;
        black.b[0] = 0; black.b[1] = 0; black.b[2] = 0; black.b[3] = fade;
        label[0] = (char)g_player_slots[0].controller + '1';
        label[1] = 0;
        text_draw_plain(label, (float)adv + g_hud_69d66c + LABEL_GAP, g_hud_69d670, black.packed, NUMBER_SCALE, 0);
    } else {
        /* every connected controller gets a card on the screen */
        float cards_used;
        int panels_drawn = 0;
        int j;

        black.b[0] = 0; black.b[1] = 0; black.b[2] = 0; black.b[3] = fade;
        if (g_menu_7ef7d0 != 0) {
            ui_rt2d_text_draw(0x24a, 320.0f, 330.0f, black.packed, NUMBER_SCALE, 2);
            ui_rt2d_text_draw(0x249, 320.0f, 350.0f, black.packed, NUMBER_SCALE, 2);
            ui_rt2d_text_draw(0x282, 320.0f, 370.0f, black.packed, NUMBER_SCALE, 2);
            ui_rt2d_text_draw(0x283, 320.0f, 390.0f, black.packed, NUMBER_SCALE, 2);
        }
        cards_used = 0.0f;
        for (j = 0; j < CARD_COUNT; j++)
            if (g_controller_cards[j].team_choice != -1)
                cards_used = cards_used + 1.0f;
        if (cards_used < 2.0f)
            text_draw_box(0x32, 80.0f, 80.0f, 480.0f, 100.0f, black.packed, 0.7f);
        for (j = 0; j < CARD_COUNT; j++) {
            ControllerCard *card = &g_controller_cards[j];
            int team = card->team_choice;
            int duplicate;
            int k;
            int digit;

            if (team == -1)
                continue;
            if (!panels_drawn) {
                white.b[3] = fade;
                team_chips_draw(card->y, white, team_colour);
                panels_drawn = 1;
            }
            menu_draw_selection_frame((int)card->y, fade);
            if (team == 0) {
                ui_keyboard_icon_find(j, card->x, card->y - KEYBOARD_ICON_LIFT, team_colour[0].packed, 0);
            } else {
                duplicate = 0;
                for (k = 0; k < CARD_COUNT; k++)
                    if (team == g_controller_cards[k].team_choice && team > 0 && k != j)
                        duplicate = 1;
                if (team == TEAM_ANY)
                    colour = any_colour;
                else
                    colour = team_colour[team];
                if (duplicate) {
                    /* the same team on two cards: the colour flashes */
                    char amount = (char)(int)flash;

                    if (team == TEAM_ANY)
                        amount = amount << 1;
                    colour.b[0] = colour.b[0] + amount;
                    colour.b[1] = colour.b[1] + amount;
                    colour.b[2] = colour.b[2] + amount;
                }
                ui_keyboard_icon_find(j, card->x, card->y - KEYBOARD_ICON_LIFT, colour.packed, team == TEAM_ANY);
            }
            adv = (int)text_width(LABEL_TEXT_ID, LABEL_SCALE);
            adv = adv / 2 + 4;
            black.b[0] = 0; black.b[1] = 0; black.b[2] = 0; black.b[3] = fade;
            digit = j;
            if (g_menu_7ef7d0 != 0) {
                if (j >= CARD_GROUP)
                    digit = j - CARD_GROUP;
                if (digit >= CARD_GROUP)
                    digit = digit - CARD_GROUP;
            }
            label[0] = (char)digit + '1';
            label[1] = 0;
            text_draw_plain(label, (float)adv + card->x + LABEL_GAP, card->y, black.packed, NUMBER_SCALE, 0);
        }
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
}

