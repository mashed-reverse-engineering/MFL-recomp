#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };

#define VOICE_POS_Y_OFS (0x5c)
#define VOICE_POS_Z_OFS (0x60)


