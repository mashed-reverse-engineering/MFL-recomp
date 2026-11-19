/* Test harness hook (not game code): runs at the call to app_frame_wait in app_main_loop, counts race frames and freezes the
   game when the target (written by the test script through /proc/PID/mem) is reached, so memory dumps are taken at an exact frame.
   Optional speed-ups for the test runs (all off by default, switched on by the script with `poke`; none touches the race itself):
     fast_menu_scale  N > 1: menu page transitions, menu item animations and message boxes run N times faster. Only the 12 instructions that read
                      g_frame_seconds inside message_box_update, menu_update and menu_items_animate_update are redirected (dwords.txt) to
                      fast_ui_seconds = g_frame_seconds * N; the key repeat timers (menu_up/down_pressed_any), the race and everything else keep the
                      real value; the 1 s waits of the gamesave state machines are skipped by forcing g_gamesave_wait_timer (an earlier global scaling of g_frame_seconds broke key handling). */
extern int g_race_phase;
extern int app_frame_wait(void);
extern int g_tick_number;
extern int g_menu_current;   /* menu stack depth */
extern int timer_ticks(void);
extern void input_update(void);
extern int FUN_004d1b90(int camera);   /* RwCameraBeginUpdate wrapper: 0 = do not draw */
extern float g_frame_seconds;
extern float g_gamesave_wait_timer;
extern void RpRandomSeed(unsigned int seed);
extern void __stdcall Sleep(unsigned long ms);

void (*volatile conf_hook)(void) = 0;   /* conformance tests (harness_conf/conf.c, --conftest): the script writes the address of conf_poll here */
volatile int freeze_target = 0x7fffffff;
volatile int race_frames = 0;
volatile int frozen = 0;          /* once the game is frozen: 1 = target frame reached, 2 = the race ended (phase left 6) before it */
volatile int frames_total = 0;
volatile int fast_menu_scale = 1;
volatile int fast_noshow = 0;    /* 1: do not draw the loading phase (see begin_update_hook) */
volatile int scen_flags = 0;     /* scripted driving, bit mask: 1 accelerate, 2 steer left/right, 4 equip + fire every weapon, 8 disable the opponents' controls, 16 throttle already in the countdown, 32 win the round, 64 lose the round, 256 no freeze on a phase change (training lessons go 6 -> 5 -> 6),
                                     512 the game's own AI (ai_bot_update) drives player 0 while racing and no lesson dialog is up (input keyed to the game state, not to race_frames),
                                     1024 training lesson 4 (power-slides): handbrake + left for the first 30 frames of the game's own "HANDBRAKE NOW" hint (its cooldown g_app_state_69d1d8 = 100 -> 70),
                                     2048 the intro fly-over (race phase 3) always runs to its natural end: no menu_select input reaches the game in phase 3 (W12, REP ld7: a late harness Return skipped part of it) */
volatile int fast_nolimit = 0;   /* bit mask of the phases without frame limiter: 1 menus / Press START, 2 loading, 4 race (see frame_end_ticks) */
volatile float fast_ui_seconds = 0.0f;   /* read by the patched front end instructions */
volatile int unlock_all = 0;     /* 1 (SCEN_UNLOCK=1, scenarios/lib.sh): every front-end frame (race phase 0) gets the state of the game's own 100 % code sequence
                                    (frontend_attract_menu_input 0x4A1B70): all 270 progress words 2 and the bonus menu flag 1, so bonus menu, tournaments and
                                    bonus cameras are usable on both sides; rewriting each front-end frame covers every writer (boot defaults, save load, results) */
extern unsigned int g_game_state[];   /* struct GameState: progress[270] at +0, option_flags[0] at +0xB2C */
volatile int strike_car = -1;   /* >= 0: fire one air strike from player 0's cross-hair (see input_hook); set by the script with `poke`, cleared when done */
static int last_phase = -1;
static int last_menu = -1;
static int frame_waited = 0;     /* set by freeze_hook: the next frame_end_ticks call is the frame end (the C app_main_loop of a linked build calls it for the frame start too) */


/* ---- scripted player input (scen_flags): replaces the call of input_update() in app_main_loop (0x4A15E9) ----
   The player's input record (PlayerInput, 0x4C bytes, base 0x7EF820, index = g_player_slots[0].controller) is written right after input_update, as a function
   of the race frame, so every run does the same.  Weapons are equipped like the trainer does it: the game's own set-weapon function 0x467E60 (esi = player,
   cdecl (weapon record, id); the record is 0x8BED20 + player * 0xB4, the equipped id is the byte at +0xAC). */
static const unsigned char weapon_ids[9] = { 0x07, 0x09, 0x0A, 0x0B, 0x0C, 0x10, 0x11, 0x12, 0x13 };   /* mortar mg drum rocket mines flamethrower shotgun flashbang oil */

static void give_weapon(int player, int id)
{
    unsigned char *rec = (unsigned char *)(0x8BED20 + player * 0xB4);
    if (rec[0xAC])
        return;                        /* the game only equips on an empty slot */
    __asm {
        push esi
        mov esi, player
        mov eax, id
        push eax
        mov eax, rec
        push eax
        mov eax, 0x467E60
        call eax
        add esp, 8
        pop esi
    }
}

/* ---- weapon commands on the game thread (W14 MINE; TS3 reuses them), run by input_hook at the frame boundary ----
   drop p  = player_unequip_weapon 0x467980 (esi = slot 0x8BED20 + p * 0xB4), only when slot+0xA8 != 0 (the original has no null check);
   equip p = player_equip_weapon_by_index 0x467ED0 (cdecl (p, id)), only when slot+0xAC == 0, as the game's equip button.
   Script form: write cmd_drop (bit mask of players) and cmd_equip[p] (weapon id, 0 = none), then cmd_go = 1; the next input_hook does, for p = 0..3,
   drop p then equip p (the trainer's order, MashedTrainer Types/Game.cs EquipWeapon) in one call and sets cmd_go = 0.
   Every call is checked against the fifth mine equip record 0x6A8B58..0x6A8B6B (one past the 4 records at 0x6A8AF8, 0x18 bytes each): mine_log below. */
volatile int cmd_go = 0;
volatile int cmd_drop = 0;
volatile int cmd_equip[4] = { 0, 0, 0, 0 };
/* mine_test (output/mines_crash_2026-11-16.md section 6): 1 = G1 / G2 (per holder: drop + equip mines 0xC in one call), 2 = G0 (drop every holder,
   equip once the game has released the records).  Holders = players 0 .. mine_holders - 1; a non-holder that has a weapon is dropped (G2: two holders only).
   A cycle runs in race phase 6 with all four g_player_status (0x6AAC18) alive (not 2/3/4), no equip record 0..3 in state 2 (+0x10: release pending,
   weapon_mines_update pass 1 frees it) and no non-holder weapon; the first cycle only equips.  After mine_cap cycles past the first, mine_log[1] = 1.
   mine_log: [0] cycles run, [1] done, [2] weapon calls that changed the fifth record, [3] the first of them (cycle << 8 | player << 4 | 1 drop, 2 equip),
   [4..8] the fifth record after it, [9..13] the fifth record when the first cycle ran */
volatile int mine_test = 0;
volatile int mine_holders = 4;
volatile int mine_cap = 10;
volatile int mine_log[16];
static int mine_dropped = 0;     /* G0: holders dropped, waiting for the release */
/* mine_weapon (W14 eqf): the weapon of the test, one of the 4 with a shared pool of 4 equip records (output/weapons_equip_sweep_2026-11-17.md).
   Per weapon: record 0, stride, state field, the state "release pending", and the 5 dwords watched past the 4 records (the fifth record's writes). */
volatile int mine_weapon = 0x0C;
static const struct { int id, base, stride, state, pending, watch; } mine_pools[4] = {
    { 0x0C, 0x6A8AF8, 0x18, 0x10, 2, 0x6A8B58 },     /* mines: fifth record 0x6A8B58 (+0 mine ptr, +0xc owner, +0x10 state) */
    { 0x0A, 0x6A5C60, 0x2C, 0x10, 2, 0x6A5D10 },     /* drum: fifth slot 0x6A5D10 (+0 / +4 parts, +8 count, +0xc owner, +0x10 state) */
    { 0x12, 0x6A5D78, 0x14, 0x0C, 5, 0x6A5DC8 },     /* flashbang: fifth record 0x6A5DC8 (+8 owner, +0xc state) */
    { 0x11, 0x6AAB80, 0x24, 0x18, 2, 0x6AAC20 },     /* shotgun: fifth record 0x6AAC10, watched from +0x10 (= g_player_status[2]): +0x10 3, +0x14 slot, +0x18 1, +0x20 0 */
};

static int mine_pool(void)
{
    int i;
    for (i = 1; i < 4; i++)
        if (mine_pools[i].id == mine_weapon)
            return i;
    return 0;
}

static void weapon_call(int player, int drop, int id)
{
    unsigned char *slot = (unsigned char *)(0x8BED20 + player * 0xB4);
    volatile int *rec5 = (volatile int *)mine_pools[mine_pool()].watch;
    int before[5], i, k, changed;
    for (k = 1; k <= 2; k++) {
        if (k == 1 ? !(drop && *(int *)(slot + 0xA8)) : !(id && !*(int *)(slot + 0xAC)))
            continue;
        for (i = 0; i < 5; i++)
            before[i] = rec5[i];
        if (k == 1) {
            __asm {
                push esi
                mov esi, slot
                mov eax, 0x467980
                call eax
                pop esi
            }
        } else
            ((void (__cdecl *)(int, int))0x467ED0)(player, id);
        for (i = changed = 0; i < 5; i++)
            changed |= before[i] != rec5[i];
        if (changed && mine_log[2]++ == 0) {
            mine_log[3] = mine_log[0] << 8 | player << 4 | k;
            for (i = 0; i < 5; i++)
                mine_log[4 + i] = rec5[i];
        }
    }
}

static void mine_step(void)
{
    int p, i, ready = 1, w = mine_pool();
    if (cmd_go) {
        for (p = 0; p < 4; p++)
            weapon_call(p, (cmd_drop >> p) & 1, cmd_equip[p]);
        cmd_go = 0;
    }
    if (!mine_test || mine_log[1] || g_race_phase != 6)
        return;
    for (p = 0; p < 4; p++) {
        i = ((int *)0x6AAC18)[p];
        if (i == 2 || i == 3 || i == 4)
            return;
    }
    for (i = 0; i < 4; i++)
        if (*(int *)(mine_pools[w].base + mine_pools[w].state + i * mine_pools[w].stride) == mine_pools[w].pending)
            ready = 0;
    for (p = mine_holders; p < 4; p++)
        if (*(int *)(0x8BED20 + p * 0xB4 + 0xA8)) {
            weapon_call(p, 1, 0);
            ready = 0;
        }
    if (!ready)
        return;
    if (mine_log[0] == 0)
        for (i = 0; i < 5; i++)
            mine_log[9 + i] = ((int *)mine_pools[w].watch)[i];
    if (mine_test == 2 && mine_log[0] > 0 && !mine_dropped) {
        for (p = 0; p < mine_holders; p++)
            weapon_call(p, 1, 0);
        mine_dropped = 1;
        return;
    }
    for (p = 0; p < mine_holders; p++)
        weapon_call(p, mine_test == 1, mine_weapon);
    mine_dropped = 0;
    if (++mine_log[0] > mine_cap)
        mine_log[1] = 1;
}

/* Replaces controller_present (0x4A5B20, a jmp to 0x4A70C0): with scen_flags & 128 controllers 0 and 1 count as connected, so the colour select page of the
   multi player menu accepts two human players (the second one has no device: input_hook writes its input). */
int controller_present(int i)
{
    if (i > 3)
        return 0;
    if ((scen_flags & 128) && i < 2)
        return 1;
    return *(int *)(0x7E787C + (i << 9)) != 0;
}

void input_hook(void)
{
    int f, ctl, i;
    unsigned char *in;
    int slot;
    input_update();
    mine_step();
    if (scen_flags & 128) {                       /* two human players: controllers 0 and 1 have joined with colours 1 and 2 (player_color_id, 12 bytes per controller) */
        *(int *)0x69D028 = 1;
        *(int *)0x69D034 = 2;
    }
    /* strike_car (air strike fixture, lane TS W11): when player 0's cross-hair is aiming (hudpanel_cursor_update state 2, cursor record 0x658538:
       +0x34 state, +0x64 hit_car), aim it at an active car and set state 3 (what a lock + fire press does) in ONE frame, before the game's update
       runs: two script pokes let a frame in between reset hit_car to -1 and vehicle_get_matrix_local(-1) faulted (Q1b s1 control, s2 ours).
       The requested car is used if car_is_active (0x47A830), else the first active car of 1..3; nothing happens while none is active. */
    if (strike_car >= 0 && g_race_phase == 6 && *(int *)0x65856C == 2) {
        int (__cdecl *active)(int) = (int (__cdecl *)(int))0x47A830, c = strike_car;
        for (i = 1; c >= 0 && active(c) != 1 && i < 4; i++)
            c = i;
        if (active(c) == 1) { *(int *)0x65859C = c; *(int *)0x65856C = 3; strike_car = -1; }
    }
    /* 2048: the intro skip (camera_record_update 0x44f200..0x44f21e) fires on a menu_select byte (+0xA) going 0 -> non-0 in any of the 8 input
       records (0x7EF82A + k * 0x4C; previous-frame copies at 0x7EFCEA + k * 0x4C).  Clearing the current bytes while the race phase is 3 means no
       edge can appear, so the fly-over ends only by its camera path (340 frames on course 7, ld7_3 / ld7_4). */
    if ((scen_flags & 2048) && g_race_phase == 3)
        for (i = 0; i < 8; i++)
            *(unsigned char *)(0x7EF82A + i * 0x4C) = 0;
    if (!scen_flags || (g_race_phase != 5 && g_race_phase != 6))
        return;
    f = g_race_phase == 6 ? race_frames : -1;       /* race frame, -1 during the countdown */
    if (scen_flags & 8)
        for (i = 1; i < 4; i++)
            *(int *)(0x8B06E0 + i * 0xD04 + 0x10) = 1;   /* Car.controls_disabled of the other cars (cars 1..3 are the opponents in a 4 car battle) */
    if ((scen_flags & 96) && f >= 0) {                /* 32: win the round, 64: lose the round (points and damage like the MashedTrainer writes them) */
        int mx = ((int (*)(void))0x40CE80)();         /* score_target_b: points that win the match */
        volatile float *dmg = (volatile float *)0x65A9E8;   /* 0x28 bytes per player: front +0, back +4 (floats) */
        if (f >= 20 && (scen_flags & 32) && *(int *)(0x8D8B40) < mx - 1) *(int *)(0x8D8B40) = mx - 1;               /* player 0: one point is enough (every round) */
        if (f >= 20 && (scen_flags & 64) && *(int *)(0x8D8B40 + 4) < mx - 1) *(int *)(0x8D8B40 + 4) = mx - 1;      /* opponent 1: one point is enough */
        if (f >= 40 && f % 5 == 0)
            for (i = 0; i < 4; i++)
                if (((scen_flags & 32) && i != 0) || ((scen_flags & 64) && i != 1)) {      /* win: wreck every opponent; lose: wreck ourselves and the other opponents */
                    dmg[i * 10] = 10000.0f; dmg[i * 10 + 1] = 10000.0f;
                }
    }
    for (slot = 0; slot < ((scen_flags & 128) ? 2 : 1); slot++) {   /* 128: both human players are driven the same way */
        ctl = *(int *)(0x7F01FC + slot * 0x10);          /* g_player_slots[slot].controller */
        if (ctl < 0 || ctl > 11)
            continue;
        in = (unsigned char *)(0x7EF820 + ctl * 0x4C);
        if ((scen_flags & 1) && (f >= 0 || (scen_flags & 16)))
            in[4] = 255;                                 /* accelerate */
        if ((scen_flags & 2) && f >= 0) {
            int p = f % 60;
            if (p < 20) { in[0] = 255; in[0x0C] = 255; }                 /* left */
            else if (p >= 30 && p < 50) { in[1] = 255; in[0x0D] = 255; } /* right */
        }
        if ((scen_flags & 4) && f >= 0) {
            if (f % 20 == 1)
                give_weapon(slot, weapon_ids[(f / 20) % 9]);
            if (f % 20 == 6) { in[7] = 255; in[0x13] = 255; }            /* fire */
            if (f % 20 == 14) in[8] = 255;                               /* drop whatever is left */
            if (f % 20 == 10) in[0x3F] = 255;                            /* taunt (PlayerInput.insult) */
        }
    }
    /* 512: player 0 driven by ai_bot_update (0x41B4E0, cdecl (slot, slot type)), like race_mark_ai_slots does for the bots, only while car 0 is active
       (car_is_active 0x47A830) and no lesson dialog is up (g_car_69d2d8 at 0x69D2D8; Return must reach the dialog).  It overwrites the bytes the bits above wrote. */
    ctl = *(int *)0x7F01FC;
    if ((scen_flags & 512) && g_race_phase == 6 && ctl >= 0 && ctl <= 11 && *(int *)0x69D2D8 == 0 && *(int *)0x800488 >= 4      /* route table ready, as race_mark_ai_slots checks */
        && ((int (__cdecl *)(int))0x47A830)(0) == 1) {
        in = (unsigned char *)(0x7EF820 + ctl * 0x4C);
        ((void (__cdecl *)(int, int))0x41B4E0)(0, 2);
        in[0x0C] = in[0] ? 255 : 0;                      /* direction flags as the keyboard path sets them */
        in[0x0D] = in[1] ? 255 : 0;
        if ((scen_flags & 1024) && *(int *)0x8C8078 == 4 && *(int *)0x69D1D8 > 70) {      /* g_training_lesson 4, HANDBRAKE NOW hint shown < 30 frames ago */
            in[4] = 255; in[5] = 255;                    /* accelerate + brake = handbrake */
            in[0] = 255; in[0x0C] = 255; in[1] = 0; in[0x0D] = 0;      /* the lesson counts handbrake frames with left held (training_hint_labels_update) */
        }
    }
}

int freeze_hook(void)
{
    int r = app_frame_wait();
    frames_total++;
    if (conf_hook)
        conf_hook();
    if (g_menu_current != last_menu) {   /* the menus pick backgrounds / preview cars at random: same choice on every page entry, so screenshots are comparable */
        last_menu = g_menu_current;
        RpRandomSeed(1);
    }
    if (g_race_phase != last_phase) {   /* make the race independent of how many menu frames came before */
        if (g_race_phase == 5) {
            g_tick_number = 0;
            RpRandomSeed(1);
        }
        last_phase = g_race_phase;
    }
    fast_ui_seconds = g_frame_seconds * (float)(g_race_phase <= 3 && fast_menu_scale > 1 ? fast_menu_scale : 1);
    if (g_race_phase <= 3 && fast_menu_scale > 1)
        g_gamesave_wait_timer = 2.0f;   /* the Autosave / Save Successful boxes wait 1.0 s of delta_time (not scaled) */
    if (unlock_all && g_race_phase == 0) {
        int i;
        for (i = 0; i < 270; i++)
            g_game_state[i] = 2;
        g_game_state[0xB2C / 4] = 1;
    }
    if (g_race_phase == 6)
        race_frames++;
    if (race_frames >= freeze_target || (race_frames > 0 && g_race_phase != 6 && !(scen_flags & (96 | 256)))) {   /* win/lose scenarios run through the round end screens */   /* a race that ends early (standings screen) is frozen there, in both exes at the same state */
        frozen = race_frames >= freeze_target ? 1 : 2;   /* W13 hx5 (harness review finding 25): 2 = the round ended before the target frame */
        for (;;)
            Sleep(50);
    }
    frame_waited = 1;
    return r;
}

/* Replaces the call of timer_ticks() at the end of a frame in app_main_loop (0x4A1602).  The loop sleeps for (50000 - (now - frame start)) / 3000 ms,
   i.e. holds every frame to 1/60 s.  Everything in the game is frame locked (g_frame_ticks / g_frame_seconds are constants), so pretending the
   frame already took its time makes loading and the race run as fast as the CPU allows without changing the simulation. */
int frame_end_ticks(void)
{
    int t = timer_ticks();
    int cls = g_race_phase == 3 ? 2 : (g_race_phase >= 5 ? 4 : 1);   /* 1 menus / Press START, 2 loading, 4 race (countdown, racing, standings) */
    if (!frame_waited)
        return t;      /* frame start (tools/delink/link_ours.sh renames both timer_ticks calls of the C app_main_loop to this function) */
    frame_waited = 0;
    return (fast_nolimit & cls) ? t + 50000 : t;
}

/* Replaces the 12 calls of FUN_004d1b90 (BeginUpdate of a camera) in app_state_menu_render.  Every draw block there is guarded by "if (BeginUpdate != 0)",
   so returning 0 in race phase 3 (loading) skips drawing the scene that nobody looks at; the loader and the game logic still run. */
int begin_update_hook(int camera)
{
    if (fast_noshow && g_race_phase == 3)
        return 0;
    return FUN_004d1b90(camera);
}
