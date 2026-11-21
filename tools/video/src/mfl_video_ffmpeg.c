/* mfl_video_ffmpeg.c - FFmpeg backend of recomp/mfl_video.h (public/docs/VIDEO_INTERFACE.md).
 * Built with i686 mingw into mfl_video_ffmpeg.dll (static minimal LGPL FFmpeg inside); the exe's dispatcher (recomp/video/mfl_video_null.c)
 * loads it with LoadLibrary and takes the plain cdecl exports below as its backend, so no mingw runtime enters the MSVC link.
 * Sound (MENU W16): the mp2 track (frontend.mpg = the main-menu music, the intro movies' sound) is decoded on the game thread together with
 * the video packets into a ring buffer and played by an own miniaudio device (miniaudio converts the movie rate to the device rate); the
 * original played it through DirectShow.  set_volume maps like the original sound_volume_to_directsound_db (1/100 dB, see movie_gain).
 * Env MFL_VIDEO_AUDIO_DUMP=<file>: every frame handed to the device, appended raw s16 stereo at the movie rate (test evidence, as MFL_AUDIO_DUMP).
 * -DMFL_AUDIO_MA_NULL: miniaudio null device (Wine runs, no sound card).  FFmpeg calls stay on the game thread (built without threads);
 * the device thread only reads the ring.
 * Also builds natively (test/harness.c) for the offline determinism check. */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#include "miniaudio.h"
#include "mfl_video.h"

#define RING_FRAMES (1 << 17)      /* stereo frames, 2.7 s at 48 kHz: the demuxer reads audio at most a few video frames ahead */

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

struct mfl_video {
    AVFormatContext *fmt;
    AVCodecContext *dec;
    AVPacket *pkt;
    AVFrame *next;                 /* decoded, not yet due (valid while pending) */
    AVFrame *show;                 /* the frame on screen */
    struct SwsContext *sws;
    uint8_t *bgra;                 /* show converted to BGRA, pitch w*4 */
    int stream, w, h;
    int pending;                   /* next holds a frame */
    int shown;                     /* show holds a frame */
    int dirty;                     /* show changed since the last acquire */
    int eof, ended, finished;
    int loop;                      /* open(loop != 0): at the end of the stream seek back to the start instead of ending (the menu backdrop) */
    int64_t pass_start;            /* ndecoded when the current pass began (a pass without a frame ends the loop: no endless seek) */
    int64_t t0;                   /* av_gettime_relative() at start, -1 = not started */
    int64_t pts0;                  /* pts of the first frame (AV_NOPTS_VALUE until seen) */
    int64_t next_us, show_us;      /* presentation times relative to the first frame, us */
    int64_t frame_dur_us;
    int64_t ndecoded;
    /* sound: game thread writes ring/wr, device thread reads ring/rd (single producer, single consumer) */
    AVCodecContext *adec;
    AVFrame *af;
    int astream;                   /* -1 = no sound */
    int dev_live;
    ma_device dev;
    FILE *dump;
    volatile float gain;
    volatile uint32_t rd, wr;      /* frame counters, ring index = counter % RING_FRAMES */
    int16_t ring[RING_FRAMES * 2];
};

/* device thread: copy what the game thread decoded, silence on underrun */
static void audio_cb(ma_device *d, void *out, const void *in, ma_uint32 n)
{
    mfl_video *v = d->pUserData;
    int16_t *o = out;
    uint32_t rd = v->rd, have = __atomic_load_n(&v->wr, __ATOMIC_ACQUIRE) - rd, i;
    float g = v->gain;
    (void)in;
    for (i = 0; i < n; i++, o += 2) {
        if (i < have) {
            const int16_t *s = &v->ring[((rd + i) % RING_FRAMES) * 2];
            o[0] = (int16_t)(s[0] * g); o[1] = (int16_t)(s[1] * g);
        } else o[0] = o[1] = 0;
    }
    __atomic_store_n(&v->rd, rd + (have < n ? have : n), __ATOMIC_RELEASE);
    if (v->dump) { fwrite(out, 4, n, v->dump); fflush(v->dump); }   /* flush: the harness kills the process */
}

/* game thread: decode one audio packet into the ring (frames that do not fit are dropped) */
static void audio_feed(mfl_video *v, const AVPacket *pkt)
{
    if (avcodec_send_packet(v->adec, pkt) < 0) return;
    while (avcodec_receive_frame(v->adec, v->af) == 0) {
        int n = v->af->nb_samples, ch = v->af->ch_layout.nb_channels, planar = av_sample_fmt_is_planar(v->af->format), i;
        uint32_t wr = v->wr, room = RING_FRAMES - (wr - __atomic_load_n(&v->rd, __ATOMIC_ACQUIRE));
        const int16_t *l = (const int16_t *)v->af->extended_data[0], *r = planar && ch > 1 ? (const int16_t *)v->af->extended_data[1] : l;
        if (v->af->format != AV_SAMPLE_FMT_S16 && v->af->format != AV_SAMPLE_FMT_S16P) { av_frame_unref(v->af); continue; }
        if ((uint32_t)n > room) n = (int)room;
        for (i = 0; i < n; i++) {
            int16_t *d = &v->ring[((wr + i) % RING_FRAMES) * 2];
            if (planar) { d[0] = l[i]; d[1] = r[i]; }
            else { d[0] = l[i * ch]; d[1] = l[i * ch + (ch > 1)]; }
        }
        __atomic_store_n(&v->wr, wr + n, __ATOMIC_RELEASE);
        av_frame_unref(v->af);
    }
}

/* opens the mp2 decoder of the movie's audio track; any failure = a silent movie (as before) */
static void audio_open(mfl_video *v)
{
    const AVCodec *codec = NULL;
    v->astream = av_find_best_stream(v->fmt, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    if (v->astream < 0 || !codec) { v->astream = -1; return; }
    v->adec = avcodec_alloc_context3(codec);
    v->af = av_frame_alloc();
    if (!v->adec || !v->af || avcodec_parameters_to_context(v->adec, v->fmt->streams[v->astream]->codecpar) < 0 ||
        avcodec_open2(v->adec, codec, NULL) < 0 || v->adec->sample_rate <= 0) v->astream = -1;
}

static void audio_start(mfl_video *v)
{
#ifdef MFL_AUDIO_MA_NULL
    ma_backend only[1] = { ma_backend_null };
    ma_context_config cc = ma_context_config_init();
    static ma_context ctx; static int ctx_ok;
    if (!ctx_ok) ctx_ok = ma_context_init(only, 1, &cc, &ctx) == MA_SUCCESS;
    ma_context *pctx = ctx_ok ? &ctx : NULL;
#else
    ma_context *pctx = NULL;
#endif
    ma_device_config cfg;
    const char *p;
    if (v->astream < 0 || v->dev_live) return;
    cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format = ma_format_s16;
    cfg.playback.channels = 2;
    cfg.sampleRate = (ma_uint32)v->adec->sample_rate;
    cfg.dataCallback = audio_cb;
    cfg.pUserData = v;
    p = getenv("MFL_VIDEO_AUDIO_DUMP");
    v->dump = p && *p ? fopen(p, "ab") : NULL;
    if (ma_device_init(pctx, &cfg, &v->dev) != MA_SUCCESS) goto fail;
    v->dev_live = 1;
    if (ma_device_start(&v->dev) == MA_SUCCESS) return;
    ma_device_uninit(&v->dev);
    v->dev_live = 0;
fail:
    if (v->dump) { fclose(v->dump); v->dump = NULL; }
}

/* Loop: back to byte 0 (MPEG-PS, the original's EC_COMPLETE handler did put_CurrentPosition(0)), flush both decoders, and let the next frame
 * continue the timeline one frame after the last one (decode(): expect = show_us + frame_dur_us once ndecoded > 0, pts0 re-derived).
 * The audio packets keep flowing into the ring, so the sound loops without a gap. 1 = rewound, 0 = not looping / seek failed. */
static int rewind_stream(mfl_video *v)
{
    if (!v->loop || v->ndecoded <= v->pass_start || av_seek_frame(v->fmt, -1, 0, AVSEEK_FLAG_BYTE) < 0) return 0;
    avcodec_flush_buffers(v->dec);
    if (v->adec) avcodec_flush_buffers(v->adec);
    v->eof = 0;
    v->pts0 = AV_NOPTS_VALUE;
    v->pass_start = v->ndecoded;
    return 1;
}

/* Decodes the next video frame into v->next. 1 = frame, 0 = end of stream (sets ended). */
static int decode(mfl_video *v)
{
    for (;;) {
        int r = avcodec_receive_frame(v->dec, v->next);
        if (r == 0) {
            int64_t pts = v->next->best_effort_timestamp, expect;
            AVRational tb = v->fmt->streams[v->stream]->time_base;
            if (v->dec->framerate.num > 0 && v->dec->framerate.den > 0)   /* MPEG-1 sequence header rate (the stream's avg rate can be wrong) */
                v->frame_dur_us = av_rescale_q(1, av_inv_q(v->dec->framerate), AV_TIME_BASE_Q);
            expect = v->ndecoded ? v->show_us + v->frame_dur_us : 0;   /* one frame after the previous */
            if (pts == AV_NOPTS_VALUE) {
                v->next_us = expect;
            } else {
                if (v->pts0 == AV_NOPTS_VALUE) v->pts0 = pts - av_rescale_q(expect, AV_TIME_BASE_Q, tb);
                v->next_us = av_rescale_q(pts - v->pts0, tb, AV_TIME_BASE_Q);
            }
            v->ndecoded++;
            return v->pending = 1;
        }
        if (r != AVERROR(EAGAIN) || v->eof) {   /* AVERROR_EOF after drain, or a decode error */
            if (rewind_stream(v)) continue;
            v->ended = 1;
            return v->pending = 0;
        }
        r = av_read_frame(v->fmt, v->pkt);
        if (r < 0) { v->eof = 1; avcodec_send_packet(v->dec, NULL); continue; }   /* drain the decoder */
        if (v->pkt->stream_index == v->stream) avcodec_send_packet(v->dec, v->pkt);
        else if (v->pkt->stream_index == v->astream) audio_feed(v, v->pkt);
        av_packet_unref(v->pkt);
    }
}

/* Clock-free step: shows the pending frame and decodes the one after it. 1 = a frame was shown, 0 = none left.
 * update() is this step driven by the wall clock; the offline harness calls it directly. */
EXPORT int mfl_video_ffmpeg_next(mfl_video *v)
{
    AVFrame *t;
    if (!v->pending) return 0;
    t = v->show; v->show = v->next; v->next = t;
    av_frame_unref(v->next);
    v->show_us = v->next_us;
    v->shown = v->dirty = 1;
    decode(v);
    return 1;
}

EXPORT void mfl_video_close(mfl_video *v)
{
    if (!v) return;
    if (v->dev_live) ma_device_uninit(&v->dev);   /* stops and waits for the device thread: no audio_cb after this */
    if (v->dump) fclose(v->dump);
    av_frame_free(&v->af);
    avcodec_free_context(&v->adec);
    sws_freeContext(v->sws);
    av_free(v->bgra);
    av_frame_free(&v->next);
    av_frame_free(&v->show);
    av_packet_free(&v->pkt);
    avcodec_free_context(&v->dec);
    avformat_close_input(&v->fmt);
    free(v);
}

EXPORT mfl_video *mfl_video_open(const char *path, int loop)
{
    mfl_video *v = calloc(1, sizeof *v);
    const AVCodec *codec = NULL;
    AVRational fr;
    if (!v) return NULL;
    v->loop = loop;
    v->t0 = -1;
    v->pts0 = AV_NOPTS_VALUE;
    v->astream = -1;               /* set before the first decode(): open reads packets */
    v->gain = 1.0f;
    av_log_set_level(AV_LOG_QUIET);
    if (avformat_open_input(&v->fmt, path, NULL, NULL) < 0) { free(v); return NULL; }
    if (avformat_find_stream_info(v->fmt, NULL) < 0) goto fail;
    v->stream = av_find_best_stream(v->fmt, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
    if (v->stream < 0 || !codec) goto fail;
    v->dec = avcodec_alloc_context3(codec);
    if (!v->dec || avcodec_parameters_to_context(v->dec, v->fmt->streams[v->stream]->codecpar) < 0) goto fail;
    v->dec->flags |= AV_CODEC_FLAG_BITEXACT;
    v->dec->idct_algo = FF_IDCT_SIMPLE;   /* same C IDCT on every target -> same pixels */
    if (avcodec_open2(v->dec, codec, NULL) < 0) goto fail;
    v->w = v->dec->width;
    v->h = v->dec->height;
    fr = v->fmt->streams[v->stream]->avg_frame_rate;
    if (!fr.num || !fr.den) fr = v->dec->framerate.num ? v->dec->framerate : (AVRational){25, 1};
    v->frame_dur_us = av_rescale_q(1, av_inv_q(fr), AV_TIME_BASE_Q);
    v->pkt = av_packet_alloc();
    v->next = av_frame_alloc();
    v->show = av_frame_alloc();
    v->bgra = av_malloc((size_t)v->w * v->h * 4);
    if (!v->pkt || !v->next || !v->show || !v->bgra || v->w <= 0 || v->h <= 0) goto fail;
    audio_open(v);
    v->sws = sws_getContext(v->w, v->h, v->dec->pix_fmt, v->w, v->h, AV_PIX_FMT_BGRA,
                            SWS_BILINEAR | SWS_BITEXACT | SWS_ACCURATE_RND, NULL, NULL, NULL);
    if (!v->sws || !decode(v)) goto fail;   /* a stream without a frame fails open */
    return v;
fail:
    mfl_video_close(v);
    return NULL;
}

EXPORT int mfl_video_start(mfl_video *v)
{
    if (v->t0 < 0) v->t0 = av_gettime_relative();
    audio_start(v);
    return 1;
}

/* Shows every frame whose time has come (late ones are decoded, only the newest is converted at acquire).
 * Finished = no frame left and the last one has been on screen for one frame duration (EC_COMPLETE). */
EXPORT int mfl_video_update(mfl_video *v)
{
    int64_t now;
    if (v->t0 < 0 || v->finished) return 0;
    now = av_gettime_relative() - v->t0;
    while (v->pending && v->next_us <= now) mfl_video_ffmpeg_next(v);
    if (v->ended && !v->pending && now >= v->show_us + v->frame_dur_us) v->finished = 1;
    return v->dirty;
}

EXPORT int mfl_video_acquire_frame(mfl_video *v, mfl_video_frame *out)
{
    if (!v->shown) return 0;
    if (v->dirty) {
        uint8_t *dst[4] = {v->bgra, NULL, NULL, NULL};
        int dst_pitch[4] = {v->w * 4, 0, 0, 0};
        sws_scale(v->sws, (const uint8_t *const *)v->show->data, v->show->linesize, 0, v->h, dst, dst_pitch);
        v->dirty = 0;
    }
    out->width = v->w;
    out->height = v->h;
    out->pitch = v->w * 4;
    out->pixels = v->bgra;
    return 1;
}

EXPORT int mfl_video_finished(const mfl_video *v) { return v->finished; }
EXPORT void mfl_video_get_size(const mfl_video *v, int *w, int *h) { *w = v->w; *h = v->h; }
/* the original's mapping (sound_volume_to_directsound_db 0x4a3930): IBasicAudio volume in 1/100 dB = -2000 + 2000 * x from x = 0.1 up,
   -10000 + 10000 * x below; amplitude = 10^(dB/20) */
static float movie_gain(float x)
{
    float cdb = x >= 0.1f ? -2000.0f + 2000.0f * x : -10000.0f + 10000.0f * x;
    return x <= 0.0f ? 0.0f : cdb >= 0.0f ? 1.0f : powf(10.0f, cdb / 2000.0f);
}

EXPORT void mfl_video_set_volume(mfl_video *v, float linear) { v->gain = movie_gain(linear); }
