/* RenderWare Audio (Criterion Software) DirectSound voice: _RwaDXAudioVoiceFinishExecute, MFL.exe 0x005c9fb0.  Written from the
 * MFL.exe disassembly (objdump).  Applies the voice's dirty flags (+0x28) to its output: either the software mixer voice (+0x11c,
 * output flag 8) or the DirectSound buffer (+0xdc IDirectSoundBuffer, +0xe0 IDirectSound3DBuffer, +0xe4 IKsPropertySet for the
 * EAX / I3DL2 properties), then updates the play position. */
#include "mfl_tier2.h"

typedef struct RwaDXVoice {
  char pad00[0x28];
  RwUInt32 flags;              /* +0x28 dirty / state bits, see VF_* */
  float frequency;             /* +0x2c */
  float volume;                /* +0x30 */
  float pan;                   /* +0x34 -1..1 */
  float gain3d;                /* +0x38 listener gain, smoothed */
  float pitch3d;               /* +0x3c listener frequency factor (doppler) */
  RwInt32 room, roomHF, roomRolloff;      /* +0x40 I3DL2 listener block as an_rwa_005c9c80.c (roomRolloff is a float, copied raw here) */
  char pad4c[0x70 - 0x4c];
  RwInt32 direct, obstruction, obstructionLFRatio, occlusion, occlusionLFRatio;      /* +0x70 .. +0x80 (the ratios are floats, copied raw) */
  char *wave;                  /* +0x84 (+0x2c sample rate, +0x38 bits, +0x39 channels, +0x54 flags: 2 = mono 3D) */
  RwUInt32 position;           /* +0x88 in sample frames */
  RwUInt32 cursor;             /* +0x8c play cursor (bytes) */
  RwUInt32 start;              /* +0x90 position to restart from (bytes) */
  char *owner;                 /* +0x94 output: +0x10 master volume, +0x14 listener, +0x4c, +0x50 caps (VO_*) */
  float minDistance;           /* +0x98 */
  RwUInt32 ds3dSize;           /* +0x9c DS3DBUFFER: dwSize */
  float position3d[3];         /* +0xa0 */
  float velocity3d[3];         /* +0xac */
  char padb8[0xd4 - 0xb8];
  float maxDistance;           /* +0xd4 (DS3DBUFFER.flMaxDistance) */
  char padd8[4];
  void **dsBuffer;             /* +0xdc IDirectSoundBuffer */
  void **ds3dBuffer;           /* +0xe0 IDirectSound3DBuffer */
  void **propertySet;          /* +0xe4 IKsPropertySet */
  RwUInt32 eaxBlock[0x34 / 4]; /* +0xe8 the voice's own EAX block */
  char *soft;                 /* +0x11c software mixer voice */
  struct RwaListLink { struct RwaListLink *next, **prevNext; } link;      /* +0x120 on the output's 3D pan voice list (flag 2) */
} RwaDXVoice;
typedef char RwaDXVoiceCheck[(sizeof(RwaDXVoice) == 0x128) ? 1 : -1];

#define VF_PLAYING_CHANGED 0x01
#define VF_3D 0x04
#define VF_EAX 0x08
#define VF_EAX_ROOM 0x10
#define VF_FREQUENCY 0x20
#define VF_VOLUME 0x40
#define VF_PAN 0x80
#define VF_RESTART 0x100
#define VF_PLAY 0x200
#define VF_SEEK 0x400
#define VF_LOOP 0x800
#define VF_FIRST 0x1000

/* software mixer voice fields */
#define SOFT_F(s, o) (*(float *)((s) + (o)))
#define SOFT_U(s, o) (*(RwUInt32 *)((s) + (o)))
#define BITS(x) (*(RwUInt32 *)&(x))      /* the original copies these floats with integer moves (bit exact) */

typedef struct { float position[3], velocity[3], minDistance, maxDistance; } RwaListenerIn;
typedef struct { float gain, pitch, pan; } RwaListenerOut;

/*005b9630*/ extern void _rwaListenerCalculateVoiceAttributes(void *listener, RwInt32 count, RwaListenerIn *in, RwaListenerOut *out, void *a5);
/*005cd3e0*/ extern RwInt32 _rwaDSBufferIsPlaying(void ***buffer);
/*005bf2b0*/ extern RwInt32 RwaMathConvertLinearToLog(float linear);
/*004b2c30*/ extern double __cdecl _pow(double x, double y);      /* the CRT function (cl would expand pow() to _CIpow) */
/*005d8530*/ extern RwUInt32 rwa_soft_voice_get_position_005d8530(char *soft);
/*005f85dc*/ extern undefined g_rwa_guid_eax20_buffer;
/*005f859c*/ extern undefined g_rwa_guid_eax10_buffer;
/*005f85fc*/ extern undefined g_rwa_guid_i3dl2_buffer;
/*005f860c*/ extern undefined g_rwa_guid_eax_buffer_dirty;

/* COM methods by vtable slot (stdcall, `this` first) */
#define COM(p, slot, T) ((T)(*(void ***)(p))[slot])
typedef RwInt32 (__stdcall *Ds1)(void *self);
typedef RwInt32 (__stdcall *Ds2)(void *self, RwUInt32 a);
typedef RwInt32 (__stdcall *Ds3p)(void *self, void *a, RwUInt32 b);
typedef RwInt32 (__stdcall *Ds4)(void *self, RwUInt32 a, RwUInt32 b, RwUInt32 c);
typedef RwInt32 (__stdcall *KsSet)(void *self, const void *guid, RwUInt32 id, void *inst, RwUInt32 instLen, void *data, RwUInt32 len);
#define DSB_GETCURRENTPOSITION 4
#define DSB_PLAY 12
#define DSB_SETCURRENTPOSITION 13
#define DSB_SETVOLUME 15
#define DSB_SETPAN 16
#define DSB_SETFREQUENCY 17
#define DSB_STOP 18
#define DS3DB_SETALLPARAMETERS 12
#define KS_SET 4

#define INT32_INDEFINITE (-2147483647 - 1)
/* a 32-bit fistp under chop rounding (the original's inline conversion): (int) in range, else the integer indefinite */
#define trunc32(x) (((x) >= -2147483648.0f && (x) < 2147483648.0f) ? (RwInt32)(x) : INT32_INDEFINITE)

/* pan -1..1 -> DirectSound pan in hundredths of a dB (-10000..10000) */
#define DS_PAN(p) ((((p) * 0.5f + 1.0f) * 10000.0f) - 10000.0f)

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwaDXVoice *rwa_dxaudio_voice_finish_execute_005c9fb0(RwaDXVoice *v)
{
  char *o = v->owner;
  RwUInt32 soft = *(RwUInt32 *)(o + 0x50) & 8;
  RwUInt32 f;
  RwInt32 playing;
  float t;

  if (!v->wave) {
    v->position = 0;
    v->flags &= 0xfffffa03;
    return v;
  }

  /* 3D position */
  if ((*(RwUInt8 *)(v->wave + 0x54) & 2) && *(void **)(o + 0x14)) {
    if (soft) {
      if (v->flags & VF_3D) {
        char *s = v->soft;

        SOFT_F(s, 0x64) = v->minDistance;
        SOFT_F(s, 0x68) = v->maxDistance;
        SOFT_F(s, 0x4c) = -v->position3d[0];
        SOFT_U(s, 0x50) = BITS(v->position3d[1]);
        SOFT_U(s, 0x54) = BITS(v->position3d[2]);
        SOFT_F(s, 0x58) = -v->velocity3d[0];
        SOFT_U(s, 0x5c) = BITS(v->velocity3d[1]);
        SOFT_U(s, 0x60) = BITS(v->velocity3d[2]);
        v->flags &= ~VF_3D;
      }
    } else {
      RwaListenerIn in;
      RwaListenerOut out;

      in.maxDistance = v->maxDistance;
      in.minDistance = v->minDistance;
      in.position[0] = -v->position3d[0];
      BITS(in.position[1]) = BITS(v->position3d[1]);
      BITS(in.position[2]) = BITS(v->position3d[2]);
      in.velocity[0] = -v->velocity3d[0];
      BITS(in.velocity[1]) = BITS(v->velocity3d[1]);
      BITS(in.velocity[2]) = BITS(v->velocity3d[2]);
      _rwaListenerCalculateVoiceAttributes(o + 0x14, 1, &in, &out, *(void **)(o + 0x4c));
      f = v->flags;
      if (f & VF_FIRST)
        BITS(v->gain3d) = BITS(out.gain);
      else
        v->gain3d = (out.gain - v->gain3d) * 0.5f + v->gain3d;
      f |= VF_VOLUME;
      v->flags = f;
      if (*(RwUInt8 *)(v->owner + 0x50) & 0x40) {      /* pitch and pan from the listener */
        BITS(v->pitch3d) = BITS(out.pitch);
        v->flags = f | VF_FREQUENCY;
        t = DS_PAN(out.pan + out.pan - 1.0f);
        COM(v->dsBuffer, DSB_SETPAN, Ds2)(v->dsBuffer, trunc32(t));
      } else if (f & VF_3D) {
        RwUInt32 *params = &v->ds3dSize;
        void **b3 = v->ds3dBuffer;

        COM(b3, DS3DB_SETALLPARAMETERS, Ds3p)(b3, params, 0);
        v->flags &= ~VF_3D;
      }
    }
  }

  /* first update: the frequency starts at the wave's sample rate */
  f = v->flags;
  if (f & VF_FIRST) {
    if (!(f & VF_FREQUENCY)) {
      __int64 rate = *(RwUInt32 *)(v->wave + 0x2c);

      v->flags |= VF_FREQUENCY;
      v->frequency = (float)rate;
    }
    v->flags &= ~VF_FIRST;
  }

  if ((*(RwUInt8 *)(v->owner + 0x50) & 7) && (v->flags & VF_EAX) && (v->flags & VF_PLAYING_CHANGED) && v->propertySet) {
    COM(v->propertySet, KS_SET, KsSet)(v->propertySet, &g_rwa_guid_eax_buffer_dirty, 0, 0, 0, v->eaxBlock, 0x34);
    v->flags &= ~VF_EAX;
  }

  if (v->flags & VF_FREQUENCY) {
    if (soft) {
      SOFT_F(v->soft, 0x34) = v->frequency;
      SOFT_U(v->soft, 0xcc) |= 4;
    } else {
      t = v->pitch3d * v->frequency;
      COM(v->dsBuffer, DSB_SETFREQUENCY, Ds2)(v->dsBuffer, trunc32(t));
    }
    v->flags &= ~VF_FREQUENCY;
  }

  if (v->flags & VF_VOLUME) {
    if (soft) {
      SOFT_F(v->soft, 0x3c) = v->volume;
      SOFT_U(v->soft, 0xcc) |= 0x10;
    } else {
      float master = *(float *)(v->owner + 0x10);      /* (master * gain3d) * volume, in this order */
      RwInt32 db = RwaMathConvertLinearToLog(v->volume * (master * v->gain3d));      /* cl keeps this evaluation order */

      COM(v->dsBuffer, DSB_SETVOLUME, Ds2)(v->dsBuffer, db);
    }
    v->flags &= ~VF_VOLUME;
  }

  if ((v->flags & VF_PAN) && v->wave && !(*(RwUInt32 *)(v->wave + 0x54) & 2)) {
    if (soft) {
      SOFT_U(v->soft, 0xcc) |= 2;
      SOFT_F(v->soft, 0x48) = v->pan;
    } else {
      t = DS_PAN(v->pan);
      COM(v->dsBuffer, DSB_SETPAN, Ds2)(v->dsBuffer, trunc32(t));
    }
    v->flags &= ~VF_PAN;
  }

  if (soft) {
    v->position = rwa_soft_voice_get_position_005d8530(v->soft);
    v->flags &= 0xfffffaef;
    return v;
  }

  /* DirectSound buffer: restart / play / seek, then the play position */
  playing = _rwaDSBufferIsPlaying((void ***)&v->dsBuffer);
  if (v->flags & VF_RESTART) {
    if (playing) {
      COM(v->dsBuffer, DSB_STOP, Ds1)(v->dsBuffer);
      playing = 0;
    }
    COM(v->dsBuffer, DSB_SETCURRENTPOSITION, Ds2)(v->dsBuffer, v->start);
    v->cursor = v->start;
    f = v->flags & ~VF_SEEK;
    v->flags = f;
    if (f & VF_PLAY) {
      if (COM(v->dsBuffer, DSB_PLAY, Ds4)(v->dsBuffer, 0, 0, (f >> 11) & 1) >= 0)
        playing = 1;
      v->start = 0;
    }
    v->flags &= ~(VF_RESTART | VF_PLAY);
  }
  if (v->flags & VF_SEEK) {
    COM(v->dsBuffer, DSB_SETCURRENTPOSITION, Ds2)(v->dsBuffer, v->cursor);
    v->flags &= ~VF_SEEK;
  }
  if (playing)
    COM(v->dsBuffer, DSB_GETCURRENTPOSITION, Ds3p)(v->dsBuffer, &v->cursor, 0);
  else
    v->cursor = v->start;
  v->position = v->cursor / (*(RwUInt8 *)(v->wave + 0x38) >> 3) / *(RwUInt8 *)(v->wave + 0x39);

  if (!(v->flags & VF_EAX_ROOM))
    return v;
  if (v->propertySet) {
#define caps (*(RwUInt8 *)(v->owner + 0x50))      /* read at each test, as the original does */

    RwInt32 p[13];      /* I3DL2 / EAX 2 buffer properties: direct, directHF, room, roomHF, roomRolloff, obstruction, obstructionLFRatio,
                           occlusion, occlusionLFRatio (I3DL2 ends here), occlusionRoomRatio, outsideVolumeHF, airAbsorption, flags */

    if (caps & 5) {      /* filled for I3DL2 and EAX 2 */
      p[0] = v->direct;
      p[1] = 0;
      p[2] = v->room;
      p[3] = v->roomHF;
      p[4] = v->roomRolloff;
      p[5] = v->obstruction;
      p[6] = v->obstructionLFRatio;
      p[7] = v->occlusion;
      p[8] = v->occlusionLFRatio;
    }
    if (caps & 1) {
      COM(v->propertySet, KS_SET, KsSet)(v->propertySet, &g_rwa_guid_i3dl2_buffer, 0, 0, 0, p, 0x24);
    } else if (caps & 2) {      /* EAX 1: the room level as a linear factor */
      float level = (float)_pow(10.0, v->room * 0.0005f);

      COM(v->propertySet, KS_SET, KsSet)(v->propertySet, &g_rwa_guid_eax10_buffer, 0, 0, 0, &level, 4);
    } else if (caps & 4) {
      *(float *)&p[9] = 0.5f;
      p[10] = v->roomHF;
      *(float *)&p[11] = 1.0f;
      p[12] = 1;
      COM(v->propertySet, KS_SET, KsSet)(v->propertySet, &g_rwa_guid_eax20_buffer, 1, 0, 0, p, 0x34);
    }
#undef caps
  }
  v->flags &= ~VF_EAX_ROOM;
  return v;
}

/*005ca7d0*/ extern void _rwaDXAudioVoiceSetWave(RwaDXVoice *voice, void *wave);
/*005baf80*/ extern void FUN_005baf80(void *obj, RwUInt32 a, RwUInt32 b);

/* MFL.exe 0x005c9f50 (the symbol table calls it _RwaDXAudioVoiceCreate; it takes a voice down): leaves the 3D pan voice list
   (flag 2), drops the wave, and passes (a, b) on to the software mixer voice when the output mixes itself. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void _RwaDXAudioVoiceCreate(RwaDXVoice *v, RwUInt32 a, RwUInt32 b)
{
  if (v->flags & 2) {
    *v->link.prevNext = v->link.next;
    v->link.next->prevNext = v->link.prevNext;
  }
  _rwaDXAudioVoiceSetWave(v, 0);
  if (*(RwUInt8 *)(v->owner + 0x50) & 8)
    FUN_005baf80(v->soft, a, b);
}
