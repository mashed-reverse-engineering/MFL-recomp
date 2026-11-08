/* fakedev_raster.c: raster slots of the fake D3D9 device (fakedev.c) plus a fake IDirect3D9 object, for the X2 d3dras check (plan_engine_switch_window2_2026-11-08
   v2, O1: lane X's methods live here, fakedev.c stays lane D's).  Included by the check after fakedev.c (#include "fakedev_raster.c"); call fakedev_raster_init()
   after fakedev_init().
   Device slots: 23 CreateTexture, 25 CreateCubeTexture, 30 UpdateSurface, 32 GetRenderTargetData, 36 CreateOffscreenPlainSurface.
   Objects (one heap pool, ids 0x7e500000 + n, refcounted; any other method = TRAP):
     texture / cube texture: 2 Release, 18 GetSurfaceLevel(level) / GetCubeMapSurface(face, level) (a new surface each call; level past the chain fails)
     surface: 2 Release, 12 GetDesc, 13 LockRect (pitch and bits are fake per surface, never dereferenced by the slice), 14 UnlockRect
   IDirect3D9 (fakedev_d3d9): 10 CheckDeviceFormat, 12 CheckDepthStencilMatch; the answer is a fixed function of (fakedev_capsseed, usage, formats), so one seed
   is one consistent device, plus per-call overrides.  Every call logs its slot (FAKEDEV_TAG | 0x2000 | slot for objects) and its arguments.
   Failure injection (set by the check before an op): fakedev_texhr (Create*Texture), fakedev_offhr (CreateOffscreenPlainSurface), fakedev_levelhr
   (GetSurfaceLevel / GetCubeMapSurface), fakedev_surflockhr (LockRect); 0 = succeed. */
#define FDR_OBJS 16384
struct fdr_obj { void **vtbl; uint32_t id, kind, w, h, levels, usage, fmt, pool, refs, pitch, locked; };
enum { FDR_TEX = 1, FDR_CUBE, FDR_SURF };
static void *fdr_texvtbl[22], *fdr_cubevtbl[22], *fdr_surfvtbl[17], *fdr_d3dvtbl[17];
static struct fdr_obj *fdr_objs;
static int fdr_nobj;
static void *fdr_d3dobj[2];
void *fakedev_d3d9 = fdr_d3dobj;
uint32_t fakedev_capsseed;
int32_t fakedev_texhr, fakedev_offhr, fakedev_levelhr, fakedev_surflockhr, fakedev_fmtflip;

static void fdr_trap(int k, int n) { printf("TRAP unexpected %s method %d\n", k == 0 ? "IDirect3D9" : k == FDR_SURF ? "surface" : "texture", n); fflush(stdout); ExitProcess(5); }
#define FT_D(n) static long __stdcall fdt_d##n(void *s) { fdr_trap(0, n); return (long)0x8876086c; }
#define FT_T(n) static long __stdcall fdt_t##n(void *s) { fdr_trap(FDR_TEX, n); return (long)0x8876086c; }
#define FT_S(n) static long __stdcall fdt_s##n(void *s) { fdr_trap(FDR_SURF, n); return (long)0x8876086c; }
FT_D(0) FT_D(1) FT_D(2) FT_D(3) FT_D(4) FT_D(5) FT_D(6) FT_D(7) FT_D(8) FT_D(9) FT_D(11) FT_D(13) FT_D(14) FT_D(15) FT_D(16)
FT_T(0) FT_T(1) FT_T(3) FT_T(4) FT_T(5) FT_T(6) FT_T(7) FT_T(8) FT_T(9) FT_T(10) FT_T(11) FT_T(12) FT_T(13) FT_T(14) FT_T(15) FT_T(16) FT_T(17) FT_T(19) FT_T(20) FT_T(21)
FT_S(0) FT_S(1) FT_S(3) FT_S(4) FT_S(5) FT_S(6) FT_S(7) FT_S(8) FT_S(9) FT_S(10) FT_S(11) FT_S(15) FT_S(16)

static uint32_t fdr_id(void *o) { return o ? ((struct fdr_obj *)o)->id : 0; }
static struct fdr_obj *fdr_new(int kind, uint32_t w, uint32_t h, uint32_t levels, uint32_t usage, uint32_t fmt, uint32_t pool) {
  struct fdr_obj *o;
  if (fdr_nobj == FDR_OBJS) { printf("fakedev_raster: object pool full\n"); fflush(stdout); ExitProcess(6); }
  o = &fdr_objs[fdr_nobj]; o->id = 0x7e500000u + fdr_nobj++; o->kind = kind;
  o->vtbl = kind == FDR_TEX ? fdr_texvtbl : kind == FDR_CUBE ? fdr_cubevtbl : fdr_surfvtbl;
  o->w = w; o->h = h; o->levels = levels; o->usage = usage; o->fmt = fmt; o->pool = pool; o->refs = 1; o->locked = 0;
  o->pitch = ((w ? w : 1) * 4 + 3) & ~3u; o->pitch += (o->id & 3) * 64;      /* pads like a real driver's, differs per surface */
  fakedev_log(o->id);
  return o;
}
void *fakedev_surface_new(uint32_t w, uint32_t h, uint32_t fmt, uint32_t usage) { return fdr_new(FDR_SURF, w, h, 1, usage, fmt, 0); }
void *fakedev_texture_new(uint32_t w, uint32_t h, uint32_t levels, uint32_t usage, uint32_t fmt, uint32_t pool) { return fdr_new(FDR_TEX, w, h, levels, usage, fmt, pool); }

/* objects */
static unsigned long __stdcall fdr_Release(struct fdr_obj *o) { fakedev_log(FAKEDEV_TAG | 0x2002); fakedev_log(o->id); if (o->refs) o->refs--; else fakedev_log(0xdead0bad); fakedev_log(o->refs); return o->refs; }
static uint32_t fdr_chain(struct fdr_obj *t) { uint32_t n = 1, m = t->w > t->h ? t->w : t->h; while (m > 1) { m >>= 1; n++; } return t->levels ? t->levels : n; }
static long fdr_level(struct fdr_obj *t, uint32_t face, uint32_t level, void **pp) {
  uint32_t w, h;
  fakedev_log(t->id); fakedev_log(face); fakedev_log(level); fakedev_log((uint32_t)fakedev_levelhr);
  if (fakedev_levelhr < 0 || level >= fdr_chain(t) || face > 5) { *pp = 0; return fakedev_levelhr < 0 ? fakedev_levelhr : (long)0x8876086c; }
  w = t->w >> level; h = (t->kind == FDR_CUBE ? t->w : t->h) >> level;
  *pp = fdr_new(FDR_SURF, w ? w : 1, h ? h : 1, 1, t->usage, t->fmt, t->pool);
  return 0;
}
static long __stdcall fdr_GetSurfaceLevel(struct fdr_obj *t, uint32_t level, void **pp) { fakedev_log(FAKEDEV_TAG | 0x2012); return fdr_level(t, 0, level, pp); }
static long __stdcall fdr_GetCubeMapSurface(struct fdr_obj *t, uint32_t face, uint32_t level, void **pp) { fakedev_log(FAKEDEV_TAG | 0x2112); return fdr_level(t, face, level, pp); }
static long __stdcall fdr_GetDesc(struct fdr_obj *s, uint32_t *d) {
  fakedev_log(FAKEDEV_TAG | 0x200c); fakedev_log(s->id);
  d[0] = s->fmt; d[1] = 1; d[2] = s->usage; d[3] = s->pool; d[4] = 0; d[5] = 0; d[6] = s->w; d[7] = s->h;
  return 0;
}
static long __stdcall fdr_LockRect(struct fdr_obj *s, int32_t *lr, const int32_t *rect, uint32_t flags) {
  fakedev_log(FAKEDEV_TAG | 0x200d); fakedev_log(s->id); fakedev_log(flags); fakedev_log((uint32_t)fakedev_surflockhr); fakedev_log(rect ? 1 : 0);
  if (rect) { fakedev_log(rect[0]); fakedev_log(rect[1]); fakedev_log(rect[2]); fakedev_log(rect[3]); }
  if (fakedev_surflockhr < 0) return fakedev_surflockhr;
  s->locked++;
  lr[0] = (int32_t)s->pitch; lr[1] = (int32_t)(0x40000000u + (s->id & 0xffff) * 0x10000u + (rect ? (uint32_t)rect[1] * s->pitch + (uint32_t)rect[0] * 4 : 0));
  return 0;
}
static long __stdcall fdr_UnlockRect(struct fdr_obj *s) { fakedev_log(FAKEDEV_TAG | 0x200e); fakedev_log(s->id); fakedev_log(s->locked); if (s->locked) s->locked--; return 0; }

/* device slots */
static long __stdcall fdr_CreateTexture(void *d, uint32_t w, uint32_t h, uint32_t levels, uint32_t usage, uint32_t fmt, uint32_t pool, void **pp, void *share) {
  fakedev_log(FAKEDEV_TAG | 23); fakedev_log(w); fakedev_log(h); fakedev_log(levels); fakedev_log(usage); fakedev_log(fmt); fakedev_log(pool);
  fakedev_log((uint32_t)(uintptr_t)share); fakedev_log((uint32_t)fakedev_texhr);
  if (fakedev_texhr < 0) { *pp = 0; return fakedev_texhr; }
  *pp = fdr_new(FDR_TEX, w, h, levels, usage, fmt, pool);
  return 0;
}
static long __stdcall fdr_CreateCubeTexture(void *d, uint32_t edge, uint32_t levels, uint32_t usage, uint32_t fmt, uint32_t pool, void **pp, void *share) {
  fakedev_log(FAKEDEV_TAG | 25); fakedev_log(edge); fakedev_log(levels); fakedev_log(usage); fakedev_log(fmt); fakedev_log(pool);
  fakedev_log((uint32_t)(uintptr_t)share); fakedev_log((uint32_t)fakedev_texhr);
  if (fakedev_texhr < 0) { *pp = 0; return fakedev_texhr; }
  *pp = fdr_new(FDR_CUBE, edge, edge, levels, usage, fmt, pool);
  return 0;
}
static long __stdcall fdr_UpdateSurface(void *d, void *src, const int32_t *rect, void *dst, const int32_t *pt) {
  fakedev_log(FAKEDEV_TAG | 30); fakedev_log(fdr_id(src)); fakedev_log(fdr_id(dst)); fakedev_log(rect ? 1 : 0); fakedev_log(pt ? 1 : 0);
  if (rect) { fakedev_log(rect[0]); fakedev_log(rect[1]); fakedev_log(rect[2]); fakedev_log(rect[3]); }
  if (pt) { fakedev_log(pt[0]); fakedev_log(pt[1]); }
  return fakedev_hr;
}
static long __stdcall fdr_GetRenderTargetData(void *d, void *rt, void *dst) { fakedev_log(FAKEDEV_TAG | 32); fakedev_log(fdr_id(rt)); fakedev_log(fdr_id(dst)); return fakedev_hr; }
static long __stdcall fdr_CreateOffscreenPlainSurface(void *d, uint32_t w, uint32_t h, uint32_t fmt, uint32_t pool, void **pp, void *share) {
  fakedev_log(FAKEDEV_TAG | 36); fakedev_log(w); fakedev_log(h); fakedev_log(fmt); fakedev_log(pool); fakedev_log((uint32_t)(uintptr_t)share); fakedev_log((uint32_t)fakedev_offhr);
  if (fakedev_offhr < 0) { *pp = 0; return fakedev_offhr; }
  *pp = fdr_new(FDR_SURF, w, h, 1, 0, fmt, pool);
  return 0;
}

/* IDirect3D9: a fixed answer per (seed, usage, formats): 3 in 4 formats exist, D3DOK_NOAUTOGEN sometimes for AUTOGENMIPMAP; fakedev_fmtflip != 0 inverts
   the next answer (and is cleared) */
static long fdr_answer(uint32_t a, uint32_t b, uint32_t c) {
  uint32_t h = fakedev_capsseed ^ a * 0x9e3779b1u ^ b * 0x85ebca77u ^ c * 0xc2b2ae3du; long r;
  h ^= h >> 15; h *= 0x2c1b3c6du; h ^= h >> 12;
  r = (h & 3) == 0 ? (long)0x8876086a : (a == 0x400 && (h & 0x30) == 0x30) ? 0x0876086f : 0;      /* NOTAVAILABLE / NOAUTOGEN / OK */
  if (fakedev_fmtflip) { fakedev_fmtflip = 0; r = r < 0 ? 0 : (long)0x8876086a; }
  return r;
}
static long __stdcall fdr_CheckDeviceFormat(void *o, uint32_t ad, uint32_t dt, uint32_t afmt, uint32_t usage, uint32_t rtype, uint32_t fmt) {
  long r = fdr_answer(usage, fmt, rtype + afmt * 7);
  fakedev_log(FAKEDEV_TAG | 0x3010); fakedev_log(ad); fakedev_log(dt); fakedev_log(afmt); fakedev_log(usage); fakedev_log(rtype); fakedev_log(fmt); fakedev_log((uint32_t)r);
  return r;
}
static long __stdcall fdr_CheckDepthStencilMatch(void *o, uint32_t ad, uint32_t dt, uint32_t afmt, uint32_t rt, uint32_t ds) {
  long r = fdr_answer(0x5000 + rt, ds, afmt);
  if (r > 0) r = 0;
  fakedev_log(FAKEDEV_TAG | 0x3012); fakedev_log(ad); fakedev_log(dt); fakedev_log(afmt); fakedev_log(rt); fakedev_log(ds); fakedev_log((uint32_t)r);
  return r;
}

void fakedev_raster_reset(uint32_t capsseed) { fdr_nobj = 0; fakedev_capsseed = capsseed; fakedev_texhr = fakedev_offhr = fakedev_levelhr = fakedev_surflockhr = fakedev_fmtflip = 0; }
void fakedev_raster_init(void) {
  int i;
  void *const d[17] = { (void *)fdt_d0, (void *)fdt_d1, (void *)fdt_d2, (void *)fdt_d3, (void *)fdt_d4, (void *)fdt_d5, (void *)fdt_d6, (void *)fdt_d7, (void *)fdt_d8,
                        (void *)fdt_d9, (void *)fdr_CheckDeviceFormat, (void *)fdt_d11, (void *)fdr_CheckDepthStencilMatch, (void *)fdt_d13, (void *)fdt_d14,
                        (void *)fdt_d15, (void *)fdt_d16 };
  void *const t[22] = { (void *)fdt_t0, (void *)fdt_t1, (void *)fdr_Release, (void *)fdt_t3, (void *)fdt_t4, (void *)fdt_t5, (void *)fdt_t6, (void *)fdt_t7, (void *)fdt_t8,
                        (void *)fdt_t9, (void *)fdt_t10, (void *)fdt_t11, (void *)fdt_t12, (void *)fdt_t13, (void *)fdt_t14, (void *)fdt_t15, (void *)fdt_t16,
                        (void *)fdt_t17, (void *)fdr_GetSurfaceLevel, (void *)fdt_t19, (void *)fdt_t20, (void *)fdt_t21 };
  void *const s[17] = { (void *)fdt_s0, (void *)fdt_s1, (void *)fdr_Release, (void *)fdt_s3, (void *)fdt_s4, (void *)fdt_s5, (void *)fdt_s6, (void *)fdt_s7, (void *)fdt_s8,
                        (void *)fdt_s9, (void *)fdt_s10, (void *)fdt_s11, (void *)fdr_GetDesc, (void *)fdr_LockRect, (void *)fdr_UnlockRect, (void *)fdt_s15, (void *)fdt_s16 };
  for (i = 0; i < 17; i++) { fdr_d3dvtbl[i] = d[i]; fdr_surfvtbl[i] = s[i]; }
  for (i = 0; i < 22; i++) { fdr_texvtbl[i] = t[i]; fdr_cubevtbl[i] = t[i]; }
  fdr_cubevtbl[18] = (void *)fdr_GetCubeMapSurface;
  fdr_d3dobj[0] = fdr_d3dvtbl;
  fakedev_vtbl[23] = (void *)fdr_CreateTexture; fakedev_vtbl[25] = (void *)fdr_CreateCubeTexture; fakedev_vtbl[30] = (void *)fdr_UpdateSurface;
  fakedev_vtbl[32] = (void *)fdr_GetRenderTargetData; fakedev_vtbl[36] = (void *)fdr_CreateOffscreenPlainSurface;
  if (!fdr_objs) fdr_objs = calloc(FDR_OBJS, sizeof *fdr_objs);      /* heap, not .bss (the check maps the exe image over its own .bss block) */
  fakedev_raster_reset(0);
}
