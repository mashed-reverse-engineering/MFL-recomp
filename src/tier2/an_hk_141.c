#include "mfl_tier2.h"
/*005be600*/ extern int __cdecl guid_compare(int a1, void *a2);
/*005f7914*/ extern undefined g_data_5f7914;
/*005f7924*/ extern undefined g_data_5f7924;
/*005f7934*/ extern undefined g_data_5f7934;
/*005f7944*/ extern undefined g_data_5f7944;
/*005f7954*/ extern undefined g_data_5f7954;
/*005f7974*/ extern undefined g_data_5f7974;
/*005f79b4*/ extern undefined g_data_5f79b4;
#define DEFAULT_BITRATE (0x1f400) /* 128 kbit/s, used when no bitrate is set (inferred) */
#define DEFAULT_SAMPLE_RATE (48000) /* used when the stream reports no sample rate (inferred) */
#define MP3_FRAME_COEF (0x90) /* 144: MPEG-1 Layer III frame size = 144 * bitrate / sample rate (inferred) */
/*
 * Returns a per-stream size value for the stream's format (inferred: the
 * MP3 frame length in bytes on the frame-size path).
 *   stream: uint array. [0] = sample rate, [1] = format handle passed to
 *           guid_compare, [3] = byte whose top 5 bits are used (inferred
 *           sample-size field), [4] = pointer to bitrate (NULL = default).
 *   returns: the size for the matched format; 0 on the tag 5f7974 path, and
 *           4 or 36 (inferred header sizes) when no earlier tag matches.
 */
uint FUN_005bcc50(uint *stream)

{
  int match;
  uint sample_rate;

  /* tag 5f7924: size from byte 3 */
  match = guid_compare(stream[1],&g_data_5f7924);
  if (match == 0) {
    return (uint)(byte)((byte)stream[3] >> 3);
  }
  /* tags 5f7914 / 5f7934 / 5f7944: fixed sizes */
  match = guid_compare(stream[1],&g_data_5f7914);
  if (match == 0) {
    return 0x10;
  }
  match = guid_compare(stream[1],&g_data_5f7934);
  if (match == 0) {
    return 4;
  }
  match = guid_compare(stream[1],&g_data_5f7944);
  if (match == 0) {
    return 8;
  }
  /* tags 5f7954 and 5f79b4 must both match, and 5f7974 must not, for the frame-size path */
  match = guid_compare(stream[1],&g_data_5f7954);
  if (match != 0) {
    match = guid_compare(stream[1],&g_data_5f79b4);
    if (match != 0) {
      match = guid_compare(stream[1],&g_data_5f7974);
      if (match == 0) {
        /* frame size = MP3_FRAME_COEF * bitrate / sample rate */
        if ((int *)stream[4] == (int *)0x0) {
          match = DEFAULT_BITRATE;
        }
        else {
          match = *(int *)stream[4];
        }
        sample_rate = *stream;
        if (sample_rate == 0) {
          sample_rate = DEFAULT_SAMPLE_RATE;
        }
        return (uint)(match * MP3_FRAME_COEF) / sample_rate;
      }
      return 0;
    }
  }
  return (-(uint)(*(char *)((char *)stream + 0xd) != '\x01') & 0xffffffe0) + 0x24;
}


