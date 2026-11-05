/* rwa_stream_format_unpack_005c9530 (0x005c9530): stream format read as one block (_rwaStreamFormatCreateFromPack): turn the packed tail into the pointers of the header, sub streams, items and wave formats */
#include "mfl_tier2.h"

/*005be810*/ extern void *rwa_unique_id_set_uuid_005be810(void *id, void *uuid);
/*005be7d0*/ extern void *rwa_unique_id_set_name_005be7d0(void *id, char *name);
/*005bd310*/ extern int _rwaWaveFormatSerialize(void *format, const void *src, int write, int swap);
/*005bcfa0*/ extern RwUInt32 _rwaWaveFormatGetSize(const void *format);      /* bytes the serialized format took */
/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0 = equal */
/*005f7954*/ extern undefined g_data_5f7954;      /* rwaWAVEFORMAT_XADPCM */
/*005f79b4*/ extern undefined g_data_5f79b4;      /* rwaWAVEFORMAT_IMAADPCM */
/*00949d60*/ extern RwUInt32 g_data_949d60;      /* string alignment of the pack */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define UUID_BYTES 16
#define STR_ADVANCE(p) ((rwstrlen(p) + g_data_949d60) & ~(g_data_949d60 - 1))      /* the string, its terminator, aligned */

typedef struct RwaUniqueID { void *uuid; char *name; } RwaUniqueID;      /* non-NULL in the pack = present */

typedef struct RwaStreamItem { RwaUniqueID id; char pad8[0xc]; } RwaStreamItem;      /* 0x14 */

typedef struct RwaStreamSub {      /* 0x20 */
  RwaUniqueID id;
  char pad8[4];
  RwUInt32 *formatSlots;   /* +0x0c one word per format */
  RwUInt32 numItems;       /* +0x10 */
  RwaStreamItem *items;    /* +0x14 */
  char pad18[8];
} RwaStreamSub;

typedef struct RwaWaveFormatHdr { char pad0[4]; const void *dataType; char pad8[5]; unsigned char channels; } RwaWaveFormatHdr;

typedef struct RwaStreamFormatEntry {      /* 0x28 */
  RwaUniqueID id;
  char pad8[4];
  RwUInt32 blockSamples;   /* +0x0c */
  char pad10[4];
  RwaWaveFormatHdr *wave;  /* +0x14 */
  unsigned short blockAlign;      /* +0x18 */
  unsigned short blockBytes;      /* +0x1a */
  char pad1c[0xc];
} RwaStreamFormatEntry;

typedef struct RwaStreamFormat {
  RwaUniqueID id;
  char pad8[4];
  RwUInt32 numSubs;        /* +0x0c */
  RwaStreamSub *subs;      /* +0x10 */
  RwUInt32 numFormats;     /* +0x14 */
  RwaStreamFormatEntry *formats;      /* +0x18 */
  char pad1c[0x10];
  char data[1];            /* +0x2c the packed tail */
} RwaStreamFormat;

/* The pointers in the pack are only "present" flags; each present item takes the next piece of the tail, in this order: own
   uuid, own name, sub stream array, the subs' format slot arrays, uuids, names, item arrays, the items' uuids and names, the
   format array, the serialized wave formats, the formats' uuids and names.  Xbox / IMA ADPCM stereo streams get 64 sample
   blocks of 36 bytes (4 aligned).  Returns fmt, NULL when a wave format does not read. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaStreamFormat *rwa_stream_format_unpack_005c9530(RwaStreamFormat *fmt)
{
  char *p = fmt->data;
  RwUInt32 i, j, numSubs, slots;
  RwaStreamSub *sub;
  RwaStreamFormatEntry *f;

  if (fmt->id.uuid) {
    rwa_unique_id_set_uuid_005be810(&fmt->id, p);
    p += UUID_BYTES;
  }
  if (fmt->id.name) {
    rwa_unique_id_set_name_005be7d0(&fmt->id, p);
    p += STR_ADVANCE(p);
  }

  numSubs = fmt->numSubs;
  slots = fmt->numFormats;
  fmt->subs = (RwaStreamSub *)p;
  p += numSubs * sizeof(RwaStreamSub);
  for (i = 0; i < fmt->numSubs; i++) {
    sub = &fmt->subs[i];
    if (sub->formatSlots) {
      sub->formatSlots = (RwUInt32 *)p;
      p += slots * sizeof(RwUInt32);
    }
  }
  for (i = 0; i < fmt->numSubs; i++) {
    sub = &fmt->subs[i];
    if (sub->id.uuid) {
      rwa_unique_id_set_uuid_005be810(sub, p);
      p += UUID_BYTES;
    }
  }
  for (i = 0; i < fmt->numSubs; i++) {
    sub = &fmt->subs[i];
    if (sub->id.name) {
      rwa_unique_id_set_name_005be7d0(sub, p);
      p += STR_ADVANCE(p);
    }
  }
  for (i = 0; i < fmt->numSubs; i++) {
    sub = &fmt->subs[i];
    if (sub->items) {
      sub->items = (RwaStreamItem *)p;
      p += sub->numItems * sizeof(RwaStreamItem);
    }
  }
  for (i = 0; i < fmt->numSubs; i++) {
    sub = &fmt->subs[i];
    for (j = 0; j < sub->numItems; j++)
      if (sub->items[j].id.uuid) {
        rwa_unique_id_set_uuid_005be810(&sub->items[j], p);
        p += UUID_BYTES;
      }
  }
  for (i = 0; i < fmt->numSubs; i++) {
    sub = &fmt->subs[i];
    for (j = 0; j < sub->numItems; j++)
      if (sub->items[j].id.name) {
        rwa_unique_id_set_name_005be7d0(&sub->items[j], p);
        p += STR_ADVANCE(p);
      }
  }

  fmt->formats = (RwaStreamFormatEntry *)p;
  p += fmt->numFormats * sizeof(RwaStreamFormatEntry);
  for (i = 0; i < fmt->numFormats; i++) {
    f = &fmt->formats[i];
    if (f->wave) {
      f->wave = (RwaWaveFormatHdr *)p;
      if (!_rwaWaveFormatSerialize(p, p, 0, 0))
        return 0;
      p += (_rwaWaveFormatGetSize(p) + g_data_949d60 - 1) & ~(g_data_949d60 - 1);
    }
  }
  if ((RwaUUIDCompare(fmt->formats[0].wave->dataType, &g_data_5f7954) == 0 || RwaUUIDCompare(fmt->formats[0].wave->dataType, &g_data_5f79b4) == 0) &&
      fmt->formats[0].wave->channels == 2) {
    fmt->formats[0].blockSamples = 64;
    fmt->formats[0].blockBytes = 36;
    fmt->formats[0].blockAlign = 4;
  }
  for (i = 0; i < fmt->numFormats; i++) {
    f = &fmt->formats[i];
    if (f->id.uuid) {
      rwa_unique_id_set_uuid_005be810(f, p);
      p += UUID_BYTES;
    }
  }
  for (i = 0; i < fmt->numFormats; i++) {
    f = &fmt->formats[i];
    if (f->id.name) {
      rwa_unique_id_set_name_005be7d0(f, p);
      p += STR_ADVANCE(p);
    }
  }
  return fmt;
}
