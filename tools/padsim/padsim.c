/* padsim8.dll: virtual DirectInput 8 game controllers for MFL.exe / MFLng.exe tests (Wine container or Windows; no kernel device needed).
 * The exe's dinput8.dll import is renamed to padsim8.dll (patch_import.py); this DLL loads the real dinput8, wraps IDirectInput8A and puts
 * simulated devices in front of the real ones.  Everything that is not a simulated device (the system keyboard: xdotool keys) goes to the real dinput8.
 * No padsim.cfg in the working directory (App_Executables) = plain pass-through.
 *
 * padsim.cfg lines:
 *   out=<file>            log (default padsim.log): API calls of interest, executed commands, force-feedback effects; never per-poll lines
 *   real=0                hide the real game controllers (default 1: they are enumerated after the simulated ones; the keyboard is always real)
 *   live=<file>           command file read while the game runs (default padsim.live): complete lines appended to it are executed at the next poll
 *   dev=<type> [name]     one simulated device, index = order of the dev lines (max 8).  Types: see types[] (xbox ps4 generic joystick wheel keyboard)
 *   <command>             executed at load (start state)
 *   at <tick> <command>   executed when the tick counter reaches <tick>; tick = number of GetDeviceState calls on device 0 (the game: one per frame;
 *                         the key dialog: one per 20 ms timer).  Lines must be in ascending tick order.
 * Commands (D = device index):
 *   button D B 0|1        button B up / down (keyboard type: B = DIK code)
 *   press D B [N]         button B down for N ticks (default 6), then up
 *   axis D x|y|z|rx|ry|rz|s0|s1 V     V = -1000 .. 1000 (0 = centre), scaled to the device range (default 0 .. 65535, DIPROP_RANGE is honoured);
 *                         raw DirectInput sense: x -1000 = left, y -1000 = stick forward / menu up
 *   pov D V               hat: hundredths of a degree (0 up, 9000 right, 18000 down, 27000 left), -1 = centred
 *   plug D 0|1            unplug / plug: an unplugged device is not enumerated (DIEDFL_ATTACHEDONLY), Poll = DIERR_INPUTLOST, Acquire = DIERR_UNPLUGGED
 * Only the ANSI interface (IID_IDirectInput8A, what the game asks for) is simulated; a W request is passed through and logged.
 * Build: make (i686-w64-mingw32-gcc). */
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dinput.h>

typedef struct { const char *key, *name; DWORD devtype; WORD vid, pid; int nbtn, npov, ff; unsigned axes; } Type;   /* axes: bit i = long i of DIJOYSTATE (X Y Z Rx Ry Rz S0 S1) */
static const Type types[] = {      /* as Windows shows these devices through DirectInput */
    { "xbox",     "Controller (XBOX 360 For Windows)", 0x10215, 0x045e, 0x028e, 10, 1, 1, 0x1f },   /* Z = both triggers on one axis */
    { "ps4",      "Wireless Controller",               0x10215, 0x054c, 0x09cc, 14, 1, 1, 0x3f },
    { "generic",  "USB Gamepad",                       0x10215, 0x0079, 0x0006, 12, 1, 0, 0x27 },
    { "joystick", "Logitech Extreme 3D",               0x10214, 0x046d, 0xc215, 12, 1, 0, 0x63 },   /* Rz twist, S0 throttle */
    { "wheel",    "Logitech Driving Force USB",        0x10116, 0x046d, 0xc294, 12, 1, 1, 0x03 },   /* X wheel, Y combined pedals */
    { "keyboard", "Keyboard 2",                        0x00413, 0, 0, 256, 0, 0, 0 },
};
static const char *axn[8] = { "x", "y", "z", "rx", "ry", "rz", "s0", "s1" };
typedef struct { const Type *t; char name[260]; int attached; LONG lo, hi, ax[8]; DWORD pov; BYTE btn[256]; int hold[256]; } Dev;
#define MAXDEV 8
static Dev devs[MAXDEV];
static int ndev, cfg_real = 1, loaded, nsched, isched, neff;
static unsigned tick_no; static DWORD live_off;
static char cfg_out[260] = "padsim.log", cfg_live[260] = "padsim.live";
static struct { unsigned at; char s[100]; } sched[4096];
static FILE *lg;
#define LG(...) do { if (lg) { fprintf(lg, __VA_ARGS__); fputc('\n', lg); fflush(lg); } } while (0)
#define KBD(p) ((p)->t->devtype == 0x413)

static void cmd(const char *s) {
    char v[16], a[16]; int d = -1, y = 6, i, n = sscanf(s, "%15s %d %15s %d", v, &d, a, &y);
    Dev *p = &devs[d < 0 || d >= ndev ? 0 : d];
    if (n < 3 || d < 0 || d >= ndev) goto bad;
    if (!strcmp(v, "button") && n == 4) { p->btn[atoi(a) & 255] = y ? 0x80 : 0; p->hold[atoi(a) & 255] = 0; }
    else if (!strcmp(v, "press")) { p->btn[atoi(a) & 255] = 0x80; p->hold[atoi(a) & 255] = y > 0 ? y : 1; }
    else if (!strcmp(v, "axis") && n == 4) { for (i = 0; i < 8 && strcmp(a, axn[i]); i++) ; if (i == 8) goto bad; p->ax[i] = y < -1000 ? -1000 : y > 1000 ? 1000 : y; }
    else if (!strcmp(v, "pov")) p->pov = (DWORD)atoi(a);
    else if (!strcmp(v, "plug")) p->attached = atoi(a) != 0;
    else goto bad;
    LG("CMD %u %s", tick_no, s); return;
bad: LG("BAD %u %s", tick_no, s);
}

static void chomp(char *l) { l[strcspn(l, "\r\n")] = 0; }

static void load(void) {
    char l[256]; FILE *f = fopen("padsim.cfg", "r");
    loaded = 1; if (!f) return;
    while (fgets(l, sizeof l, f)) {
        chomp(l); if (!l[0] || l[0] == '#') continue;
        if (!strncmp(l, "out=", 4)) { strncpy(cfg_out, l + 4, 259); continue; }
        if (!strncmp(l, "live=", 5)) { strncpy(cfg_live, l + 5, 259); continue; }
        if (!strncmp(l, "real=", 5)) { cfg_real = atoi(l + 5); continue; }
        if (!lg) lg = fopen(cfg_out, "w");
        if (!strncmp(l, "dev=", 4)) {
            char ty[16] = ""; int k, n = 0; sscanf(l + 4, "%15s %n", ty, &n);
            for (k = 0; k < (int)(sizeof types / sizeof *types) && strcmp(ty, types[k].key); k++) ;
            if (k == (int)(sizeof types / sizeof *types) || ndev == MAXDEV) { LG("BAD %s", l); continue; }
            Dev *p = &devs[ndev++]; p->t = &types[k]; p->attached = 1; p->lo = 0; p->hi = 65535; p->pov = (DWORD)-1;
            strncpy(p->name, l[4 + n] ? l + 4 + n : types[k].name, 259);
            LG("DEV %d %s \"%s\"", ndev - 1, ty, p->name);
        } else if (!strncmp(l, "at ", 3)) {
            int n = 0; unsigned at = 0;
            if (sscanf(l + 3, "%u %n", &at, &n) == 1 && nsched < 4096 && strlen(l + 3 + n) < 100) { sched[nsched].at = at; strcpy(sched[nsched++].s, l + 3 + n); } else LG("BAD %s", l);
        } else cmd(l);
    }
    fclose(f);
    DeleteFileA(cfg_live);      /* a live file of an earlier run must not replay */
}

static void live(void) {
    WIN32_FILE_ATTRIBUTE_DATA fa; char l[128]; FILE *f;
    if (!GetFileAttributesExA(cfg_live, GetFileExInfoStandard, &fa) || fa.nFileSizeLow <= live_off || !(f = fopen(cfg_live, "rb"))) return;
    fseek(f, live_off, SEEK_SET);
    while (fgets(l, sizeof l, f) && strchr(l, '\n')) { live_off = ftell(f); chomp(l); if (l[0]) cmd(l); }      /* a line without its newline yet waits for the next poll */
    fclose(f);
}

static void tick(void) {
    tick_no++;
    for (int d = 0; d < ndev; d++) for (int b = 0; b < 256; b++) if (devs[d].hold[b] && !--devs[d].hold[b]) devs[d].btn[b] = 0;
    while (isched < nsched && sched[isched].at <= tick_no) cmd(sched[isched++].s);
    live();
}

/* ---- GUIDs ---------------------------------------------------------------------------------------------------------------------- */
static GUID ginst(int i) { GUID g = { 0x53444150, 0x4d49, 0, { 0, 0, 0, 0, 0, 0, 0, (BYTE)i } }; return g; }      /* "PADSIM", device index in the last byte */
static GUID gprod(const Type *t) { GUID g = { MAKELONG(t->vid, t->pid), 0, 0, { 0, 0, 'P', 'I', 'D', 'V', 'I', 'D' } }; return g; }
static int devof(REFGUID g) { for (int i = 0; i < ndev; i++) { GUID x = ginst(i); if (!memcmp(g, &x, sizeof x)) return i; } return -1; }
static void fill_inst(DIDEVICEINSTANCEA *o, int i) {
    Dev *p = &devs[i]; memset(o, 0, sizeof *o); o->dwSize = sizeof *o;
    o->guidInstance = ginst(i); o->guidProduct = gprod(p->t); o->dwDevType = p->t->devtype;
    strcpy(o->tszInstanceName, p->name); strcpy(o->tszProductName, p->name);
    if (!KBD(p)) { o->wUsagePage = 1; o->wUsage = (p->t->devtype & 0xff) == 0x15 ? 5 : 4; }
}
static const char *effname(REFGUID g) {
    return IsEqualGUID(g, &GUID_ConstantForce) ? "constant" : IsEqualGUID(g, &GUID_Sine) ? "sine" : IsEqualGUID(g, &GUID_Square) ? "square" :
           IsEqualGUID(g, &GUID_RampForce) ? "ramp" : IsEqualGUID(g, &GUID_Spring) ? "spring" : IsEqualGUID(g, &GUID_Damper) ? "damper" : "other";
}

/* stubs for the slots nobody needs yet: stdcall, so the argument count (after `this`) must be right */
#define STUB(n, r, ...) static HRESULT WINAPI n(void *s, ##__VA_ARGS__) { return r; }
STUB(ok0, DI_OK) STUB(ok1, DI_OK, void *a) STUB(ok2, DI_OK, void *a, void *b) STUB(ok3, DI_OK, void *a, void *b, void *c)
STUB(un1, DIERR_UNSUPPORTED, void *a) STUB(un2, DIERR_UNSUPPORTED, void *a, void *b) STUB(un3, DIERR_UNSUPPORTED, void *a, void *b, void *c)
STUB(un4, DIERR_UNSUPPORTED, void *a, void *b, void *c, void *d)

/* ---- IDirectInputEffect (logged only) ------------------------------------------------------------------------------------------- */
typedef struct { void **vt; LONG ref; int id, dev; GUID g; } EF;
static void log_eff(const char *what, EF *e, LPCDIEFFECT p, DWORD flags) {
    char m[120] = ""; const LONG *t = p ? p->lpvTypeSpecificParams : NULL;
    if (p && t && p->cbTypeSpecificParams == sizeof(DICONSTANTFORCE)) sprintf(m, " magnitude=%ld", t[0]);
    else if (p && t && p->cbTypeSpecificParams == sizeof(DIRAMPFORCE)) sprintf(m, " start=%ld end=%ld", t[0], t[1]);
    else if (p && t && p->cbTypeSpecificParams == sizeof(DIPERIODIC)) sprintf(m, " magnitude=%ld offset=%ld phase=%ld period=%ld", t[0], t[1], t[2], t[3]);
    if (!p) { LG("FF %u %s dev=%d effect=%d %s (no params)", tick_no, what, e->dev, e->id, effname(&e->g)); return; }
    LG("FF %u %s dev=%d effect=%d %s flags=%lx duration=%ld gain=%ld axes=%ld dir0=%ld trigger=%lx%s", tick_no, what, e->dev, e->id, effname(&e->g), flags,
       p->dwDuration, p->dwGain, p->cAxes, p->cAxes && p->rglDirection ? p->rglDirection[0] : 0, p->dwTriggerButton, m);
}
static HRESULT WINAPI ef_qi(EF *e, REFIID r, void **o) { if (IsEqualGUID(r, &IID_IUnknown) || IsEqualGUID(r, &IID_IDirectInputEffect)) { e->ref++; *o = e; return S_OK; } *o = NULL; return E_NOINTERFACE; }
static ULONG WINAPI ef_addref(EF *e) { return ++e->ref; }
static ULONG WINAPI ef_release(EF *e) { ULONG r = --e->ref; if (!r) { LG("FF %u release dev=%d effect=%d", tick_no, e->dev, e->id); free(e); } return r; }
static HRESULT WINAPI ef_guid(EF *e, GUID *g) { *g = e->g; return DI_OK; }
static HRESULT WINAPI ef_setparams(EF *e, LPCDIEFFECT p, DWORD fl) { log_eff("set", e, p, fl); return DI_OK; }
static HRESULT WINAPI ef_start(EF *e, DWORD it, DWORD fl) { LG("FF %u start dev=%d effect=%d iterations=%lu flags=%lx", tick_no, e->dev, e->id, it, fl); return DI_OK; }
static HRESULT WINAPI ef_stop(EF *e) { LG("FF %u stop dev=%d effect=%d", tick_no, e->dev, e->id); return DI_OK; }
static HRESULT WINAPI ef_status(EF *e, DWORD *st) { *st = 0; return DI_OK; }
static void *ef_vt[] = { ef_qi, ef_addref, ef_release, ok3 /*Initialize*/, ef_guid, un2 /*GetParameters*/, ef_setparams, ef_start, ef_stop, ef_status, ok0 /*Download*/, ok0 /*Unload*/, un1 /*Escape*/ };

/* ---- IDirectInputDevice8A ------------------------------------------------------------------------------------------------------- */
typedef struct { void **vt; LONG ref; int i, acq; } SD;
#define P(d) (&devs[(d)->i])
static HRESULT WINAPI sd_qi(SD *d, REFIID r, void **o) { if (IsEqualGUID(r, &IID_IUnknown) || IsEqualGUID(r, &IID_IDirectInputDevice8A)) { d->ref++; *o = d; return S_OK; } LG("dev %d QueryInterface: other interface", d->i); *o = NULL; return E_NOINTERFACE; }
static ULONG WINAPI sd_addref(SD *d) { return ++d->ref; }
static ULONG WINAPI sd_release(SD *d) { ULONG r = --d->ref; if (!r) { LG("dev %d released at tick %u", d->i, tick_no); free(d); } return r; }
static int naxes(const Type *t) { int n = 0; for (int a = 0; a < 8; a++) n += t->axes >> a & 1; return n; }
static HRESULT WINAPI sd_caps(SD *d, DIDEVCAPS *c) {
    const Type *t = P(d)->t; DWORD sz = c->dwSize;
    if (sz != sizeof *c && sz != sizeof(DIDEVCAPS_DX3)) return DIERR_INVALIDPARAM;
    memset(c, 0, sz); c->dwSize = sz; c->dwFlags = (P(d)->attached ? DIDC_ATTACHED : 0) | (t->ff ? DIDC_FORCEFEEDBACK : 0);
    c->dwDevType = t->devtype; c->dwAxes = naxes(t); c->dwButtons = t->nbtn; c->dwPOVs = t->npov;
    LG("dev %d GetCapabilities: axes=%d buttons=%d povs=%d ff=%d", d->i, naxes(t), t->nbtn, t->npov, t->ff); return DI_OK;
}
static HRESULT WINAPI sd_enumobj(SD *d, LPDIENUMDEVICEOBJECTSCALLBACKA cb, void *ref, DWORD fl) {
    static const GUID *ag[8] = { &GUID_XAxis, &GUID_YAxis, &GUID_ZAxis, &GUID_RxAxis, &GUID_RyAxis, &GUID_RzAxis, &GUID_Slider, &GUID_Slider };
    const Type *t = P(d)->t; DIDEVICEOBJECTINSTANCEA o; int kb = KBD(P(d));
    LG("dev %d EnumObjects flags=%lx", d->i, fl);
#define OBJ(g, ofs, ty, ...) do { memset(&o, 0, sizeof o); o.dwSize = sizeof o; o.guidType = g; o.dwOfs = ofs; o.dwType = ty; sprintf(o.tszName, __VA_ARGS__); \
        if (t->ff && (o.dwType & DIDFT_ABSAXIS) && o.dwOfs < 8) { o.dwType |= DIDFT_FFACTUATOR; o.dwFlags = DIDOI_FFACTUATOR; } \
        if (cb(&o, ref) == DIENUM_STOP) return DI_OK; } while (0)
    if (fl == DIDFT_ALL || (fl & DIDFT_AXIS)) for (int a = 0; a < 8; a++) if (t->axes >> a & 1) OBJ(*ag[a], a * 4, DIDFT_ABSAXIS | DIDFT_MAKEINSTANCE(a), "%s axis", axn[a]);
    if (fl == DIDFT_ALL || (fl & DIDFT_POV)) for (int v = 0; v < t->npov; v++) OBJ(GUID_POV, 0x20 + v * 4, DIDFT_POV | DIDFT_MAKEINSTANCE(v), "Hat switch");
    if (fl == DIDFT_ALL || (fl & DIDFT_BUTTON)) for (int b = 0; b < t->nbtn; b++) OBJ(kb ? GUID_Key : GUID_Button, kb ? b : 0x30 + b, DIDFT_PSHBUTTON | DIDFT_MAKEINSTANCE(b), kb ? "Key %d" : "Button %d", b);
    return DI_OK;
}
static HRESULT WINAPI sd_getprop(SD *d, REFGUID id, DIPROPHEADER *h) {
    if (id == DIPROP_RANGE) { ((DIPROPRANGE *)h)->lMin = P(d)->lo; ((DIPROPRANGE *)h)->lMax = P(d)->hi; return DI_OK; }
    if (id == DIPROP_VIDPID) { ((DIPROPDWORD *)h)->dwData = MAKELONG(P(d)->t->vid, P(d)->t->pid); return DI_OK; }
    LG("dev %d GetProperty %u: unsupported", d->i, (unsigned)(UINT_PTR)id); return DIERR_UNSUPPORTED;
}
static HRESULT WINAPI sd_setprop(SD *d, REFGUID id, const DIPROPHEADER *h) {      /* one range for all axes of the device: per-axis ranges when a client needs them */
    if (id == DIPROP_RANGE) { P(d)->lo = ((const DIPROPRANGE *)h)->lMin; P(d)->hi = ((const DIPROPRANGE *)h)->lMax; LG("dev %d SetProperty range %ld..%ld (obj %lx how %lu)", d->i, P(d)->lo, P(d)->hi, h->dwObj, h->dwHow); }
    else LG("dev %d SetProperty %u = %ld (obj %lx how %lu): accepted, no effect", d->i, (unsigned)(UINT_PTR)id, h->dwSize >= sizeof(DIPROPDWORD) ? (long)((const DIPROPDWORD *)h)->dwData : 0, h->dwObj, h->dwHow);
    return DI_OK;
}
static HRESULT WINAPI sd_acquire(SD *d) { if (!P(d)->attached) return DIERR_UNPLUGGED; if (d->acq) return S_FALSE; d->acq = 1; LG("dev %d Acquire at tick %u", d->i, tick_no); return DI_OK; }
static HRESULT WINAPI sd_unacquire(SD *d) { d->acq = 0; return DI_OK; }
static HRESULT lost(SD *d) { if (P(d)->attached) return d->acq ? DI_OK : DIERR_NOTACQUIRED; if (d->acq) { d->acq = 0; return DIERR_INPUTLOST; } return DIERR_NOTACQUIRED; }
static HRESULT WINAPI sd_poll(SD *d) { return lost(d); }
static HRESULT WINAPI sd_state(SD *d, DWORD cb, void *out) {
    Dev *p = P(d); const Type *t = p->t; DIJOYSTATE2 js; HRESULT hr;
    if (d->i == 0) tick();
    if ((hr = lost(d)) != DI_OK) return hr;
    if (KBD(p)) { if (cb != 256) return DIERR_INVALIDPARAM; memcpy(out, p->btn, 256); return DI_OK; }
    if (cb != sizeof(DIJOYSTATE) && cb != sizeof js) return DIERR_INVALIDPARAM;
    memset(&js, 0, sizeof js);
    for (int a = 0; a < 8; a++) if (t->axes >> a & 1) (&js.lX)[a] = p->lo + (LONG)((LONGLONG)(p->ax[a] + 1000) * (p->hi - p->lo) / 2000);
    for (int v = 0; v < 4; v++) js.rgdwPOV[v] = v < t->npov ? p->pov : (DWORD)-1;
    memcpy(js.rgbButtons, p->btn, t->nbtn < 128 ? t->nbtn : 128);
    memcpy(out, &js, cb); return DI_OK;
}
static HRESULT WINAPI sd_data(SD *d, DWORD cb, void *buf, DWORD *n, DWORD fl) { *n = 0; return lost(d); }      /* buffered data: none */
static HRESULT WINAPI sd_format(SD *d, LPCDIDATAFORMAT f) { LG("dev %d SetDataFormat: data size 0x%lx, %lu objects, flags %lx", d->i, f->dwDataSize, f->dwNumObjs, f->dwFlags); return DI_OK; }
static HRESULT WINAPI sd_coop(SD *d, HWND w, DWORD fl) { LG("dev %d SetCooperativeLevel flags=%lx", d->i, fl); return DI_OK; }
static HRESULT WINAPI sd_info(SD *d, DIDEVICEINSTANCEA *o) { if (o->dwSize != sizeof *o) { LG("dev %d GetDeviceInfo: size 0x%lx", d->i, o->dwSize); return DIERR_INVALIDPARAM; } fill_inst(o, d->i); return DI_OK; }
static HRESULT WINAPI sd_createeffect(SD *d, REFGUID g, LPCDIEFFECT p, LPDIRECTINPUTEFFECT *out, LPUNKNOWN outer) {
    if (!P(d)->t->ff) { LG("dev %d CreateEffect: device has no force feedback", d->i); return DIERR_UNSUPPORTED; }
    EF *e = calloc(1, sizeof *e); e->vt = ef_vt; e->ref = 1; e->id = neff++; e->dev = d->i; e->g = *g;
    log_eff("create", e, p, 0); *out = (LPDIRECTINPUTEFFECT)e; return DI_OK;
}
static HRESULT WINAPI sd_enumeffects(SD *d, LPDIENUMEFFECTSCALLBACKA cb, void *ref, DWORD ty) {
    static const struct { const GUID *g; DWORD ty; const char *n; } ef[] = { { &GUID_ConstantForce, DIEFT_CONSTANTFORCE, "Constant" }, { &GUID_Sine, DIEFT_PERIODIC, "Sine" } };
    LG("dev %d EnumEffects type=%lx", d->i, ty);
    for (int i = 0; P(d)->t->ff && i < 2; i++) if (ty == DIEFT_ALL || DIEFT_GETTYPE(ty) == ef[i].ty) {
        DIEFFECTINFOA o; memset(&o, 0, sizeof o); o.dwSize = sizeof o; o.guid = *ef[i].g; o.dwEffType = ef[i].ty | DIEFT_FFATTACK | DIEFT_FFFADE;
        o.dwStaticParams = o.dwDynamicParams = DIEP_ALLPARAMS; strcpy(o.tszName, ef[i].n);
        if (cb(&o, ref) == DIENUM_STOP) break;
    }
    return DI_OK;
}
static HRESULT WINAPI sd_ffstate(SD *d, DWORD *st) { if (!P(d)->t->ff) return DIERR_UNSUPPORTED; *st = DIGFFS_ACTUATORSON | DIGFFS_POWERON | DIGFFS_EMPTY; return DI_OK; }
static HRESULT WINAPI sd_ffcmd(SD *d, DWORD c) { LG("FF %u command dev=%d flags=%lx", tick_no, d->i, c); return P(d)->t->ff ? DI_OK : DIERR_UNSUPPORTED; }
static void *sd_vt[32] = {
    sd_qi, sd_addref, sd_release, sd_caps, sd_enumobj, sd_getprop, sd_setprop, sd_acquire, sd_unacquire, sd_state, sd_data, sd_format,
    ok1 /*SetEventNotification*/, sd_coop, un3 /*GetObjectInfo*/, sd_info, ok2 /*RunControlPanel*/, ok3 /*Initialize*/,
    sd_createeffect, sd_enumeffects, un2 /*GetEffectInfo*/, sd_ffstate, sd_ffcmd, ok3 /*EnumCreatedEffectObjects*/, un1 /*Escape*/, sd_poll,
    un4 /*SendDeviceData*/, un4 /*EnumEffectsInFile*/, un4 /*WriteEffectToFile*/, un3 /*BuildActionMap*/, un3 /*SetActionMap*/, un1 /*GetImageInfo*/ };

/* ---- IDirectInput8A wrapper ----------------------------------------------------------------------------------------------------- */
typedef struct { void **vt; LONG ref; IDirectInput8A *real; } DI;
static HRESULT WINAPI di_qi(DI *s, REFIID r, void **o) { if (IsEqualGUID(r, &IID_IUnknown) || IsEqualGUID(r, &IID_IDirectInput8A)) { s->ref++; *o = s; return S_OK; } LG("IDirectInput8 QueryInterface: other interface"); *o = NULL; return E_NOINTERFACE; }
static ULONG WINAPI di_addref(DI *s) { return ++s->ref; }
static ULONG WINAPI di_release(DI *s) { ULONG r = --s->ref; if (!r) { IDirectInput8_Release(s->real); free(s); } return r; }
static HRESULT WINAPI di_create(DI *s, REFGUID g, LPDIRECTINPUTDEVICE8A *out, LPUNKNOWN outer) {
    int i = devof(g); if (i < 0) return IDirectInput8_CreateDevice(s->real, g, out, outer);
    SD *d = calloc(1, sizeof *d); d->vt = sd_vt; d->ref = 1; d->i = i; *out = (LPDIRECTINPUTDEVICE8A)d;
    LG("CreateDevice dev %d", i); return DI_OK;
}
typedef struct { LPDIENUMDEVICESCALLBACKA cb; void *ref; } RealEnum;
static BOOL CALLBACK real_enum(const DIDEVICEINSTANCEA *o, void *ref) {      /* real=0 hides the real game controllers only */
    RealEnum *r = ref; int t = o->dwDevType & 0xff;
    if (!cfg_real && t >= DI8DEVTYPE_JOYSTICK && t <= DI8DEVTYPE_SUPPLEMENTAL) return DIENUM_CONTINUE;
    return r->cb(o, r->ref);
}
static HRESULT WINAPI di_enum(DI *s, DWORD ty, LPDIENUMDEVICESCALLBACKA cb, void *ref, DWORD fl) {
    int n = 0; RealEnum r = { cb, ref };
    for (int i = 0; i < ndev; i++) {
        Dev *p = &devs[i]; DIDEVICEINSTANCEA o; int t = p->t->devtype & 0xff;
        if (!(ty == DI8DEVCLASS_ALL || (ty == DI8DEVCLASS_GAMECTRL && !KBD(p)) || (ty == DI8DEVCLASS_KEYBOARD && KBD(p)) || (int)ty == t)) continue;
        if (((fl & DIEDFL_ATTACHEDONLY) && !p->attached) || ((fl & DIEDFL_FORCEFEEDBACK) && !p->t->ff)) continue;
        fill_inst(&o, i); n++;
        if (cb(&o, ref) == DIENUM_STOP) { LG("EnumDevices type=%lx flags=%lx: %d simulated, stopped by the callback", ty, fl, n); return DI_OK; }
    }
    LG("EnumDevices type=%lx flags=%lx: %d simulated", ty, fl, n);
    return IDirectInput8_EnumDevices(s->real, ty, real_enum, &r, fl);
}
static HRESULT WINAPI di_status(DI *s, REFGUID g) { int i = devof(g); return i < 0 ? IDirectInput8_GetDeviceStatus(s->real, g) : devs[i].attached ? DI_OK : DI_NOTATTACHED; }
static HRESULT WINAPI di_panel(DI *s, HWND w, DWORD fl) { return IDirectInput8_RunControlPanel(s->real, w, fl); }
static HRESULT WINAPI di_init(DI *s, HINSTANCE h, DWORD v) { return IDirectInput8_Initialize(s->real, h, v); }
static HRESULT WINAPI di_find(DI *s, REFGUID g, LPCSTR n, LPGUID o) { return IDirectInput8_FindDevice(s->real, g, n, o); }
static HRESULT WINAPI di_sem(DI *s, LPCSTR u, LPDIACTIONFORMATA a, LPDIENUMDEVICESBYSEMANTICSCBA cb, void *ref, DWORD fl) { return IDirectInput8_EnumDevicesBySemantics(s->real, u, a, cb, ref, fl); }
static HRESULT WINAPI di_conf(DI *s, LPDICONFIGUREDEVICESCALLBACK cb, LPDICONFIGUREDEVICESPARAMSA p, DWORD fl, void *ref) { return IDirectInput8_ConfigureDevices(s->real, cb, p, fl, ref); }
static void *di_vt[] = { di_qi, di_addref, di_release, di_create, di_enum, di_status, di_panel, di_init, di_find, di_sem, di_conf };

__declspec(dllexport) HRESULT WINAPI DirectInput8Create(HINSTANCE inst, DWORD ver, REFIID riid, LPVOID *out, LPUNKNOWN outer) {
    static HRESULT (WINAPI *real)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
    HMODULE h; HRESULT hr;
    if (!loaded) load();
    if (!real && (!(h = LoadLibraryA("dinput8.dll")) || !(real = (void *)GetProcAddress(h, "DirectInput8Create")))) { LG("real dinput8.dll not loaded"); return E_FAIL; }
    hr = real(inst, ver, riid, out, outer);
    LG("DirectInput8Create version=%lx hr=%lx, %d simulated device(s)%s", ver, hr, ndev, IsEqualGUID(riid, &IID_IDirectInput8A) ? "" : " (not the A interface: passed through)");
    if (FAILED(hr) || !ndev || !IsEqualGUID(riid, &IID_IDirectInput8A)) return hr;
    DI *s = calloc(1, sizeof *s); s->vt = di_vt; s->ref = 1; s->real = *out; *out = s;
    return hr;
}
