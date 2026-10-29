// cluster-entries: movie_renderer_create
/* Installed cluster movie_renderer_create (tools/cluster_install.py from output/cluster_pilot_z2/movie_renderer_create.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS */
#include "mfl_tier2.h"
#include <malloc.h>
extern __declspec(dllimport) int __stdcall CoCreateInstance(const void *clsid, void *outer, int context, const void *iid, void *out);
extern __declspec(dllimport) char * __stdcall lstrcpyA(char *dst, const char *src);
extern __declspec(dllimport) int __stdcall lstrlenA(const char *s);
/*004a3420*/ extern void app_error_message_box(const char *format, ...);
/*004b3c0c*/ extern void *operator_new(unsigned int size);
/*004a3070*/ extern wchar_t * __stdcall multibyte_to_wide_checked(wchar_t *dst, const char *src, int dst_chars, unsigned int code_page);
/*004b3bcf*/ extern wchar_t *_wcsncpy(wchar_t *dst, const wchar_t *src, unsigned int count);

/* A COM object: first member is the pointer to the method table. */
typedef struct ComObject { void **vtbl; } ComObject;
#define COM(obj, index, type) ((type)((ComObject *)(obj))->vtbl[index])
#define COM_RELEASE(obj) (COM(obj, 2, int (__stdcall *)(void *)))
typedef int (__stdcall *ComCall1)(void *self);
typedef int (__stdcall *ComCall2)(void *self, void *a);
typedef int (__stdcall *ComCall3)(void *self, void *a, void *b);
typedef int (__stdcall *ComCall4)(void *self, void *a, void *b, void *c);
typedef int (__cdecl *CodePageFn)(void);

#define E_FAIL_HRESULT ((int)0x80004005)
#define VFW_E_NOT_FOUND ((int)0x80040216)
#define CLSCTX_INPROC_SERVER 3

extern ComObject *g_movie_renderer_76e8bc;   /* filter graph */
extern ComObject *g_movie_renderer_76e8b0;   /* the texture renderer filter */
extern ComObject *g_movie_renderer_76e8c0;   /* media control */
extern void *g_movie_renderer_76e8b8, *g_movie_renderer_76e8b4, *g_app_76e8ac;
extern int g_movie_renderer_76e8a8;          /* the movie number */
extern int g_movie_renderer_76e88c, g_app_62ad50;
extern const char g_movie_renderer_5e212c[], g_movie_renderer_5e1f9c[], g_movie_renderer_5e20ac[], g_video_5e209c[], g_movie_renderer_5e0f9c[], g_movie_renderer_5e0fac[];
extern const char *PTR_s_toastart_pc_movies_frontend_mpg_0062ad3c[];   /* movie file names */
extern CodePageFn PTR_timer_select_implementation_0062ad54;

/* Builds the DirectShow graph for movie `movie`: the filter graph, our texture renderer ("TEXTURERENDERER"), the file source, and runs it.
   Returns 0 or the failing HRESULT (E_FAIL when the graph or the renderer object could not be created); every failure after the graph exists shows a message box.
   The original also registers a C++ exception frame (fs:[0]) around the body; its only job is to Release the two COM pointers on unwinding, which the
   straight-line releases below already do on every normal exit, so no EH is reproduced (diffemu SEH=1 compares the restored fs:[0] chain: equal). */
static void * movie_renderer_construct_r(void * p0, int p1, int * p2);
extern void movie_renderer_construct(void);
int movie_renderer_create(int movie)
{
  ComObject *source = 0, *pin = 0;
  int hr = 0, result = E_FAIL_HRESULT, arg = 0;
  const char *message = 0;
  char path[260];
  wchar_t wide[260];

  g_movie_renderer_76e8a8 = movie;
  do {
    void *mem;
    ComObject *renderer;
    wchar_t *converted;
    int chars, code_page;

    if (CoCreateInstance(g_movie_renderer_5e212c, 0, CLSCTX_INPROC_SERVER, g_movie_renderer_5e1f9c, &g_movie_renderer_76e8bc) < 0) break;
    mem = operator_new(0x170);
    renderer = mem ? movie_renderer_construct_r(mem, 0, &hr) : 0;
    if (hr < 0 || !renderer) { message = "Could not create texture renderer object!  hr=0x%x"; arg = hr; break; }
    renderer = (ComObject *)((char *)renderer + 0xc);       /* the IBaseFilter inside the object */
    if (renderer) COM(renderer, 1, ComCall1)(renderer);      /* AddRef (the original keeps the C++ null check of the interface pointer) */
    if (g_movie_renderer_76e8b0) COM_RELEASE(g_movie_renderer_76e8b0)(g_movie_renderer_76e8b0);
    g_movie_renderer_76e8b0 = renderer;
    hr = COM(g_movie_renderer_76e8bc, 3, ComCall3)(g_movie_renderer_76e8bc, renderer, L"TEXTURERENDERER");
    if (hr < 0) { message = "Could not add renderer filter to graph!  hr=0x%x"; arg = hr; result = hr; break; }

    lstrcpyA(path, PTR_s_toastart_pc_movies_frontend_mpg_0062ad3c[movie]);
    path[259] = 0;
    code_page = PTR_timer_select_implementation_0062ad54();
    chars = lstrlenA(path) + 1;
    converted = multibyte_to_wide_checked((wchar_t *)_alloca(chars * 2), path, chars, code_page);   /* the original probes the stack (_alloca_probe = _chkstk) */
    _wcsncpy(wide, converted, 0x104);
    hr = COM(g_movie_renderer_76e8bc, 14, ComCall4)(g_movie_renderer_76e8bc, wide, L"SOURCE", &source);
    result = hr;
    if (hr == VFW_E_NOT_FOUND) { message = "Could not add source filter to graph (hr==VFW_E_NOT_FOUND) (filename=%s)"; arg = (int)path; break; }
    if (hr < 0) { message = "Could not add source filter to graph!  hr=0x%x"; arg = hr; break; }
    hr = COM(source, 11, ComCall3)(source, L"Output", &pin);
    result = hr;
    if (hr < 0) { message = "Could not find output pin!  hr=0x%x"; arg = hr; break; }
    hr = COM(g_movie_renderer_76e8bc, 12, ComCall2)(g_movie_renderer_76e8bc, pin);
    result = hr;
    if (hr < 0) { message = "Could not render source output pin!  hr=0x%x"; arg = hr; break; }
    /* QueryInterface for the media control, the two other interfaces and the media event (results other than the last are not checked) */
    COM(g_movie_renderer_76e8bc, 0, ComCall3)(g_movie_renderer_76e8bc, (void *)g_movie_renderer_5e20ac, &g_movie_renderer_76e8c0);
    COM(g_movie_renderer_76e8bc, 0, ComCall3)(g_movie_renderer_76e8bc, (void *)g_video_5e209c, &g_movie_renderer_76e8b8);
    COM(g_movie_renderer_76e8bc, 0, ComCall3)(g_movie_renderer_76e8bc, (void *)g_movie_renderer_5e0f9c, &g_movie_renderer_76e8b4);
    hr = COM(g_movie_renderer_76e8bc, 0, ComCall3)(g_movie_renderer_76e8bc, (void *)g_movie_renderer_5e0fac, &g_app_76e8ac);
    hr = COM(g_movie_renderer_76e8c0, 7, ComCall1)(g_movie_renderer_76e8c0);     /* Run */
    result = hr;
    if (hr < 0) { message = "Could not run the DirectShow graph!  hr=0x%x"; arg = hr; break; }
    g_movie_renderer_76e88c = 0;
    g_app_62ad50 = 1;
    result = 0;
  } while (0);
  if (message) app_error_message_box(message, arg);
  if (pin) COM_RELEASE(pin)(pin);
  if (source) COM_RELEASE(source)(source);
  return result;
}

/* movie_renderer_construct_r: C -> original movie_renderer_construct (registers ecx,s0,s1) */
__declspec(naked) void * movie_renderer_construct_r(void * p0, int p1, int * p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 12]
        call movie_renderer_construct
        ret
    }
}
