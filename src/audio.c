/* MFL.exe audio: functions moved verbatim from batch21.c, batch22.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x004687c0
void voice_queue_slot_release(int i)
{
    g_weapon_ai_8be1dc[i] = 0;
}

// FUNCTION: MFL 0x004687d0
void voice_queue_slots_clear(void)
{
    g_weapon_8be1e0[0] = 0;
    g_weapon_8be1e0[1] = 0;
    g_weapon_8be1e0[2] = 0;
    g_weapon_8be1e0[3] = 0;
}

// FUNCTION: MFL 0x00468790
int voice_queue_slot_alloc(int v)
{
    int i = 0;
    do {
        if (g_weapon_8be1e0[i] == 0) {
            g_weapon_8be1e0[i] = v + 1;
            return i + 1;
        }
        i++;
    } while (i < 4);
    return i;
}

/* naked on purpose: the original clobbers only EAX and its callers (whole program optimised) keep values in ECX/EDX across the call;
   the compiled C used ECX/EDX and stalled the race at frame 25.  Equivalent C: team == 1 && a == 0x23 && b == 0x22 ? 0 : 1 */
// FUNCTION: MFL 0x00468a90
__declspec(naked) int sound_allowed_for_pair(int slot, int a, int b)
{
    __asm {
        mov eax, [esp+4]
        shl eax, 4
        mov eax, [eax+g_player_slots+8]
        dec eax
        jne ret1
        cmp dword ptr [esp+8], 0x23
        jne ret1
        cmp dword ptr [esp+0xc], 0x22
        jne ret1
        xor eax, eax
        ret
    ret1:
        mov eax, 1
        ret
    }
}

// FUNCTION: MFL 0x0046e570
int sound_music_for_menu_page(int page)
{
    switch (page) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 8: case 9: case 0xb: case 0xc: case 0xd:
    case 0xf: case 0x11: case 0x12: case 0x14: case 0x16: case 0x17: case 0x18: case 0x19: case 0x1a:
    case 0x1e: case 0x21: case 0x22: case 0x24: case 0x25: case 0x26: case 0x27:
        return 1;
    default:
        return 0;
    }
}
