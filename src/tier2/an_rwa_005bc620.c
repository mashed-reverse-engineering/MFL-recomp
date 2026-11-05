/* rwa_wave_create_from_pack_005bc620 (0x005bc620): wave: create a wave from a wave pack chunk (formats, ids, name, definition) */
#include "mfl_tier2.h"

/*005bf320*/ extern void FUN_005bf320(void *data, RwUInt32 bytes);      /* byte-swap 32-bit words */
/*005bd310*/ extern void _rwaWaveFormatSerialize(void *format, const void *src, int write, int swap);
/*005bcfa0*/ extern RwUInt32 _rwaWaveFormatGetSize(const void *format);      /* bytes the serialized format took */
/*005be6c0*/ extern void _rwaUUIDSerialize(void *dst, const void *src, int swap);
/*005ba200*/ extern void RwaObjDefEnum(void *a1, void *a2, void *callback, void *data, int a5);
/*005bcd80*/ extern void _rwaWaveDefFindByObjCallback(void);
/*005bd2e0*/ extern char *RwaWaveDefFindByUUID(const void *uuid);
/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0 = equal */
/*005bcfc0*/ extern int _rwaWaveDefIsTargetFormatSupported(char *def, void *format);
/*005bcea0*/ extern void _rwaWaveDefFormatCopyToWaveFormat(const void *uuid, void *format, int a3);
/*005ba5c0*/ extern void *sub_23DCA0(void *a1, const void *uuid, void *a3);      /* RwaObjDefFindByUUID */
/*005bac30*/ extern void RwaObjEnum(void *callback, void *data);
/*005bac10*/ extern void rwa_obj_005bac10(void);      /* RwaObjEnum callback: the first object of a definition */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bc290*/ extern char *RwaWaveCreate(char *def, void *obj, void *format, void *targetFormat, RwUInt32 flags, void *block);
/*005bc030*/ extern int RwaWaveDefAssignID(char *wave, const void *uuid, const char *name);
/*005f7694*/ extern undefined g_data_5f7694;      /* the generic (software) wave definition */
/*005f836c*/ extern undefined g_data_5f836c;      /* the soft mixer wave definition */
/*00949d60*/ extern RwUInt32 g_data_949d60;      /* string alignment of the pack */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define RWA_HINT 0x30806
/* pack flags (first word of the pack) */
#define PACK_UUID   0x1      /* 16-byte wave id follows */
#define PACK_NAME   0x2      /* aligned name string follows */
#define PACK_DEF    0x4      /* 16-byte definition id follows */
#define PACK_OBJ    0x8      /* 16-byte object definition id follows */
/* creation flags */
#define WF_LOOPBIT  0x01     /* the looping flag comes from bit 1 of flags, not from the pack */
#define WF_NONAME   0x04
#define WF_NOUUID   0x08
#define WF_SWAP     0x10
#define WF_TARGET   0x20     /* overwrite the caller's target format */

typedef struct RwaWaveFormat { RwUInt32 w[7]; } RwaWaveFormat;      /* 0x1c bytes */
typedef struct RwaUUID { RwUInt32 w[4]; } RwaUUID;

/* pack: flags, source format, target format, a word (looping), then the optional ids / name.  def / obj given by the
   caller win over the pack's.  Returns the wave or NULL. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_wave_create_from_pack_005bc620(RwUInt32 *pack, char *def, void *obj, RwUInt32 *target, RwUInt32 flags)
{
  struct {      /* the locals in the original's frame order (the stubbed serializers see the format words) */
    const char *name;
    void *uuid;
    void *found;      /* RwaObjEnum: {result, key} */
    void *key;
    RwUInt32 looping;
    void *searchKey;      /* RwaObjDefEnum: {key, result} */
    char *searchDef;
    RwaWaveFormat target;
    RwaWaveFormat source;
  } f;
  RwUInt32 packFlags = pack[0];
  char *p = (char *)(pack + 1);
  int swap = (flags >> 4) & 1;
  RwUInt32 size, len;
  char *block;
  char *wave;
  RwUInt32 off;

  f.uuid = 0;
  f.name = 0;
  f.found = 0;
  if (swap)
    FUN_005bf320(&packFlags, 4);
  _rwaWaveFormatSerialize(&f.source, p, 0, swap);
  p += _rwaWaveFormatGetSize(&f.source);
  _rwaWaveFormatSerialize(&f.target, p, 0, swap);
  p += _rwaWaveFormatGetSize(&f.target);
  if (flags & WF_LOOPBIT)
    f.looping = (flags >> 1) & 1;
  else {
    f.looping = *(RwUInt32 *)p;      /* (taken before the swap, as the original) */
    if (swap)
      FUN_005bf320(p, 4);
  }
  p += 4;
  if (packFlags & PACK_UUID) {
    if (!(flags & WF_NOUUID)) {
      f.uuid = p;
      _rwaUUIDSerialize(p, p, swap);
    }
    p += 0x10;
  }
  if (packFlags & PACK_NAME) {
    if (!(flags & WF_NONAME))
      f.name = p;
    p += (rwstrlen(p) + g_data_949d60) & ~(g_data_949d60 - 1);
  }
  if (packFlags & PACK_DEF) {
    if (!def) {
      if (obj) {      /* the definition of the given object */
        f.searchKey = obj;
        f.searchDef = 0;
        RwaObjDefEnum(0, 0, (void *)_rwaWaveDefFindByObjCallback, &f.searchKey, 1);
        def = f.searchDef;
        if (!def)
          return 0;
      } else {
        _rwaUUIDSerialize(p, p, swap);
        def = RwaWaveDefFindByUUID(p);
        if (!def)
          return 0;
        if (RwaUUIDCompare(*(void **)def, &g_data_5f7694) == 0) {      /* the generic definition: use the soft mixer's when it can take the format */
          def = RwaWaveDefFindByUUID(&g_data_5f836c);
          if (!_rwaWaveDefIsTargetFormatSupported(def, &f.target))
            _rwaWaveDefFormatCopyToWaveFormat(*(void **)(def + 0x28) ? (*(void ***)(def + 0x28))[*(int *)(def + 0x2c)] : 0,
                                              &f.target, 0);
        }
      }
    }
    p += 0x10;
  }
  if (!target)
    target = &f.target.w[0];
  if (*target == 0 || (flags & WF_TARGET))
    *target = f.target.w[0];
  if (!obj) {
    void *key;

    if (packFlags & PACK_OBJ) {
      if (!def) {
        _rwaUUIDSerialize(p, p, swap);
        key = sub_23DCA0(0, p, 0);
        if (!key)
          return 0;
      } else {
        f.found = *(void **)(def + 0x30);
        key = f.found;
      }
    } else
      key = f.found;
    f.key = key;
    f.found = 0;
    RwaObjEnum((void *)rwa_obj_005bac10, &f.found);
    obj = f.found;
  }

  off = *(RwUInt32 *)(def + 0x34) + 0x5c;      /* the wave object, then the copied id and name */
  size = (f.uuid ? 0x10 : 0) + off;
  if (f.name) {
    len = rwstrlen((RwChar *)f.name) + 1;
    size += len;
  }
  block = (char *)FUN_005bf0d0(size, RWA_HINT);
  if (!block)
    return 0;
  if (f.uuid) {
    *(RwaUUID *)(block + off) = *(RwaUUID *)f.uuid;
    f.uuid = block + off;
    off += 0x10;
  }
  if (f.name) {
    char *dst = block + off;

    rwstrcpy((RwChar *)dst, (RwChar *)f.name);
    f.name = dst;
  }
  wave = RwaWaveCreate(def, obj, &f.source, target, f.looping, block);
  if (!wave || !size)
    return 0;
  *(RwUInt32 *)(wave + 0x54) &= ~4u;
  if (!RwaWaveDefAssignID(wave, f.uuid, f.name))
    return 0;
  return wave;
}
