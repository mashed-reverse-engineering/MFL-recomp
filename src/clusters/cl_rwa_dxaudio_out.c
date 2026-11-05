// cluster-entries: RwaDXAudioOutputUnregister _RwaDXAudioOutputCreate _RwaDXAudioOutputFinishExecute _RwaDXAudioOutputGetCreationParams _rwaDXAudioOutputGetSpeakerConfig _rwaDXAudioOutputGetFreeVoice _rwaDSDMOInit _rwaDSDMOCreate _rwaDSDMODestroy _rwaDSDMOProcess _rwaDSDMOSetI3DLReverbParams
/* RenderWare Audio PC output (rwadxaudioout.c + the reverb DMO part of rwadsound.c), software-mixer path only, over
 * mfl_audio (recomp/mfl_audio.h) instead of DirectSound 8. Slice 1 of output/audio_route_2026-11-05.md.
 *
 * Function names are the recomp/symbols.txt names (several are wrong rwad-oracle guesses; the real role is in the
 * comment of each function). Layout of the output object is unchanged: RWA code that stays original
 * (_rwaDXAudioOutputSetGain / SetListener / SetSpeakerConfig / SetEnvironment, the DX voice and wave code, RwaSoftOutput)
 * reads the same fields at the same offsets.
 *
 * EQUIVALENCE ARGUMENT (diffemu cannot run DirectSound, so this is the review basis):
 *  E1 Mode. The original picks software mode when DirectSound has no free hardware 3D buffers (always on Wine and on
 *     Windows Vista+); with reverb requested (MFL: GetCreationParams default reverb=1) it sets flag 0x08 (RWA soft
 *     mixer + I3DL2 DMO) and fast3D 0x40 (no DS listener). The gdbcov runs confirm exactly this path (listener, EAX,
 *     _rwaDSBufferUpload never hit, DMO create + I3DL2 params hit). This port always takes it: flags = 0x08|0x20|0x40.
 *     Invariant: with 0x08 set, every DirectSound call site left in rwadxaudiovoice / rwadxaudiowave is behind a
 *     soft-voice (+0x11c) or flags&0x08 test (checked: 0x5cccd0 wave create, 0x5ca7d0 SetWave, 0x5c9fb0 voice flush),
 *     so the `device` slot (+0x54, was IDirectSound8*) is never used as a COM object.
 *  E2 Mixer input. The RWA mix callback (0x5d8d00, soft output) is called with the same argument (the period in bytes
 *     given to SetMixerCallback, 80 ms of 48 kHz stereo 16 bit = 15360 B for MFL) once per period of audio time, from one
 *     thread, never concurrently: the original mixer thread (0x5cc720) filled one half of a 2-period looping buffer when
 *     the write cursor crossed it (<= 1 fill per 10 ms poll); mfl_audio_stream_start guarantees the same contract. So the
 *     sequence of mix callback calls, and with the same game input the sequence of returned busses, is the same; only the
 *     wall-clock phase differs, as it does between two runs of the original.
 *  E3 Output. Original: pcm = sat16(dry + I3DL2(wet)) (0x5cc950 after DMO Process, in place: effects buffer unused
 *     because period >= 0.16 s of mono bytes for every rate with the default 160 ms latency). Here: the same when the
 *     backend offers reverb, dry only when it does not (null, miniaudio) - the original would have FAILED the stream
 *     when CoCreateInstance(I3DL2 DMO) failed; here the stream runs without reverb (deliberate).
 *  E4 Device format: primary SetFormat(2 ch, rate, bits) + GetFormat -> mfl_audio_open(want, got); got goes to +0x02/+0x04.
 *  E5 Dropped without replacement (backend's business, no RWA-visible state): SetCooperativeLevel (create and every 30
 *     FinishExecute calls), IDirectSound8::SetSpeakerConfig, GetCaps / probe buffers, ZoomFX and EAX probes.
 * Not ported (YAGNI, unreachable in this mode): hardware listener / EAX branches, the effects latency buffer.
 * ponytail: params are not written back (the original sets params->softwareMixing / fast3D in some combinations; with
 * the GetCreationParams defaults it writes nothing). */
#include <stddef.h>
#include <string.h>
#include "mfl_audio.h"

#define DXOUT_HWREVERB     0x007   /* hardware I3DL2 / EAX property sets: never set here */
#define DXOUT_SOFTMIXER    0x008   /* RWA mixes everything (RwaSoftOutput); DS got one stream + reverb DMO */
#define DXOUT_ZOOMFX       0x010
#define DXOUT_SOFTWARE     0x020   /* no hardware buffers */
#define DXOUT_FAST3D       0x040   /* no DirectSound 3D listener */
#define DXOUT_ENVDIRTY     0x080   /* env changed (set by _rwaDXAudioOutputSetEnvironment) */
#define DXOUT_SPEAKERDIRTY 0x100   /* set by _rwaDXAudioOutputSetSpeakerConfig */
#define DXOUT_GLOBALFOCUS  0x400

typedef mfl_audio_i3dl2 RwaEnvironment;          /* RWA's environment = the 12 I3DL2 fields in the same order */

typedef struct RwaDSDMO {                        /* 0x38 bytes, embedded in the output state */
    mfl_audio *device;                           /* +0x00 was IMediaObject * */
    int active;                                  /* +0x04 was IMediaObjectInPlace * */
    unsigned frameBytes;                         /* +0x08 device frame size (set by SetMixerCallback) */
    unsigned char unused[0x2c];                  /* was the two CMediaBuffer wrappers */
} RwaDSDMO;

typedef struct RwaMixerData { int numBusses; char **busses; } RwaMixerData;   /* busses[0] dry, busses[1] reverb send */
typedef RwaMixerData *(*RwaMixerCallback)(unsigned bytes, void *user);
typedef struct RwaMixerCallbackData { RwaMixerCallback callback; unsigned bytes; void *user; } RwaMixerCallbackData;

typedef struct RwaDXAudioOutputParams {          /* creation params, 0x28 bytes */
    void *hwnd;                                  /* 0x00 */
    const void *deviceGuid;                      /* 0x04 */
    int softwareMixing;                          /* 0x08 */
    int reverb;                                  /* 0x0c */
    int fast3D;                                  /* 0x10 */
    unsigned sampleRate;                         /* 0x14 */
    unsigned short bitDepth;                     /* 0x18 */
    unsigned short numVoices;                    /* 0x1a */
    unsigned latencyMs;                          /* 0x1c */
    int globalFocus;                             /* 0x20 */
    int speakerConfig;                           /* 0x24: 1 = stereo, else ask the device */
} RwaDXAudioOutputParams;

typedef struct RwaDXAudioOutputState {           /* at RwaObj + 0x28 */
    unsigned short numVoices;                    /* 0x00 */
    unsigned short bitDepth;                     /* 0x02 device format */
    unsigned sampleRate;                         /* 0x04 device format */
    void *hwnd;                                  /* 0x08 */
    unsigned short unused0c;                     /* 0x0c */
    unsigned short coopCounter;                  /* 0x0e was the SetCooperativeLevel refresh counter */
    float gain;                                  /* 0x10 */
    void *listener;                              /* 0x14 RwaListener object */
    float unknown18;                             /* 0x18 */
    RwaEnvironment env;                          /* 0x1c */
    int speakerConfig;                           /* 0x4c */
    unsigned flags;                              /* 0x50 DXOUT_* */
    mfl_audio *device;                           /* 0x54 was IDirectSound8 * */
    void *dsBuffer[3];                           /* 0x58 hardware probe buffer: stays NULL */
    void *dsListener[2];                         /* 0x64 stays NULL (fast3D) */
    void *voiceList[2];                          /* 0x6c list head */
    int streaming;                               /* 0x74 was the streaming DirectSound buffer */
    void *unused78[2];                           /* 0x78 */
    RwaDSDMO dmo;                                /* 0x80 */
    RwaMixerCallback mixer;                      /* 0xb8 */
    unsigned periodBytes;                        /* 0xbc */
    void *mixerUser;                             /* 0xc0 */
    unsigned char unusedC4[0x30];                /* 0xc4 buffer size, cursor, semaphore, thread, effects buffer: now the backend's */
    void *softOutput;                            /* 0xf4 RwaSoftOutput object */
} RwaDXAudioOutputState;

typedef struct RwaDXAudioOutput {
    unsigned char rwaObj[0x20];                  /* RwaObj header (core) */
    RwaDXAudioOutputState *state;                /* 0x20 = &s */
    unsigned char rwaObj24[4];
    RwaDXAudioOutputState s;                     /* 0x28 */
} RwaDXAudioOutput;

#if defined(_M_IX86) || defined(__i386__)         /* the layout must match the original object (0x120 bytes) */
typedef char dxout_check_flags[offsetof(RwaDXAudioOutput, s.flags) == 0x78 ? 1 : -1];
typedef char dxout_check_dmo[offsetof(RwaDXAudioOutput, s.dmo) == 0xa8 ? 1 : -1];
typedef char dxout_check_mixer[offsetof(RwaDXAudioOutput, s.mixer) == 0xe0 ? 1 : -1];
typedef char dxout_check_soft[offsetof(RwaDXAudioOutput, s.softOutput) == 0x11c ? 1 : -1];
typedef char dxout_check_size[sizeof(RwaDXAudioOutput) == 0x120 ? 1 : -1];
typedef char dxout_check_params[sizeof(RwaDXAudioOutputParams) == 0x28 ? 1 : -1];
#endif

typedef struct RwaObjHdr { unsigned char hdr[0x20]; unsigned char *state; } RwaObjHdr;
typedef struct RwaSoftOutputParams { void *output; unsigned numVoices; unsigned sampleRate; unsigned periodMs; } RwaSoftOutputParams;

/* original RWA code that stays */
/*005cbac0*/ extern int FUN_005cbac0(void *obj);              /* module open: CoInitialize + _rwaDSOpen on the first instance */
/*005cbb00*/ extern int FUN_005cbb00(void *obj);              /* module close */
/*005bace0*/ extern void *_rwaObjFinishExecute(void *obj, int flag);
/*005baf80*/ extern void *RwaObjDestroy(void *obj, int a2, int a3);
/*005d8da0*/ extern void *RwaSoftOutputCreate(RwaSoftOutputParams *params, int a2, int a3, int a4);
/*005cc2a0*/ extern void _rwaDXAudioOutputSetGain(void *obj, float gain);
/*005cc2e0*/ extern void _rwaDXAudioOutputSetListener(void *obj, void *listener);
/*005cc300*/ extern void _rwaDXAudioOutputSetSpeakerConfig(void *obj, int config);
/*005cc950*/ extern void FUN_005cc950(char *dst, char *a, char *b, unsigned bytes);   /* dst = sat16(a + b), tier 2 */

int  _rwaDXAudioOutputGetSpeakerConfig(RwaDXAudioOutput *obj, const RwaMixerCallbackData *data);
void _rwaDXAudioOutputGetFreeVoice(void *user, void *pcm, unsigned frames);

/* I3DL2 defaults the output starts with (room -10000 mB = reverb inaudible until the game sets an environment) */
static const RwaEnvironment k_default_env = { -10000, -10000, 0.0f, 1.49f, 0.83f, -2602, 0.007f, 200, 0.011f, 100.0f, 100.0f, 5000.0f };

/* ---- reverb DMO (rwadsound.c) -> mfl_audio_reverb_* ---- */

/*005cd810*/ void _rwaDSDMOInit(RwaDSDMO *dmo) { dmo->device = 0; dmo->active = 0; }

/*005cd830 was: CoCreateInstance(I3DL2 reverb DMO) + SetInputType/SetOutputType. Returns dmo, or NULL without reverb. */
RwaDSDMO *_rwaDSDMOCreate(mfl_audio *device, const RwaEnvironment *env, RwaDSDMO *dmo)
{
    _rwaDSDMOInit(dmo);
    dmo->device = device;
    dmo->active = mfl_audio_reverb_set(device, env);
    return dmo->active ? dmo : 0;
}

/*005cda00 was: Release + Flush */
void _rwaDSDMODestroy(RwaDSDMO *dmo)
{
    if (dmo->active) mfl_audio_reverb_set(dmo->device, 0);
    _rwaDSDMOInit(dmo);
}

/*005cda50 was: ProcessInput(src) / ProcessOutput(dest), `bytes` of device-format PCM (same 4 arguments). The backend
 * reverb works in place, so src is copied to dest first (src == dest: in place, the mixer's case). Returns dmo, NULL off. */
RwaDSDMO *_rwaDSDMOProcess(RwaDSDMO *dmo, void *src, void *dest, unsigned bytes)
{
    if (!dmo->active) return 0;
    if (src != dest) memcpy(dest, src, bytes);
    mfl_audio_reverb_process(dmo->device, dest, bytes / dmo->frameBytes);
    return dmo;
}

/*005cdb10 was: QI IDirectSoundFXI3DL2Reverb + SetAllParameters. Returns 1 on success. */
int _rwaDSDMOSetI3DLReverbParams(RwaDSDMO *dmo, const RwaEnvironment *env)
{
    return dmo->active ? mfl_audio_reverb_set(dmo->device, env) : 0;
}

/* ---- the output object (rwadxaudioout.c) ---- */

/*005cb570 really the output's create callback (oracle name wrong). Returns obj, or NULL when no device opens
 * (the original returns NULL when DirectSoundCreate8 fails). */
void *RwaDXAudioOutputUnregister(RwaDXAudioOutput *obj, void *size, const RwaDXAudioOutputParams *params)
{
    RwaDXAudioOutputState *s = &obj->s;
    mfl_audio_format want, got;
    RwaSoftOutputParams soft;
    (void)size;

    if (!FUN_005cbac0(obj)) return 0;
    s->numVoices = params->numVoices;
    s->hwnd = params->hwnd;
    s->unused0c = 0;
    s->coopCounter = 0;
    s->gain = 1.0f;
    s->unknown18 = 1.0f;
    s->listener = 0;
    s->device = 0;
    s->softOutput = 0;
    s->flags = DXOUT_SOFTMIXER | DXOUT_SOFTWARE | DXOUT_FAST3D;          /* E1 */
    if (params->globalFocus) s->flags |= DXOUT_GLOBALFOCUS;
    s->streaming = 0;
    _rwaDXAudioOutputGetSpeakerConfig(obj, 0);                            /* no mixer yet */

    want.rate = (int)params->sampleRate;
    want.channels = 2;
    want.bits = params->bitDepth;
    s->device = mfl_audio_open(&want, &got);                              /* E4 */
    if (!s->device) { FUN_005cbb00(obj); return 0; }
    s->sampleRate = (unsigned)got.rate;
    s->bitDepth = (unsigned short)got.bits;

    soft.output = obj;                                                    /* starts the stream (SetMixerCallback) */
    soft.numVoices = params->numVoices;
    soft.sampleRate = params->sampleRate;
    soft.periodMs = params->latencyMs >> 1;
    s->softOutput = RwaSoftOutputCreate(&soft, 4, 0, 0);
    if (!s->softOutput) { mfl_audio_close(s->device); s->device = 0; FUN_005cbb00(obj); return 0; }

    _rwaDXAudioOutputSetGain(obj, 1.0f);
    if (params->speakerConfig == 1)
        _rwaDXAudioOutputSetSpeakerConfig(obj, 1);
    else {
        s->speakerConfig = 1;                    /* what the original maps DSSPEAKER_STEREO to; RWA mixes <= 2 channels */
        s->flags &= ~DXOUT_SPEAKERDIRTY;
    }
    s->flags |= DXOUT_ENVDIRTY;                  /* flags & 0xf is always set here */
    s->env = k_default_env;
    s->voiceList[0] = s->voiceList[1] = &s->voiceList[0];
    return obj;
}

/*005cbb20 really the destroy callback */
int _RwaDXAudioOutputCreate(RwaDXAudioOutput *obj)
{
    RwaDXAudioOutputState *s = obj->state;
    _rwaDXAudioOutputGetSpeakerConfig(obj, 0);   /* stop the stream */
    _rwaDXAudioOutputSetListener(obj, 0);
    if (s->softOutput) RwaObjDestroy(s->softOutput, 0, 0);
    mfl_audio_close(s->device);
    s->device = 0;
    return FUN_005cbb00(obj);
}

/*005cbb90 once per RWA update (game thread) */
void *_RwaDXAudioOutputFinishExecute(RwaDXAudioOutput *obj)
{
    RwaDXAudioOutputState *s = &obj->s;
    _rwaObjFinishExecute(obj, 1);
    if (s->listener) {                           /* fast3D: no DS listener, just acknowledge the dirty bits */
        unsigned *dirty = (unsigned *)(((RwaObjHdr *)s->listener)->state + 0x48);
        if (*dirty & 0x7f) *dirty &= 0x80;
    }
    if ((s->flags & DXOUT_ENVDIRTY) && (s->flags & (DXOUT_HWREVERB | DXOUT_SOFTMIXER))) {
        if (s->flags & DXOUT_SOFTMIXER) _rwaDSDMOSetI3DLReverbParams(&s->dmo, &s->env);
        s->flags &= ~DXOUT_ENVDIRTY;
    }
    s->flags &= ~DXOUT_SPEAKERDIRTY;             /* E5: the speaker layout is the backend's */
    return obj;
}

/*005cc0d0 default creation params; the original opened a temporary DirectSound object to read its caps. Without
 * hardware buffers (the only case this port supports) it gives software mixing, 32 voices, fast3D. Returns objDef. */
void *_RwaDXAudioOutputGetCreationParams(void *objDef, RwaDXAudioOutputParams *p)
{
    mfl_audio_format want, got;
    mfl_audio *probe;

    p->hwnd = 0;
    p->deviceGuid = 0;
    p->softwareMixing = 0;
    p->reverb = 1;
    p->fast3D = 0;
    p->sampleRate = 48000;
    p->bitDepth = 16;
    p->latencyMs = 160;
    p->globalFocus = 0;
    p->speakerConfig = 1;
    want.rate = 48000;
    want.channels = 2;
    want.bits = 16;
    probe = mfl_audio_open(&want, &got);
    if (!probe) { p->numVoices = 0; return objDef; }                  /* = DirectSoundCreate8 failed */
    mfl_audio_close(probe);
    p->softwareMixing = 1;
    p->numVoices = 32;
    p->fast3D = 1;
    return objDef;
}

/*005cc3a0 really _rwaDXAudioOutputSetMixerCallback: data = start pulling the RWA mix, NULL = stop. Returns 1 while
 * a stream runs (callers ignore it). */
int _rwaDXAudioOutputGetSpeakerConfig(RwaDXAudioOutput *obj, const RwaMixerCallbackData *data)
{
    RwaDXAudioOutputState *s = &obj->s;
    unsigned frameBytes;

    if (!data) {
        if (s->streaming) {
            mfl_audio_stream_stop(s->device);    /* returns after the last fill */
            if (s->flags & DXOUT_SOFTMIXER) _rwaDSDMODestroy(&s->dmo);
            s->streaming = 0;
        }
        s->mixer = 0;
        s->periodBytes = 0;
        s->mixerUser = 0;
        return 0;
    }
    frameBytes = 2u * (s->bitDepth >> 3);
    if (s->flags & DXOUT_SOFTMIXER) _rwaDSDMOCreate(s->device, &s->env, &s->dmo);   /* E3: no reverb is not fatal */
    s->dmo.frameBytes = frameBytes;
    s->mixer = data->callback;
    s->periodBytes = data->bytes;
    s->mixerUser = data->user;
    if (!mfl_audio_stream_start(s->device, s->periodBytes / frameBytes, _rwaDXAudioOutputGetFreeVoice, s)) {
        if (s->flags & DXOUT_SOFTMIXER) _rwaDSDMODestroy(&s->dmo);
        s->mixer = 0;
        s->periodBytes = 0;
        s->mixerUser = 0;
        return 0;
    }
    s->streaming = 1;
    return 1;
}

/*005cc720 was the mixer thread body (rwad: _RwaDXAudioThread); now the fill callback, run on the backend's audio
 * thread once per period (E2). */
void _rwaDXAudioOutputGetFreeVoice(void *user, void *pcm, unsigned frames)
{
    RwaDXAudioOutputState *s = (RwaDXAudioOutputState *)user;
    RwaMixerData *mix = s->mixer(s->periodBytes, s->mixerUser);
    char *dry = mix->busses[0];

    if ((s->flags & DXOUT_SOFTMIXER) && s->dmo.active) {               /* E3 */
        char *wet = mix->busses[1];
        (void)frames;                                                   /* == periodBytes / frame size */
        _rwaDSDMOProcess(&s->dmo, wet, wet, s->periodBytes);           /* in place, as the original's in-place branch */
        FUN_005cc950((char *)pcm, dry, wet, s->periodBytes);
    } else
        memcpy(pcm, dry, s->periodBytes);
}
