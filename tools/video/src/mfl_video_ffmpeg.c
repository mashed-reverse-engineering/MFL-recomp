/* mfl_video_ffmpeg.c - FFmpeg backend of recomp/mfl_video.h (public/docs/VIDEO_INTERFACE.md).
 * Built with i686 mingw into mfl_video_ffmpeg.dll (static minimal LGPL FFmpeg inside); the exe's dispatcher (recomp/video/mfl_video_null.c)
 * loads it with LoadLibrary and takes the plain cdecl exports below as its backend, so no mingw runtime enters the MSVC link.
 * Video only: the mp2 track is ignored (movie sound stays silent until the audio backend), set_volume is a no-op.
 * Single threaded: FFmpeg is built without thread support; call it from the game thread only.
 * Also builds natively (test/harness.c) for the offline determinism check. */
#include <stdlib.h>
#include <stdint.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include "mfl_video.h"

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
    int64_t t0;                    /* av_gettime_relative() at start, -1 = not started */
    int64_t pts0;                  /* pts of the first frame (AV_NOPTS_VALUE until seen) */
    int64_t next_us, show_us;      /* presentation times relative to the first frame, us */
    int64_t frame_dur_us;
    int64_t ndecoded;
};

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
        if (r != AVERROR(EAGAIN)) { v->ended = 1; return v->pending = 0; }   /* AVERROR_EOF after drain, or a decode error */
        if (v->eof) { v->ended = 1; return v->pending = 0; }
        r = av_read_frame(v->fmt, v->pkt);
        if (r < 0) { v->eof = 1; avcodec_send_packet(v->dec, NULL); continue; }   /* drain the decoder */
        if (v->pkt->stream_index == v->stream) avcodec_send_packet(v->dec, v->pkt);
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
    (void)loop;   /* ponytail: every caller passes 0; seek to 0 on end when a looping caller appears */
    if (!v) return NULL;
    v->t0 = -1;
    v->pts0 = AV_NOPTS_VALUE;
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
EXPORT void mfl_video_set_volume(mfl_video *v, float linear) { (void)v; (void)linear; }
