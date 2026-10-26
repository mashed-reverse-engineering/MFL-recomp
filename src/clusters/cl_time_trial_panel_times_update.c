// cluster-entries: time_trial_panel_times_update
/* Installed cluster time_trial_panel_times_update (tools/cluster_install.py from output/cluster_pilot6/time_trial_panel_times_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster time_trial_panel_times_update: the time trial panel updates the shown time (current time, then the difference to the best / replay split) and the
   state at +0x1b4 (4 = counting in, 0 = running, 1 = slower, 2 = faster, 3 = equal). */
typedef struct TimeTrialPanel {
    char unknown_00[0x188];
    int shown[3];          /* 0x188: shown time {minutes, seconds, hundredths} */
    int split_time[3];     /* 0x194: time of the current split */
    char unknown_1a0[0x14];
    int state;             /* 0x1b4 */
    char unknown_1b8[4];
    int split_ticks;       /* 0x1bc */
    int intro_ticks;       /* 0x1c0 */
    int finish_ticks;      /* 0x1c4: sign of the low byte = still waiting for the finish compare */
    int split_index;       /* 0x1c8 */
} TimeTrialPanel;

extern float time_get_fraction(int which);
extern int time_get_seconds(int which);
extern int time_get_minutes(int which);
extern int race_get_phase(void);
extern int car_get_mode(int car, int *mode, int *other);
extern int replay_best_get_split_time(int split, float *fraction, int *seconds, int *minutes);
extern int replay_current_get_split_time(int split, float *fraction, int *seconds, int *minutes);

static void time_pack_centiseconds_r(const int * p0, int * p1);
extern void time_pack_centiseconds(void);
static void time_unpack_centiseconds_r(int p0, int * p1);
extern void time_unpack_centiseconds(void);
static void time_make_r(int * p0, int p1, int p2, float p3);
extern void time_make(void);
/* was: EBX = panel.  The 0x30 byte frame of the original is one object (the callees write behind pointers into it): frame.* are its locals in address order. */
void time_trial_panel_times_update_n(TimeTrialPanel *panel)
{
    struct {
        float fraction;      /* local_30 */
        int seconds;         /* local_2c */
        int minutes;         /* local_28 */
        int packed_a;        /* local_24 */
        int packed_b;        /* local_20: also the car mode out parameter */
        int other;           /* local_1c */
        int now[3];          /* local_18 */
        int split[3];        /* local_c */
    } frame;
    float f;
    int m;

    frame.seconds = 0;
    frame.minutes = 0;
    frame.fraction = 0;
    if (panel->intro_ticks <= 0x50) {
        panel->state = 4;
        f = time_get_fraction(1);
        frame.packed_a = time_get_seconds(1);
        m = time_get_minutes(1);
        time_make_r(panel->shown, m, frame.packed_a, f);
        panel->intro_ticks = panel->intro_ticks + 1;
        return;
    }
    f = time_get_fraction(0);
    frame.packed_a = time_get_seconds(0);
    m = time_get_minutes(0);
    time_make_r(frame.now, m, frame.packed_a, f);
    panel->shown[0] = frame.now[0];
    panel->shown[1] = frame.now[1];
    panel->shown[2] = frame.now[2];
    panel->state = 0;
    if (panel->split_index == 0 && replay_current_get_split_time(0, &frame.fraction, &frame.seconds, &frame.minutes) != 0) {
        panel->split_index = 1;
        panel->split_ticks = 0;
    }
    else if (panel->split_index == 1 && replay_current_get_split_time(1, &frame.fraction, &frame.seconds, &frame.minutes) != 0) {
        panel->split_index = 2;
        panel->split_ticks = 0;
    }
    if (panel->split_ticks == 0) {
        time_make_r(panel->split_time, frame.minutes, frame.seconds, frame.fraction);
    }
    if (panel->split_ticks <= 0x46 && panel->split_index > 0) {
        panel->split_ticks = panel->split_ticks + 1;
        if (replay_best_get_split_time(panel->split_index - 1, &frame.fraction, &frame.seconds, &frame.minutes) != 0) {
            time_make_r(frame.split, frame.minutes, frame.seconds, frame.fraction);
            time_pack_centiseconds_r(panel->split_time, &frame.packed_a);
            time_pack_centiseconds_r(frame.split, &frame.packed_b);
            if (frame.packed_a < frame.packed_b) {
                frame.packed_a = frame.packed_b - frame.packed_a;
                panel->state = 2;
            }
            else if (frame.packed_b < frame.packed_a) {
                frame.packed_a = frame.packed_a - frame.packed_b;
                panel->state = 1;
            }
            else {
                frame.packed_a = 0;
                panel->state = 3;
            }
            time_unpack_centiseconds_r(frame.packed_a, panel->shown);
        }
    }
    if (race_get_phase() == RACE_PHASE_STANDINGS) {
        car_get_mode(0, &frame.packed_b, &frame.other);
        if (frame.packed_b == 0 && (char)panel->finish_ticks < 0) {
            f = time_get_fraction(1);
            frame.packed_a = time_get_seconds(1);
            m = time_get_minutes(1);
            time_make_r(frame.split, m, frame.packed_a, f);
            time_pack_centiseconds_r(frame.split, &frame.packed_a);
            time_pack_centiseconds_r(frame.now, &frame.packed_b);
            if (frame.packed_b < frame.packed_a) {
                frame.packed_b = frame.packed_a - frame.packed_b;
                panel->state = 2;
            }
            else if (frame.packed_a < frame.packed_b) {
                frame.packed_b = frame.packed_b - frame.packed_a;
                panel->state = 1;
            }
            else {
                frame.packed_b = 0;
                panel->state = 3;
            }
            time_unpack_centiseconds_r(frame.packed_b, panel->shown);
        }
        if (frame.now[0] == 9 && frame.now[1] == 0x3b && frame.now[2] == 99) {
            panel->shown[0] = -1;
            panel->shown[1] = -1;
            panel->shown[2] = -1;
        }
        panel->finish_ticks = panel->finish_ticks + 1;
    }
}

/* time_pack_centiseconds_r: C -> original time_pack_centiseconds (registers eax,s0) */
__declspec(naked) void time_pack_centiseconds_r(const int * p0, int * p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov eax, dword ptr [esp + 8]
        call time_pack_centiseconds
        add esp, 4
        ret
    }
}

/* time_unpack_centiseconds_r: C -> original time_unpack_centiseconds (registers eax,edi) */
__declspec(naked) void time_unpack_centiseconds_r(int p0, int * p1)
{
    __asm {
        push edi
        mov eax, dword ptr [esp + 8]
        mov edi, dword ptr [esp + 12]
        call time_unpack_centiseconds
        pop edi
        ret
    }
}

/* time_make_r: C -> original time_make (registers esi,eax,ecx,s0) */
__declspec(naked) void time_make_r(int * p0, int p1, int p2, float p3)
{
    __asm {
        push esi
        push dword ptr [esp + 20]
        mov esi, dword ptr [esp + 12]
        mov eax, dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 20]
        call time_make
        add esp, 4
        pop esi
        ret
    }
}

/* entry thunk at the original address of time_trial_panel_times_update: register inputs (ebx) -> natural C time_trial_panel_times_update_n; ret 0 */
__declspec(naked) void time_trial_panel_times_update(void)
{
    __asm {
        push ebx
        call time_trial_panel_times_update_n
        add esp, 4
        ret
    }
}
