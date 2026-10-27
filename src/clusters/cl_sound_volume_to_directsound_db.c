// cluster-entries: sound_volume_to_directsound_db
/* Installed cluster sound_volume_to_directsound_db (tools/cluster_install.py from output/cluster_pilot_kc/sound_volume_to_directsound_db.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005dd378*/ extern float k_f_0_1;
/*005deb3c*/ extern float k_f_n2000;
/*005df304*/ extern float k_f_n10000;
/*0076e8ac*/ extern int **g_app_76e8ac; /* IDirectSoundBuffer: this, vtable at *this */
/* sound_volume_to_directsound_db */

#define DSERR_UNSUPPORTED 0x80004001   /* E_NOTIMPL */
#define VTABLE_SET_VOLUME 0x1c
#define VTABLE_GET_VOLUME 0x20

/* Reads the buffer's linear volume (0..1) and sets it again in DirectSound hundredths of dB: from
   volume 0.1 up the range is -2000 .. 0, below it falls steeply towards -10000. Returns 0 when the buffer has no volume. */
undefined4 sound_volume_to_directsound_db(void)

{
  int hr;
  int base;
  int attenuation;
  float volume;

  hr = (*(int (__stdcall **)(void *, float *))((char *)*g_app_76e8ac + VTABLE_GET_VOLUME))(g_app_76e8ac, &volume);
  if (hr == DSERR_UNSUPPORTED) {
    return 0;
  }
  if (k_f_0_1 <= volume) {
    attenuation = (int)(volume * k_f_n2000);
    base = -2000;
  }
  else {
    attenuation = (int)(volume * k_f_n10000);
    base = -10000;
  }
  (*(int (__stdcall **)(void *, int))((char *)*g_app_76e8ac + VTABLE_SET_VOLUME))(g_app_76e8ac, base - attenuation);
  return 1;
}

