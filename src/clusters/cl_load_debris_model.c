// cluster-entries: load_debris_model
/* Installed cluster load_debris_model (tools/cluster_install.py from output/cluster_pilot_pg2/load_debris_model.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042f780*/ extern void *__cdecl clump_load_by_name(const char *name, int unused_a, int unused_b);

#define DEBRIS_VEHICLE_COUNT 4
#define DEBRIS_MODEL_COUNT 6                 /* debris0.dff .. debris5.dff */

/* debris slot record of one vehicle (stride 0x208) */
typedef struct DebrisRecord {
  unsigned char unknown_00[0x208];
} DebrisRecord;
/*0065c0d0*/ extern DebrisRecord g_game_65c0d0[DEBRIS_VEHICLE_COUNT];
/*00607838*/ extern const char *PTR_s_debris0_dff_00607838[DEBRIS_MODEL_COUNT];

/* load_debris_model: loads one debris model per vehicle and builds the vehicle's debris slots from it.  `model_choice` selects the
   model per vehicle (its entry modulo 6); NULL means model 0 for all */
static void debris_slots_init_for_vehicle_r(void * p0, void * p1, int p2);
extern void debris_slots_init_for_vehicle(void);
void load_debris_model(int *model_choice)
{
  int vehicle;
  int model;

  for (vehicle = 0; vehicle < DEBRIS_VEHICLE_COUNT; vehicle++) {
    if (model_choice != 0) {
      model = model_choice[vehicle] % DEBRIS_MODEL_COUNT;
    } else {
      model = 0;
    }
    debris_slots_init_for_vehicle_r(&g_game_65c0d0[vehicle], clump_load_by_name(PTR_s_debris0_dff_00607838[model], 0, 0), vehicle);
  }
}

/* debris_slots_init_for_vehicle_r: C -> original debris_slots_init_for_vehicle (registers edi,eax,s0) */
__declspec(naked) void debris_slots_init_for_vehicle_r(void * p0, void * p1, int p2)
{
    __asm {
        push edi
        push dword ptr [esp + 16]
        mov edi, dword ptr [esp + 12]
        mov eax, dword ptr [esp + 16]
        call debris_slots_init_for_vehicle
        add esp, 4
        pop edi
        ret
    }
}
