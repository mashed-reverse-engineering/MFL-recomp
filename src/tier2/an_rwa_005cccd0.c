/* rwa_dxaudio_wave_create_005cccd0 (0x005cccd0): DirectSound wave: create the wave's sound buffer (or the software mixer's wave), choosing the DirectSound buffer flags */
#include "mfl_tier2.h"

typedef struct OsVersionInfo { RwUInt32 size, major, minor, build, platform; char csd[128]; } OsVersionInfo;      /* OSVERSIONINFOA */
extern __declspec(dllimport) RwBool __stdcall GetVersionExA(OsVersionInfo *info);
/*005da470*/ extern void *rwa_soft_wave_create_005da470(void *mixer, void *format, void *targetFormat, int is3d, int a5);
/*005b8060*/ extern void RwaWaveDictRemoveAllWave(void *a1);
/*005bc280*/ extern int FUN_005bc280(void *wave);
/*005cd170*/ extern int _rwaDSBufferCreate(void *directSound, void *format, int is3d, int a4, RwUInt32 flags, void *out);

#define DSBCAPS_STATIC                0x02
#define DSBCAPS_CTRLFREQUENCY         0x20
#define DSBCAPS_CTRLPAN               0x40
#define DSBCAPS_CTRLVOLUME            0x80
#define DSBCAPS_LOCSOFTWARE           0x08
#define DSBCAPS_CTRLPOSITIONNOTIFY    0x100
#define DSBCAPS_CTRLFX                0x200
#define DSBCAPS_GLOBALFOCUS           0x8000
#define DSBCAPS_GETCURRENTPOSITION2   0x10000
#define DSBCAPS_LOCDEFER              0x40000
#define VER_PLATFORM_WIN32_WINDOWS    1

#define DF_SOFTWARE  0x08      /* output definition flags (+0x50) */
#define DF_FORCE_SOFTWARE_BUFFERS 0x20
#define DF_3D_PAN    0x40
#define DF_GLOBAL_FOCUS 0x400

typedef struct RwaDXWave {
  char pad0[0x2c];
  char format[8];          /* +0x2c the wave format (+0x34 its length) */
  RwUInt32 length;         /* +0x34 */
  char pad38[0x10];
  RwUInt32 bufferBytes;    /* +0x48 */
  void *soft;              /* +0x4c software mixer wave */
  char pad50[4];
  RwUInt32 flags;          /* +0x54: 2 = 3D */
  char pad58[4];
  void *buffer;            /* +0x5c */
  RwUInt32 x60, x64;
} RwaDXWave;

/* Returns the wave, NULL when the buffer could not be created. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaDXWave *rwa_dxaudio_wave_create_005cccd0(RwaDXWave *w, char *outputObj)
{
  char *def = *(char **)(outputObj + 0x20);
  RwUInt32 dsFlags, is3d;
  OsVersionInfo osv;

  w->bufferBytes = w->length;
  if (*(unsigned char *)(def + 0x50) & DF_SOFTWARE) {
    w->soft = rwa_soft_wave_create_005da470(*(void **)(def + 0xf4), w->format, w->format, (w->flags >> 1) & 1, 0);
    RwaWaveDictRemoveAllWave(w->soft);
    return w->soft ? w : 0;
  }
  is3d = (w->flags >> 1) & 1;
  dsFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
  if (FUN_005bc280(w)) {      /* position notify on Windows 98 / ME (9x kernel, version above 4.0) */
    osv.size = sizeof(osv);
    GetVersionExA(&osv);
    if (osv.platform == VER_PLATFORM_WIN32_WINDOWS && (osv.major > 4 || (osv.major == 4 && osv.minor > 0)))
      dsFlags |= DSBCAPS_CTRLPOSITIONNOTIFY;
  }
  if (!is3d || (*(RwUInt32 *)(def + 0x50) & DF_3D_PAN)) {      /* panned instead of a 3D buffer */
    is3d = 0;
    dsFlags |= DSBCAPS_CTRLPAN;
  }
  if (*(RwUInt32 *)(def + 0x50) & DF_FORCE_SOFTWARE_BUFFERS) {
    dsFlags |= DSBCAPS_LOCSOFTWARE;
    if (*(RwUInt32 *)(def + 0x50) & DF_SOFTWARE)
      dsFlags |= DSBCAPS_CTRLFX;
  } else
    dsFlags |= DSBCAPS_LOCDEFER;
  if (*(RwUInt32 *)(def + 0x50) & DF_GLOBAL_FOCUS)
    dsFlags |= DSBCAPS_GLOBALFOCUS;
  w->x60 = 0;
  w->buffer = 0;
  w->x64 = 0;
  return _rwaDSBufferCreate(*(void **)(def + 0x54), w->format, is3d, 1, dsFlags, &w->buffer) ? w : 0;
}
