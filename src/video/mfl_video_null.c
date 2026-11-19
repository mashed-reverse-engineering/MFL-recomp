/* mfl_video_null.c - dispatcher for recomp/mfl_video.h plus the default "blank" backend, used while no real backend (FFmpeg, libVLC) is
 * registered with mfl_video_set_backend at init.
 *
 * The blank backend plays a movie that opens fine, has no pixels and is over at once. This is what the original does when its movie
 * renderer is present but delivers nothing: the game marks the movie active, keeps the menu background movie active (keep flag) and draws
 * its video quad untextured and white (gfx_draw_movie_rect_2d via menu_layout_build_a, 512x512, first draw of every menu frame), while the
 * intro/credits movies end after the first poll and are skipped. Failing open instead would leave the menu panels on the black clear colour.
 *
 * DLL backend (Windows builds): at the first mfl_video_open with no backend registered, mfl_video_ffmpeg.dll (FFmpeg, built with mingw,
 * p8 package) is loaded with LoadLibrary and its plain cdecl exports (same names and signatures as mfl_video.h) become the backend.
 * No DLL or a missing export: the blank backend stays (behaviour unchanged). */
#include "mfl_video.h"

mfl_video *g_movie_video;                       /* the one movie the game plays at a time (NULL = none) */

static char blank_movie;                        /* the one blank movie: only its address matters */
static mfl_video *blank_open(const char *path, int loop) { (void)path; (void)loop; return (mfl_video *)&blank_movie; }
static int blank_start(mfl_video *v) { (void)v; return 1; }
static int blank_update(mfl_video *v) { (void)v; return 0; }                        /* never a new frame */
static int blank_acquire_frame(mfl_video *v, mfl_video_frame *out) { (void)v; (void)out; return 0; }
static int blank_finished(const mfl_video *v) { (void)v; return 1; }
static void blank_get_size(const mfl_video *v, int *w, int *h) { (void)v; *w = 0; *h = 0; }
static void blank_set_volume(mfl_video *v, float linear) { (void)v; (void)linear; }
static void blank_close(mfl_video *v) { (void)v; }

static const mfl_video_backend g_blank_backend = {
    "blank", blank_open, blank_start, blank_update, blank_acquire_frame, blank_finished, blank_get_size, blank_set_volume, blank_close
};
/* Must stay zero-initialised (.bss): an initialised static adds bytes to the standalone link's .data, which shifts the whole original
 * bss (DELTA layout, original code addresses it absolutely) by 0x20 -> crash at mfl_video_open. NULL means the blank backend. */
static const mfl_video_backend *g_backend_set;
#define g_backend (g_backend_set ? g_backend_set : &g_blank_backend)

#ifdef _WIN32
/* Same rule for the DLL backend: zero-initialised statics (.bss) and const tables (.rdata) only, no string literals (cl 13 without /GF
 * puts literals into .data). */
extern __declspec(dllimport) void *__stdcall LoadLibraryA(const char *name);
extern __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
static const char k_dll_name[] = "mfl_video_ffmpeg.dll";
static const char k_backend_name[] = "ffmpeg";
/* export names in mfl_video_backend slot order (open .. close), '\0' separated, empty name ends the table */
static const char k_exports[] = "mfl_video_open\0mfl_video_start\0mfl_video_update\0mfl_video_acquire_frame\0mfl_video_finished\0"
                                "mfl_video_get_size\0mfl_video_set_volume\0mfl_video_close\0";
static mfl_video_backend g_dll_backend;
static int g_dll_tried;

static void dll_backend_load(void)
{
    void *dll;
    void **slot = (void **)&g_dll_backend.open;
    const char *name = k_exports;
    g_dll_tried = 1;
    dll = LoadLibraryA(k_dll_name);
    if (dll == 0) return;
    for (; *name; slot++) {
        *slot = GetProcAddress(dll, name);
        if (*slot == 0) return;                 /* incomplete DLL: keep the blank backend (the DLL stays loaded, harmless) */
        while (*name++) ;
    }
    g_dll_backend.name = k_backend_name;
    g_backend_set = &g_dll_backend;
}
#define DLL_BACKEND_LOAD() do { if (!g_dll_tried && !g_backend_set) dll_backend_load(); } while (0)
#else
#define DLL_BACKEND_LOAD() ((void)0)
#endif

void mfl_video_set_backend(const mfl_video_backend *b) { g_backend_set = b; }
mfl_video *mfl_video_open(const char *path, int loop) { DLL_BACKEND_LOAD(); return g_backend->open(path, loop); }
int mfl_video_start(mfl_video *v) { return v ? g_backend->start(v) : 0; }
int mfl_video_update(mfl_video *v) { return v ? g_backend->update(v) : 0; }
int mfl_video_acquire_frame(mfl_video *v, mfl_video_frame *out) { return v ? g_backend->acquire_frame(v, out) : 0; }
int mfl_video_finished(const mfl_video *v) { return v ? g_backend->finished(v) : 1; }
void mfl_video_get_size(const mfl_video *v, int *w, int *h) { if (v) g_backend->get_size(v, w, h); else { *w = 0; *h = 0; } }
void mfl_video_set_volume(mfl_video *v, float linear) { if (v) g_backend->set_volume(v, linear); }
void mfl_video_close(mfl_video *v) { if (v) g_backend->close(v); }
