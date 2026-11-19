#include "mfl_tier2.h"
/*008be7c0*/ extern undefined4 g_sound_stream_slots;


/* sound_find_active_voice_for_slot */

/* "searches the 4 voice records for an active one with matching id and slot" */

undefined4 __cdecl sound_find_active_voice_for_slot(int voice_id,char *slot_record)

{
  int voice;
  int voice_index;

  for (voice_index = 0; voice_index < 4; voice_index = voice_index + 1) {
    voice = (&g_sound_stream_slots)[voice_index];
    if ((voice != 0) && (*(int *)(voice + 0xc) == 1) &&
       (*(int *)(voice + 0x28) == voice_id) && (*(int *)(slot_record + 4) == voice_index)) {
      return 1;
    }
  }
  return 0;
}
