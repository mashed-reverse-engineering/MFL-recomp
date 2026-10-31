/* mfl_video.h - platform independent movie interface (built in: recomp/video/mfl_video_null.c is the dispatcher + null backend).
 * Replaces the statically linked DirectShow code (strmbase + Criterion RwaDS, 0x4ac600..0x4b0b70,
 * 0x5b5eb0..0x5d09d0) on non-Windows targets. Backends (FFmpeg, libVLC, DirectShow shim) implement
 * struct mfl_video_backend. See public/docs/VIDEO_INTERFACE.md for the call-site mapping.
 * Single player: the game plays one movie at a time (globals g_app_76e8a4 raster, g_app_76e890 active). */
#ifndef MFL_VIDEO_H
#define MFL_VIDEO_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct mfl_video mfl_video;           /* opaque player handle */

typedef struct {
    int width, height;                         /* decoded frame size in pixels */
    int pitch;                                 /* bytes per row of pixels */
    const void *pixels;                        /* top-left, 32 bit BGRA (matches D3DFMT_A8R8G8B8 upload) */
} mfl_video_frame;

/* Open and start paused-or-playing. path = game-relative path from the movie table
 * (e.g. "toastart\\pc\\movies\\frontend.mpg"). loop: 0 = play once (all current callers).
 * Returns NULL on failure (original: message box "Could not ..." + HRESULT, see doc). */
mfl_video *mfl_video_open(const char *path, int loop);

/* Start/resume playback (original: IMediaControl::Run, called inside movie_renderer_create). */
int mfl_video_start(mfl_video *v);

/* Poll once per frame (original movie_renderer_poll: drains media events). Non-blocking.
 * Returns 1 if a NEW frame was decoded since the last acquire. Sets the finished flag on end of stream. */
int mfl_video_update(mfl_video *v);

/* Borrow the newest frame; valid until the next mfl_video_update/release. The game uploads it into the
 * RwRaster it created (g_app_76e8a4) or draws it as a textured quad (video_draw_textured_quad).
 * Returns 0 if no frame is available yet. */
int mfl_video_acquire_frame(mfl_video *v, mfl_video_frame *out);

/* 1 once the stream has ended (EC_COMPLETE equivalent). Sticky until close. */
int mfl_video_finished(const mfl_video *v);

/* Native video size (original: app_get_two_size_pairs, video vs texture size). */
void mfl_video_get_size(const mfl_video *v, int *w, int *h);

/* Volume 0.0..1.0 linear (original: sound_volume_to_directsound_db converts to 1/100 dB for
 * IBasicAudio::put_Volume; backends do their own mapping). */
void mfl_video_set_volume(mfl_video *v, float linear);

/* Stop and free (original: movie_stop = IBasicAudio/stop calls, movie_renderer_release, RwRasterDestroy). NULL ok. */
void mfl_video_close(mfl_video *v);

/* Backend registration (init-time, before the first mfl_video_open). */
typedef struct mfl_video_backend {
    const char *name;                          /* "ffmpeg", "libvlc", "directshow" */
    mfl_video *(*open)(const char *path, int loop);
    int  (*start)(mfl_video *);
    int  (*update)(mfl_video *);
    int  (*acquire_frame)(mfl_video *, mfl_video_frame *);
    int  (*finished)(const mfl_video *);
    void (*get_size)(const mfl_video *, int *, int *);
    void (*set_volume)(mfl_video *, float);
    void (*close)(mfl_video *);
} mfl_video_backend;
void mfl_video_set_backend(const mfl_video_backend *b);

#ifdef __cplusplus
}
#endif
#endif
