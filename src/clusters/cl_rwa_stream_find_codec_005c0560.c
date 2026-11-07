// cluster-entries: rwa_stream_find_codec_005c0560
/* Installed cluster rwa_stream_find_codec_005c0560 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_find_codec_005c0560.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream codecs (register function: wanted format KEY in edi, list in s0, entered through the generated thunk): walk the codec
   list (backwards, data list entries) and return the first codec object one of whose format entries (object + 0x54 -> table)
   accepts KEY: same type uuid, same two format bytes, same flag bit 0 and KEY->value inside [lo, hi]; 0 if none. */
#include "mfl_tier2.h"

typedef struct RwaLListEntry { struct RwaLListEntry *next, *prev; void *data; } RwaLListEntry;

typedef struct RwaCodecKey {
  RwUInt32 value;           /* checked against [lo, hi] */
  void *type;               /* +4: uuid */
  RwUInt32 w08;
  unsigned char b0c, b0d;   /* +0xc, +0xd */
  unsigned char pad0e[10];
  unsigned char flags;      /* +0x18: bit 0 compared */
} RwaCodecKey;

typedef struct RwaCodecFormat {
  void *type;               /* uuid */
  unsigned char b04, b05;
  unsigned char pad06[2];
  RwUInt32 lo, hi;          /* +8, +0xc */
  unsigned char flags;      /* +0x10: bit 0 */
} RwaCodecFormat;

typedef struct RwaCodecFormats {
  RwUInt32 w00;
  RwaCodecFormat **formats;      /* +4 */
  RwUInt32 count;                /* +8 */
} RwaCodecFormats;

/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0 = equal */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
char *rwa_stream_find_codec_005c0560_n(RwaCodecKey *key, RwaLListEntry *list)
{
  RwaLListEntry *e;
  RwUInt32 i;

  for (e = list->prev; e != list; e = e->prev) {
    char *codec = e->data;
    RwaCodecFormats *info = *(RwaCodecFormats **)(codec + 0x54);
    for (i = 0; i < info->count; i++) {
      RwaCodecFormat *f = info->formats[i];
      RwUInt32 formatFlag = f->flags & 1, keyFlag = key->flags & 1;
      if (RwaUUIDCompare(key->type, f->type) == 0 && key->b0c == f->b04 && keyFlag == formatFlag && key->b0d == f->b05 &&
          key->value <= f->hi && key->value >= f->lo)
        return codec;
    }
  }
  return 0;
}

/* entry thunk at the original address of rwa_stream_find_codec_005c0560: register inputs (edi,s0) -> natural C rwa_stream_find_codec_005c0560_n; ret 0 */
__declspec(naked) void rwa_stream_find_codec_005c0560(void)
{
    __asm {
        push dword ptr [esp + 4]
        push edi
        call rwa_stream_find_codec_005c0560_n
        add esp, 8
        ret
    }
}
