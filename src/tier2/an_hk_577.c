#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0044a4f0*/ extern int camera_get_position();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*00555ff0*/ extern struct RwV3d * RpSplineFindPosition(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, struct RwV3d *p4);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dd328*/ extern float k_f_0_6;
/*005dd330*/ extern float float_1_0;
/*005dd36c*/ extern float k_f_0_4;
/*005dd628*/ extern float k_f_0_25;
/*005dd7f0*/ extern float k_f_30;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb40*/ extern float k_f_27;
/*005ddb4c*/ extern float k_f_0_011111111;
/*005ddc28*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de20c*/ extern float k_f_2_5;
/*005dfce8*/ extern float k_f_0_55;
/*005dfcec*/ extern float k_f_n27;
/*005dfcf0*/ extern float k_f_n2_8;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*006ad1e4*/ extern undefined4 g_sound_6ad1e4;
/*006ae0a0*/ extern undefined4 g_sound_ambience_6ae0a0;
/*006ae0a4*/ extern undefined4 g_sound_ambience_6ae0a4;
/*006ae0a8*/ extern undefined4 g_sound_ambience_6ae0a8;
/*006ae1a8*/ extern float g_sound_ambience_6ae1a8;
/*008bd7b0*/ extern undefined4 g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;

/* byte offsets into g_sound_groups that hold voice slot indices (inferred from use) */
#define SOUND_GROUP_FADE_OUT_OFF (0x470)
#define SOUND_GROUP_FADE_IN_OFF (0x590)
#define SOUND_GROUP_SPLINE_OFF (0x500)
/* offsets inside a voice record (inferred from use) */
#define VOICE_FLAGS_OFF (0x34)
#define VOICE_FLAG_SETUP_DONE (2)
#define VOICE_HANDLE_OFF (0x30)
#define VOICE_FREQ_OFF (0x3c)
#define VOICE_POS_X_OFF (0x58)
#define VOICE_POS_Y_OFF (0x5c)
#define VOICE_POS_Z_OFF (0x60)
/* raw IEEE-754 bits of 22050.0f (default playback frequency in Hz, inferred), stored and passed as a DWORD */
#define VOICE_DEFAULT_FREQ_BITS (0x46ac4400)
#define VOICE_DEFAULT_FREQ_BITS_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */
/* x87 status word bit positions used by the emulated compare (C0, C2, C3) */
#define FPU_C0_SHIFT (8)
#define FPU_C2_SHIFT (10)
#define FPU_C3_SHIFT (14)

/* sound_ambience_course_f_24 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "course ids 0x0F and 0x24 (spline based)" */

/*
 * Per-frame ambience update for the spline-based courses (inferred).
 * Takes no parameters and returns nothing. Does nothing when multiplayer
 * muting is on and two or more players are present. Otherwise it sets the
 * volume of two height-driven voices from the camera y (height) and moves
 * the spline ambience voice to a point on g_sound_6ad1e4 chosen from camera x and z.
 */
void sound_ambience_course_f_24(void)

{
  int voice_slot;
  int voice_handle;
  float fade_raw;
  float fade_clamped;
  uint voice_rec;
  undefined4 fpu_status;
  /* camera_get_position fills x, y, z (from the second member on); height_t is scratch */
  struct {
    float height_t;
    float x;
    float y;
    float z;
  } camera_pos;
  undefined2 status_hi;
  
  /* Skip the whole update only when multiplayer muting is on and 2+ players are present */
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) {
    camera_get_position(&camera_pos.x);
    /* Fade-out voice: loud when the camera is low (inferred) */
    voice_slot = *(int *)(g_sound_groups + SOUND_GROUP_FADE_OUT_OFF);
    voice_rec = g_voices[voice_slot].voice;
    /* First use: set up the voice record (flag bit not set yet) (inferred) */
    if ((*(byte *)(voice_rec + VOICE_FLAGS_OFF) & VOICE_FLAG_SETUP_DONE) == 0) {
      FUN_005b6a40(voice_rec,1);
      *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQ_OFF) = VOICE_DEFAULT_FREQ_BITS;
      voice_handle = *(int *)(g_voices[voice_slot].voice + VOICE_HANDLE_OFF);
      voice_rec = 0;
      if (voice_handle != 0) {
        voice_rec = FUN_005b7300(voice_handle,4,1,VOICE_DEFAULT_FREQ_BITS_F);
      }
    }
    /* fade_raw = camera height ratio minus 0.25 (inferred) */
    fade_raw = (camera_pos.y + k_f_2_5) * k_f_0_125 - k_f_0_25;
    /* Ghidra artifact: x87 compare flags packed into fpu_status (never read here) */
    status_hi = (undefined2)(voice_rec >> 0x10);
    fpu_status = CONCAT22(status_hi,(ushort)(fade_raw < k_f_0) << FPU_C0_SHIFT | (ushort)(((fade_raw) != (fade_raw)) || ((k_f_0) != (k_f_0))) << FPU_C2_SHIFT |
                           (ushort)(fade_raw == k_f_0) << FPU_C3_SHIFT);
    fade_clamped = k_f_0;
    if ((fade_raw >= k_f_0) &&
       (fpu_status = CONCAT22(status_hi,(ushort)(fade_raw < float_1_0) << FPU_C0_SHIFT |
                               (ushort)(((fade_raw) != (fade_raw)) || ((float_1_0) != (float_1_0))) << FPU_C2_SHIFT |
                               (ushort)(fade_raw == float_1_0) << FPU_C3_SHIFT), fade_clamped = fade_raw,
       fade_raw >= float_1_0 && (fade_raw == float_1_0) == 0)) {
      fade_clamped = float_1_0;
    }
    /* Fade-out volume is the inverse of the clamped fade_raw */
    voice_set_volume(voice_slot,float_1_0 - fade_clamped);
    /* Fade-in voice: volume = camera height ratio clamped to [0, 1] (inferred) */
    voice_slot = *(int *)(g_sound_groups + SOUND_GROUP_FADE_IN_OFF);
    voice_rec = g_voices[voice_slot].voice;
    if ((*(byte *)(voice_rec + VOICE_FLAGS_OFF) & VOICE_FLAG_SETUP_DONE) == 0) {
      FUN_005b6a40(voice_rec,1);
      *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQ_OFF) = VOICE_DEFAULT_FREQ_BITS;
      voice_handle = *(int *)(g_voices[voice_slot].voice + VOICE_HANDLE_OFF);
      voice_rec = 0;
      if (voice_handle != 0) {
        voice_rec = FUN_005b7300(voice_handle,4,1,VOICE_DEFAULT_FREQ_BITS_F);
      }
    }
    /* height_t: camera height ratio, clamped to [0, 1] in place below */
    camera_pos.height_t = (camera_pos.y + k_f_2_5) * k_f_0_125;
    status_hi = (undefined2)(voice_rec >> 0x10);
    fpu_status = CONCAT22(status_hi,(ushort)(camera_pos.height_t < k_f_0) << FPU_C0_SHIFT |
                           (ushort)(((camera_pos.height_t) != (camera_pos.height_t)) || ((k_f_0) != (k_f_0))) << FPU_C2_SHIFT |
                           (ushort)(camera_pos.height_t == k_f_0) << FPU_C3_SHIFT);
    /* Clamp height_t to [0, 1] */
    if (camera_pos.height_t < k_f_0) {
      camera_pos.height_t = 0.0;
    }
    else {
      fpu_status = CONCAT22(status_hi,(ushort)(camera_pos.height_t < float_1_0) << FPU_C0_SHIFT |
                             (ushort)(((camera_pos.height_t) != (camera_pos.height_t)) || ((float_1_0) != (float_1_0))) << FPU_C2_SHIFT |
                             (ushort)(camera_pos.height_t == float_1_0) << FPU_C3_SHIFT);
      if (camera_pos.height_t >= float_1_0 && (camera_pos.height_t == float_1_0) == 0) {
        camera_pos.height_t = 1.0;
      }
    }
    voice_set_volume(voice_slot,camera_pos.height_t);
    voice_slot = *(int *)(g_sound_groups + SOUND_GROUP_SPLINE_OFF);
    voice_rec = g_voices[voice_slot].voice;
    if ((*(byte *)(voice_rec + VOICE_FLAGS_OFF) & VOICE_FLAG_SETUP_DONE) == 0) {
      FUN_005b6a40(voice_rec,1);
      *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQ_OFF) = VOICE_DEFAULT_FREQ_BITS;
      voice_handle = *(int *)(g_voices[voice_slot].voice + VOICE_HANDLE_OFF);
      voice_rec = 0;
      if (voice_handle != 0) {
        voice_rec = FUN_005b7300(voice_handle,4,1,VOICE_DEFAULT_FREQ_BITS_F);
      }
    }
    /* Spline ambience: only when the spline is loaded */
    if (g_sound_6ad1e4 != (RpSpline *)0x0) {
      /* Spline parameter t: fixed ramp in the x <= -2.8 zone, z-based ramp when z > -27 (inferred).
       Outside both zones t keeps its last value. */
      if (camera_pos.x <= k_f_n2_8) {
        g_sound_ambience_6ae1a8 = (camera_pos.z + k_f_30) * k_f_0_025 * k_f_0_4 + k_f_0_6;
      }
      else if (k_f_n27 < camera_pos.z) {
        g_sound_ambience_6ae1a8 = k_f_0_55 - (camera_pos.z + k_f_27) * k_f_0_011111111 * k_f_0_55;
      }
      /* Clamp t to [0, 1] */
      if (g_sound_ambience_6ae1a8 <= float_1_0) {
        if (g_sound_ambience_6ae1a8 < k_f_0) {
          g_sound_ambience_6ae1a8 = 0.0;
        }
      }
      else {
        g_sound_ambience_6ae1a8 = 1.0;
      }
      /* Point on the spline at t, copied into the voice record's position */
      RpSplineFindPosition
                (g_sound_6ad1e4,10,g_sound_ambience_6ae1a8,(RwV3d *)&g_sound_ambience_6ae0a0,
                 (RwV3d *)0x0);
      voice_rec = g_voices[voice_slot].voice;
      *(undefined4 *)(voice_rec + VOICE_POS_X_OFF) = g_sound_ambience_6ae0a0;
      *(undefined4 *)(voice_rec + VOICE_POS_Y_OFF) = g_sound_ambience_6ae0a4;
      *(undefined4 *)(voice_rec + VOICE_POS_Z_OFF) = g_sound_ambience_6ae0a8;
      voice_handle = *(int *)(g_voices[voice_slot].voice + VOICE_HANDLE_OFF);
      voice_rec = 0;
      if (voice_handle != 0) {
        voice_rec = FUN_005b72a0(voice_handle,0,1,&g_sound_ambience_6ae0a0);
      }
    }
    /* Spline voice plays at full volume */
    voice_set_volume(voice_slot,1.0);
  }
  return;
}


