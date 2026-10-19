// cluster-entries: video_mode_name_format_004a8670
/* Installed cluster video_mode_name_format_004a8670 (tools/cluster_install.py from output/cluster_pilot_hb1/video_mode_name_format_004a8670.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern int sprintf();
extern char *strcat(char *dest, const char *src);
/* video_mode_name_format_004a8670 */
/* compile: /GS */

/* "800 x 600 x 32 [Fullscreen][Interlaced]": the mode as text (duplicate name: was video_mode_name_format, first address 004a84d0) */

void video_mode_name_format_004a8670_n(int *in_ebx, char *param_2)

{
  unsigned int flags;
  char flag_text[128] = "";

  flags = in_ebx[3];
  if ((flags & 1) == 0) {
    strcat(flag_text,"[Window]");
  }
  else {
    strcat(flag_text,"[Fullscreen]");
  }
  if ((flags & 2) != 0) {
    strcat(flag_text,"[Interlaced]");
  }
  if ((flags & 4) != 0) {
    strcat(flag_text,"[Flicker Free Interlaced]");
  }
  if ((flags & 0x100) != 0) {
    strcat(flag_text,"[Fullscreen antialiasing mode 0]");
  }
  if ((flags & 0x200) != 0) {
    strcat(flag_text,"[Fullscreen antialiasing mode 1]");
  }
  sprintf(param_2,"%d x %d x %d %s",*in_ebx,in_ebx[1],in_ebx[2],flag_text);
  return;
}

/* entry thunk at the original address of video_mode_name_format_004a8670: register inputs (ebx,s0) -> natural C video_mode_name_format_004a8670_n; ret 0 */
__declspec(naked) void video_mode_name_format_004a8670(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ebx
        call video_mode_name_format_004a8670_n
        add esp, 8
        ret
    }
}
