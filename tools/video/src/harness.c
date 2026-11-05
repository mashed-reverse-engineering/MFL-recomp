/* harness.c [-rt] [-dump last.bgra] <movie.mpg>... : decodes each movie through the mfl_video exports and prints one FNV-1a 64 hash per BGRA frame,
 * then "<path> frames N size WxH". Default = clock-free (mfl_video_ffmpeg_next, every frame); -rt = the game's path
 * (start + update/acquire polled every 5 ms until finished, prints shown frames and wall ms).
 * Native: linked with mfl_video_ffmpeg.c. _WIN32: loads mfl_video_ffmpeg.dll with LoadLibrary like the exe does. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mfl_video.h"

#ifdef _WIN32
#include <windows.h>
static mfl_video *(*p_open)(const char *, int);
static int (*p_start)(mfl_video *), (*p_update)(mfl_video *), (*p_next)(mfl_video *), (*p_acquire)(mfl_video *, mfl_video_frame *);
static int (*p_finished)(const mfl_video *);
static void (*p_close)(mfl_video *);
#define LOAD(p, n) if (!(*(FARPROC *)&p = GetProcAddress(dll, n))) { printf("missing export %s\n", n); return 2; }
static int load(void)
{
    HMODULE dll = LoadLibraryA("mfl_video_ffmpeg.dll");
    if (!dll) { printf("LoadLibrary failed %lu\n", GetLastError()); return 2; }
    LOAD(p_open, "mfl_video_open") LOAD(p_start, "mfl_video_start") LOAD(p_update, "mfl_video_update")
    LOAD(p_next, "mfl_video_ffmpeg_next") LOAD(p_acquire, "mfl_video_acquire_frame") LOAD(p_finished, "mfl_video_finished")
    LOAD(p_close, "mfl_video_close")
    return 0;
}
static int64_t now_ms(void) { return GetTickCount(); }
static void sleep_ms(int ms) { Sleep(ms); }
#else
#include <time.h>
int mfl_video_ffmpeg_next(mfl_video *v);
#define p_open mfl_video_open
#define p_start mfl_video_start
#define p_update mfl_video_update
#define p_next mfl_video_ffmpeg_next
#define p_acquire mfl_video_acquire_frame
#define p_finished mfl_video_finished
#define p_close mfl_video_close
static int load(void) { return 0; }
static int64_t now_ms(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec * 1000LL + t.tv_nsec / 1000000; }
static void sleep_ms(int ms) { struct timespec t = {0, ms * 1000000L}; nanosleep(&t, NULL); }
#endif

static uint64_t fnv(const mfl_video_frame *f)
{
    uint64_t h = 1469598103934665603ULL;
    int x, y;
    for (y = 0; y < f->height; y++) {
        const unsigned char *p = (const unsigned char *)f->pixels + (size_t)y * f->pitch;
        for (x = 0; x < f->width * 4; x++) h = (h ^ p[x]) * 1099511628211ULL;
    }
    return h;
}

int main(int argc, char **argv)
{
    int i, rt = 0;
    const char *dump = NULL;
    if (load()) return 2;
    for (i = 1; i < argc; i++) {
        mfl_video *v;
        mfl_video_frame f;
        long n = 0;
        if (!strcmp(argv[i], "-rt")) { rt = 1; continue; }
        if (!strcmp(argv[i], "-dump") && i + 1 < argc) { dump = argv[++i]; continue; }
        v = p_open(argv[i], 0);
        if (!v) { printf("%s open FAILED\n", argv[i]); return 1; }
        p_start(v);
        if (!rt) {
            while (p_next(v)) {
                if (!p_acquire(v, &f)) { printf("acquire FAILED at %ld\n", n); return 1; }
                printf("%ld %016llx\n", n++, (unsigned long long)fnv(&f));
            }
        } else {
            int64_t t0 = now_ms();
            while (!p_finished(v)) {
                if (p_update(v) && p_acquire(v, &f)) n++;
                sleep_ms(5);
            }
            printf("%s rt shown %ld wall_ms %lld\n", argv[i], n, (long long)(now_ms() - t0));
        }
        p_acquire(v, &f);
        printf("%s frames %ld size %dx%d\n", argv[i], n, f.width, f.height);
        if (dump) {   /* last frame as raw BGRA, for a pixel sanity check against the reference ffmpeg */
            FILE *o = fopen(dump, "wb");
            int y;
            for (y = 0; o && y < f.height; y++) fwrite((const char *)f.pixels + (size_t)y * f.pitch, 4, f.width, o);
            if (o) fclose(o);
            dump = NULL;
        }
        p_close(v);
    }
    return 0;
}
