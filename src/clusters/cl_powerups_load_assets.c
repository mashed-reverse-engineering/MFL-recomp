// cluster-entries: powerups_load_assets
/* Installed cluster powerups_load_assets (tools/cluster_install.py from output/cluster_pilot_zy/powerups_load_assets.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008bed20*/ extern char player_weapon_slots[];
/*006aac28*/ extern char g_weapon_6aac28[];
/*006aaef8*/ extern int g_weapons_6aaef8[];
/*006aaf08*/ extern int g_particles_6aaf08;
/*0060de60*/ extern int weapon_descriptor_count;
/*0060dc34*/ extern void (*PTR_gatling_load_models_0060dc34[])(void);
/*0042f860*/ extern int texdict_load_named_b(char *name, int a, int b);
/*004650e0*/ extern void headlight_lazer_init(void);
/*004644a0*/ extern void powerups_init(void);
/*0045f620*/ extern void explosion_queue_clear_frames(void);
/*0048cf80*/ extern void lua_run_script_file(char *name, void *state, void (*register_functions)(void));
/*004492b0*/ extern void lua_register_course_functions(void);
/*004144d0*/ extern void load_lazer_model(void);
/*004042a0*/ extern void load_bomb_model(void);

#define WEAPON_SLOT_SIZE (0xb4)
#define WEAPON_DESCRIPTOR_STRIDE (0x10)

/* Loads the power-up assets: clears the 4 per player weapon slots (and their effect state copies), loads PowerUpModels.txd, runs every weapon's model loader from the descriptor table,
   initialises headlights / power-ups / the explosion queue, runs powerups_all.lua and loads the lazer and bomb models. `ecx_slot` is the push ecx frame slot of the original (the register is never read). */
static void weapon_fx_state_clear_r(void * p0);
extern void weapon_fx_state_clear(void);
void powerups_load_assets_n(int ecx_slot)
{
  int state_slot[1];
  int i;

  for (i = 0; i < 4; i++) {
    weapon_fx_state_clear_r(player_weapon_slots + i * WEAPON_SLOT_SIZE);
    *(int *)(player_weapon_slots + i * WEAPON_SLOT_SIZE + 0xb0) = i;
    weapon_fx_state_clear_r(g_weapon_6aac28 + i * WEAPON_SLOT_SIZE);
    g_weapons_6aaef8[i] = 0;
  }
  g_particles_6aaf08 = texdict_load_named_b("PowerUpModels.txd", 0, 0);
  for (i = 0; i < weapon_descriptor_count; i++) {
    PTR_gatling_load_models_0060dc34[i * WEAPON_DESCRIPTOR_STRIDE]();
  }
  headlight_lazer_init();
  powerups_init();
  explosion_queue_clear_frames();
  lua_run_script_file("powerups_all.lua", state_slot, lua_register_course_functions);
  load_lazer_model();
  load_bomb_model();
}

/* weapon_fx_state_clear_r: C -> original weapon_fx_state_clear (registers esi) */
__declspec(naked) void weapon_fx_state_clear_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_state_clear
        pop esi
        ret
    }
}

/* entry thunk at the original address of powerups_load_assets: register inputs (ecx) -> natural C powerups_load_assets_n; ret 0 */
__declspec(naked) void powerups_load_assets(void)
{
    __asm {
        push ecx
        call powerups_load_assets_n
        add esp, 4
        ret
    }
}
