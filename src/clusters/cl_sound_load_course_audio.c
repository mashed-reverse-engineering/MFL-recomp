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
extern int FUN_005b7300(int voice, int what, int a, int b);
extern int FUN_005b6a80(int voice, int v);
extern int sound_course_audio_init(char *path, int course);
extern int sound_spline_create_a(void);
extern int sound_spline_create_b(void);
extern int FID_conflict__wprintf(const char *fmt, ...);
extern int g_sound_groups, g_sound_7dabec;
extern void *g_sound_7dabe8;
extern unsigned int g_sound_6ad088[];
extern int PTR_s_skid_on_wet_tarmac_006229a0[];

/* Loads the audio bank of a course ("toastaudio\pc\audio\pcdics\<course>.rws"): opens the stream, reads the sound chunks into the bank, creates the voices of the sound groups
   (loop flag, volume) and starts the course specific audio. */
void sound_load_course_audio(int course_id)
{
  char path[128] = {0};
  char name[16];
  int *mem;
  unsigned int i;
  unsigned int n;
  unsigned int voice;
  unsigned int total;
  int offs;
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
    mem = (int *)(*(void *(**)(unsigned int, unsigned int, int))((char *)RwEngineInstance + 0x114))(0x10001, 1, 0);
    FID_conflict__wprintf("loading courseB\n");
    FID_conflict__wprintf("loading courseC\n");
    if (mem == 0) {
      g_sound_7dabe8 = 0;
      g_sound_7dabec = 0;
    }
    else {
      g_sound_7dabec = 0x10000;
      g_sound_7dabe8 = mem;
    }
    FID_conflict__wprintf("loading courseD\n");
    g_sound_6ad1f8 = 0;
    while (RwStreamFindChunk(stream, 0x809, 0, 0) != 0) {
      FID_conflict__wprintf("loading courseDA\n");
      g_sound_6ad1f8 = FUN_005b82b0(0, stream, g_sound_6ae0bc, 0, 0, 0);
      FID_conflict__wprintf("loading courseDB\n");
    }
    FID_conflict__wprintf("loading courseE\n");
    (*(void (**)(void *))((char *)RwEngineInstance + 0x10c))(mem);
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
      n = g_sound_6ae098;
      offs = g_sound_groups + 0x84;
      do {
        if (*(unsigned int *)offs != 0) {
          total = total + *(unsigned int *)offs;
        }
        offs = offs + 0x90;
        n = n - 1;
        g_sound_load_6ad8a0 = total;
      } while (n != 0);
    }
    for (voice = g_sound_6ad8a4; voice < g_sound_6ad8a4 + g_sound_load_6ad8a0; voice++) {
      *(unsigned int *)(g_voices[voice].voice + 0x34) |= 4;
      if (*(int *)(g_voices[voice].voice + 0x30) != 0) {
        FUN_005b7300(*(int *)(g_voices[voice].voice + 0x30), 8, 1, 1);
      }
      *(int *)(g_voices[voice].voice + 0x44) = 0;
      if (*(int *)(g_voices[voice].voice + 0x30) != 0) {
        FUN_005b7300(*(int *)(g_voices[voice].voice + 0x30), 10, 1, 0);
      }
    }
    voice = g_sound_6ad8a4;
    for (n = 0; n < (unsigned int)g_sound_6ae098; n++) {
      offs = g_sound_groups + n * 0x90;
      *(unsigned int *)(offs + 0x80) = voice;
      for (i = 0; i < *(unsigned int *)(offs + 0x84); i++) {
        FUN_005b6a80(g_voices[voice].voice, g_sound_6ad088[n]);
        if (*(int *)(offs + 0x88) == 0) {
          *(unsigned int *)(g_voices[voice].voice + 0x34) &= 0xfffffffb;
          if (*(int *)(g_voices[voice].voice + 0x30) != 0) {
            FUN_005b7300(*(int *)(g_voices[voice].voice + 0x30), 8, 1, 0);
          }
        }
        else {
          *(unsigned int *)(g_voices[voice].voice + 0x34) |= 4;
          if (*(int *)(g_voices[voice].voice + 0x30) != 0) {
            FUN_005b7300(*(int *)(g_voices[voice].voice + 0x30), 8, 1, 1);
          }
        }
        *(int *)(g_voices[voice].voice + 0x74) = *(int *)(offs + 0x8c);
        if (*(int *)(g_voices[voice].voice + 0x30) != 0) {
          FUN_005b7300(*(int *)(g_voices[voice].voice + 0x30), 3, 1, *(int *)(offs + 0x8c));
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

