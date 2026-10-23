/* MFL.exe ui: functions moved verbatim from batch12.c, batch13.c, batch22.c, batch23.c, batch7.c, batch9.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_hud_69d664[];

// FUNCTION: MFL 0x0047fe10
void hud_labels_clear(void)
{
    int *p = g_car_6c29f4;
    int n = 8;
    do {
        *p = 0;
        p += 0x2c;
        n--;
    } while (n != 0);
}

// FUNCTION: MFL 0x0047fed0
char *hud_label_find_free(void)
{
    char *p = g_car_6c2958;
    int i = 0;
    do {
        if (*(int *)(p + 0x9c) == 0)
            return p;
        p += 0xb0;
        i++;
    } while (i < 8);
    return 0;
}

// FUNCTION: MFL 0x00480130
void text_page_reset_line(void)
{
    g_car_6c2ee4[g_rwp_trigger_group_count].line = 0;
}

// FUNCTION: MFL 0x00480150
__declspec(naked) void text_page_next(void)   /* naked: original clobbers only EAX/ECX; equivalent C: p = &g_car_6c2ee4[n]; p->prev_line = p->line; p->line = 0; p->zero = 0; n++ */
{
    __asm {
        mov eax, g_rwp_trigger_group_count
        imul eax, eax, 0x8c
        add eax, offset g_car_6c2ee4
        mov ecx, [eax+0x80]
        mov [eax+0x84], ecx
        xor ecx, ecx
        mov [eax+0x80], ecx
        mov [eax+0x88], ecx
        inc g_rwp_trigger_group_count
        ret
    }
}

// FUNCTION: MFL 0x00480020
void wstring_length_skip(unsigned short *dst, unsigned short *src)
{
    if ((short)*src > 0) {
        unsigned int n = *src;
        do {
            src++;
            *dst = *src;
            dst++;
            n--;
        } while (n != 0);
        *dst = 0;
        return;
    }
    *dst = 0;
}

// FUNCTION: MFL 0x0047fad0
void wstring_from_bytes(unsigned short *dst, unsigned char *src)
{
    int n = (*(int (**)(unsigned char *))(RwEngineInstance + 0xf4))(src);
    if (n <= 0) {
        *dst = 0;
        return;
    }
    do {
        *dst = *src;
        dst++;
        src++;
        n--;
    } while (n != 0);
    *dst = 0;
}

// FUNCTION: MFL 0x004a04e0
void ui_colour_a_set(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    g_app_62c5d0 = r;
    g_app_62c5d1 = g;
    g_app_62c5d3 = a;
    g_app_62c5d2 = b;
}

// FUNCTION: MFL 0x004a0510
void ui_colour_b_set(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    g_ui_colour_62c5d4 = r;
    g_ui_colour_62c5d5 = g;
    g_ui_colour_62c5d7 = a;
    g_ui_colour_62c5d6 = b;
}

// FUNCTION: MFL 0x0041d9e0
void btb_panel_render_all(void)
{
    int i = 0;
    do {
        btb_panel_slot_render(i);
        i++;
    } while (i < 2);
}

// FUNCTION: MFL 0x0041da30
void btb_panel_set_visible_all(int *v)
{
    int i = 0;
    do {
        btb_panel_slot_set_visible(i, v[i]);
        i++;
    } while (i < 2);
}

// FUNCTION: MFL 0x00431a30
void ui_clear_cbe8(void)
{
    int i;
    for (i = 0xc; i != 0; i--)
        g_ui_69cbe8[12 - i] = 0;
}

// FUNCTION: MFL 0x00437eb0
int menu_is_page_13_selected_2(void)
{
    if (g_menu_records[g_menu_current].page_id == MENU_PAGE_SOUND && g_menu_records[g_menu_current].selected == 2)
        return 1;
    return 0;
}

// FUNCTION: MFL 0x00432d20
void menu_table_clear_a(void)
{
    int *p = &g_menu_records[1].page_id;
    int i;
    do {
        p[-1] = 0;
        p[0] = 0;
        p[1] = 0;
        for (i = 0; i < 12; i++)
            p[2 + i] = 0;
        p[0xe] = 0;
        p += 0x10;
    } while ((int)p < ((int)&g_hud_69d664));
}
