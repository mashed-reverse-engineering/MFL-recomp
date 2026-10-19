#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*006187f8*/ extern undefined4 g_sfx_table;

/* voice record offsets, in bytes from the voice pointer in g_voices[].voice */
#define VOICE_OFF_FREQ (0x3c)
/* bit in the voice flags: set = voice busy, sfx not started (inferred) */
/* voice slots per g_sfx_table entry (inferred) */
/* float bit pattern 22050.0f (playback frequency) stored at VOICE_OFF_FREQ and passed as p3 (inferred) */


