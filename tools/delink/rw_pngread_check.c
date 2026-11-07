/* rw_pngread_check.c: bit-exact check of the librw DLL's RtPNGImageRead (tools/delink/rw_pngread.txt, librw-mfl capi/rwpngread.cpp on the DLL's own
   libpng 1.0.12 + zlib 1.1.3) against the ORIGINAL code of MFL.exe (RtPNGImageRead 0x524880 on the exe's libpng), both run natively (Wine).  Image
   mapping as rw_rwmath_check.c (this exe at 0x400000, MFL.exe's sections in a bss block spanning 0x4b0000..0x94b000).  RwStreamOpen / Read / Close,
   RwImageCreate / AllocatePixels / FreePixels / Destroy, RwErrorSet and the error code helper 0x53eac0 are jmp-patched to harness functions that both
   sides use (the DLL through mflrw_pngread_bind with the exe addresses): the "file name" is a harness memory file.  The engine's malloc / free
   (RwEngineInstance 0x7d2128 +0x108 / +0x10c) are a harness heap (pattern-filled blocks, live count and bytes tracked).
   usage: rw_pngread_check.exe <mflrw.dll> <MFL.exe> <dir with list.txt + the png files> [variants=200] [neg]
   Per file: the file as it is + `variants` corrupt copies (random byte flips anywhere, truncation, a zeroed CRC, a flipped IDAT byte).  Compared per case:
   NULL or not, the image header (width, height, depth, stride), every pixel row, the palette, the RwError log and the engine heap balance after the call.
   neg = negative fixture: the DLL side reads the file with one IHDR width byte flipped (the CRC then fails): DIFF expected on the files. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8;
#define LO 0x4b0000u
#define HI 0x94b000u
static unsigned char mfl_area[0x700000] __attribute__((aligned(4096)));
static int load_image(const char *path) {
  FILE *f = fopen(path, "rb"); unsigned char *d; long sz; IMAGE_NT_HEADERS32 *nt; IMAGE_SECTION_HEADER *s; int i; DWORD old;
  if (!f) return 0; fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET); d = malloc(sz); fread(d, 1, sz, f); fclose(f);
  if ((uintptr_t)mfl_area > LO || (uintptr_t)mfl_area + sizeof mfl_area < HI) { printf("bss block at %p does not cover %x..%x\n", mfl_area, LO, HI); return 0; }
  if (!VirtualProtect((void *)LO, HI - LO, PAGE_EXECUTE_READWRITE, &old)) { printf("VirtualProtect failed %lu\n", GetLastError()); return 0; }
  nt = (IMAGE_NT_HEADERS32 *)(d + ((IMAGE_DOS_HEADER *)d)->e_lfanew); s = IMAGE_FIRST_SECTION(nt);
  for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
    u32 va = 0x400000 + s[i].VirtualAddress, n = s[i].SizeOfRawData < s[i].Misc.VirtualSize || !s[i].Misc.VirtualSize ? s[i].SizeOfRawData : s[i].Misc.VirtualSize, k;
    for (k = 0; k < n; k++) if (va + k >= LO && va + k < HI) ((unsigned char *)(uintptr_t)(va + k))[0] = d[s[i].PointerToRawData + k];
  }
  return 1;
}
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
static void jmp_patch(u32 va, void *to) { *(u8 *)(uintptr_t)va = 0xe9; *(int32_t *)(uintptr_t)(va + 1) = (int32_t)((u32)(uintptr_t)to - (va + 5)); }
static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }

/* ---- harness engine heap, stream, image, error */
static long live, livebytes;
static void *__cdecl e_malloc(u32 n, u32 hint) { u32 *p = n > (64u << 20) ? 0 : malloc(n + 8); if (!p) return 0;      /* corrupt sizes: a failed allocation, as the game heap gives */ p[0] = n; p[1] = 0x5eed; memset(p + 2, 0xa5, n); live++; livebytes += n; return p + 2; }
/* freed blocks stay in quarantine until the case ends, so a second free of the same block is seen (counted, compared) instead of corrupting the heap */
static u32 *quar[100000]; static int nquar, badfree;
static void __cdecl e_free(void *q) { u32 *p; if (!q) return; p = (u32 *)q - 2; if (p[1] != 0x5eed) { badfree++; return; } live--; livebytes -= p[0]; p[1] = 0xdead; if (nquar < 100000) quar[nquar++] = p; else free(p); }
static void quar_flush(void) { while (nquar) free(quar[--nquar]); }
typedef struct { u8 *data; u32 len; u32 magic; } MemFile;
typedef struct { MemFile *f; u32 pos; } MemStream;
static int nopen;
static void *__cdecl s_open(int type, int access, const void *data) { MemFile *f = (MemFile *)data; MemStream *s; if (type != 2 || access != 1 || !f || f->magic != 0x4d454d46) return 0; s = calloc(1, sizeof *s); s->f = f; nopen++; return s; }
/* past the end the rest of the buffer is zeroed (the real RwStreamRead leaves it as it was; RtPNGImageRead ignores short reads, so a truncated file
   would make libpng skip chunk lengths taken from stale bytes, gigabytes of reads): both sides see the same zeros */
static u32 __cdecl s_read(void *st, void *buf, u32 n) { MemStream *s = st; u32 k = s->f->len - s->pos < n ? s->f->len - s->pos : n; memcpy(buf, s->f->data + s->pos, k); if (k < n) memset((u8 *)buf + k, 0, n - k); s->pos += k; return k; }
static int __cdecl s_close(void *st, void *d) { if (st) { free(st); nopen--; } return 1; }
static int nimg;
static u8 *__cdecl i_create(int w, int h, int depth) { u8 *im = calloc(1, 0x20); *(int *)(im + 4) = w; *(int *)(im + 8) = h; *(int *)(im + 0xc) = depth; nimg++; return im; }
static u8 *__cdecl i_alloc(u8 *im) {
  int w = *(int *)(im + 4), h = *(int *)(im + 8), d = *(int *)(im + 0xc), st = ((d == 32 ? 4 : 1) * w + 3) & ~3;
  if ((long long)st * h > (64 << 20) || w <= 0 || h <= 0) return 0;      /* absurd sizes from corrupt headers: refused like an allocation failure */
  *(int *)(im + 0x10) = st; *(u8 **)(im + 0x14) = calloc(1, (size_t)st * h + 16);
  if (d == 4 || d == 8) *(u8 **)(im + 0x18) = calloc(4, 1u << d);
  return im;
}
static u8 *__cdecl i_freepix(u8 *im) { free(*(u8 **)(im + 0x14)); free(*(u8 **)(im + 0x18)); *(u8 **)(im + 0x14) = *(u8 **)(im + 0x18) = 0; return im; }
static int __cdecl i_destroy(u8 *im) { i_freepix(im); free(im); nimg--; return 1; }
static u32 errlog[8]; static int nerr;
static void *__cdecl e_errset(u32 *e) { if (nerr < 4) { errlog[2 * nerr] = e[0]; errlog[2 * nerr + 1] = e[1]; } nerr++; return e; }
static int __cdecl e_code(int code, ...) { return code; }

typedef void *(__cdecl *ReadFn)(const void *name);
typedef void (__cdecl *Bind)(void *engine, void *open, void *read, void *close, void *create, void *alloc, void *freepix, void *destroy, void *errset, void *code);
static volatile int cur_file, cur_var, side;
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p file %d variant %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_file, cur_var, side); fflush(stdout); ExitProcess(4); return 0; }
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT file %d variant %d\n", cur_file, cur_var); fflush(stdout); ExitProcess(3); return 0; }

typedef struct { int null, w, h, d, st; u8 *pix, *pal; long live, livebytes; int nerr; u32 err[8]; int nopen, badfree; } Res;
static void take(Res *r, u8 *im) {
  memset(r, 0, sizeof *r); r->null = im == 0; r->live = live; r->livebytes = livebytes; r->nerr = nerr; memcpy(r->err, errlog, sizeof errlog); r->nopen = nopen; r->badfree = badfree;
  if (im) { r->w = *(int *)(im + 4); r->h = *(int *)(im + 8); r->d = *(int *)(im + 0xc); r->st = *(int *)(im + 0x10); r->pix = *(u8 **)(im + 0x14); r->pal = *(u8 **)(im + 0x18); }
}
static int same(Res *a, Res *b) {
  if (a->null != b->null || a->badfree != b->badfree || a->live != b->live || a->livebytes != b->livebytes || a->nerr != b->nerr || memcmp(a->err, b->err, sizeof a->err) || a->nopen != b->nopen) return 0;
  if (a->null) return 1;
  if (a->w != b->w || a->h != b->h || a->d != b->d || a->st != b->st || !a->pix != !b->pix || !a->pal != !b->pal) return 0;
  if (a->pix && memcmp(a->pix, b->pix, (size_t)a->st * a->h)) return 0;
  if (a->pal && memcmp(a->pal, b->pal, 4u << a->d)) return 0;
  return 1;
}

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int V = argc > 4 ? atoi(argv[4]) : 200, neg = argc > 5 && !strcmp(argv[5], "neg"), nf = 0, fi, bad = 0, cases = 0, nulls = 0, imgs = 0, depth[33] = {0};
  char path[600], line[300]; FILE *lf; ReadFn dll, orig = (ReadFn)0x524880; Bind bind; u8 *eng;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + 2000u * (u32)V), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  dll = (ReadFn)GetProcAddress(m, "rw37_RtPNGImageRead"); bind = (Bind)GetProcAddress(m, "mflrw_pngread_bind");
  if (!dll || !bind) { printf("rw37_RtPNGImageRead / mflrw_pngread_bind MISSING export\n"); return 1; }
  eng = calloc(1, 0x200); W(0x7d2128) = (u32)(uintptr_t)eng; *(void **)(eng + 0x108) = (void *)e_malloc; *(void **)(eng + 0x10c) = (void *)e_free;
  jmp_patch(0x4dc370, (void *)s_open); jmp_patch(0x4dbe70, (void *)s_read); jmp_patch(0x4dc2a0, (void *)s_close);
  jmp_patch(0x4dd550, (void *)i_create); jmp_patch(0x4dd610, (void *)i_alloc); jmp_patch(0x4dd6d0, (void *)i_freepix); jmp_patch(0x4dd5b0, (void *)i_destroy);
  jmp_patch(0x4ea160, (void *)e_errset); jmp_patch(0x53eac0, (void *)e_code);
  bind((void *)0x7d2128, (void *)0x4dc370, (void *)0x4dbe70, (void *)0x4dc2a0, (void *)0x4dd550, (void *)0x4dd610, (void *)0x4dd6d0, (void *)0x4dd5b0, (void *)0x4ea160, (void *)0x53eac0);
  printf("exe mapped, DLL %s, %d variants per file%s\n", argv[1], V, neg ? "  NEGATIVE FIXTURE: IHDR width byte flipped on the DLL side" : "");
  snprintf(path, sizeof path, "%s\\list.txt", argv[3]); lf = fopen(path, "r"); if (!lf) { printf("cannot open %s\n", path); return 2; }
  /* the NULL name case first (RwError, no stream) */
  { Res r[2]; for (side = 0; side < 2; side++) { u8 *im; nerr = 0; memset(errlog, 0, sizeof errlog); im = (side ? orig : dll)(0); take(&r[side], im); }
    printf("NULL name: %s (error log %08x %08x)\n", same(&r[0], &r[1]) ? "EQUAL" : "DIFF", r[1].err[0], r[1].err[1]); bad += !same(&r[0], &r[1]); }
  for (fi = 0; fgets(line, sizeof line, lf); fi++) {
    FILE *f; long n; u8 *data, *buf; int v, fb = 0;
    line[strcspn(line, "\r\n")] = 0; if (!*line) continue;
    snprintf(path, sizeof path, "%s\\%s", argv[3], line); f = fopen(path, "rb"); if (!f) { printf("cannot open %s\n", path); return 2; }
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET); data = malloc(n); fread(data, 1, n, f); fclose(f); buf = malloc(n + 64); nf++;
    for (v = 0; v <= V; v++) {
      MemFile mf[2]; Res r[2]; u32 len = (u32)n; int k;
      cur_file = fi; cur_var = v; rs = 0x9e3779b9u * (fi + 1) + 7919u * v;
      memcpy(buf, data, n);
      if (v) switch (rnd() % 4) {
        case 0: for (k = 1 + rnd() % 4; k; k--) buf[rnd() % n] ^= (u8)(1 + rnd() % 255); break;
        case 1: len = 8 + rnd() % (n - 8); break;
        case 2: { u32 o = 8; while (o + 12 <= n) { u32 cl = buf[o] << 24 | buf[o + 1] << 16 | buf[o + 2] << 8 | buf[o + 3]; if (o + 12 + cl > n) break; if (rnd() % 3 == 0) { memset(buf + o + 8 + cl, 0, 4); break; } o += 12 + cl; } break; }
        case 3: { u32 o = 8; while (o + 12 <= n) { u32 cl = buf[o] << 24 | buf[o + 1] << 16 | buf[o + 2] << 8 | buf[o + 3]; if (o + 12 + cl > n) break; if (!memcmp(buf + o + 4, "IDAT", 4) && cl) { buf[o + 8 + rnd() % cl] ^= 0x10; break; } o += 12 + cl; } break; }
      }
      for (side = 0; side < 2; side++) {
        u8 *im; mf[side].data = side || !neg ? buf : malloc(len); mf[side].len = len; mf[side].magic = 0x4d454d46;
        if (!side && neg) { memcpy(mf[0].data, buf, len); if (len > 19) mf[0].data[19] ^= 0x01; }
        nerr = 0; memset(errlog, 0, sizeof errlog); badfree = 0; live = 0; livebytes = 0;
        im = (side ? orig : dll)(&mf[side]);
        take(&r[side], im);
        if (side) { if (!same(&r[0], &r[1])) { if (++fb <= 2) printf("  DIFF %s variant %d: null %d/%d %dx%d d%d / %dx%d d%d, heap live %ld/%ld (%ld/%ld B), errors %d/%d\n", line, v, r[0].null, r[1].null,
                      r[0].w, r[0].h, r[0].d, r[1].w, r[1].h, r[1].d, r[0].live, r[1].live, r[0].livebytes, r[1].livebytes, r[0].nerr, r[1].nerr); }
                    else if (v == 0 && !r[1].null) depth[r[1].d & 31]++; }
        if (!side && neg) free(mf[0].data);
      }
      cases++; nulls += r[1].null; imgs += !r[1].null;
      { Res *q; for (q = r; q < r + 2; q++) if (!q->null) { free(q->pix); free(q->pal); } }      /* the images themselves stay (the leak of the error path is counted in nimg) */
      quar_flush();      /* heap balance is compared per case and side (reset before each call); a leak on one side shows as DIFF there */
    }
    bad += fb != 0;
    printf("%-36s %6ld B  %s%s\n", line, n, fb ? "DIFF" : "EQUAL", fb ? "" : "");
    free(data); free(buf);
  }
  printf("files %d, cases %d (images %d, NULL %d), depth 4/8/32 of the intact files %d/%d/%d: %s\n", nf, cases, imgs, nulls, depth[4], depth[8], depth[0], bad ? "DIFF" : "ALL EQUAL");
  printf("DONE\n");
  return bad != 0;
}
