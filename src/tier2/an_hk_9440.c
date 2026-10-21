/* compile: /GS */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*004dc370*/ extern RwStream *RwStreamOpen(RwStreamType type, RwStreamAccessType access, const void *data);
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *length, RwUInt32 *version);
/*004dc2a0*/ extern RwBool RwStreamClose(RwStream *stream, void *data);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(const char *format, ...);
/* audio library (RenderWare Audio, names unknown) */
/*005b82b0*/ extern int FUN_005b82b0(int p0, RwStream *stream, int device, int p3, int p4, int p5);
/*005b8000*/ extern uint FUN_005b8000(int dictionary);
/*005b7fb0*/ extern int FUN_005b7fb0(int dictionary, const char *wave_name);
/*005c98e0*/ extern int FUN_005c98e0(int device, int p1, int p2, int p3, int p4);
/*005b7650*/ extern void FUN_005b7650(int device, int *channels, uint count);
/*005b68e0*/ extern uint FUN_005b68e0(int p0, int p1, int p2, int p3, int p4);
/*005b6a80*/ extern void FUN_005b6a80(uint voice, int wave);
/*005b7300*/ extern void FUN_005b7300(int sound_object, int param, int enable, int value);

/* One permanent wave of PERMDICT.RWS (0x94 bytes, table at 0x618778). */
typedef struct PermWave {
  char name[0x80];
  uint first_voice;     /* +0x80: first voice slot, filled in here */
  uint voice_count;     /* +0x84 */
  int looping;          /* +0x88: voice flag 4 and sound parameter 8 */
  int param3;           /* +0x8c: voice +0x74 and sound parameter 3 */
  int unknown_90;
} PermWave;

/*00618778*/ extern PermWave g_sound_load_618778[];      /* permanent waves */
/*006ad118*/ extern int g_sound_6ad118[];                /* wave handle per permanent wave */
/*006ad8f0*/ extern int g_sound_6ad8f0[];                /* channels created on the device */
/*006ad210*/ extern VoiceSlot g_voices[];
/*006ad8a4*/ extern uint g_sound_6ad8a4;                 /* permanent voice count */
/*006ad8a8*/ extern uint g_sound_6ad8a8;                 /* permanent wave count */
/*006ae09c*/ extern int g_sound_6ae09c;                  /* wave dictionary */
/*006ae0bc*/ extern int g_sound_6ae0bc;                  /* audio device */
/*006ae0c0*/ extern uint g_sound_6ae0c0;                 /* channel count */
/*007dabe8*/ extern void *g_sound_7dabe8;                /* stream read buffer */
/*007dabec*/ extern int g_sound_7dabec;
/*008bd7b4*/ extern uint g_sound_8bd7b4;

typedef void *(*RwCallocFunc)(size_t, size_t, RwUInt32);
#define RwCalloc RW_GLOBAL(0x114, RwCallocFunc)      /* memoryFuncs.rwcalloc */
#define VOICE_COUNT 210
#define VOICE_FIELD(slot, off) (*(int *)(*(char **)&g_voices[slot].voice + (off)))

/* Loads the permanent wave dictionary PERMDICT.RWS, creates the device channels and all voices, then assigns
   voice_count consecutive voices to every permanent wave (wave, loop flag, parameter 3). */
void sound_load_perm_dict(void)
{
  char unused[16];      /* the /GS cookie of the original: a local buffer the release build optimised away */
  RwStream *stream;
  void *buffer;
  uint i;
  uint voice;
  uint wave;
  uint k;

  stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "toastaudio\\pc\\audio\\pcdics\\PERMDICT.RWS");
  buffer = RwCalloc(0x10001, 1, 0);
  if (buffer != 0) {
    g_sound_7dabe8 = buffer;
    g_sound_7dabec = 0x10000;
  } else {
    g_sound_7dabe8 = 0;
    g_sound_7dabec = 0;
  }
  g_sound_6ae09c = 0;
  while (RwStreamFindChunk(stream, 0x809, 0, 0)) {
    g_sound_6ae09c = FUN_005b82b0(0, stream, g_sound_6ae0bc, 0, 0, 0);
  }
  RwFree(buffer);
  RwStreamClose(stream, 0);
  g_sound_6ad8a8 = FUN_005b8000(g_sound_6ae09c);
  FID_conflict__wprintf("numpermwaves %d\n", g_sound_6ad8a8);
  for (i = 0; i < g_sound_6ad8a8; i++) {
    g_sound_6ad118[i] = FUN_005b7fb0(g_sound_6ae09c, g_sound_load_618778[i].name);
  }
  for (i = 0; i < g_sound_6ae0c0; i++) {
    g_sound_6ad8f0[i] = FUN_005c98e0(g_sound_6ae0bc, 0, 2, 0, 0);
  }
  FUN_005b7650(g_sound_6ae0bc, g_sound_6ad8f0, g_sound_6ae0c0);

  g_sound_6ad8a4 = 0;
  for (i = 0; i < g_sound_6ad8a8; i++) {
    if (g_sound_load_618778[i].voice_count > 0) g_sound_6ad8a4 += g_sound_load_618778[i].voice_count;
  }
  for (i = 0; i < VOICE_COUNT; i++) {
    g_voices[i].voice = FUN_005b68e0(0, 0, 0, 0, 0);
  }
  for (i = 0; i < g_sound_6ad8a4; i++) {
    VOICE_FIELD(i, 0x34) |= 4;
    if (VOICE_FIELD(i, 0x30) != 0) FUN_005b7300(VOICE_FIELD(i, 0x30), 8, 1, 1);
    VOICE_FIELD(i, 0x44) = 0;
    if (VOICE_FIELD(i, 0x30) != 0) FUN_005b7300(VOICE_FIELD(i, 0x30), 10, 1, 0);
  }

  voice = 0;
  for (wave = 0; wave < g_sound_6ad8a8; wave++) {
    PermWave *w = &g_sound_load_618778[wave];
    w->first_voice = voice;
    for (k = 0; k < w->voice_count; k++) {
      FUN_005b6a80(g_voices[voice].voice, g_sound_6ad118[wave]);
      if (w->looping == 0) {
        VOICE_FIELD(voice, 0x34) &= ~4;
        if (VOICE_FIELD(voice, 0x30) != 0) FUN_005b7300(VOICE_FIELD(voice, 0x30), 8, 1, 0);
      } else {
        VOICE_FIELD(voice, 0x34) |= 4;
        if (VOICE_FIELD(voice, 0x30) != 0) FUN_005b7300(VOICE_FIELD(voice, 0x30), 8, 1, 1);
      }
      VOICE_FIELD(voice, 0x74) = w->param3;
      if (VOICE_FIELD(voice, 0x30) != 0) FUN_005b7300(VOICE_FIELD(voice, 0x30), 3, 1, w->param3);
      voice++;
    }
  }
  g_sound_6ad8a4 = voice;
  g_sound_8bd7b4 = voice;
  FID_conflict__wprintf("numpermvirtualvoices %d\n", voice);
}
