/* mfl_audio_null.c - dispatcher for recomp/mfl_audio.h plus the default "null" backend.
 *
 * Backend selection is fixed at link time: this file is the only backend in the game exe build, so the gates always run
 * on the null backend (no device, no sound, nothing host specific). A build that wants sound links a backend object
 * (mfl_audio_miniaudio.c) and calls mfl_audio_set_backend at init; the cl 13 exe can get it as a DLL only when compiled
 * with -DMFL_AUDIO_DLL (loads mfl_audio_miniaudio.dll, data export `mfl_audio_backend_miniaudio`).
 *
 * The null backend is a virtual device: open succeeds, and a thread pulls `fill` once per period of wall-clock time
 * (polling every 10 ms, at most one fill per poll, missed periods are skipped) - the pacing of the original DirectSound
 * mixer thread (0x5cc720). The mixed audio is discarded. A device that fails to open would stop RWA's software voices
 * (they only advance when the mixer is pulled), so streams would never end.
 * Deterministic: no device, same code path on every host; the period phase follows wall time, as in the original
 * (on Windows read straight from kernel32, see ticks_now: a harness that fakes the game's clock leaves the device alone).
 *
 * Same .bss rule as recomp/video/mfl_video_null.c: no initialised non-const statics (they shift the original bss in the
 * standalone link). NULL backend pointer = null backend. */
#include "mfl_audio.h"

/* ponytail: one static device and a fixed period buffer (no malloc, .bss only); 64 KiB = 340 ms of 48 kHz stereo 16 bit,
 * RWA uses 80 ms. Raise it if a caller ever asks for more (stream_start fails then). */
#define NULL_MAX_PERIOD_BYTES 65536

#ifdef _WIN32
#ifdef _MSC_VER
typedef __int64 ticks_t;                       /* cl 13 has no long long */
#else
typedef long long ticks_t;
#endif
extern __declspec(dllimport) void *__stdcall CreateThread(void *sa, unsigned long stack, unsigned long (__stdcall *fn)(void *), void *arg, unsigned long flags, unsigned long *id);
extern __declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *h, unsigned long ms);
extern __declspec(dllimport) int __stdcall CloseHandle(void *h);
extern __declspec(dllimport) void __stdcall Sleep(unsigned long ms);
extern __declspec(dllimport) int __stdcall QueryPerformanceCounter(ticks_t *t);
extern __declspec(dllimport) int __stdcall QueryPerformanceFrequency(ticks_t *f);
extern __declspec(dllimport) void *__stdcall GetModuleHandleA(const char *name);
extern __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
/* The device clock is real time, like the original's DirectSound play cursor.  It is fetched from kernel32 itself, past the
 * exe's import table: a test harness that virtualises the GAME's clock by patching the import table (tools/d3dtrace
 * vclock=1) must not speed the device up as well (it did: 9039 periods where the original ran 141). */
typedef int (__stdcall *qpc_fn)(ticks_t *t);
static const char k_kernel32[] = "kernel32.dll";
static const char k_qpc[] = "QueryPerformanceCounter";
static qpc_fn g_device_qpc;
static ticks_t ticks_now(void)
{
    ticks_t t;
    if (!g_device_qpc) g_device_qpc = (qpc_fn)GetProcAddress(GetModuleHandleA(k_kernel32), k_qpc);
    if (!g_device_qpc) g_device_qpc = (qpc_fn)QueryPerformanceCounter;
    g_device_qpc(&t);
    return t;
}
static ticks_t ticks_per_s(void) { ticks_t f; QueryPerformanceFrequency(&f); return f; }
static void nap_10ms(void) { Sleep(10); }
#else
#include <pthread.h>
#include <time.h>
typedef long long ticks_t;
static ticks_t ticks_now(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return (ticks_t)t.tv_sec * 1000000000LL + t.tv_nsec; }
static ticks_t ticks_per_s(void) { return 1000000000LL; }
static void nap_10ms(void) { struct timespec t; t.tv_sec = 0; t.tv_nsec = 10000000L; nanosleep(&t, 0); }
#endif

struct mfl_audio {
    int refs;                                  /* open count (GetCreationParams probes while nothing streams) */
    mfl_audio_format fmt;
    mfl_audio_fill_fn fill;
    void *user;
    unsigned period;                           /* frames */
    volatile unsigned long played;             /* frames handed out */
    volatile int stop;
#ifdef _WIN32
    void *thread;
#else
    pthread_t thread;
#endif
    int running;
    unsigned char pcm[NULL_MAX_PERIOD_BYTES];
};
static struct mfl_audio g_null_dev;

static void null_pump(struct mfl_audio *a)     /* audio thread body */
{
    ticks_t t0 = ticks_now(), hz = ticks_per_s();
    ticks_t slot = 0;                          /* last period slot filled */
    while (!a->stop) {
        ticks_t due = (ticks_now() - t0) * a->fmt.rate / hz / a->period;
        if (due > slot) {
            a->fill(a->user, a->pcm, a->period);
            a->played += a->period;
            slot = due;
        } else
            nap_10ms();
    }
}
#ifdef _WIN32
static unsigned long __stdcall null_thread(void *a) { null_pump((struct mfl_audio *)a); return 0; }
#else
static void *null_thread(void *a) { null_pump((struct mfl_audio *)a); return 0; }
#endif

static mfl_audio *null_open(const mfl_audio_format *want, mfl_audio_format *got)
{
    if (want->channels < 1 || want->channels > 2 || (want->bits != 8 && want->bits != 16) || want->rate <= 0) return 0;
    if (!g_null_dev.running) g_null_dev.fmt = *want;
    g_null_dev.refs++;
    *got = g_null_dev.fmt;
    return &g_null_dev;
}
static void null_get_caps(const mfl_audio *a, mfl_audio_caps *c) { (void)a; c->hw_voices = 0; c->reverb = 0; }
static int null_stream_start(mfl_audio *a, unsigned n, mfl_audio_fill_fn f, void *u)
{
    if (a->running || n * (unsigned)(a->fmt.channels * a->fmt.bits / 8) > NULL_MAX_PERIOD_BYTES) return 0;
    a->fill = f;
    a->user = u;
    a->period = n;
    a->played = 0;
    a->stop = 0;
#ifdef _WIN32
    a->thread = CreateThread(0, 0, null_thread, a, 0, 0);
    if (!a->thread) return 0;
#else
    if (pthread_create(&a->thread, 0, null_thread, a)) return 0;
#endif
    a->running = 1;
    return 1;
}
static void null_stream_stop(mfl_audio *a)
{
    if (!a->running) return;
    a->stop = 1;
#ifdef _WIN32
    WaitForSingleObject(a->thread, 0xffffffffUL);
    CloseHandle(a->thread);
#else
    pthread_join(a->thread, 0);
#endif
    a->running = 0;
}
static unsigned long null_get_position(const mfl_audio *a) { return a->played; }
static void null_close(mfl_audio *a)
{
    if (a->refs > 0 && --a->refs == 0) null_stream_stop(a);
}

static const mfl_audio_backend g_null_backend = {
    "null", null_open, null_get_caps, null_stream_start, null_stream_stop, null_get_position, null_close
};
static const mfl_audio_backend *g_backend_set;
#define B (g_backend_set ? g_backend_set : &g_null_backend)

#ifdef MFL_AUDIO_DLL
extern void *mfl_load_backend_dll(const char *name);      /* recomp/video/mfl_video_null.c (always in the exe): <exe folder>\mflng\<name>, else <name> beside the exe */
extern __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
static const char k_dll_name[] = "mfl_audio_miniaudio.dll";
static const char k_export[] = "mfl_audio_backend_miniaudio";
static int g_dll_tried;
static void dll_backend_load(void)
{
    void *dll;
    g_dll_tried = 1;
    dll = mfl_load_backend_dll(k_dll_name);
    if (dll) g_backend_set = (const mfl_audio_backend *)GetProcAddress(dll, k_export);   /* NULL = stay null */
}
#define DLL_BACKEND_LOAD() do { if (!g_dll_tried && !g_backend_set) dll_backend_load(); } while (0)
#else
#define DLL_BACKEND_LOAD() ((void)0)
#endif

void mfl_audio_set_backend(const mfl_audio_backend *b) { g_backend_set = b; }

mfl_audio *mfl_audio_open(const mfl_audio_format *want, mfl_audio_format *got) { DLL_BACKEND_LOAD(); return B->open(want, got); }
void mfl_audio_get_caps(const mfl_audio *a, mfl_audio_caps *c)
{
    if (a) B->get_caps(a, c); else null_get_caps(a, c);
    if (!B->voice_create) c->hw_voices = 0;              /* a backend without the voice slots never offers hardware voices */
    if (!B->reverb_process) c->reverb = 0;
}
int  mfl_audio_stream_start(mfl_audio *a, unsigned n, mfl_audio_fill_fn f, void *u) { return a && f && n ? B->stream_start(a, n, f, u) : 0; }
void mfl_audio_stream_stop(mfl_audio *a) { if (a) B->stream_stop(a); }
unsigned long mfl_audio_get_position(const mfl_audio *a) { return a ? B->get_position(a) : 0; }
void mfl_audio_close(mfl_audio *a) { if (a) B->close(a); }

int  mfl_audio_reverb_set(mfl_audio *a, const mfl_audio_i3dl2 *p) { return a && B->reverb_set ? B->reverb_set(a, p) : 0; }
void mfl_audio_reverb_process(mfl_audio *a, void *pcm, unsigned n) { if (a && B->reverb_process) B->reverb_process(a, pcm, n); }

/* hardware voices: only reachable when get_caps reported hw_voices > 0, i.e. the backend fills these slots */
mfl_audio_voice *mfl_audio_voice_create(mfl_audio *a, const mfl_audio_format *f, unsigned bytes, int is_3d)
{ return a && B->voice_create ? B->voice_create(a, f, bytes, is_3d) : 0; }
int  mfl_audio_voice_write(mfl_audio_voice *v, unsigned off, const void *src, unsigned n) { return v ? B->voice_write(v, off, src, n) : 0; }
int  mfl_audio_voice_play(mfl_audio_voice *v, int loop) { return v ? B->voice_play(v, loop) : 0; }
void mfl_audio_voice_stop(mfl_audio_voice *v) { if (v) B->voice_stop(v); }
int  mfl_audio_voice_is_playing(mfl_audio_voice *v) { return v ? B->voice_is_playing(v) : 0; }
void mfl_audio_voice_set_cursor(mfl_audio_voice *v, unsigned off) { if (v) B->voice_set_cursor(v, off); }
unsigned mfl_audio_voice_get_cursor(mfl_audio_voice *v) { return v ? B->voice_get_cursor(v) : 0; }
void mfl_audio_voice_set_volume(mfl_audio_voice *v, int mb) { if (v) B->voice_set_volume(v, mb); }
void mfl_audio_voice_set_pan(mfl_audio_voice *v, int mb) { if (v) B->voice_set_pan(v, mb); }
void mfl_audio_voice_set_frequency(mfl_audio_voice *v, unsigned hz) { if (v) B->voice_set_frequency(v, hz); }
void mfl_audio_voice_set_3d(mfl_audio_voice *v, const mfl_audio_3d *p) { if (v) B->voice_set_3d(v, p); }
void mfl_audio_voice_destroy(mfl_audio_voice *v) { if (v) B->voice_destroy(v); }
void mfl_audio_set_listener(mfl_audio *a, const mfl_audio_listener *l) { if (a && B->set_listener) B->set_listener(a, l); }
