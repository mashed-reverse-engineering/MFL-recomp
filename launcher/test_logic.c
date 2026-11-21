/* test_logic.c <releases.json> <MFLng-<tag>.zip> <tag of that zip>: native check of logic.c (build.sh test).
   releases.json = an answer of the GitHub releases API; the zip = the release asset of <tag> in the mflng/ sub folder layout (W17 LAYOUT C).
   Fixture pair without a download: launcher/mk_test_fixture.py <dir> (prints the three arguments); a real release of that layout works too.
   The flat zips before it (r20261120 ...) do not: their SHA256SUMS and BUILDINFO.txt are not in mflng/.
   Second half (update_tests): the update itself on folder trees, in <folder of the zip>/upd_work (deleted and rebuilt on every run).
   Native only (POSIX file calls): the launcher's Win32 file layer, elevation and locked files need the Wine scenarios / Windows. */
#pragma GCC diagnostic ignored "-Wformat-truncation"      /* test paths: built from one short work folder */
#include "logic.h"
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)

static char *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); char *b; long l;
    if (!f) { printf("cannot open %s\n", p); exit(2); }
    fseek(f, 0, SEEK_END); l = ftell(f); fseek(f, 0, SEEK_SET);
    b = (char *)malloc((size_t)l + 1); if (fread(b, 1, (size_t)l, f) != (size_t)l) exit(2); b[l] = 0; fclose(f);
    if (n) *n = (size_t)l;
    return b;
}

/* ================= the update on folder trees: upd_stage / upd_apply (logic.c, the code the launcher runs) over a POSIX file layer =================
   A scenario builds a game folder and a launcher data folder, runs an update and compares the COMPLETE resulting trees with the
   expected ones: a file too many (a leftover, a file of the release before) fails like a missing or a changed one. */
enum { ARENA = 32 << 20, TMAX = 180 };
static char *g_arena; static size_t g_ap;      /* every tree of a scenario; fresh() starts it again */
static void *ta(size_t n)
{
    void *p; n = (n + 15) & ~(size_t)15;
    if (g_ap + n > ARENA) { printf("test arena full\n"); exit(2); }
    p = g_arena + g_ap; g_ap += n; memset(p, 0, n);
    return p;
}
typedef struct { char name[128]; const char *data; size_t len; } TFile;
typedef struct { int n; TFile f[TMAX]; } Tree;      /* a set of files: a release, an expected or a loaded folder tree */
static Tree *t_new(void) { return (Tree *)ta(sizeof(Tree)); }
static const TFile *t_get(const Tree *t, const char *name) { int i; for (i = 0; i < t->n; i++) if (!strcmp(t->f[i].name, name)) return &t->f[i]; return NULL; }
static void t_set(Tree *t, const char *name, const void *d, size_t len)
{
    int i; char *c = (char *)ta(len + 1);
    memcpy(c, d, len);
    for (i = 0; i < t->n && strcmp(t->f[i].name, name); i++);
    if (i == t->n) { if (t->n == TMAX || strlen(name) > 127) { printf("test tree: too many files or a name too long: %s\n", name); exit(2); } strcpy(t->f[t->n++].name, name); }
    t->f[i].data = c; t->f[i].len = len;
}
static void t_puts(Tree *t, const char *name, const char *s) { t_set(t, name, s, strlen(s)); }
static void t_del(Tree *t, const char *name) { int i; for (i = 0; i < t->n; i++) if (!strcmp(t->f[i].name, name)) { t->f[i] = t->f[--t->n]; return; } }
static void t_add(Tree *t, const char *prefix, const Tree *o) { char nm[300]; int i; for (i = 0; i < o->n; i++) { snprintf(nm, sizeof nm, "%s%s", prefix, o->f[i].name); t_set(t, nm, o->f[i].data, o->f[i].len); } }
static Tree *both(const Tree *a, const char *prefix, const Tree *b) { Tree *t = t_new(); t_add(t, "", a); if (b) t_add(t, prefix, b); return t; }

/* a release: every file is "<name> build <v>", BUILDINFO names the tag of build v, SHA256SUMS lists every other file by its name */
#define BI "mflng/BUILDINFO.txt"
#define SUMS "mflng/SHA256SUMS"
#define SELF "MFLng-launch.exe"
static const char *const C1[] = { "MFLng.exe", "MFLng.nofix.exe", SELF, "mflrw.dll", "mflng/mfl_audio_miniaudio.dll", "mflng/mfl_video_ffmpeg.dll",
    "mflng/avformat-61.dll", "mflng/avcodec-61.dll", "mflng/avutil-59.dll", "mflng/swscale-8.dll", "mflng/INSTALL.txt", "mflng/LICENSES/zlib.txt", "mflng/LICENSES/ffmpeg.txt", NULL };
/* the layout before W17: everything beside the exes, LICENSES/ at the top */
static const char *const FLAT[] = { "MFLng.exe", "MFLng.nofix.exe", SELF, "mflrw.dll", "mfl_audio_miniaudio.dll", "mfl_video_ffmpeg.dll",
    "avformat-61.dll", "avcodec-61.dll", "avutil-59.dll", "swscale-8.dll", "INSTALL.txt", "LICENSES/zlib.txt", "LICENSES/ffmpeg.txt", NULL };
static void radd(Tree *r, int v, const char *name) { char b[200]; snprintf(b, sizeof b, "%s build %d\n", name, v); t_puts(r, name, b); }
static void sums(Tree *r, const char *where)
{
    static char b[TMAX * 200]; unsigned char h[32]; char hx[65]; int i; size_t l = 0;
    t_del(r, where); b[0] = 0;
    for (i = 0; i < r->n; i++) { sha256((const unsigned char *)r->f[i].data, r->f[i].len, h); hex32(h, hx); l += (size_t)sprintf(b + l, "%s  %s\n", hx, r->f[i].name); }
    t_puts(r, where, b);
}
static Tree *rel(int v, const char *const *names, const char *bi)      /* without the SHA256SUMS: sums() after the last change */
{
    Tree *r = t_new(); char b[200];
    for (; *names; names++) radd(r, v, *names);
    snprintf(b, sizeof b, "cl 19.44\nrelease r209901%02d-0000-abcdef0 prerelease\npublic commit abcdef0\n", v); t_puts(r, bi, b);
    return r;
}
static Tree *relC(int v) { Tree *r = rel(v, C1, BI); sums(r, SUMS); return r; }
static Tree *relmix(int v)      /* against relC: two files gone, three new (one a new licence), one DLL moved up beside the exes, mflrw.dll moved into mflng/ */
{
    Tree *r = rel(v, C1, BI);
    t_del(r, "mflng/avcodec-61.dll"); t_del(r, "mflng/LICENSES/zlib.txt"); t_del(r, "mflng/mfl_video_ffmpeg.dll"); t_del(r, "mflrw.dll");
    radd(r, v, "mflng/avcodec-62.dll"); radd(r, v, "mflng/LICENSES/opus.txt"); radd(r, v, "mflng/mfl_input_sdl.dll"); radd(r, v, "mfl_video_ffmpeg.dll"); radd(r, v, "mflng/mflrw.dll");
    sums(r, SUMS);
    return r;
}
static void w16(unsigned char **p, unsigned long v) { *(*p)++ = (unsigned char)(v & 255); *(*p)++ = (unsigned char)(v >> 8 & 255); }
static void w32(unsigned char **p, unsigned long v) { w16(p, v & 0xffff); w16(p, v >> 16 & 0xffff); }
static unsigned char *mkzip(const Tree *r, size_t *zn)      /* a stored zip with the two folder entries `zip -r` writes; malloc'd to its exact size (ASan sees a read past it) */
{
    const char *nm[TMAX + 2], *dt[TMAX + 2]; size_t ln[TMAX + 2], off[TMAX + 2], cap = 22, nl; unsigned long c; unsigned char *z, *p, *cd; int i, n = 0;
    nm[n] = "mflng/"; dt[n] = ""; ln[n++] = 0; nm[n] = "mflng/LICENSES/"; dt[n] = ""; ln[n++] = 0;
    for (i = 0; i < r->n; i++) { nm[n] = r->f[i].name; dt[n] = r->f[i].data; ln[n++] = r->f[i].len; }
    for (i = 0; i < n; i++) cap += 76 + 2 * strlen(nm[i]) + ln[i];
    p = z = (unsigned char *)malloc(cap);
    for (i = 0; i < n; i++) {
        c = crc32_buf((const unsigned char *)dt[i], ln[i]); nl = strlen(nm[i]); off[i] = (size_t)(p - z);
        w32(&p, 0x04034b50UL); w16(&p, 20); w16(&p, 0); w16(&p, 0); w16(&p, 0); w16(&p, 0x5421); w32(&p, c); w32(&p, ln[i]); w32(&p, ln[i]); w16(&p, nl); w16(&p, 0);
        memcpy(p, nm[i], nl); p += nl; memcpy(p, dt[i], ln[i]); p += ln[i];
    }
    cd = p;
    for (i = 0; i < n; i++) {
        c = crc32_buf((const unsigned char *)dt[i], ln[i]); nl = strlen(nm[i]);
        w32(&p, 0x02014b50UL); w16(&p, 20); w16(&p, 20); w16(&p, 0); w16(&p, 0); w16(&p, 0); w16(&p, 0x5421); w32(&p, c); w32(&p, ln[i]); w32(&p, ln[i]);
        w16(&p, nl); w16(&p, 0); w16(&p, 0); w16(&p, 0); w16(&p, 0); w32(&p, 0); w32(&p, off[i]);
        memcpy(p, nm[i], nl); p += nl;
    }
    c = (unsigned long)(p - cd);
    w32(&p, 0x06054b50UL); w16(&p, 0); w16(&p, 0); w16(&p, (unsigned long)n); w16(&p, (unsigned long)n); w32(&p, c); w32(&p, (unsigned long)(cd - z)); w16(&p, 0);
    *zn = (size_t)(p - z);
    return z;
}

/* ---- folders on disk ---- */
static void put(const char *root, const char *name, const void *d, size_t n)      /* with the folders it needs */
{
    char p[UPD_PATH], *c; FILE *f;
    snprintf(p, sizeof p, "%s/%s", root, name);
    for (c = p + 1; (c = strchr(c, '/')); c++) { *c = 0; mkdir(p, 0777); *c = '/'; }
    if (!(f = fopen(p, "wb")) || fwrite(d, 1, n, f) != n) { printf("cannot write %s\n", p); exit(2); }
    fclose(f);
}
static void puts_at(const char *root, const char *name, const char *s) { put(root, name, s, strlen(s)); }
static void unpack(const char *root, const Tree *t) { int i; for (i = 0; i < t->n; i++) put(root, t->f[i].name, t->f[i].data, t->f[i].len); }
static void t_load(Tree *t, const char *root, const char *sub)
{
    char p[UPD_PATH], nm[UPD_PATH]; DIR *d; struct dirent *e; struct stat s; size_t n; char *b;
    snprintf(p, sizeof p, "%s/%s", root, sub);
    if (!(d = opendir(p))) return;
    while ((e = readdir(d))) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
        snprintf(nm, sizeof nm, "%s%s%s", sub, *sub ? "/" : "", e->d_name);
        snprintf(p, sizeof p, "%s/%s", root, nm);
        if (stat(p, &s)) continue;
        if (S_ISDIR(s.st_mode)) t_load(t, root, nm);
        else { b = slurp(p, &n); t_set(t, nm, b, n); free(b); }
    }
    closedir(d);
}
static int same(const Tree *want, const char *root, const char *label)      /* the folder holds exactly these files with these bytes */
{
    Tree *got = t_new(); const TFile *g; int i, bad = 0;
    t_load(got, root, "");
    for (i = 0; i < want->n; i++) {
        if (!(g = t_get(got, want->f[i].name))) { printf("  [%s] missing: %s\n", label, want->f[i].name); bad++; }
        else if (g->len != want->f[i].len || memcmp(g->data, want->f[i].data, g->len)) { printf("  [%s] other content: %s\n", label, want->f[i].name); bad++; }
    }
    for (i = 0; i < got->n; i++) if (!t_get(want, got->f[i].name)) { printf("  [%s] not expected: %s\n", label, got->f[i].name); bad++; }
    return !bad;
}
#define TREE(want, root, label) do { if (!same(want, root, label)) { printf("FAIL line %d: tree '%s'\n", __LINE__, label); fails++; } } while (0)
static void rm_rf(const char *p)      /* only ever the test's own work folder */
{
    DIR *d; struct dirent *e; char q[UPD_PATH]; struct stat s;
    if (!strstr(p, "/upd_work")) { printf("rm_rf: not a test folder: %s\n", p); exit(2); }
    if (lstat(p, &s)) return;
    if (!S_ISDIR(s.st_mode)) { unlink(p); return; }
    if ((d = opendir(p))) {
        while ((e = readdir(d))) if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) { snprintf(q, sizeof q, "%s/%s", p, e->d_name); rm_rf(q); }
        closedir(d);
    }
    rmdir(p);
}

/* ---- the file layer (logic.h os_*) with injected failures: rule = fail the nth (0 = every) call of <op> whose "from>to" holds <pat>,
   only once rule <after> (1-based, 0 = none) has fired ---- */
typedef struct { const char *op, *pat; int nth, after, hits, fired; } Inj;
static Inj g_inj[4]; static int g_ninj;
static char g_log[1 << 16];
static void fail_on(const char *op, const char *pat, int nth, int after) { Inj j; j.op = op; j.pat = pat; j.nth = nth; j.after = after; j.hits = j.fired = 0; g_inj[g_ninj++] = j; }
static int inj(const char *op, const char *a, const char *b)
{
    char s[2 * UPD_PATH + 2]; int i;
    snprintf(s, sizeof s, "%s>%s", a, b ? b : "");
    for (i = 0; i < g_ninj; i++) {
        Inj *j = &g_inj[i];
        if (strcmp(j->op, op) || !strstr(s, j->pat) || (j->after && !g_inj[j->after - 1].fired)) continue;
        if (++j->hits == j->nth || !j->nth) { j->fired = 1; errno = EACCES; return 1; }
    }
    return 0;
}
static int raw_write(const char *p, const void *b, size_t n)
{
    FILE *f = fopen(p, "wb"); int ok;
    if (!f) return 0;
    ok = fwrite(b, 1, n, f) == n; ok &= !fclose(f);
    if (!ok) unlink(p);
    return ok;
}
unsigned char *os_read(const char *p, size_t *n)
{
    struct stat s; FILE *f; unsigned char *b;
    if (stat(p, &s) || !S_ISREG(s.st_mode) || !(f = fopen(p, "rb"))) return NULL;
    b = (unsigned char *)malloc((size_t)s.st_size + 1);
    if (fread(b, 1, (size_t)s.st_size, f) != (size_t)s.st_size) { free(b); b = NULL; } else { b[s.st_size] = 0; if (n) *n = (size_t)s.st_size; }
    fclose(f);
    return b;
}
int os_write(const char *p, const void *b, size_t n) { return !inj("write", p, NULL) && raw_write(p, b, n); }
int os_exists(const char *p) { struct stat s; return !stat(p, &s); }
int os_rename(const char *a, const char *b) { return !inj("rename", a, b) && !rename(a, b); }
int os_copy(const char *a, const char *b)
{
    size_t n = 0; unsigned char *d; int ok;
    if (inj("copy", a, b) || !(d = os_read(a, &n))) return 0;
    ok = raw_write(b, d, n); free(d);
    return ok;
}
int os_delete(const char *p) { return !inj("delete", p, NULL) && (!unlink(p) || errno == ENOENT); }
int os_mkdir(const char *p) { return !inj("mkdir", p, NULL) && !mkdir(p, 0777); }
int os_rmdir(const char *p) { return !rmdir(p); }
void os_rmtree(const char *p) { rm_rf(p); }
long os_err(void) { return errno; }
void os_list(const char *dir, void (*f)(const char *name, void *ctx), void *ctx)
{
    static char nm[256][256]; char p[UPD_PATH]; DIR *d = opendir(dir); struct dirent *e; struct stat s; int n = 0, i;
    if (!d) return;
    while ((e = readdir(d)) && n < 256) { snprintf(p, sizeof p, "%s/%s", dir, e->d_name); if (!stat(p, &s) && S_ISREG(s.st_mode)) strcpy(nm[n++], e->d_name); }
    closedir(d);
    for (i = 0; i < n; i++) f(nm[i], ctx);      /* after the listing: the callback deletes */
}
void os_log(const char *fmt, ...)
{
    size_t l = strlen(g_log); va_list a;
    if (l > sizeof g_log - 3000) return;
    va_start(a, fmt); vsnprintf(g_log + l, 2900, fmt, a); va_end(a); strcat(g_log, "\n");
}

/* ---- scenarios ---- */
static char WORK[UPD_PATH / 4], INST[UPD_PATH / 2], DATA[UPD_PATH / 2], g_serr[300];
static Tree *G, *INI;      /* the game's own files; the launcher's data folder before any update */
static void fresh(const char *name)      /* a game folder without MFLng, a data folder with the ini; every tree made before is gone */
{
    char w[UPD_PATH / 2];
    g_ap = 0; g_ninj = 0; g_log[0] = 0;
    snprintf(w, sizeof w, "%s/%s", WORK, name); rm_rf(w);
    snprintf(INST, sizeof INST, "%s/game", w); snprintf(DATA, sizeof DATA, "%s/data", w);
    G = t_new();
    t_puts(G, "MFL.exe", "the game\n"); t_puts(G, "launch.exe", "the game's launcher\n"); t_puts(G, "gamesave.bin", "save\n"); t_puts(G, "videocfg.bin", "video\n");
    t_puts(G, "Readme.txt", "the game's own readme\n"); t_puts(G, "mflng_settings.cfg", "prefs\n"); t_puts(G, "TOASTART/PC/Setup.bmp", "picture\n");
    INI = t_new(); t_puts(INI, "mflng-launch.ini", "[launcher]\nchannel=prerelease\n");
    unpack(INST, G); unpack(DATA, INI);
}
static int stage(const Tree *r, const char *tag)
{
    size_t zn; unsigned char *z = mkzip(r, &zn); char st[UPD_PATH]; int ok;
    snprintf(st, sizeof st, "%s/update", DATA);
    ok = upd_stage(z, zn, st, tag, g_serr, sizeof g_serr); free(z);
    return ok;
}
static int update(const Tree *r) { g_ninj = 0; return stage(r, NULL) ? upd_apply(INST, DATA, SELF) : 2; }      /* 2 = refused when staging */
static void settle(void)      /* what the next launcher start does (cleanup_thread) and the next update's first step */
{
    char p[UPD_PATH];
    g_ninj = 0; CHECK(upd_clean(INST) == 0);
    snprintf(p, sizeof p, "%s/update", DATA); upd_clear(p);
    snprintf(p, sizeof p, "%s/backup.prev", DATA); upd_clear(p);
}
static int at(const char *root, const char *name) { char p[UPD_PATH]; snprintf(p, sizeof p, "%s/%s", root, name); return os_exists(p); }

/* one failure at every point of an update: the nth write / rename / copy / mkdir / delete fails.  Then either the update went through
   all the same, or the game folder and the data folder are exactly as before (bare: the folders it made are gone too). */
static void sweep(const char *name, int bare)
{
    static const char *const ops[] = { "write", "rename", "copy", "mkdir", "delete" };
    int o, nth, r, hit, runs = 0, back = 0; char label[80];
    for (o = 0; o < 5; o++) for (nth = 1, hit = 1; hit; nth++) {
        Tree *v1, *nx, *older;
        fresh(name); v1 = relC(1); nx = relmix(3);
        older = t_new(); t_puts(older, "backup/MFLng.exe", "the backup level before\n");
        if (!bare) { unpack(INST, v1); unpack(DATA, older); }
        CHECK(stage(nx, NULL));
        fail_on(ops[o], "", nth, 0);
        r = upd_apply(INST, DATA, SELF); hit = g_inj[0].fired;
        snprintf(label, sizeof label, "%s: %s %d fails -> %d", name, ops[o], nth, r);
        settle(); runs++;
        if (r == 1) { TREE(both(G, "", nx), INST, label); TREE(bare ? INI : both(INI, "backup/", v1), DATA, label); }
        else {
            back++;
            if (r) { printf("FAIL: %s\n", label); fails++; }
            TREE(bare ? G : both(G, "", v1), INST, label); TREE(bare ? INI : both(INI, "", older), DATA, label);
            if (bare) CHECK(!at(INST, "mflng"));
        }
    }
    printf("  sweep %s: %d updates with one failure each, %d rolled back, the others completed\n", name, runs, back);
}

static void update_tests(const char *zip_path)
{
    static const char *const bad[] = {
        "INSTALL.txt", "BUILDINFO.txt", "SHA256SUMS", "readme.md",                                   /* no text file beside the exes any more */
        "MFL.exe", "MFL_original.exe", "launch.exe", "gamesave.bin", "d3d9.dll", "mflng/d3d9.dll",       /* game files, foreign DLLs */
        "../MFLng.exe", "mflng/../MFL.exe", "mflng/../../x.txt", "/abs.txt", "C:/x.txt", "C:x.txt",      /* out of the game folder */
        "mflng\\mfl_evil.dll", "..\\mfl_evil.dll", "MFLng.exe:ads", "mflng/mfl_x.dll:s.txt",              /* backslash, alternate data stream */
        "mflng/mfl_x.dll.", "mflng/mfl_x.dll ", "MFLNG/mfl_x.dll", "mflng/MFLNG.EXE",                    /* trailing dot / space, case variants */
        "mflng/mflng/mflrw.dll", "LICENSES/zlib.txt", "mflng/LICENSES/sub/x.txt", "mflng/mods/x.txt",    /* other folders */
        "mflng/nul.txt", "mflng/NUL.tar.txt", "mflng/LICENSES/COM1.txt", "mflng/.hidden.txt",            /* device names, dot files */
        "mflng/install.txt",                                                                         /* = mflng/INSTALL.txt on Windows: twice */
        NULL };
    Tree *v1, *v2, *v3, *v4, *f1, *f2, *b, *want; char p[UPD_PATH], st[UPD_PATH]; unsigned char *z; size_t zn; ZipEntry ze[UPD_MAXF]; int i, k, n;
    snprintf(WORK, sizeof WORK, "%s", zip_path);
    if (strrchr(WORK, '/')) *strrchr(WORK, '/') = 0; else strcpy(WORK, ".");
    strcat(WORK, "/upd_work"); rm_rf(WORK);
    g_arena = (char *)malloc(ARENA);

    /* 1. a first install, then: every file replaced; files new and gone in mflng/ and DLLs moved between the game folder and mflng/; back again */
    fresh("chain");
    v1 = relC(1); CHECK(update(v1) == 1);
    TREE(both(G, "", v1), INST, "first install (mflng/ and mflng/LICENSES/ are made)"); TREE(INI, DATA, "first install: data folder");
    CHECK(strstr(g_log, "replaced mflng/mfl_audio_miniaudio.dll (new file)") != NULL);
    v2 = relC(2); CHECK(update(v2) == 1);
    TREE(both(G, "", v2), INST, "every file replaced"); TREE(both(INI, "backup/", v1), DATA, "every file replaced: backup = build 1");
    v3 = relmix(3); g_log[0] = 0; CHECK(update(v3) == 1);
    TREE(both(G, "", v3), INST, "new, removed and moved files"); TREE(both(INI, "backup/", v2), DATA, "new, removed and moved: backup = build 2, one level");
    CHECK(strstr(g_log, "removed mflng/avcodec-61.dll") && strstr(g_log, "removed mflrw.dll") && strstr(g_log, "replaced mflng/mflrw.dll (new file)"));
    printf("---- update.log of the update with new, removed and moved files ----\n%s----\n", g_log);
    v4 = relC(4); CHECK(update(v4) == 1);
    TREE(both(G, "", v4), INST, "moved back"); TREE(both(INI, "backup/", v3), DATA, "moved back: backup = build 3");

    /* 2. what the user put there stays: a release file he changed that the next release drops, his own files in mflng/, a folder of his */
    fresh("user");
    v1 = relC(1); unpack(INST, v1);
    puts_at(INST, "mflng/avutil-59.dll", "the user's own FFmpeg build\n"); puts_at(INST, "mflng/notes.txt", "mine\n"); puts_at(INST, "mflng/mfl_custom.dll", "mine\n");
    puts_at(INST, "mflng/mods/readme.txt", "mine\n"); puts_at(INST, "portable.txt", "");
    v2 = rel(2, C1, BI); t_del(v2, "mflng/avutil-59.dll"); t_del(v2, "mflng/swscale-8.dll"); sums(v2, SUMS);
    CHECK(update(v2) == 1);
    want = both(G, "", v2); t_puts(want, "mflng/avutil-59.dll", "the user's own FFmpeg build\n"); t_puts(want, "mflng/notes.txt", "mine\n");
    t_puts(want, "mflng/mfl_custom.dll", "mine\n"); t_puts(want, "mflng/mods/readme.txt", "mine\n"); t_puts(want, "portable.txt", "");
    TREE(want, INST, "the user's files stay, the unchanged dropped file goes");
    CHECK(strstr(g_log, "kept mflng/avutil-59.dll") && strstr(g_log, "removed mflng/swscale-8.dll"));

    /* 3. a zip without the launcher: the running launcher is never removed */
    fresh("self");
    v1 = relC(1); unpack(INST, v1);
    v2 = rel(2, C1, BI); t_del(v2, SELF); sums(v2, SUMS);
    CHECK(update(v2) == 1);
    want = both(G, "", v2); t_set(want, SELF, t_get(v1, SELF)->data, t_get(v1, SELF)->len);
    TREE(want, INST, "the launcher stays when the zip has none");

    /* 4. refused zips: nothing is staged, nothing changes.  One file the launcher does not install refuses the whole zip. */
    fresh("refuse");
    v1 = relC(1); unpack(INST, v1); want = both(G, "", v1);
    for (i = 0; bad[i]; i++) {
        b = rel(2, C1, BI); radd(b, 2, bad[i]); sums(b, SUMS);
        k = update(b);
        if (k != 2) { printf("FAIL: a zip with '%s' was not refused (%d)\n", bad[i], k); fails++; }
        TREE(want, INST, bad[i]); TREE(INI, DATA, bad[i]);
    }
    printf("  %d zips with one file too many refused, e.g.: %s\n", i, g_serr);
    b = rel(2, C1, BI); CHECK(update(b) == 2);                                              /* no mflng/SHA256SUMS */
    f2 = rel(2, FLAT, "BUILDINFO.txt"); sums(f2, "SHA256SUMS"); CHECK(update(f2) == 2);     /* a zip of the flat layout */
    printf("  flat zip: %s\n", g_serr);
    b = relC(2); t_puts(b, "mflng/mfl_video_ffmpeg.dll", "changed after the list was made\n"); CHECK(update(b) == 2);
    b = relC(2); radd(b, 2, "MFLng.extra.exe"); CHECK(update(b) == 2);                      /* a program file the list does not have */
    b = rel(2, C1, BI); t_del(b, BI); sums(b, SUMS); CHECK(update(b) == 2);                 /* no BUILDINFO: the installed tag would stay unknown */
    b = rel(2, C1, BI); t_puts(b, BI, "cl 19.44\n"); sums(b, SUMS); CHECK(update(b) == 2);
    CHECK(!stage(relC(2), "r20990109-0000-abcdef0"));                                       /* not the release that was asked for */
    b = t_new(); radd(b, 2, "mflng/INSTALL.txt"); t_puts(b, BI, t_get(v1, BI)->data); sums(b, SUMS); CHECK(update(b) == 2);      /* no program file */
    b = rel(2, C1, BI); for (i = 0; i < 100; i++) { snprintf(p, sizeof p, "mflng/LICENSES/l%03d.txt", i); radd(b, 2, p); } sums(b, SUMS);
    CHECK(update(b) == 2);                                                                  /* more entries than UPD_MAXF: never a part of a zip */
    snprintf(st, sizeof st, "%s/update", DATA);
    CHECK(!upd_stage((const unsigned char *)"this is not a zip file, only some text that is long enough", 40, st, NULL, g_serr, sizeof g_serr));
    z = mkzip(relC(2), &zn);
    CHECK(!upd_stage(z, zn / 2, st, NULL, g_serr, sizeof g_serr));
    n = zip_list(z, zn, ze, UPD_MAXF); CHECK(n > 5);
    for (i = 0; i < n; i++) if (!strcmp(ze[i].name, "MFLng.exe")) ((unsigned char *)ze[i].data)[3] ^= 0x40;
    CHECK(!upd_stage(z, zn, st, NULL, g_serr, sizeof g_serr) && strstr(g_serr, "damaged"));
    free(z);
    TREE(want, INST, "after every refused zip"); TREE(INI, DATA, "after every refused zip: no staging left");

    /* 5. the staging folder is the user's: a staged file changed after the check stops the apply step; a file put there is not installed */
    fresh("tamper");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1); snprintf(st, sizeof st, "%s/update", DATA);
    CHECK(stage(v2, NULL)); puts_at(st, "mflng/mfl_audio_miniaudio.dll", "swapped\n");
    CHECK(upd_apply(INST, DATA, SELF) == 0 && strstr(g_log, "does not match SHA256SUMS"));
    TREE(both(G, "", v1), INST, "changed staged file"); TREE(INI, DATA, "changed staged file: data folder");
    CHECK(stage(v2, NULL)); puts_at(st, "MFL.exe", "evil\n"); puts_at(st, "launch.exe", "evil\n"); puts_at(st, "mflng/evil.bin", "evil\n"); puts_at(st, "INSTALL.txt", "evil\n");
    CHECK(upd_apply(INST, DATA, SELF) == 1);
    TREE(both(G, "", v2), INST, "files put into the staging folder"); TREE(both(INI, "backup/", v1), DATA, "files put into the staging folder: data folder");

    /* 6. one failure at every point: over an install and as a first install */
    sweep("sweep", 0);
    sweep("sweep_bare", 1);

    /* 7. the way back fails too (the 5th file cannot be placed, then no .old can be renamed back and no backup copy written): the answer
       is -1, and every file of the old build is in the game folder or in backup\ (the backup is not cleared) */
    fresh("stuck");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1);
    CHECK(stage(v2, NULL));
    fail_on("rename", ".new>", 5, 0); fail_on("rename", ".old>", 0, 1); fail_on("copy", "", 0, 1);
    CHECK(upd_apply(INST, DATA, SELF) == -1); g_ninj = 0;
    CHECK(strstr(g_log, "ROLLBACK FAILED") && strstr(g_log, "rollback incomplete"));
    for (i = k = 0; i < v1->n; i++) {
        unsigned char *d; size_t l = 0; const TFile *f = &v1->f[i]; int ok;
        snprintf(p, sizeof p, "%s/%s", INST, f->name); d = os_read(p, &l); ok = d && l == f->len && !memcmp(d, f->data, l); free(d);
        if (ok) continue;
        snprintf(p, sizeof p, "%s/backup/%s", DATA, f->name); d = os_read(p, &l); ok = d && l == f->len && !memcmp(d, f->data, l); free(d);
        if (!ok) { printf("FAIL: %s of the old build is neither in the game folder nor in the backup\n", f->name); fails++; }
        k++;
    }
    CHECK(k == 5);

    /* 8. the flat layout before W17 (everything beside the exes, its SHA256SUMS at the top).
       a) the launcher of that time and a layout C zip: it looks for the zip entry "SHA256SUMS", finds none and stops before it writes
          anything ("The zip has no SHA256SUMS").  So a layout C zip must never have one at the top: with it, that launcher would
          install the top-level files only (it skips every name with a '/') and leave a mix of two builds.  (4. refuses such a zip.)
       b) this launcher over a flat install (the exe copied there, or the zip unpacked over it by hand): mflng/ is made, the release
          is installed, and the files the flat SHA256SUMS lists with the bytes they still have are removed: nothing else. */
    fresh("flat");
    f1 = rel(1, FLAT, "BUILDINFO.txt"); sums(f1, "SHA256SUMS"); unpack(INST, f1);
    v2 = relC(2); CHECK(!t_get(v2, "SHA256SUMS") && !name_allowed("SHA256SUMS"));
    CHECK(update(v2) == 1);
    TREE(both(G, "", v2), INST, "flat install -> layout C"); TREE(both(INI, "backup/", f1), DATA, "flat install -> layout C: backup = the flat build");
    CHECK(!at(INST, "LICENSES"));
    fresh("flat_hand");      /* layout C unpacked by hand over the flat install, then the first update */
    f1 = rel(1, FLAT, "BUILDINFO.txt"); sums(f1, "SHA256SUMS"); unpack(INST, f1); unpack(INST, relC(2));
    v3 = relC(3); CHECK(update(v3) == 1);
    TREE(both(G, "", v3), INST, "flat + hand-unpacked layout C -> update"); CHECK(!at(INST, "LICENSES"));
    fresh("flat_game");      /* files of those names that are not the flat build's stay */
    f1 = rel(1, FLAT, "BUILDINFO.txt"); sums(f1, "SHA256SUMS"); unpack(INST, f1);
    puts_at(INST, "INSTALL.txt", "the game's own install notes\n"); puts_at(INST, "LICENSES/eula.txt", "the game's\n"); puts_at(INST, "Manual.txt", "the game's\n");
    v2 = relC(2); CHECK(update(v2) == 1);
    want = both(G, "", v2); t_puts(want, "INSTALL.txt", "the game's own install notes\n"); t_puts(want, "LICENSES/eula.txt", "the game's\n"); t_puts(want, "Manual.txt", "the game's\n");
    TREE(want, INST, "flat install with files of the game's own");

    /* c) a layout C install whose INSTALL.txt was still beside the exes (the first layout C releases), updated by the launcher of that
          time to a release with mflng/INSTALL.txt: that launcher removes nothing, the old INSTALL.txt stays.  This launcher finds it in
          the SHA256SUMS of the backup (the release before the installed one) and removes it with its first update. */
    fresh("install_txt");
    v1 = rel(1, C1, BI); t_del(v1, "mflng/INSTALL.txt"); radd(v1, 1, "INSTALL.txt"); sums(v1, SUMS);
    unpack(INST, v1); unpack(INST, relC(2)); unpack(DATA, both(t_new(), "backup/", v1));
    v3 = relC(3); CHECK(update(v3) == 1);
    TREE(both(G, "", v3), INST, "INSTALL.txt left beside the exes by an older launcher");

    /* 9. portable: the data folder is <game folder>/mflng/mflng-launch.  A failed first install leaves mflng/ (it holds the data folder) */
    fresh("portable");
    snprintf(DATA, sizeof DATA, "%s/mflng/mflng-launch", INST); unpack(DATA, INI);
    v1 = relC(1); CHECK(stage(v1, NULL)); fail_on("write", "", 3, 0);
    CHECK(upd_apply(INST, DATA, SELF) == 0); settle();
    want = both(G, "mflng/mflng-launch/", INI); TREE(want, INST, "portable: failed first install");
    CHECK(update(v1) == 1); v2 = relC(2); CHECK(update(v2) == 1);
    want = both(G, "mflng/mflng-launch/", INI); t_add(want, "", v2); t_add(want, "mflng/mflng-launch/backup/", v1);
    TREE(want, INST, "portable: two updates");

    /* 10. leftovers of an update that was cut off: <release file>.old / .new go at the next start, nothing else */
    fresh("leftovers");
    v1 = relC(1); unpack(INST, v1);
    puts_at(INST, "MFLng.exe.old", "x"); puts_at(INST, "mflng/mfl_audio_miniaudio.dll.new", "x"); puts_at(INST, "mflng/LICENSES/zlib.txt.old", "x"); puts_at(INST, "mflng/SHA256SUMS.new", "x");
    puts_at(INST, "gamesave.bin.old", "mine"); puts_at(INST, "MFL.exe.old", "mine"); puts_at(INST, "Readme.txt.old", "mine"); puts_at(INST, "notes.txt.new", "mine"); puts_at(INST, "mflng/mods/a.dll.old", "mine");
    CHECK(upd_clean(INST) == 0);
    want = both(G, "", v1); t_puts(want, "gamesave.bin.old", "mine"); t_puts(want, "MFL.exe.old", "mine"); t_puts(want, "Readme.txt.old", "mine"); t_puts(want, "notes.txt.new", "mine"); t_puts(want, "mflng/mods/a.dll.old", "mine");
    TREE(want, INST, "leftovers");

    /* 11. the launcher exe cannot be renamed to .old (a user's report: Windows error 5, access denied).
       a) the file can be written (the apply step runs from a copy, so the launcher is not running): it is written over */
    fresh("self_rename");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1); puts_at(INST, SELF ".old", "left by an update before\n");
    CHECK(stage(v2, NULL)); fail_on("rename", SELF ">", 0, 0);
    CHECK(upd_apply(INST, DATA, SELF) == 1 && strstr(g_log, "replaced " SELF " by writing over it") && g_inj[0].fired); settle();
    TREE(both(G, "", v2), INST, "launcher not renamable: written over"); TREE(both(INI, "backup/", v1), DATA, "launcher not renamable: backup = build 1");
    /* b) it is in use as well (no rename, no write): everything else is installed, the new launcher waits as MFLng-launch.new.exe
          (a start of the launcher swaps it in: launcher.c) and no cleanup takes it away */
    fresh("self_busy");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1);
    CHECK(stage(v2, NULL)); fail_on("rename", SELF ">", 0, 0); fail_on("write", "/game/" SELF ">", 0, 0);
    CHECK(upd_apply(INST, DATA, SELF) == 2 && strstr(g_log, "takes its place at the next start")); settle();
    want = both(G, "", v2); t_set(want, SELF, t_get(v1, SELF)->data, t_get(v1, SELF)->len); t_set(want, "MFLng-launch.new.exe", t_get(v2, SELF)->data, t_get(v2, SELF)->len);
    TREE(want, INST, "launcher in use: the rest installed, the new launcher beside it"); TREE(both(INI, "backup/", v1), DATA, "launcher in use: backup = build 1");
    /* c) and then another file fails: all of it goes back, the waiting launcher too (never half an update) */
    fresh("self_busy_back");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1);
    CHECK(stage(v2, NULL)); fail_on("rename", SELF ">", 0, 0); fail_on("write", "/game/" SELF ">", 0, 0); fail_on("rename", "dll.new>", 0, 0);
    CHECK(upd_apply(INST, DATA, SELF) == 0); settle();
    TREE(both(G, "", v1), INST, "launcher in use, then a failure: as before"); TREE(INI, DATA, "launcher in use, then a failure: data folder");
    /* e) what an attempt before left behind (a user's report: error 183 at backup.prev, then error 5 at the launcher): a backup.prev
          with folders this launcher does not know, .new files, a .old of the launcher.  The next update goes through and leaves none. */
    fresh("stale");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1); unpack(DATA, both(t_new(), "backup/", relC(0)));
    puts_at(DATA, "backup.prev/MFLng.exe", "older\n"); puts_at(DATA, "backup.prev/LICENSES/zlib.txt", "flat layout\n"); puts_at(DATA, "backup.prev/mods/deep/x.bin", "unknown folder\n");
    puts_at(INST, SELF ".old", "stale\n"); puts_at(INST, SELF ".new", "stale\n"); puts_at(INST, "mflng/mfl_audio_miniaudio.dll.new", "stale\n"); puts_at(INST, "MFLng.exe.old", "stale\n");
    CHECK(update(v2) == 1); settle();
    TREE(both(G, "", v2), INST, "leftovers of an attempt before"); TREE(both(INI, "backup/", v1), DATA, "stale backup.prev: backup = build 1, one level");
    /* the backup cannot be moved to backup.prev at all: the update goes on without the older backup */
    fresh("no_rotate");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1); unpack(DATA, both(t_new(), "backup/", relC(0)));
    CHECK(stage(v2, NULL)); fail_on("rename", "backup>", 0, 0);
    CHECK(upd_apply(INST, DATA, SELF) == 1 && strstr(g_log, "the older backup is dropped")); settle();
    TREE(both(G, "", v2), INST, "backup not movable"); TREE(both(INI, "backup/", v1), DATA, "backup not movable: backup = build 1");
    /* d) any other file that cannot be renamed still rolls the update back */
    fresh("other_rename");
    v1 = relC(1); v2 = relC(2); unpack(INST, v1);
    CHECK(stage(v2, NULL)); fail_on("rename", "mflrw.dll>", 0, 0);
    CHECK(upd_apply(INST, DATA, SELF) == 0 && strstr(g_log, "FAIL: cannot rename")); settle();
    TREE(both(G, "", v1), INST, "a DLL not renamable: rolled back");

    (void)v4;
    free(g_arena);
    printf("update tests done: work folder %s\n", WORK);
}

int main(int argc, char **argv)
{
    unsigned char h[32]; char hx[65], want[65]; Release r[40]; Installed in; ZipEntry ze[64];
    int n, i, nz, hashed = 0; size_t zn; unsigned char *z; char *sums = NULL, *bi = NULL, *js;
    if (argc != 4 && argc != 5) { printf("usage: test_logic <releases.json> <zip> <tag> [deflate vectors]\n"); return 2; }

    if (argc == 5) {      /* raw DEFLATE vectors (agents/launch_w16/mkvectors.py): exact output, then corrupted input must fail cleanly (ASan build) */
        size_t vn, pos = 0; unsigned char *v = (unsigned char *)slurp(argv[4], &vn); int nv = 0, bad = 0, fuzz = 0; unsigned seed = 12345;
        while (pos + 8 <= vn) {
            size_t rl = v[pos] | v[pos + 1] << 8 | (size_t)v[pos + 2] << 16 | (size_t)v[pos + 3] << 24, cl = v[pos + 4] | v[pos + 5] << 8 | (size_t)v[pos + 6] << 16 | (size_t)v[pos + 7] << 24;
            const unsigned char *raw = v + pos + 8, *cz = raw + rl; unsigned char *o = (unsigned char *)malloc(rl + 1), *c = (unsigned char *)malloc(cl + 1); int k;
            if (inflate_raw(cz, cl, o, rl) != 0 || memcmp(o, raw, rl)) bad++;
            if (rl && inflate_raw(cz, cl, o, rl - 1) == 0) bad++;      /* output one byte too small */
            if (inflate_raw(cz, cl ? cl - 1 : 0, o, rl) == 0 && cl > 1) bad++;      /* truncated input */
            for (k = 0; k < 40 && cl; k++) {      /* flipped bytes: any result, but no crash / overrun */
                memcpy(c, cz, cl); seed = seed * 1103515245u + 12345u; c[(seed >> 8) % cl] ^= (unsigned char)(1 + (seed >> 24) % 255);
                inflate_raw(c, cl, o, rl); fuzz++;
            }
            free(o); free(c); pos += 8 + rl + cl; nv++;
        }
        { unsigned char g[4096], o[8192]; int k; for (k = 0; k < 2000; k++) { size_t j; for (j = 0; j < sizeof g; j++) { seed = seed * 1103515245u + 12345u; g[j] = (unsigned char)(seed >> 16); }
            inflate_raw(g, (size_t)(k % 4096), o, (size_t)(k * 7 % 8192)); fuzz++; } }
        CHECK(nv == 200 && bad == 0);
        CHECK(crc32_buf((const unsigned char *)"123456789", 9) == 0xcbf43926UL);
        printf("deflate vectors: %d, failures %d, corrupted / random inputs survived: %d\n", nv, bad, fuzz);
        free(v);
    }

    sha256((const unsigned char *)"abc", 3, h); hex32(h, hx);
    CHECK(!strcmp(hx, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
    sha256((const unsigned char *)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56, h); hex32(h, hx);
    CHECK(!strcmp(hx, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"));
    sha256((const unsigned char *)"", 0, h); hex32(h, hx);
    CHECK(!strcmp(hx, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));

    /* the real API answer */
    js = slurp(argv[1], NULL); n = parse_releases(js, r, 40); free(js);
    printf("releases: %d, first %s pre=%d url=%s digest=%.12s...\n", n, n > 0 ? r[0].tag : "-", n > 0 ? r[0].pre : -1, n > 0 ? r[0].url : "-", n > 0 ? r[0].digest : "-");
    CHECK(n >= 1 && !strcmp(r[0].tag, argv[3]) && r[0].body && strlen(r[0].body) > 20 && strlen(r[0].digest) == 64);
    CHECK(strstr(r[0].url, "/MFLng-") && !strstr(r[0].url, "maps"));
    for (i = 0; i < n; i++) CHECK(r[i].pre);      /* every release so far is a pre-release (release.yml) */
    CHECK(pick_newest(r, n, 0) == NULL);         /* channel release: nothing to offer */
    CHECK(pick_newest(r, n, 1) == &r[0]);        /* channel prerelease: the newest of all */
    z = (unsigned char *)slurp(argv[2], &zn);    /* the downloaded zip against the API's asset digest */
    sha256(z, zn, h); hex32(h, hx); CHECK(!strcmp(hx, r[0].digest));
    printf("zip sha256 %s the API digest\n", strcmp(hx, r[0].digest) ? "DIFFERS from" : "equals");
    free_releases(r, n);

    /* JSON edge cases */
    CHECK(parse_releases("{\"a\":1}", r, 4) == -1);
    n = parse_releases("[{\"tag_name\":\"r20990101-abcdef0\",\"draft\":false,\"prerelease\":false,\"body\":\"x \\\"tag_name\\\":\\\"bad\\\" \\u00e9\\n\","
                       "\"assets\":[{\"name\":\"MFLng-r20990101-abcdef0-maps.zip\",\"browser_download_url\":\"https://h/MFLng-r20990101-abcdef0-maps.zip\"},"
                       "{\"name\":\"MFLng-r20990101-abcdef0.zip\",\"digest\":null,\"browser_download_url\":\"https://h/MFLng-r20990101-abcdef0.zip\"}]},"
                       "{\"tag_name\":\"r20980101-0000000\",\"draft\":true,\"prerelease\":false,\"assets\":[]}]", r, 4);
    CHECK(n == 2 && !strcmp(r[0].tag, "r20990101-abcdef0") && !r[0].pre && !strcmp(r[0].body, "x \"tag_name\":\"bad\" \xc3\xa9\n"));
    CHECK(!strcmp(r[0].url, "https://h/MFLng-r20990101-abcdef0.zip") && r[0].digest[0] == 0 && r[1].draft);
    CHECK(pick_newest(r, n, 0) == &r[0]);
    free_releases(r, n);
    CHECK(parse_releases("[{\"tag_name\":\"r1\",\"body\":\"unterminated", r, 4) == 0);

    /* tag and channel rules */
    parse_buildinfo("cl 19.44\nrelease r20261120-e2ddfb7 prerelease\r\n", &in);
    CHECK(!strcmp(in.tag, "r20261120-e2ddfb7") && in.pre && default_channel_pre(&in));
    CHECK(update_state(&in, "r20261120-e2ddfb7") == ST_UPTODATE);
    CHECK(update_state(&in, "r20261121-1234567") == ST_UPDATE);
    CHECK(update_state(&in, "r20261119-1234567") == ST_NEWER_INSTALLED);
    CHECK(update_state(&in, "r20261120-cf82fc4") == ST_UPDATE);      /* same day, other build: offer the API's newest */
    parse_buildinfo("release r20261201-1111111\n", &in);
    CHECK(!in.pre && !default_channel_pre(&in));
    parse_buildinfo(NULL, &in);
    CHECK(!in.tag[0] && !default_channel_pre(&in) && update_state(&in, "r20261120-e2ddfb7") == ST_UPDATE);

    /* the real release zip */
    nz =zip_list(z, zn, ze, 64);
    CHECK(nz > 5);
    for (i = 0; i < nz; i++)      /* zip_get leaves room for a terminator */
        if (!strcmp(ze[i].name, "mflng/SHA256SUMS") && (sums = (char *)zip_get(&ze[i]))) sums[ze[i].usize] = 0;
        else if (!strcmp(ze[i].name, "mflng/BUILDINFO.txt") && (bi = (char *)zip_get(&ze[i]))) bi[ze[i].usize] = 0;
    CHECK(sums && bi);
    if (!sums || !bi) return 1;
    for (i = 0; i < nz; i++) {
        unsigned char *d; int listed, ok;
        if (!name_allowed(ze[i].name)) { printf("  skip %s (not an install file)\n", ze[i].name); CHECK(ze[i].name[strlen(ze[i].name) - 1] == '/'); continue; }
        d = zip_get(&ze[i]); CHECK(d != NULL); if (!d) continue;
        sha256(d, ze[i].usize, h); hex32(h, hx);
        listed = sums_lookup(sums, ze[i].name, want); ok = !listed || !strcmp(hx, want);
        if (listed) hashed++;
        CHECK(strchr(ze[i].name, '/') || needs_hash(ze[i].name));      /* at the top only program files: no SHA256SUMS (a flat launcher would take the zip for its own layout), no text file */
        CHECK(ok); CHECK(listed || !needs_hash(ze[i].name));
        printf("  %-28s %8lu B %s\n", ze[i].name, (unsigned long)ze[i].usize, listed ? (ok ? "sha256 OK" : "sha256 MISMATCH") : "(not in SHA256SUMS)");
        free(d);
    }
    CHECK(hashed >= 4);
    parse_buildinfo(bi, &in);
    /* the `release <tag> prerelease` line release.yml writes: installed = this tag, channel prerelease */
    CHECK(!strcmp(in.tag, argv[3]) && update_state(&in, argv[3]) == ST_UPTODATE && default_channel_pre(&in));
    printf("zip BUILDINFO: tag '%s' -> default channel %s\n", in.tag, default_channel_pre(&in) ? "prerelease" : "release");
    for (i = 0; i < nz; i++) if (needs_hash(ze[i].name) && ze[i].csize > 100) {      /* a flipped byte must fail (crc or inflate) */
        unsigned char *d, keep = ze[i].data[50]; ((unsigned char *)ze[i].data)[50] ^= 0x55;
        d = zip_get(&ze[i]); CHECK(d == NULL); free(d); ((unsigned char *)ze[i].data)[50] = keep; break;
    }
    CHECK(zip_list(z, 100, ze, 64) == -1);

    /* W17 LAYOUT C: the exes and mflrw.dll at the top; the backend DLLs, INSTALL.txt, BUILDINFO.txt, SHA256SUMS, LICENSES/ in mflng/ */
    CHECK(name_allowed("MFLng.exe") && name_allowed("MFLng.nofix.exe") && name_allowed("MFLng-launch.exe") && name_allowed("mflrw.dll") && name_allowed("mflng/INSTALL.txt"));
    /* no text file and no SHA256SUMS beside the exes: the game folder has text files of its own, and INSTALL.txt lives in mflng/ */
    CHECK(!name_allowed("INSTALL.txt") && !name_allowed("BUILDINFO.txt") && !name_allowed("SHA256SUMS") && !name_allowed("Readme.txt"));
    /* name_ours: also what the layouts before put beside the exes; only ever a reason to REMOVE a file the installed SHA256SUMS lists */
    CHECK(name_ours("INSTALL.txt") && name_ours("BUILDINFO.txt") && name_ours("SHA256SUMS") && name_ours("LICENSES/zlib.txt") && name_ours("mflng/mflrw.dll") && name_ours("avcodec-61.dll"));
    CHECK(!name_ours("MFL.exe") && !name_ours("launch.exe") && !name_ours("gamesave.bin") && !name_ours("../x.txt") && !name_ours("LICENSES/../x.txt") && !name_ours("LICENSES/a/b.txt")
          && !name_ours("TOASTART/x.txt") && !name_ours("mflng/LICENSES/SHA256SUMS") && !name_ours("d3d9.dll") && !name_ours("nul.txt") && !name_ours(".txt") && !name_ours(""));
    /* Windows device names, with any extension and in any case; names that only start like one are files */
    CHECK(!name_allowed("mflng/nul.txt") && !name_allowed("mflng/CON.txt") && !name_allowed("mflng/LICENSES/Aux.txt") && !name_allowed("mflng/com1.txt") && !name_allowed("mflng/LPT9.x.txt"));
    CHECK(name_allowed("mflng/null.txt") && name_allowed("mflng/console.txt") && name_allowed("mflng/com.txt") && name_allowed("mflng/LICENSES/com10.txt"));
    CHECK(!name_allowed("") && !name_allowed("mflng/") && !name_allowed("MFLng.exe ") && !name_allowed("MFLng.exe.") && !name_allowed("MFLng.EXE") && !name_allowed("mflng/MFLng.exe:x")
          && !name_allowed("MFLNG/mflrw.dll") && !name_allowed("/MFLng.exe") && !name_allowed("C:MFLng.exe") && !name_allowed("mflng//mflrw.dll") && !name_allowed("./MFLng.exe"));
    CHECK(name_allowed("mflng/mfl_audio_miniaudio.dll") && name_allowed("mflng/LICENSES/zlib.txt") && name_allowed("mflng/BUILDINFO.txt") && name_allowed("mflng/SHA256SUMS"));
    CHECK(name_allowed("mflng/mfl_video_ffmpeg.dll") && name_allowed("mflng/avformat-61.dll") && name_allowed("mflng/avcodec-61.dll") && name_allowed("mflng/avutil-59.dll") && name_allowed("mflng/swscale-8.dll"));
    /* a DLL may lie in either place (one that does not load from mflng/ goes back beside the exes without a new launcher) */
    CHECK(name_allowed("mfl_video_ffmpeg.dll") && name_allowed("avcodec-61.dll") && name_allowed("mflng/mflrw.dll"));
    /* LICENSES/ only inside mflng/; no other folder, no second level */
    CHECK(!name_allowed("LICENSES/zlib.txt") && !name_allowed("mflng") && !name_allowed("mflng/mflng/mflrw.dll") && !name_allowed("videocfg.bin"));
    CHECK(!name_allowed("MFL.exe") && !name_allowed("MFL_original.exe") && !name_allowed("launch.exe") && !name_allowed("../MFLng.exe"));
    CHECK(!name_allowed("avcodec-61.exe") && !name_allowed("avcodec.lib") && !name_allowed("postproc-58.dll") && !name_allowed("d3d9.dll"));
    CHECK(!name_allowed("mflng/MFL.exe") && !name_allowed("mflng/MFL_original.exe") && !name_allowed("mflng/launch.exe") && !name_allowed("mflng/../MFLng.exe") && !name_allowed("mflng/"));
    CHECK(!name_allowed("mflng/avcodec-61.exe") && !name_allowed("mflng/avcodec.lib") && !name_allowed("mflng/postproc-58.dll") && !name_allowed("mflng/d3d9.dll"));
    CHECK(!name_allowed("mflng/LICENSES/../x.txt") && !name_allowed("mflng/a/b.txt") && !name_allowed("mflng/LICENSES/") && !name_allowed("mflng/videocfg.bin") && !name_allowed("mflng/gamesave.bin"));
    CHECK(!name_allowed("C:\\x.txt") && !name_allowed("mflng\\MFLng.exe") && !name_allowed("mflng/LICENSES/a/b.txt") && !name_allowed("mflng/.x.txt"));
    /* SHA256SUMS lists zip names */
    CHECK(sums_lookup(sums, "MFLng-launch.exe", want) && sums_lookup(sums, "mflrw.dll", want) && sums_lookup(sums, "mflng/mfl_video_ffmpeg.dll", want) && !sums_lookup(sums, "mfl_video_ffmpeg.dll", want));

    free(z); free(sums); free(bi);
    update_tests(argv[2]);
    printf(fails ? "test_logic: %d FAILED\n" : "test_logic: all passed\n", fails);
    return fails != 0;
}
