// cluster-entries: build_info_frame
/* Installed cluster build_info_frame (tools/cluster_install.py from output/cluster_pilot_pg2/build_info_frame.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00469400*/ extern void sound_streams_resume(void);
/*004693c0*/ extern void __cdecl sound_streams_pause(int pause);
/*00473550*/ extern void __cdecl sound_frame_begin(int flag, int pause);
/*004a5930*/ extern void joypads_poll_frame(void);

/* build_info_frame: one frame of the build info screen: pauses the sound streams, polls the pads, draws the current page (the two
   arguments are handed through to the page drawing in registers), starts the sound frame and resumes the streams */
static void build_info_display_r(int p0, int p1);
extern void build_info_display(void);
void build_info_frame(int draw_arg_a, int draw_arg_b)
{
  sound_streams_pause(1);
  joypads_poll_frame();
  build_info_display_r(draw_arg_a, draw_arg_b);
  sound_frame_begin(1, 1);   /* the original passes only the first value; the second one is the stack slot of the pause flag above, 1 */
  sound_streams_resume();
}

/* build_info_display_r: C -> original build_info_display (registers ecx,eax) */
__declspec(naked) void build_info_display_r(int p0, int p1)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        mov eax, dword ptr [esp + 8]
        call build_info_display
        ret
    }
}
