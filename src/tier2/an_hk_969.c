#include "mfl_tier2.h"
/*0076e890*/ extern undefined4 g_app_76e890;
/*0076e894*/ extern undefined4 g_app_76e894;
/*0076e8b4*/ extern int *g_movie_renderer_76e8b4;
/*0076e8b8*/ extern undefined4 *g_movie_renderer_76e8b8;
/* DirectShow COM methods (stdcall: the callee pops the arguments), slot offsets in the vtable */
typedef int (__stdcall *GetEventMethod)(int *self,int *event_code,int *param1,int *param2,int timeout);
typedef int (__stdcall *FreeEventParamsMethod)(int *self,int event_code,int param1,int param2);
typedef int (__stdcall *RendererMethod)(undefined4 *self,int a,int b);
#define MEDIA_EVENT_GET_EVENT (0x20)
#define MEDIA_EVENT_FREE_EVENT_PARAMS (0x30)
#define RENDERER_SLOT_STOP (0x20)
#define MEDIA_EVENT_CODE_COMPLETE (1)
/* movie_renderer_poll */

/* "polls the DirectShow media event; on event 1 (complete, inferred) calls slot 0x20 of the renderer and clears g_app_76e890 unless g_app_76e894 is set" */

void movie_renderer_poll(void)

{
  int event_code;
  int event_param2;
  int event_param1;
  
  if (g_movie_renderer_76e8b4 != (int *)0x0) {
    if (-1 < (*(GetEventMethod *)(*g_movie_renderer_76e8b4 + MEDIA_EVENT_GET_EVENT))
                      (g_movie_renderer_76e8b4,&event_code,&event_param1,&event_param2,0)) {
      if (event_code == MEDIA_EVENT_CODE_COMPLETE) {
        (*(RendererMethod *)(*g_movie_renderer_76e8b8 + RENDERER_SLOT_STOP))(g_movie_renderer_76e8b8,0,0);
        if (g_app_76e894 == 0) {
          g_app_76e890 = 0;
        }
      }
      (*(FreeEventParamsMethod *)(*g_movie_renderer_76e8b4 + MEDIA_EVENT_FREE_EVENT_PARAMS))
                (g_movie_renderer_76e8b4,event_code,event_param1,event_param2);
    }
  }
  return;
}
