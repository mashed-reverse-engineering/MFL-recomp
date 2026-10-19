#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0044a4f0*/ extern int camera_get_position();
/*004590f0*/ extern int course_c26_stub_return_0();
/*00459100*/ extern int course_object_get_position();
/*00459130*/ extern int __cdecl course_object_is_inactive(undefined4 p0, undefined4 p1, undefined4 p2);
/*00459160*/ extern double course_object_get_float_16c();
/*00459180*/ extern double course_c26_checkpoint_scale_value();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*00481fd0*/ extern double random_range();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dd620*/ extern float float_10_0;
/*005ddb9c*/ extern float k_f_n10;
/*005ddc28*/ extern float k_f_0;
/*005df458*/ extern float k_f_200;
/*005df99c*/ extern float k_f_22050;
/*005df9c8*/ extern float k_f_112_1;
/*005df9cc*/ extern float k_f_15050;
/*005df9d0*/ extern float k_f_15000;
/*006ad07c*/ extern undefined4 g_sound_ambience_6ad07c;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*006ad1fc*/ extern float g_sound_ambience_6ad1fc;
/*006ad8e0*/ extern undefined4 g_sound_ambience_6ad8e0;
/*006ad8e4*/ extern undefined4 g_sound_ambience_6ad8e4;
/*006ad8e8*/ extern float g_sound_ambience_6ad8e8;
/*006ae0ac*/ extern float g_sound_ambience_6ae0ac;
/*008bd7b0*/ extern char *g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;

/* voice record layout (byte offsets into the record g_voices[i].voice points to; not in ghidra_types.h) */
#define VOICE_REC_HANDLE (0x30) /* secondary handle passed to FUN_005b7300 / FUN_005b72a0 */
#define VOICE_REC_FLAGS (0x34) /* byte: bit VOICE_FLAG_PLAYING set once the voice has been started */
#define VOICE_FLAG_PLAYING (2)
#define VOICE_REC_FREQ (0x3c) /* float: playback frequency in Hz (default 22050.0f; inferred) */
#define VOICE_REC_POS_X (0x58) /* float: voice position (inferred) */
#define VOICE_REC_POS_Y (0x5c)
#define VOICE_REC_POS_Z (0x60)

/* values written to the voice frequency and position, as raw float bit patterns (undefined4 stores) */
#define VOICE_FREQ_DEFAULT (0x46ac4400) /* 22050.0f */
#define VOICE_FREQ_DEFAULT_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */
#define FREQ_JITTER_A_MAX (0x447a0000) /* 1000.0f: random_range upper bound added to the frequency */
#define FREQ_JITTER_B_MAX (0x453b8000) /* 3000.0f: random_range upper bound added to the frequency */
#define OFFSET_A_X (0x3f333333) /* 0.7f: voice position offset set A (inferred) */
#define OFFSET_A_Y (0x40733333) /* 3.8f */
#define OFFSET_A_Z (0xc2e03333) /* -112.1f */
#define OFFSET_B_X (0xbe19999a) /* -0.15f: voice position offset set B (inferred) */
#define OFFSET_B_Y (0x3fd9999a) /* 1.7f */
#define OFFSET_B_Z (0x4309199a) /* 137.1f */

/* per-frame ambience roll (inferred) */
#define AMBIENT_SLOT_COUNT (8) /* voices per group / max active emitters (inferred) */
#define AMBIENT_ROLL_MAX (0x14) /* upper bound of the random roll */
#define AMBIENT_ROLL_HIT (10) /* roll value that triggers the extra ambient voice */

/* sound_ambience_course_c_26 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "course ids 0x0C and 0x26 (course object positions)" */

/*
 * Per-frame ambience sound update for course object positions (inferred).
 * Does nothing when multiplayer mute is on and 2+ players are present.
 * Otherwise resets the voice volumes, then for each course object within
 * 10 units of the camera's height (inferred) starts up to AMBIENT_SLOT_COUNT
 * ambient voices at that object's position, and places the fixed emitters.
 * Takes no parameters and returns nothing.
 */
void sound_ambience_course_c_26(void)

{
  int aux_handle; /* voice record secondary handle (+VOICE_REC_HANDLE) */
  char *voice_rec; /* address of the current voice record */
  float fdist; /* reused: object z minus camera z, then voice frequency (inferred) */
  int obj_count; /* reused: course object count, then voice index */
  undefined4 call_result; /* return value of the voice calls (unused) */
  int emitter_idx; /* reused: emitter index, then voice index */
  int obj_index; /* course object index, or slot index in the 8-slot loop */
  int voice_idx; /* index into g_voices */
  float10 ret_f10; /* float result of a callee, kept at extended precision */
  undefined4 one_f_bits; /* bit pattern of 1.0f */
  struct {
    undefined4 pos_x; /* scratch voice position (x, y, z) */
    undefined4 pos_y;
    float pos_z;
    undefined1 camera_xy [8]; /* camera position, x and y (Ghidra split, inferred) */
    float camera_z; /* camera position, z (inferred) */
  } frame;
  
  /* skip the whole update when multiplayer mute is on and 2+ players are present */
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) {
    emitter_idx = 0;
    obj_count = course_c26_stub_return_0();
    call_result = camera_get_position(frame.camera_xy);
    obj_index = 0;
    /* silence the first AMBIENT_SLOT_COUNT voices of groups +0x470 and +0x500 */
    do {
      voice_set_volume(*(int *)(g_sound_groups + 0x470) + obj_index,0.0);
      call_result = voice_set_volume(*(int *)(g_sound_groups + 0x500) + obj_index,0.0);
      obj_index = obj_index + 1;
    } while (obj_index < AMBIENT_SLOT_COUNT);
    /* for each course object near the camera's height, start its ambient voices */
    if (0 < obj_count) {
      obj_index = 0;
      do {
        course_object_get_position(obj_index,&g_sound_ambience_6ad8e0);
        fdist = g_sound_ambience_6ad8e8 - frame.camera_z;
        if (fdist <= k_f_0) {
          if (k_f_n10 < fdist) goto LAB_0046ddf9;
        }
        else if (fdist < float_10_0) {
LAB_0046ddf9:
          /* at most AMBIENT_SLOT_COUNT emitters per frame */
          if (emitter_idx < AMBIENT_SLOT_COUNT) {
            ret_f10 = (float10)course_object_get_float_16c(obj_index);
            g_sound_ambience_6ae0ac = (float)ret_f10;
            ret_f10 = (float10)course_c26_checkpoint_scale_value(obj_index);
            g_sound_ambience_6ad1fc = (float)ret_f10;
            voice_idx = *(int *)(g_sound_groups + 0x470) + emitter_idx;
            if ((*(byte *)(g_voices[voice_idx].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
              FUN_005b6a40(g_voices[voice_idx].voice,1);
            }
            fdist = g_sound_ambience_6ad1fc * g_sound_ambience_6ae0ac * k_f_15000 + k_f_22050;
            *(float *)(g_voices[voice_idx].voice + VOICE_REC_FREQ) = fdist;
            aux_handle = *(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE);
            if (aux_handle != 0) {
              FUN_005b7300(aux_handle,4,1,fdist);
            }
            voice_rec = g_voices[voice_idx].voice;
            *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = g_sound_ambience_6ad8e0;
            *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = g_sound_ambience_6ad8e4;
            *(float *)(voice_rec + VOICE_REC_POS_Z) = g_sound_ambience_6ad8e8;
            aux_handle = *(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE);
            call_result = 0;
            if (aux_handle != 0) {
              call_result = FUN_005b72a0(aux_handle,0,1,&g_sound_ambience_6ad8e0);
            }
            one_f_bits = 0x3f800000;
            voice_set_volume(voice_idx,1.0);
            voice_idx = course_object_is_inactive(obj_index,voice_idx,one_f_bits);
            if (voice_idx != 0) {
              voice_idx = *(int *)(g_sound_groups + 0x620);
              if ((*(byte *)(g_voices[voice_idx].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
                FUN_005b6a40(g_voices[voice_idx].voice,1);
              }
              *(undefined4 *)(g_voices[voice_idx].voice + VOICE_REC_FREQ) = VOICE_FREQ_DEFAULT;
              aux_handle = *(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE);
              if (aux_handle != 0) {
                FUN_005b7300(aux_handle,4,1,VOICE_FREQ_DEFAULT_F);
              }
              voice_rec = g_voices[voice_idx].voice;
              *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = g_sound_ambience_6ad8e0;
              *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = g_sound_ambience_6ad8e4;
              *(float *)(voice_rec + VOICE_REC_POS_Z) = g_sound_ambience_6ad8e8;
              aux_handle = *(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE);
              call_result = 0;
              if (aux_handle != 0) {
                call_result = FUN_005b72a0(aux_handle,0,1,&g_sound_ambience_6ad8e0);
              }
              voice_set_volume(voice_idx,1.0);
              /* occasional extra voice (1 in 2 roll), only while the flag is clear (inferred) */
              if ((g_sound_ambience_6ad07c == 0) && (voice_idx = rp_random_int_range(0,1), voice_idx == 1)) {
                voice_idx = rp_random_int_range(0,2);
                voice_idx = *(int *)(voice_idx * 0x90 + 0x6b0 + g_sound_groups);
                if ((*(byte *)(g_voices[voice_idx].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
                  FUN_005b6a40(g_voices[voice_idx].voice,1);
                }
                ret_f10 = (float10)random_range(0,FREQ_JITTER_A_MAX);
                *(float *)(g_voices[voice_idx].voice + VOICE_REC_FREQ) = (float)(ret_f10 + (float10)k_f_22050);
                if (*(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE) != 0) {
                  ret_f10 = (float10)random_range(0,FREQ_JITTER_A_MAX);
                  FUN_005b7300(*(undefined4 *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE),4,1,
                               (float)(ret_f10 + (float10)k_f_22050));
                }
                voice_rec = g_voices[voice_idx].voice;
                *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = g_sound_ambience_6ad8e0;
                *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = g_sound_ambience_6ad8e4;
                *(float *)(voice_rec + VOICE_REC_POS_Z) = g_sound_ambience_6ad8e8;
                aux_handle = *(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE);
                call_result = 0;
                if (aux_handle != 0) {
                  call_result = FUN_005b72a0(aux_handle,0,1,&g_sound_ambience_6ad8e0);
                }
                voice_set_volume(voice_idx,1.0);
              }
              voice_idx = *(int *)(g_sound_groups + 0x500) + emitter_idx;
              if ((*(byte *)(g_voices[voice_idx].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
                FUN_005b6a40(g_voices[voice_idx].voice,1);
              }
              ret_f10 = (float10)random_range(0,FREQ_JITTER_B_MAX);
              *(float *)(g_voices[voice_idx].voice + VOICE_REC_FREQ) = (float)(ret_f10 + (float10)k_f_15050);
              if (*(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE) != 0) {
                ret_f10 = (float10)random_range(0,FREQ_JITTER_B_MAX);
                FUN_005b7300(*(undefined4 *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE),4,1,
                             (float)(ret_f10 + (float10)k_f_15050));
              }
              voice_rec = g_voices[voice_idx].voice;
              *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = g_sound_ambience_6ad8e0;
              *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = g_sound_ambience_6ad8e4;
              *(float *)(voice_rec + VOICE_REC_POS_Z) = g_sound_ambience_6ad8e8;
              aux_handle = *(int *)(g_voices[voice_idx].voice + VOICE_REC_HANDLE);
              call_result = 0;
              if (aux_handle != 0) {
                call_result = FUN_005b72a0(aux_handle,0,1,&g_sound_ambience_6ad8e0);
              }
              voice_set_volume(voice_idx,1.0);
            }
            emitter_idx = emitter_idx + 1;
          }
        }
        obj_index = obj_index + 1;
      } while (obj_index < obj_count);
    }
    /* recompute the flag from which of the three checked voices are playing */
    g_sound_ambience_6ad07c =
         (uint)((*(byte *)(g_voices[*(int *)(g_sound_groups + 0x620)].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) != 0);
    if ((*(byte *)(g_voices[*(int *)(g_sound_groups + 0x6b0)].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) != 0) {
      g_sound_ambience_6ad07c = g_sound_ambience_6ad07c + 1;
    }
    if ((*(byte *)(g_voices[*(int *)(g_sound_groups + 0x740)].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) != 0) {
      g_sound_ambience_6ad07c = g_sound_ambience_6ad07c + 1;
    }
    /* fixed emitters: two voices each from groups +0x860 and +0x8f0 (next index), then a random pick from +0x980 (inferred) */
    obj_count = *(int *)(g_sound_groups + 0x860);
    if ((*(byte *)(g_voices[obj_count].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
      FUN_005b6a40(g_voices[obj_count].voice,1);
    }
    *(undefined4 *)(g_voices[obj_count].voice + VOICE_REC_FREQ) = VOICE_FREQ_DEFAULT;
    emitter_idx = *(int *)(g_voices[obj_count].voice + VOICE_REC_HANDLE);
    call_result = 0;
    if (emitter_idx != 0) {
      call_result = FUN_005b7300(emitter_idx,4,1,VOICE_FREQ_DEFAULT_F);
    }
    voice_set_volume(obj_count,1.0);
    voice_rec = g_voices[obj_count].voice;
    frame.pos_x = OFFSET_A_X;
    frame.pos_y = OFFSET_A_Y;
    frame.pos_z = -112.1;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = OFFSET_A_X;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = OFFSET_A_Y;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Z) = OFFSET_A_Z;
    emitter_idx = *(int *)(g_voices[obj_count].voice + VOICE_REC_HANDLE);
    if (emitter_idx != 0) {
      FUN_005b72a0(emitter_idx,0,1,&frame.pos_x);
    }
    emitter_idx = obj_count + 1;
    if ((*(byte *)(g_voices[obj_count + 1].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
      FUN_005b6a40(g_voices[obj_count + 1].voice,1);
    }
    *(undefined4 *)(g_voices[emitter_idx].voice + VOICE_REC_FREQ) = VOICE_FREQ_DEFAULT;
    obj_count = *(int *)(g_voices[emitter_idx].voice + VOICE_REC_HANDLE);
    call_result = 0;
    if (obj_count != 0) {
      call_result = FUN_005b7300(obj_count,4,1,VOICE_FREQ_DEFAULT_F);
    }
    voice_set_volume(emitter_idx,1.0);
    voice_rec = g_voices[emitter_idx].voice;
    frame.pos_x = OFFSET_B_X;
    frame.pos_y = OFFSET_B_Y;
    frame.pos_z = 137.1;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = OFFSET_B_X;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = OFFSET_B_Y;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Z) = OFFSET_B_Z;
    obj_count = *(int *)(g_voices[emitter_idx].voice + VOICE_REC_HANDLE);
    if (obj_count != 0) {
      FUN_005b72a0(obj_count,0,1,&frame.pos_x);
    }
    obj_count = *(int *)(g_sound_groups + 0x8f0);
    if ((*(byte *)(g_voices[obj_count].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
      FUN_005b6a40(g_voices[obj_count].voice,1);
    }
    *(undefined4 *)(g_voices[obj_count].voice + VOICE_REC_FREQ) = VOICE_FREQ_DEFAULT;
    emitter_idx = *(int *)(g_voices[obj_count].voice + VOICE_REC_HANDLE);
    call_result = 0;
    if (emitter_idx != 0) {
      call_result = FUN_005b7300(emitter_idx,4,1,VOICE_FREQ_DEFAULT_F);
    }
    voice_set_volume(obj_count,1.0);
    voice_rec = g_voices[obj_count].voice;
    frame.pos_x = OFFSET_A_X;
    frame.pos_y = OFFSET_A_Y;
    frame.pos_z = -112.1;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = OFFSET_A_X;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = OFFSET_A_Y;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Z) = OFFSET_A_Z;
    emitter_idx = *(int *)(g_voices[obj_count].voice + VOICE_REC_HANDLE);
    if (emitter_idx != 0) {
      FUN_005b72a0(emitter_idx,0,1,&frame.pos_x);
    }
    emitter_idx = obj_count + 1;
    if ((*(byte *)(g_voices[obj_count + 1].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
      FUN_005b6a40(g_voices[obj_count + 1].voice,1);
    }
    *(undefined4 *)(g_voices[emitter_idx].voice + VOICE_REC_FREQ) = VOICE_FREQ_DEFAULT;
    obj_count = *(int *)(g_voices[emitter_idx].voice + VOICE_REC_HANDLE);
    call_result = 0;
    if (obj_count != 0) {
      call_result = FUN_005b7300(obj_count,4,1,VOICE_FREQ_DEFAULT_F);
    }
    voice_set_volume(emitter_idx,1.0);
    voice_rec = g_voices[emitter_idx].voice;
    frame.pos_x = OFFSET_B_X;
    frame.pos_y = OFFSET_B_Y;
    frame.pos_z = 137.1;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = OFFSET_B_X;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = OFFSET_B_Y;
    *(undefined4 *)(voice_rec + VOICE_REC_POS_Z) = OFFSET_B_Z;
    obj_count = *(int *)(g_voices[emitter_idx].voice + VOICE_REC_HANDLE);
    if (obj_count != 0) {
      FUN_005b72a0(obj_count,0,1,&frame.pos_x);
    }
    /* per-frame roll; AMBIENT_ROLL_HIT adds the extra ambient voice */
    obj_count = rp_random_int_range(0,AMBIENT_ROLL_MAX);
    if (obj_count == AMBIENT_ROLL_HIT) {
      obj_count = *(int *)(g_sound_groups + 0x980);
      emitter_idx = rp_random_int_range(0,1);
      obj_count = obj_count + emitter_idx;
      if ((*(byte *)(g_voices[obj_count].voice + VOICE_REC_FLAGS) & VOICE_FLAG_PLAYING) == 0) {
        FUN_005b6a40(g_voices[obj_count].voice,1);
      }
      *(undefined4 *)(g_voices[obj_count].voice + VOICE_REC_FREQ) = VOICE_FREQ_DEFAULT;
      emitter_idx = *(int *)(g_voices[obj_count].voice + VOICE_REC_HANDLE);
      call_result = 0;
      if (emitter_idx != 0) {
        call_result = FUN_005b7300(emitter_idx,4,1,VOICE_FREQ_DEFAULT_F);
      }
      voice_set_volume(obj_count,1.0);
      frame.pos_x = OFFSET_A_X;
      frame.pos_y = OFFSET_A_Y;
      emitter_idx = rp_random_int_range(0,1);
      voice_rec = g_voices[obj_count].voice;
      frame.pos_z = (float)emitter_idx * k_f_200 - k_f_112_1;
      *(undefined4 *)(voice_rec + VOICE_REC_POS_X) = frame.pos_x;
      *(undefined4 *)(voice_rec + VOICE_REC_POS_Y) = frame.pos_y;
      *(float *)(voice_rec + VOICE_REC_POS_Z) = frame.pos_z;
      obj_count = *(int *)(g_voices[obj_count].voice + VOICE_REC_HANDLE);
      if (obj_count != 0) {
        FUN_005b72a0(obj_count,0,1,&frame.pos_x);
      }
    }
  }
  return;
}


