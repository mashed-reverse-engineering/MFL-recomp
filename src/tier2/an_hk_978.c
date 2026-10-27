#include "mfl_tier2.h"
/*004b2a60*/ extern int sprintf();
/*004b43a9*/ extern int _ctime64();
/*004b43c2*/ extern int _ftime64();
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
static int raster_write_image_r(int reg_eax, RwChar * unused_2);
extern void __cdecl raster_write_image(void);
__declspec(naked) int raster_write_image_r(int reg_eax, RwChar * unused_2)
{
    __asm {
        push dword ptr [esp + 8]
        mov eax, dword ptr [esp + 8]
        call raster_write_image
        add esp, 4
        ret
    }
}

/* video_mode_name_format */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "'%.19s.%hu' mode name" */

void __cdecl video_mode_name_format(undefined4 raster,undefined4 unused_2)

{
  char ch;
  undefined4 time_text;
  char *name_cursor;
  union {
    char gs_buf[272];
    struct {
      undefined1 time_buf [8];
      undefined2 millitm;
      undefined1 time_pad[6];
      char file_name [256];
    } m;
  } frame;
  
  _ftime64(frame.m.time_buf);
  time_text = _ctime64(frame.m.time_buf);
  sprintf(frame.m.file_name,"%.19s.%hu",time_text,frame.m.millitm);
  name_cursor = frame.m.file_name;
  ch = *name_cursor;
  while (ch != '\0') {
    if (((ch == ' ') || (ch == ':')) || (ch == '.')) {
      *name_cursor = '_';
    }
    ch = name_cursor[1];
    name_cursor = name_cursor + 1;
  }
  rwstrcat(frame.m.file_name,".bmp");
  raster_write_image_r(raster,frame.m.file_name);
  return;
}


