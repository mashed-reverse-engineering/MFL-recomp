/* rwa_wave_format_serialize_005bd310 (0x005bd310): wave format (_rwaWaveFormatSerialize): write the format to a buffer or read it back in place, optionally byte-swapped */
#include <string.h>
#include "mfl_tier2.h"

/*005bf3e0*/ extern void FUN_005bf3e0(void *cursor, const void *src, RwUInt32 bytes);      /* copy to *cursor byte-swapped, advance *cursor */
/*005be6c0*/ extern void _rwaUUIDSerialize(void *a1, void *a2, int swap);
/*005bd050*/ extern void *_rwaWaveFormatFindUUID(const void *uuid);      /* the registered format uuid equal to the given one */

#define UUID_BYTES 16

typedef struct RwaWaveFormat {      /* 0x1c serialized */
  RwUInt32 sampleRate;     /* +0x00 */
  void *dataType;          /* +0x04 uuid (serialized as "present" flag, the uuid follows) */
  RwUInt32 length;         /* +0x08 */
  unsigned char bits;      /* +0x0c */
  unsigned char channels;  /* +0x0d */
  unsigned short pad0e;
  void *miscData;          /* +0x10 extra data (follows) */
  RwUInt32 miscBytes;      /* +0x14 */
  unsigned char flags;     /* +0x18: 2 = misc data owned by the buffer */
  unsigned char align;     /* +0x19 */
  unsigned short pad1a;
} RwaWaveFormat;

/* write: format -> buf (header, data type uuid, misc data); read: buf -> format, the uuid resolved to the registered one and the
   misc data pointing into buf.  Returns format, NULL when a read uuid is unknown. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaWaveFormat *rwa_wave_format_serialize_005bd310(RwaWaveFormat *format, char *buf, int write, int swap)
{
  RwUInt32 hasType;
  char *cursor;
  char *src;

  if (write) {
    cursor = buf;
    if (swap) {
      hasType = format->dataType != 0;
      FUN_005bf3e0(&cursor, &format->sampleRate, 4);
      FUN_005bf3e0(&cursor, &hasType, 4);
      FUN_005bf3e0(&cursor, &format->length, 4);
      FUN_005bf3e0(&cursor, &format->bits, 1);
      FUN_005bf3e0(&cursor, &format->channels, 1);
      *(unsigned short *)cursor = 0;
      cursor += 2;
      FUN_005bf3e0(&cursor, &format->miscData, 4);
      FUN_005bf3e0(&cursor, &format->miscBytes, 4);
      FUN_005bf3e0(&cursor, &format->flags, 1);
      FUN_005bf3e0(&cursor, &format->align, 1);
      *(unsigned short *)cursor = 0;
      cursor += 2;
    } else {
      memcpy(cursor, format, sizeof(RwaWaveFormat));
      cursor += sizeof(RwaWaveFormat);
    }
    if (format->dataType) {
      _rwaUUIDSerialize(format->dataType, cursor, swap);
      cursor += UUID_BYTES;
    }
    if (format->miscData) {
      memcpy(cursor, format->miscData, format->miscBytes);
      format->flags |= 2;
    }
    return format;
  }

  if (swap) {
    cursor = (char *)format;
    src = buf;
    FUN_005bf3e0(&cursor, src, 4);
    FUN_005bf3e0(&cursor, src += 4, 4);
    FUN_005bf3e0(&cursor, src += 4, 4);
    FUN_005bf3e0(&cursor, src += 4, 1);
    FUN_005bf3e0(&cursor, src += 1, 1);
    *(unsigned short *)cursor = 0;
    cursor += 2;
    FUN_005bf3e0(&cursor, src += 3, 4);
    FUN_005bf3e0(&cursor, src += 4, 4);
    FUN_005bf3e0(&cursor, src += 4, 1);
    FUN_005bf3e0(&cursor, src += 1, 1);
    *(unsigned short *)cursor = 0;
    src += 3;
  } else {
    memcpy(format, buf, sizeof(RwaWaveFormat));
    src = buf + sizeof(RwaWaveFormat);
  }
  if (format->dataType) {
    _rwaUUIDSerialize(src, src, swap);
    format->dataType = _rwaWaveFormatFindUUID(src);
    if (!format->dataType)
      return 0;
    src += UUID_BYTES;
  }
  if (format->miscData)
    format->miscData = src;
  return format;
}
