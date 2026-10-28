// cluster-entries: sound_load_course_audio
/* Installed cluster sound_load_course_audio (tools/cluster_install.py from output/cluster_pilot_uc3/sound_load_course_audio.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* compile: /GS */
/* DECLS-BEGIN */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*006ad1f8*/ extern undefined4 g_sound_6ad1f8;
/*006ad8a4*/ extern undefined4 g_sound_6ad8a4;
/*006ae098*/ extern undefined4 g_sound_6ae098;
/*006ae0bc*/ extern undefined4 g_sound_6ae0bc;
/*008bd7b4*/ extern undefined4 g_sound_8bd7b4;
/*006ad8a0*/ extern undefined4 g_sound_load_6ad8a0;
/* DECLS-END */
extern struct VoiceSlot g_voices[];
extern RwBool RwStreamClose(struct RwStream *stream, void *data);
extern int sprintf(char *buf, const char *fmt, ...);
extern char *strcat(char *dst, const char *src);
extern int course_table_find_entry(int course);
extern int FUN_005b82b0(int a, void *stream, int b, int c, int d, int e);
extern int FUN_005b8000(int bank);
extern int FUN_005b7fb0(int bank, int group);
extern int sound_voice_set_float_param(int voice, int what, int a, int b);
extern int FUN_005b6a80(int voice, int v);
extern int sound_course_audio_init(char *path, int course);
extern int sound_spline_create_a(void);
extern int sound_spline_create_b(void);
extern int FID_conflict__wprintf(const char *fmt, ...);
extern int g_sound_groups, g_sound_7dabec;
extern void *g_sound_7dabe8;
extern unsigned int g_sound_6ad088[];
extern int PTR_s_skid_on_wet_tarmac_006229a0[];
#define VOICE_FLAG_LOOP (4) /* set when the group loop flag (+0x88) is non-zero (inferred) */
#define SOUND_GROUP_FIRST_VOICE (0x80) /* group record: index of its first voice slot */
#define SOUND_GROUP_VOICE_COUNT (0x84)
#define SOUND_GROUP_LOOP (0x88)
#define SOUND_GROUP_VOLUME (0x8c) /* copied to voice +0x74 and sound param 3 (inferred: volume) */
#define SOUND_BANK_CHUNK_ID (0x809) /* RenderWare stream chunk holding one sound bank (inferred) */

/* Loads the audio bank of a course ("toastaudio\pc\audio\pcdics\<course>.rws"): opens the stream, reads the sound chunks into the bank, creates the voices of the sound groups
   (loop flag, volume) and starts the course specific audio. */
void sound_load_course_audio(int course_id)
{
  char path[128] = {0};
  char name[16];
  int *stream_buffer;
  unsigned int i;
  unsigned int group_index;
  unsigned int voice;
  unsigned int total;
  int group_rec;
  void *stream;

  FID_conflict__wprintf("loading course\n");
  sprintf(path, "toastaudio\\pc\\audio\\pcdics\\");
  sprintf(name, "");
  strcat(name, (char *)course_table_find_entry(course_id));
  strcat(path, (char *)course_table_find_entry(course_id));
  g_sound_groups = PTR_s_skid_on_wet_tarmac_006229a0[course_id];
  strcat(path, ".rws");
  FID_conflict__wprintf("loading courseA\n");
  stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
  if (stream != 0) {
    FID_conflict__wprintf("loading courseA %s\n", path);
    stream_buffer = (int *)(*(void *(**)(unsigned int, unsigned int, int))((char *)RwEngineInstance + 0x114))(0x10001, 1, 0);
    FID_conflict__wprintf("loading courseB\n");
    FID_conflict__wprintf("loading courseC\n");
    if (stream_buffer == 0) {
      g_sound_7dabe8 = 0;
      g_sound_7dabec = 0;
    }
    else {
      g_sound_7dabec = 0x10000;
      g_sound_7dabe8 = stream_buffer;
    }
    FID_conflict__wprintf("loading courseD\n");
    g_sound_6ad1f8 = 0;
    while (RwStreamFindChunk(stream, SOUND_BANK_CHUNK_ID, 0, 0) != 0) {
      FID_conflict__wprintf("loading courseDA\n");
      g_sound_6ad1f8 = FUN_005b82b0(0, stream, g_sound_6ae0bc, 0, 0, 0);
      FID_conflict__wprintf("loading courseDB\n");
    }
    FID_conflict__wprintf("loading courseE\n");
    (*(void (**)(void *))((char *)RwEngineInstance + 0x10c))(stream_buffer);
    FID_conflict__wprintf("loading courseF\n");
    RwStreamClose(stream, 0);
    FID_conflict__wprintf("loading courseG\n");
    g_sound_6ae098 = FUN_005b8000(g_sound_6ad1f8);
    for (i = 0; i < (unsigned int)g_sound_6ae098; i++) {
      g_sound_6ad088[i] = FUN_005b7fb0(g_sound_6ad1f8, g_sound_groups + i * 0x90);
    }
    total = 0;
    g_sound_load_6ad8a0 = 0;
    if (g_sound_6ae098 != 0) {
      group_index = g_sound_6ae098;
      group_rec = g_sound_groups + 0x84;
      do {
        if (*(unsigned int *)group_rec != 0) {
          total = total + *(unsigned int *)group_rec;
        }
        group_rec = group_rec + 0x90;
        group_index = group_index - 1;
        g_sound_load_6ad8a0 = total;
      } while (group_index != 0);
    }
    for (voice = g_sound_6ad8a4; voice < g_sound_6ad8a4 + g_sound_load_6ad8a0; voice++) {
      *(unsigned int *)(g_voices[voice].voice + VOICE_OFF_FLAGS) |= VOICE_FLAG_LOOP;
      if (*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE) != 0) {
        sound_voice_set_float_param(*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE), 8, 1, 1);
      }
      *(int *)(g_voices[voice].voice + 0x44) = 0;
      if (*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE) != 0) {
        sound_voice_set_float_param(*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE), 10, 1, 0);
      }
    }
    voice = g_sound_6ad8a4;
    for (group_index = 0; group_index < (unsigned int)g_sound_6ae098; group_index++) {
      group_rec = g_sound_groups + group_index * 0x90;
      *(unsigned int *)(group_rec + SOUND_GROUP_FIRST_VOICE) = voice;
      for (i = 0; i < *(unsigned int *)(group_rec + SOUND_GROUP_VOICE_COUNT); i++) {
        FUN_005b6a80(g_voices[voice].voice, g_sound_6ad088[group_index]);
        if (*(int *)(group_rec + SOUND_GROUP_LOOP) == 0) {
          *(unsigned int *)(g_voices[voice].voice + VOICE_OFF_FLAGS) &= ~VOICE_FLAG_LOOP;
          if (*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE) != 0) {
            sound_voice_set_float_param(*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE), 8, 1, 0);
          }
        }
        else {
          *(unsigned int *)(g_voices[voice].voice + VOICE_OFF_FLAGS) |= VOICE_FLAG_LOOP;
          if (*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE) != 0) {
            sound_voice_set_float_param(*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE), 8, 1, 1);
          }
        }
        *(int *)(g_voices[voice].voice + 0x74) = *(int *)(group_rec + SOUND_GROUP_VOLUME);
        if (*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE) != 0) {
          sound_voice_set_float_param(*(int *)(g_voices[voice].voice + VOICE_OFF_HANDLE), 3, 1, *(int *)(group_rec + SOUND_GROUP_VOLUME));
        }
        voice++;
      }
    }
    g_sound_8bd7b4 = voice;
    sound_course_audio_init(name, course_id);
    if (course_id == 0) {
      sound_spline_create_b();
    }
    else if (course_id == 0x24) {
      sound_spline_create_a();
    }
  }
}

