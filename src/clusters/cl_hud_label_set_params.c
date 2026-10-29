// cluster-entries: hud_label_set_params hud_label_add_from_bytes hud_label_add_world_text hud_label_add_from_dat_string
/* Installed cluster hud_label_set_params (tools/cluster_install.py from output/cluster_pilot5/hud_label_set_params.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster hud_label_set_params: HUD text labels.  A label is allocated (hud_label_find_free), gets its text (wide string copy of a byte string / a dat string)
   and its screen position, colour and flags through hud_label_set_params. */
typedef struct HudLabel {
    char unknown_00[0x80];
    float world[3];        /* 0x80: world position (hud_label_add_world_text) */
    float x;               /* 0x8c: screen position in 640x480 */
    float y;               /* 0x90 */
    float delta_x;         /* 0x94: (320 - x) * 0.01 */
    float delta_y;         /* 0x98: (240 - y) * 0.01 */
    int flags;             /* 0x9c: always 0x5dc here */
    int param_a0;          /* 0xa0: RGBA bytes copied from *a (callers pass unsigned char[4] colours) */
    int param_a4;          /* 0xa4: RGBA bytes copied from *b */
    int param_a8;          /* 0xa8: float bits, text height factor (24.0, 32.0, 10.0 at the callers; hud_labels_render_all passes it as the scale) */
    int param_ac;          /* 0xac: float bits (0 .. 2.0 at the callers); not read by hud_labels_render_all */
} HudLabel;

extern float k_f_320;
extern float k_f_240;
extern float k_f_0_01;
extern HudLabel *hud_label_find_free(void);
extern void wstring_from_bytes(HudLabel *label, const char *text);
extern void wstring_length_skip(HudLabel *label, const void *text);
extern const void *dat_get_string(int id);
extern void world_to_screen_640x480(float *world, float *screen);

/* was: ECX = screen position (x, y), EAX = label; then the stack arguments flags .. d. */
void hud_label_set_params_n(const float *pos, HudLabel *label, int flags, const int *a, const int *b, int c, int d)
{
    label->x = pos[0];
    label->delta_x = (k_f_320 - label->x) * k_f_0_01;
    label->y = pos[1];
    label->flags = flags;
    label->param_a8 = c;
    label->delta_y = (k_f_240 - label->y) * k_f_0_01;
    label->param_ac = d;
    label->param_a4 = *b;
    label->param_a0 = *a;
}

void hud_label_add_from_bytes(const char *text, const float *pos, const int *a, const int *b, int c, int d)
{
    HudLabel *label = hud_label_find_free();
    if (label != 0) {
        wstring_from_bytes(label, text);
        hud_label_set_params_n(pos, label, 0x5dc, a, b, c, d);
    }
}

void hud_label_add_world_text(int string_id, const float *world, const int *a, const int *b, int c, int d)
{
    HudLabel *label = hud_label_find_free();
    float screen[2];

    if (label != 0) {
        wstring_length_skip(label, dat_get_string(string_id));
        label->world[0] = world[0];
        label->world[1] = world[1];
        label->world[2] = world[2];
        world_to_screen_640x480(label->world, screen);
        hud_label_set_params_n(screen, label, 0x5dc, a, b, c, d);
    }
}

void hud_label_add_from_dat_string(int string_id, const float *pos, const int *a, const int *b, int c, int d)
{
    HudLabel *label = hud_label_find_free();
    if (label != 0) {
        wstring_length_skip(label, dat_get_string(string_id));
        hud_label_set_params_n(pos, label, 0x5dc, a, b, c, d);
    }
}

/* entry thunk at the original address of hud_label_set_params: register inputs (ecx,eax,s0,s1,s2,s3,s4) -> natural C hud_label_set_params_n; ret 0 */
__declspec(naked) void hud_label_set_params(void)
{
    __asm {
        push eax
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push eax
        push ecx
        call hud_label_set_params_n
        add esp, 28
        pop eax
        ret
    }
}
