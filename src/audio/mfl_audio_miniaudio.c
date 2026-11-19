/* mfl_audio_miniaudio.c - reference backend for recomp/mfl_audio.h on miniaudio (single header, public domain / MIT-0,
 * https://miniaud.io, tested with v0.11.21). Register at init: mfl_audio_set_backend(&mfl_audio_backend_miniaudio).
 *
 * Implements the core (open, caps, stream, position, close). Reports hw_voices = 0, so RWA takes its software mixer
 * path, the same path the original takes on Wine and on Windows Vista and later. Reverb is not implemented yet
 * (caps.reverb = 0: RWA's reverb bus is dropped, dry mix only).
 * ponytail: no reverb; add a portable I3DL2 reverb (reverb_set/reverb_process) when the dry-mix hashes are green.
 *
 * Fixed period: miniaudio asks for variable frame counts; RWA must be called with its fixed period (like the original
 * mixer thread, which always filled one half of the streaming buffer). data_cb keeps one period of mixed audio and
 * hands it out piecewise, calling fill only when it is used up.
 *
 * Build: compiled as its own translation unit with gcc / clang / mingw (not with the game's cl 13); define
 * MFL_AUDIO_MA_NULL to use miniaudio's null device (timer driven, no sound card; used by the self-check). */
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#include "miniaudio.h"
#include "mfl_audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mfl_audio {
    FILE *dump;                                /* MFL_AUDIO_DUMP=<file>: every frame handed to the device, appended raw (default off) */
    ma_context ctx;
    ma_device dev;
    int dev_live;                              /* dev initialised (stream running) */
    mfl_audio_format fmt;
    unsigned frame_bytes;
    mfl_audio_fill_fn fill;
    void *user;
    unsigned char *period;                     /* one period of mixed audio from fill */
    unsigned period_frames, period_used;       /* period_used == period_frames: refill before the next frame */
    volatile unsigned long played;             /* frames handed to the device (written by the audio thread only) */
};

static void data_cb(ma_device *d, void *out, const void *in, ma_uint32 n)
{
    mfl_audio *a = (mfl_audio *)d->pUserData;
    unsigned char *dst = (unsigned char *)out;
    void *out0 = out;
    ma_uint32 n0 = n;
    (void)in;
    while (n) {
        unsigned k;
        if (a->period_used == a->period_frames) {
            a->fill(a->user, a->period, a->period_frames);
            a->period_used = 0;
        }
        k = a->period_frames - a->period_used;
        if (k > n) k = n;
        memcpy(dst, a->period + a->period_used * a->frame_bytes, k * a->frame_bytes);
        a->period_used += k;
        dst += k * a->frame_bytes;
        n -= k;
        a->played += k;
    }
    if (a->dump) { fwrite(out0, a->frame_bytes, n0, a->dump); fflush(a->dump); }   /* flush: the harness kills the process, fclose never runs */
}

static mfl_audio *ma_open(const mfl_audio_format *want, mfl_audio_format *got)
{
    mfl_audio *a;
#ifdef MFL_AUDIO_MA_NULL
    ma_backend only[1] = { ma_backend_null };
    ma_uint32 nonly = 1;
#else
    ma_backend *only = NULL;
    ma_uint32 nonly = 0;
#endif
    if (want->channels < 1 || want->channels > 2 || (want->bits != 8 && want->bits != 16) || want->rate <= 0) return NULL;
    a = (mfl_audio *)calloc(1, sizeof *a);
    if (!a) return NULL;
    if (ma_context_init(only, nonly, NULL, &a->ctx) != MA_SUCCESS) { free(a); return NULL; }
    a->fmt = *want;
    a->frame_bytes = (unsigned)(want->channels * want->bits / 8);
    *got = *want;                              /* miniaudio converts to the device format itself */
    return a;
}

static void ma_get_caps(const mfl_audio *a, mfl_audio_caps *c) { (void)a; c->hw_voices = 0; c->reverb = 0; }

static int ma_stream_start(mfl_audio *a, unsigned period_frames, mfl_audio_fill_fn fill, void *user)
{
    ma_device_config cfg;
    if (a->dev_live) return 0;
    a->period = (unsigned char *)malloc((size_t)period_frames * a->frame_bytes);
    if (!a->period) return 0;
    a->fill = fill;
    a->user = user;
    a->period_frames = period_frames;
    a->period_used = period_frames;            /* first callback fills */
    a->played = 0;
    { const char *p = getenv("MFL_AUDIO_DUMP"); a->dump = p && *p ? fopen(p, "ab") : NULL; }
    cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format = a->fmt.bits == 8 ? ma_format_u8 : ma_format_s16;
    cfg.playback.channels = (ma_uint32)a->fmt.channels;
    cfg.sampleRate = (ma_uint32)a->fmt.rate;
    cfg.periodSizeInFrames = period_frames;    /* original: streaming buffer = 2 periods */
    cfg.periods = 2;
    cfg.dataCallback = data_cb;
    cfg.pUserData = a;
    if (ma_device_init(&a->ctx, &cfg, &a->dev) != MA_SUCCESS) { if (a->dump) { fclose(a->dump); a->dump = NULL; } free(a->period); a->period = NULL; return 0; }
    a->dev_live = 1;
    if (ma_device_start(&a->dev) != MA_SUCCESS) { ma_device_uninit(&a->dev); a->dev_live = 0; if (a->dump) { fclose(a->dump); a->dump = NULL; } free(a->period); a->period = NULL; return 0; }
    return 1;
}

static void ma_stream_stop(mfl_audio *a)
{
    if (!a->dev_live) return;
    ma_device_uninit(&a->dev);                 /* stops and waits for the audio thread: no fill after this */
    a->dev_live = 0;
    if (a->dump) { fclose(a->dump); a->dump = NULL; }
    free(a->period);
    a->period = NULL;
}

static unsigned long ma_get_position(const mfl_audio *a) { return a->played; }

static void ma_close(mfl_audio *a)
{
    ma_stream_stop(a);
    ma_context_uninit(&a->ctx);
    free(a);
}

#ifdef MFL_AUDIO_BUILD_DLL                     /* mingw DLL for the cl 13 exe (mfl_audio_null.c -DMFL_AUDIO_DLL loads it) */
__declspec(dllexport)
#endif
const mfl_audio_backend mfl_audio_backend_miniaudio = {
    "miniaudio", ma_open, ma_get_caps, ma_stream_start, ma_stream_stop, ma_get_position, ma_close
    /* reverb + hardware voice slots: NULL */
};
