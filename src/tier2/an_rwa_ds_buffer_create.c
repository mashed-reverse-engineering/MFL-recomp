/* RenderWare Audio (Criterion Software) DirectSound buffer creation, MFL.exe 0x005cd170 (_rwaDSBufferCreate) and 0x005cd290
 * (_rwaDSBufferGetInterfaces).  Written from the MFL.exe disassembly (objdump); names from the symbol table.
 * A buffer object is three COM pointers: IDirectSoundBuffer, IDirectSound3DBuffer (3D buffers), IKsPropertySet (EAX / I3DL2). */
#include "mfl_tier2.h"

typedef struct ComObj { void **vtbl; } ComObj;
typedef struct RwaDSBufferSet { ComObj *buffer, *buffer3d, *props; } RwaDSBufferSet;

typedef RwInt32 (__stdcall *ComQueryInterface)(ComObj *self, const void *iid, void *out);      /* IUnknown slot 0 */
typedef RwUInt32 (__stdcall *ComRelease)(ComObj *self);                                        /* IUnknown slot 2 */
typedef RwInt32 (__stdcall *DSCreateSoundBuffer)(ComObj *self, const void *desc, void *out, void *outer);      /* IDirectSound slot 3 */
#define COM(p, slot, T) ((T)(p)->vtbl[slot])

typedef struct RwaWaveFormat {      /* the RWA wave format (0x1c bytes); only the fields used here */
  RwUInt32 sampleRate;              /* +0x00 */
  RwUInt32 w4;
  RwUInt32 bytes;                   /* +0x08 data size */
  RwUInt8 bitsPerSample, channels;  /* +0x0c / +0x0d */
} RwaWaveFormat;

typedef struct WaveFormatEx {       /* WAVEFORMATEX (18 bytes; the struct pads to 20, nothing here uses its size) */
  RwUInt16 formatTag, channels;
  RwUInt32 samplesPerSec, avgBytesPerSec;
  RwUInt16 blockAlign, bitsPerSample, size;
} WaveFormatEx;

typedef struct DSBufferDesc {       /* DSBUFFERDESC (DirectX 7+ layout, 0x24 bytes) */
  RwUInt32 size, flags, bufferBytes, reserved;
  WaveFormatEx *format;
  RwUInt32 algorithm3d[4];          /* GUID */
} DSBufferDesc;

#define DSBCAPS_3D 0x20010          /* DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE */

/*005e1e7c*/ extern RwUInt32 g_data_5e1e7c[4];      /* 3D algorithm GUID used with buffer flag 1 */
/*005e2634*/ extern RwUInt32 g_data_5e2634[4];      /* the default 3D algorithm GUID */
/*005e1eac*/ extern undefined g_data_5e1eac;         /* IID_IDirectSound3DBuffer */
/*005e1e9c*/ extern undefined g_data_5e1e9c;         /* IID_IKsPropertySet */

/* Queries the 3D buffer (want3d) and the property set (wantProps) interfaces of set->buffer.  1: ok; 0: a query failed (the 3D
   interface is released again when the property set query fails). */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwBool _rwaDSBufferGetInterfaces(RwaDSBufferSet *set, RwBool want3d, RwBool wantProps)
{
  if (want3d && COM(set->buffer, 0, ComQueryInterface)(set->buffer, &g_data_5e1eac, &set->buffer3d) < 0)
    return 0;
  if (wantProps && COM(set->buffer, 0, ComQueryInterface)(set->buffer, &g_data_5e1e9c, &set->props) < 0) {
    if (set->buffer3d)
      COM(set->buffer3d, 2, ComRelease)(set->buffer3d);
    return 0;
  }
  return 1;
}

/* Creates a sound buffer of `format` (PCM, sized for the format's data) on the DirectSound object `ds` with `flags` (3D caps added
   when want3d) and gets its interfaces into `set`.  1: ok; 0: creation or an interface query failed (the buffer is released). */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwBool _rwaDSBufferCreate(ComObj *ds, const RwaWaveFormat *format, RwBool want3d, RwBool wantProps, RwUInt32 flags, RwaDSBufferSet *set)
{
  WaveFormatEx wfx;
  DSBufferDesc desc;
  const RwUInt32 *algorithm;

  wfx.formatTag = 0;
  wfx.channels = 0;
  wfx.avgBytesPerSec = 0;
  wfx.blockAlign = 0;
  wfx.bitsPerSample = 0;
  set->buffer = 0;
  set->buffer3d = 0;
  set->props = 0;
  wfx.size = 0;
  wfx.channels = format->channels;
  wfx.bitsPerSample = format->bitsPerSample;
  wfx.samplesPerSec = format->sampleRate;
  wfx.blockAlign = (RwUInt16)((wfx.bitsPerSample >> 3) * wfx.channels);
  wfx.formatTag = 1;      /* WAVE_FORMAT_PCM */
  wfx.avgBytesPerSec = wfx.blockAlign * wfx.samplesPerSec;
  wfx.size = 0x12;
  if (want3d)
    flags |= DSBCAPS_3D;
  desc.flags = flags;
  desc.size = sizeof(desc);
  desc.bufferBytes = format->bytes;
  desc.reserved = 0;
  desc.format = &wfx;
  algorithm = (flags & 1) ? g_data_5e1e7c : g_data_5e2634;
  desc.algorithm3d[0] = algorithm[0];
  desc.algorithm3d[1] = algorithm[1];
  desc.algorithm3d[2] = algorithm[2];
  desc.algorithm3d[3] = algorithm[3];
  if (COM(ds, 3, DSCreateSoundBuffer)(ds, &desc, &set->buffer, 0) < 0)
    return 0;
  if (_rwaDSBufferGetInterfaces(set, want3d, wantProps))
    return 1;
  COM(set->buffer, 2, ComRelease)(set->buffer);
  return 0;
}
