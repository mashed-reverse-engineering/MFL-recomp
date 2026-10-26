// cluster-entries: race_phase_3_update
/* Installed cluster race_phase_3_update (tools/cluster_install.py from output/cluster_pilot9/race_phase_3_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004062d0*/ extern int __cdecl camera_path_update(undefined4 p0);
/*0040fdd0*/ extern int race_set_phase();
/*004103d0*/ extern int race_spawn_cars();
/*00410de0*/ extern int race_update_cameras();
/*00434bb0*/ extern int get_event_type();
/*0044c560*/ extern int menu_set_car_8c7460_and_reset();
/*0044c590*/ extern int camera_get_car_8c7460();
/*00451c50*/ extern int camera_records_warmup();
/*0047a100*/ extern int car_reset_state();
/*0047a830*/ extern int car_is_active();
/*006037c0*/ extern undefined4 g_phase_timer;
/*008d8b14*/ extern undefined4 g_race_setup;
/* race_phase_3_update */

/* WARNING: Unknown calling convention */
/* phase 3: counts g_phase_timer down from 100000 (x 0x44c / 60 per tick) then enters phase 4
   (12000) */

void race_phase_3_update_n(int in_ebx)

{
  int car_active;
  int event_type;
  int iVar1;
  
  if (g_phase_timer == 100000) {
    camera_records_warmup(0,0);
    menu_set_car_8c7460_and_reset(1);
    race_spawn_cars();
    race_update_cameras(1);
  }
  iVar1 = 0;
  event_type = 0xb8;
  do {
    if (*(int *)(event_type + g_race_setup) != 0) {
      car_active = car_is_active(iVar1);
      if (car_active == 1) {
        car_reset_state(iVar1);
      }
    }
    event_type = event_type + 4;
    iVar1 = iVar1 + 1;
  } while (event_type < 200);
  g_phase_timer = g_phase_timer - (uint)(in_ebx * 0x44c) / 0x3c;
  event_type = camera_path_update(in_ebx);
  if (event_type == 0) {
    event_type = camera_get_car_8c7460();
    if (event_type == 0) {
      event_type = get_event_type();
      if (event_type == EVENT_TRAINING) {
        race_set_phase(RACE_PHASE_RACING);
        g_phase_timer = 0;
        return;
      }
      race_set_phase(RACE_PHASE_INTRO);
      g_phase_timer = 12000;
    }
  }
  return;
}

/* entry thunk at the original address of race_phase_3_update: register inputs (ebx) -> natural C race_phase_3_update_n; ret 0 */
__declspec(naked) void race_phase_3_update(void)
{
    __asm {
        push ebx
        call race_phase_3_update_n
        add esp, 4
        ret
    }
}
