#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*00406cf0*/ extern int fx_objects_all_exploded();
/*00408a00*/ extern int scene_get_fx_651aa0();
/*00408a60*/ extern void * fx_object_get_position();
/*0042a150*/ extern void * course_get_object_record();
/*0042a160*/ extern int course_active_objects_count();
/*0044a4f0*/ extern int camera_get_position();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*00619010*/ extern undefined4 g_sound_frame_619010;
/*00619df0*/ extern undefined4 g_sound_frame_619df0;
#define SORT_SLOTS (10)
#define NEAREST_VOICE_COUNT (3)
#define VOICE_STREAM_OFF (0x30)
#define VOICE_FLAGS_OFF (0x34)
#define VOICE_FREQ_OFF (0x3c)
#define VOICE_POS_X_OFF (0x58)
#define VOICE_POS_Y_OFF (0x5c)
#define VOICE_POS_Z_OFF (0x60)
/* sound_nearest_objects_sort */

/* "computes camera distance of all course objects/fx and bubble-sorts them nearest first" */

void sound_nearest_objects_sort(void)

{
  int voice_slot;
  int voice_stream;
  bool swapped;
  int index;
  RwV3d *object_pos;
  char *voice;
  undefined4 *object_pos_raw;
  int object_count;
  RwReal *dist_cursor;
  RwReal dist;
  struct {
    int fx_flag;
    undefined1 pad_6c[4];
    float cam_x;
    float cam_y;
    float cam_z;
    RwV3d delta;
    RwReal distances;
    float distances_tail [9];
    int object_ptrs [SORT_SLOTS];
  } frame;

  /* clear the SORT_SLOTS distance entries */
  dist_cursor = &frame.distances;
  for (object_count = SORT_SLOTS; object_count != 0; object_count = object_count + -1) {
    *dist_cursor = 0.0;
    dist_cursor = dist_cursor + 1;
  }
  /* object_count = course objects; fx_flag = fx object present (inferred), cleared when all fx exploded */
  object_count = course_active_objects_count();
  frame.fx_flag = scene_get_fx_651aa0();
  index = fx_objects_all_exploded();
  if (index != 0) {
    frame.fx_flag = 0;
  }
  if (frame.fx_flag + object_count != 0) {
    camera_get_position(&frame.cam_x);
    /* distance from camera to every course object (its position is the first 3 floats of the record) */
    index = 0;
    if (0 < object_count) {
      do {
        object_pos = (RwV3d *)course_get_object_record(index);
        frame.delta.x = frame.cam_x - object_pos->x;
        frame.object_ptrs[index] = (int)object_pos;
        frame.delta.y = frame.cam_y - object_pos->y;
        frame.delta.z = frame.cam_z - object_pos->z;
        dist = RwV3dLength(&frame.delta);
        (&frame.distances)[index] = dist;
        index = index + 1;
      } while (index < object_count);
    }
    /* bubble sort: nearest first, swapping distances and object pointers together */
    do {
      swapped = false;
      index = 0;
      if (object_count + -1 < 1) break;
      do {
        if ((&frame.distances)[index + 1] < (&frame.distances)[index]) {
          dist = (&frame.distances)[index + 1];
          voice_slot = frame.object_ptrs[index + 1];
          (&frame.distances)[index + 1] = (&frame.distances)[index];
          (&frame.distances)[index] = dist;
          voice_stream = frame.object_ptrs[index];
          frame.object_ptrs[index] = voice_slot;
          frame.object_ptrs[index + 1] = voice_stream;
          swapped = true;
        }
        index = index + 1;
      } while (index < object_count + -1);
    } while (swapped);
    /* the NEAREST_VOICE_COUNT nearest objects drive the voices starting at g_sound_frame_619010
       (voice_slot and voice_stream are reused as temporaries in the sort above) */
    index = 0;
    do {
      if (object_count <= index) break;
      voice_slot = g_sound_frame_619010 + index;
      voice = g_voices[voice_slot].voice;
      /* voice flag bit 2 (meaning unknown) not set: (re)initialise the voice */
      if ((*(byte *)(voice + VOICE_FLAGS_OFF) & 2) == 0) {
        FUN_005b6a40(voice,1);
        *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQ_OFF) = 0x46ac4400;
        voice_stream = *(int *)(g_voices[voice_slot].voice + VOICE_STREAM_OFF);
        voice = 0;
        if (voice_stream != 0) {
          voice = FUN_005b7300(voice_stream,4,1,22050.0f);
        }
      }
      voice_set_volume(voice_slot,1.0);
      /* copy the object position into the voice */
      voice = g_voices[voice_slot].voice;
      object_pos_raw = (undefined4 *)frame.object_ptrs[index];
      *(undefined4 *)(voice + VOICE_POS_X_OFF) = *object_pos_raw;
      *(undefined4 *)(voice + VOICE_POS_Y_OFF) = object_pos_raw[1];
      *(undefined4 *)(voice + VOICE_POS_Z_OFF) = object_pos_raw[2];
      voice_slot = *(int *)(g_voices[voice_slot].voice + VOICE_STREAM_OFF);
      if (voice_slot != 0) {
        FUN_005b72a0(voice_slot,0,1,object_pos_raw);
      }
      index = index + 1;
    } while (index < NEAREST_VOICE_COUNT);
    /* if an fx object is active, its voice (g_sound_frame_619df0) gets the fx position */
    object_count = g_sound_frame_619df0;
    if (frame.fx_flag == 1) {
      voice = g_voices[g_sound_frame_619df0].voice;
      if ((*(byte *)(voice + VOICE_FLAGS_OFF) & 2) == 0) {
        FUN_005b6a40(voice,1);
        *(undefined4 *)(g_voices[object_count].voice + VOICE_FREQ_OFF) = 0x46ac4400;
        index = *(int *)(g_voices[object_count].voice + VOICE_STREAM_OFF);
        voice = 0;
        if (index != 0) {
          voice = FUN_005b7300(index,4,1,22050.0f);
        }
      }
      voice_set_volume(object_count,1.0);
      object_pos_raw = (undefined4 *)fx_object_get_position(0);
      voice = g_voices[object_count].voice;
      *(undefined4 *)(voice + VOICE_POS_X_OFF) = *object_pos_raw;
      *(undefined4 *)(voice + VOICE_POS_Y_OFF) = object_pos_raw[1];
      *(undefined4 *)(voice + VOICE_POS_Z_OFF) = object_pos_raw[2];
      object_count = *(int *)(g_voices[object_count].voice + VOICE_STREAM_OFF);
      if (object_count != 0) {
        FUN_005b72a0(object_count,0,1,object_pos_raw);
      }
    }
  }
  return;
}


