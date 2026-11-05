/* rwa_dxaudio_voice_init_005c9c80 (0x005c9c80): DirectSound voice: initialise a new voice object (software voice when the output mixes itself, 3D defaults, I3DL2 / EAX defaults) and push it once */
#include "mfl_tier2.h"

/*005d8110*/ extern void *rwa_soft_mixer_obj_create_005d8110(int a1, int a2, int a3, int a4);
/*005ca7d0*/ extern void _rwaDXAudioVoiceSetWave(void *voice, void *wave);
/*005bf2b0*/ extern int RwaMathConvertLinearToLog(float linear);
/*005c9fb0*/ extern void *rwa_dxaudio_voice_finish_execute_005c9fb0(void *voice);

#define VF_3D_DIRTY      0x0004      /* voice flags (+0x28) */
#define VF_EAX_DIRTY     0x0008
#define VF_REVERB_DIRTY  0x0010
#define VF_PARAMS_DIRTY  0x00e0      /* frequency, gain, pan */

#define DF_REVERB        0x07        /* output definition flags (+0x50): I3DL2 / EAX 1 / EAX 2 */
#define DF_SOFTWARE      0x08
#define DF_3D_PAN        0x40

#define CP_3D            0x02        /* creation flags (*params) */
#define CP_HEADRELATIVE  0x02        /* also the DS3DMODE_HEADRELATIVE bit */

typedef struct RwaI3DL2Props {      /* the listener reverb block of the voice (+0x40), I3DL2 order */
  int room, roomHF;
  float roomRolloff, decayTime, decayHFRatio;
  int reflections;
  float reflectionsDelay;
  int reverb;
  float reverbDelay, diffusion, density, hfReference;
} RwaI3DL2Props;

typedef struct DS3DBufferParams {      /* DS3DBUFFER */
  RwUInt32 dwSize;
  RwV3d position, velocity;
  RwUInt32 insideConeAngle, outsideConeAngle;
  RwV3d coneOrientation;
  int coneOutsideVolume;
  float minDistance, maxDistance;
  RwUInt32 mode;
} DS3DBufferParams;

typedef struct RwaListLink { struct RwaListLink *next, **prevNext; } RwaListLink;

typedef struct RwaDXVoice {
  char obj[0x28];
  RwUInt32 flags;        /* +0x28 */
  float frequency;       /* +0x2c */
  float gain;            /* +0x30 */
  float pan;             /* +0x34 */
  char pad38[8];
  RwaI3DL2Props reverb;  /* +0x40 */
  int direct;            /* +0x70 */
  int obstruction;       /* +0x74 */
  float obstructionLFRatio;      /* +0x78 */
  int occlusion;         /* +0x7c */
  float occlusionLFRatio;        /* +0x80 */
  void *wave;            /* +0x84 */
  RwUInt32 position, bytePos, startPos;      /* +0x88 */
  char *def;             /* +0x94 output definition (+0x50 flags, +0x6c 3D pan voice list, +0xf4 soft output) */
  float minDistance;     /* +0x98 */
  DS3DBufferParams ds3d; /* +0x9c */
  void *buffer, *buffer3d, *props;      /* +0xdc */
  int eax[8];            /* +0xe8 the voice's own EAX block (13 words) */
  float eax8;            /* +0x108 */
  int eax9;
  float eax10;           /* +0x110 */
  int eax11, eax12;
  void *soft;            /* +0x11c */
  RwaListLink link;      /* +0x120 on the 3D pan list */
} RwaDXVoice;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaDXVoice *rwa_dxaudio_voice_init_005c9c80(RwaDXVoice *v, char *outputObj, RwUInt32 *params, int a4, int a5)
{
  RwaI3DL2Props generic;
  RwaListLink **head;
  int k;

  v->buffer = 0;
  v->flags = *params;
  v->buffer3d = 0;
  v->props = 0;
  v->def = outputObj + 0x28;
  if (*(RwUInt32 *)(v->def + 0x50) & DF_SOFTWARE) {
    v->soft = rwa_soft_mixer_obj_create_005d8110(*(int *)(v->def + 0xf4), 4, a4, a5);
    if (!v->soft)
      return 0;
  }
  v->position = 0;
  v->bytePos = 0;
  v->startPos = 0;
  v->wave = 0;
  _rwaDXAudioVoiceSetWave(v, 0);

  v->flags |= VF_PARAMS_DIRTY;
  v->frequency = 48000.0f;
  v->gain = 1.0f;
  v->pan = 0.0f;
  v->ds3d.dwSize = sizeof(DS3DBufferParams);
  v->ds3d.mode = (*params >> 1) & 1;      /* DS3DMODE_HEADRELATIVE */
  v->ds3d.insideConeAngle = 360;
  v->ds3d.outsideConeAngle = 360;
  v->ds3d.position.x = 0.0f; v->ds3d.position.y = 0.0f; v->ds3d.position.z = 0.0f;
  v->ds3d.velocity.x = 0.0f; v->ds3d.velocity.y = 0.0f; v->ds3d.velocity.z = 0.0f;
  v->ds3d.coneOrientation.x = 0.0f; v->ds3d.coneOrientation.y = 0.0f; v->ds3d.coneOrientation.z = 1.0f;
  v->flags |= VF_3D_DIRTY;
  v->ds3d.coneOutsideVolume = RwaMathConvertLinearToLog(1.0f);
  v->flags |= VF_3D_DIRTY;
  v->minDistance = 1.0f;
  v->ds3d.minDistance = 9999.0f;
  v->ds3d.maxDistance = 10000.0f;

  if (*(RwUInt32 *)(v->def + 0x50) & DF_REVERB) {      /* I3DL2 "generic" environment, room levels 0 */
    generic.room = 0;
    generic.roomHF = 0;
    generic.roomRolloff = 10.0f;
    generic.decayTime = 1.49f;
    generic.decayHFRatio = 0.83f;
    generic.reflections = -2602;
    generic.reflectionsDelay = 0.007f;
    generic.reverb = 200;
    generic.reverbDelay = 0.011f;
    generic.diffusion = 100.0f;
    generic.density = 100.0f;
    generic.hfReference = 5000.0f;
    v->reverb = generic;
    v->direct = 0;
    v->obstruction = 0;
    v->obstructionLFRatio = 0.25f;
    v->occlusion = 0;
    v->occlusionLFRatio = 0.25f;
    v->flags |= VF_REVERB_DIRTY;
  }
  for (k = 0; k < 8; k++)
    v->eax[k] = 0;
  v->eax8 = 1.0f;
  v->eax9 = 0;
  v->eax10 = 1.0f;
  v->eax11 = 0;
  v->eax12 = 0;      /* the original converts 0.0f with a chop fistp */
  v->flags |= VF_EAX_DIRTY;

  if (*params & CP_3D) {
    if (*(RwUInt32 *)(v->def + 0x50) & DF_3D_PAN) {      /* panned 3D voices are updated by the output: link at the list head */
      head = (RwaListLink **)(v->def + 0x6c);
      v->link.prevNext = head;
      v->link.next = *head;
      (*head)->prevNext = &v->link.next;
      *(RwaListLink **)(v->def + 0x6c) = &v->link;
      rwa_dxaudio_voice_finish_execute_005c9fb0(v);
      return v;
    }
    v->link.prevNext = 0;
    v->link.next = 0;
  }
  rwa_dxaudio_voice_finish_execute_005c9fb0(v);
  return v;
}
