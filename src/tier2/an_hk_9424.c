#include "mfl_tier2.h"
/*004687c0*/ extern void __cdecl voice_queue_slot_release(int kind);
/*00469a60*/ extern void __cdecl voice_set_two_params(int index, int zero, float a, float b);
/*0046eee0*/ extern int sound_streams_are_idle(void);
/*005b8a40*/ extern void FUN_005b8a40(int handle, void *info);
/*005b8c40*/ extern int sound_stream_state(int handle);
/*005e64a4*/ extern float k_f_0;
/*0061794c*/ extern float g_sound_frame_61794c;
/*00617954*/ extern float g_sound_617954;
/*00629260*/ extern float g_sound_frame_629260[];
/*006ae07c*/ extern int g_sound_stream_handles[];
/*006ae0e4*/ extern int g_sound_6ae0e4[];
/*008be7c0*/ extern struct SoundStreamSlot *g_sound_stream_slots[6];
/*008d9940*/ extern int g_sound_8d9940[];
typedef struct SoundStreamSlot {
  signed char voice;
  char pad0[3];
  int stream;           /* index into g_sound_stream_handles */
  int unused8;
  int state;            /* 1 = playing */
  int unused10[2];
  float position;
  float remaining;
  int unused20;
  int kind;             /* 1..4: volume scale class */
} SoundStreamSlot;
typedef struct StreamInfo {
  int unused;
  int mode;
  float length;
  int flag;
} StreamInfo;
/* voice_queue_play_pending */
/* "starts the pending voice of the queue when the streams are idle (voice_set_two_params), then
   releases its slot" */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
void voice_queue_play_pending(void)
{
  SoundStreamSlot *slot;
  int i;
  int stream;
  int voice;
  int any_started;
  float volume;
  StreamInfo info;

  if (sound_streams_are_idle() != 0) {
    return;
  }
  any_started = 0;
  for (i = 0; i < 6; i++) {
    slot = g_sound_stream_slots[i];
    if (slot != 0) {
      voice = slot->voice;
      stream = slot->stream;
      if (slot->state == 1) {
        switch (slot->kind) {
        case 1:
          volume = 0.7f;
          break;
        case 2:
          volume = 0.6f;
          break;
        case 3:
          volume = 0.5f;
          break;
        case 4:
          volume = 0.3f;
          break;
        default:
          volume = k_f_0;
          break;
        }
        volume = volume * g_sound_frame_61794c;
        g_sound_frame_629260[voice] = 1.0f;
        if (volume < k_f_0) {
          volume = 0.0f;
        }
        if (sound_stream_state(g_sound_stream_handles[stream]) == 6) {
          voice_set_two_params(stream, 0, volume, volume);
          any_started = 1;
        }
        info.flag = 1;
        info.mode = 3;
        FUN_005b8a40(g_sound_stream_handles[stream], &info);
        if (sound_stream_state(g_sound_stream_handles[stream]) == 3 && g_sound_6ae0e4[i] == 6) {
          g_sound_stream_slots[i]->state = 0;
          g_sound_8d9940[i] = 0;
          voice_queue_slot_release(g_sound_stream_slots[i]->kind);
        }
        g_sound_6ae0e4[i] = sound_stream_state(g_sound_stream_handles[stream]);
        g_sound_stream_slots[i]->remaining = g_sound_stream_slots[i]->position - info.length;
      }
    }
  }
  g_sound_617954 = 0.65f;
  if (!any_started) {
    g_sound_617954 = 1.0f;
  }
}
