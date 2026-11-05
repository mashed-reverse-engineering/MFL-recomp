/* mfl_audio.h - platform independent audio output interface (dispatcher + null backend: recomp/audio/mfl_audio_null.c).
 * Replaces the DirectSound 8 calls of the RenderWare Audio (RWA) PC output layer, i.e. the win/dxaudio units at
 * 0x5c98e0..0x5cdc20 of MFL.exe (rwadxaudioout, rwadxaudiovoice, rwadxaudiowave, rwadsound). Backends (miniaudio,
 * DirectSound shim, SDL, ...) implement struct mfl_audio_backend. Call-site mapping: public/docs/AUDIO_INTERFACE.md.
 *
 * One device: RWA opens one output. Normal path (seen on Wine and on every Windows since Vista, where DirectSound has no
 * hardware 3D voices): RWA mixes all voices itself (RwaSoft*, 3D and resampling included) and only needs
 *   open -> stream_start(fill) -> ... -> stream_stop -> close, plus the optional in-place reverb.
 * The "hardware voice" section mirrors the DirectSound secondary-buffer path, which RWA only takes when the device
 * reports hardware 3D voices (caps.hw_voices > 0). Backends may leave all of it NULL. */
#ifndef MFL_AUDIO_H
#define MFL_AUDIO_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct mfl_audio mfl_audio;            /* opaque device */

typedef struct {
    int rate;                                  /* Hz (RWA asks for its output params, 44100 / 22050 ...) */
    int channels;                              /* 1 or 2 (RWA checks channels > 0 && <= 2) */
    int bits;                                  /* 8 (unsigned) or 16 (signed little endian); interleaved PCM */
} mfl_audio_format;

typedef struct {
    int hw_voices;                             /* free hardware 3D voices (DSCAPS dwFreeHw3DAllBuffers); 0 = RWA software mixer */
    int reverb;                                /* 1: mfl_audio_reverb_set/process work (original: I3DL2 reverb DMO) */
} mfl_audio_caps;

/* Fill exactly `frames` frames of `pcm` (device format, as returned by mfl_audio_open). Called by the backend from ONE
 * thread at a time, never concurrently, always with the period_frames given to mfl_audio_stream_start (the original
 * mixer thread also calls RWA with a fixed half-buffer size). May block briefly (RWA takes its own semaphores). */
typedef void (*mfl_audio_fill_fn)(void *user, void *pcm, unsigned frames);

/* Open the output (original: DirectSoundCreate8 + SetCooperativeLevel + primary buffer SetFormat).
 * `got` receives the format the fill callback must produce (backends that convert simply return *want).
 * NULL = no audio device; RWA then runs without output, as the original does when DirectSoundCreate8 fails.
 * The null backend never returns NULL: it is a silent virtual device whose thread pulls fill in real time. */
mfl_audio *mfl_audio_open(const mfl_audio_format *want, mfl_audio_format *got);
void mfl_audio_get_caps(const mfl_audio *a, mfl_audio_caps *caps);

/* Start pulling the mix (original: _rwaDXAudioOutputSetMixerCallback(cb): looping streaming buffer of 2 periods,
 * zero fill, Play(LOOPING), mixer thread at THREAD_PRIORITY_TIME_CRITICAL polling GetCurrentPosition every 10 ms).
 * Returns 1 on success. One stream per device. */
int  mfl_audio_stream_start(mfl_audio *a, unsigned period_frames, mfl_audio_fill_fn fill, void *user);
/* Stop (original: SetMixerCallback(NULL)). Returns after the last fill call has returned; fill is not called again. */
void mfl_audio_stream_stop(mfl_audio *a);
/* Audio clock: frames handed to the device since stream_start (32 bit, wraps after ~27 h at 44.1 kHz).
 * RwaTimer* (QueryPerformanceCounter / timeGetTime) is a host clock and NOT part of this interface. */
unsigned long mfl_audio_get_position(const mfl_audio *a);
/* Close (original: Release of the DirectSound object). Stops the stream first if needed. NULL ok. */
void mfl_audio_close(mfl_audio *a);

/* ---- optional: reverb (caps.reverb). Original: I3DL2 reverb DMO run in place on RWA's reverb bus inside the mixer
 * thread; RWA itself sums the processed bus into the dry mix. Units follow I3DL2 (mB = 1/100 dB). */
typedef struct {
    int   room, room_hf;                       /* mB */
    float room_rolloff_factor, decay_time, decay_hf_ratio;
    int   reflections;                         /* mB */
    float reflections_delay;                   /* s */
    int   reverb;                              /* mB */
    float reverb_delay, diffusion, density, hf_reference; /* s, %, %, Hz */
} mfl_audio_i3dl2;
int  mfl_audio_reverb_set(mfl_audio *a, const mfl_audio_i3dl2 *p);   /* NULL = off; returns 1 if the effect is active */
void mfl_audio_reverb_process(mfl_audio *a, void *pcm, unsigned frames); /* in place, device format; called from fill */

/* ---- optional: hardware voices (caps.hw_voices > 0). DirectSound units: volume/pan in mB (-10000..0 / -10000..10000),
 * 3D vectors in DirectSound's left-handed frame (RWA negates x before the call, as in the original). */
typedef struct mfl_audio_voice mfl_audio_voice;
typedef struct {
    float pos[3], vel[3];
    unsigned cone_inside, cone_outside;        /* degrees */
    float cone_dir[3];
    int   cone_outside_volume;                 /* mB */
    float min_distance, max_distance;
    int   mode;                                /* 0 normal, 1 head relative, 2 disabled (DS3DMODE_*) */
} mfl_audio_3d;
typedef struct {
    float pos[3], vel[3], front[3], top[3];
    float distance_factor, rolloff_factor, doppler_factor;
} mfl_audio_listener;
mfl_audio_voice *mfl_audio_voice_create(mfl_audio *a, const mfl_audio_format *f, unsigned bytes, int is_3d);
int  mfl_audio_voice_write(mfl_audio_voice *v, unsigned offset, const void *src, unsigned bytes); /* Lock+copy+Unlock */
int  mfl_audio_voice_play(mfl_audio_voice *v, int loop);
void mfl_audio_voice_stop(mfl_audio_voice *v);
int  mfl_audio_voice_is_playing(mfl_audio_voice *v);
void mfl_audio_voice_set_cursor(mfl_audio_voice *v, unsigned byte_offset);
unsigned mfl_audio_voice_get_cursor(mfl_audio_voice *v);   /* play cursor, bytes */
void mfl_audio_voice_set_volume(mfl_audio_voice *v, int mb);
void mfl_audio_voice_set_pan(mfl_audio_voice *v, int mb);
void mfl_audio_voice_set_frequency(mfl_audio_voice *v, unsigned hz);
void mfl_audio_voice_set_3d(mfl_audio_voice *v, const mfl_audio_3d *p);
void mfl_audio_voice_destroy(mfl_audio_voice *v);
void mfl_audio_set_listener(mfl_audio *a, const mfl_audio_listener *l);

/* Backend registration (init-time, before mfl_audio_open). Optional slots may be NULL. */
typedef struct mfl_audio_backend {
    const char *name;                          /* "miniaudio", "directsound", "null" */
    mfl_audio *(*open)(const mfl_audio_format *, mfl_audio_format *);
    void (*get_caps)(const mfl_audio *, mfl_audio_caps *);
    int  (*stream_start)(mfl_audio *, unsigned, mfl_audio_fill_fn, void *);
    void (*stream_stop)(mfl_audio *);
    unsigned long (*get_position)(const mfl_audio *);
    void (*close)(mfl_audio *);
    /* optional */
    int  (*reverb_set)(mfl_audio *, const mfl_audio_i3dl2 *);
    void (*reverb_process)(mfl_audio *, void *, unsigned);
    mfl_audio_voice *(*voice_create)(mfl_audio *, const mfl_audio_format *, unsigned, int);
    int  (*voice_write)(mfl_audio_voice *, unsigned, const void *, unsigned);
    int  (*voice_play)(mfl_audio_voice *, int);
    void (*voice_stop)(mfl_audio_voice *);
    int  (*voice_is_playing)(mfl_audio_voice *);
    void (*voice_set_cursor)(mfl_audio_voice *, unsigned);
    unsigned (*voice_get_cursor)(mfl_audio_voice *);
    void (*voice_set_volume)(mfl_audio_voice *, int);
    void (*voice_set_pan)(mfl_audio_voice *, int);
    void (*voice_set_frequency)(mfl_audio_voice *, unsigned);
    void (*voice_set_3d)(mfl_audio_voice *, const mfl_audio_3d *);
    void (*voice_destroy)(mfl_audio_voice *);
    void (*set_listener)(mfl_audio *, const mfl_audio_listener *);
} mfl_audio_backend;
void mfl_audio_set_backend(const mfl_audio_backend *b);

#ifdef __cplusplus
}
#endif
#endif
