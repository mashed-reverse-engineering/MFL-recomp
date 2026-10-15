/* MFL.exe ui_np: functions moved verbatim from game_globals.c, game_misc.c, game_misc2.c, panels.c, ui_misc.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x004353c0
int menu_powerup_set_for_event(void)
{
    if (get_event_type() == 0x12)
        return 1;
    return g_menu_powerup_set;
}

int menu_markers_enabled_for_event(void)
{
    /* near match: the original encodes e - 0x12 as sub eax,0x12 (83 e8 12), cl emits add eax,-18 (83 c0 ee); all else identical */
    int e = get_event_type();
    int m = g_menu_markers_enabled;
    unsigned x = e - 0x12;
    return (x ? -1 : 0) & m;
}

// FUNCTION: MFL 0x0042fb80
int menu_page_for_game_mode(void)
{
    if (game_mode == 10 || game_mode == 9 || game_mode == 8 || game_mode == 7 || game_mode == 5)
        return 0x2e;
    return 0x16;
}

// FUNCTION: MFL 0x0041d630
void btb_panel_slot_set_value(int slot, int value)
{
    g_btb_panel_658c0c[slot * 0x31] = value;
}

// FUNCTION: MFL 0x0041d930
void btb_panel_slot_set_visible(int slot, int value)
{
    g_btb_panel_658c10[slot * 0x31] = value;
}

// FUNCTION: MFL 0x0042fcb0
void menu_reset_scroll(void)
{
    g_menu_8c8074 = 0x180;
    g_menu_8c8090 = 0;
}

/* register passing function: count in EAX, spacing in ECX (ours used EDX for the count: the menus were laid out with a garbage count).
   Equivalent C: (count & 1) == 0 ? (240 - spacing / 2) - ((count - 1) >> 1) * spacing : 240 - ((count - 1) >> 1) * spacing */
// FUNCTION: MFL 0x0042fe50
__declspec(naked) int menu_item_y_position(void)
{
    __asm {
        push ebx
        mov ebx, eax
        test bl, 1
        jne odd
        mov eax, ecx
        cdq
        sub eax, edx
        mov edx, eax
        sar edx, 1
        mov eax, 0xf0
        sub eax, edx
        dec ebx
        shr ebx, 1
        imul ebx, ecx
        sub eax, ebx
        pop ebx
        ret
    odd:
        dec ebx
        shr ebx, 1
        imul ebx, ecx
        mov eax, 0xf0
        sub eax, ebx
        pop ebx
        ret
    }
}

// FUNCTION: MFL 0x0041d580
void btb_panel_slot_set_position(int slot, int *p)
{
    char *r = g_btb_panel_658b50 + slot * 0xc4;
    if (p != 0) {
        *(int *)(r + 0xa0) = p[0];
        *(int *)(r + 0xa4) = p[1];
        *(int *)(r + 0xa8) = p[2];
        return;
    }
    *(int *)(r + 0xa0) = 0;
    *(int *)(r + 0xa4) = 0;
    *(int *)(r + 0xa8) = 0;
}

// FUNCTION: MFL 0x0041d5d0
void btb_panel_slot_set_scale(int slot, int *p)
{
    char *r = g_btb_panel_658b50 + slot * 0xc4;
    if (p != 0) {
        *(int *)(r + 0x80) = p[0];
        *(int *)(r + 0x84) = p[1];
        *(int *)(r + 0x88) = p[2];
        *(int *)(r + 0x8c) = p[3];
        return;
    }
    *(int *)(r + 0x80) = 0;
    *(int *)(r + 0x84) = 0;
    *(int *)(r + 0x88) = 0;
    *(int *)(r + 0x8c) = 0;
}

// FUNCTION: MFL 0x004364c0
void ui_set_triple(int a, int b, int x, int y, int z)
{
    int off = (b + a * 2) * 0xc;
    *(int *)(g_ui_8c7fc0 + off) = x;
    *(int *)(g_ui_8c7fc4 + off) = y;
    *(int *)(g_ui_8c7fc8 + off) = z;
}

// FUNCTION: MFL 0x00437e80
int menu_is_page_13_selected_1(void)
{
    if (g_menu_records[g_menu_current].page_id == 0x13 && g_menu_records[g_menu_current].selected == 1)
        return 1;
    return 0;
}

// FUNCTION: MFL 0x00431710
void ui_menu_reset(void)
{
    g_car_69d2d8 = 0;
    g_menu_records[0].selected = 0;
    g_car_8c8070 = 0;
    g_car_8c86c4 = 0;
}
