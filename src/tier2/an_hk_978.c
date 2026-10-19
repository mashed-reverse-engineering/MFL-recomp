#include "mfl_tier2.h"
/*004b2a60*/ extern int sprintf();
/*004b43a9*/ extern int _ctime64();
/*004b43c2*/ extern int _ftime64();
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
static int raster_write_image_r(int reg_eax, RwChar * param_2);
extern void __cdecl raster_write_image(void);
__declspec(naked) int raster_write_image_r(int reg_eax, RwChar * param_2)
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

void __cdecl video_mode_name_format(undefined4 param_1,undefined4 param_2)

{
  char cVar2;
  undefined4 uVar3;
  char *pcVar4;
  union {
    char gs_buf[272];
    struct {
      undefined1 local_114 [8];
      undefined2 local_10c;
      undefined1 pad_10a[6];
      char local_104 [256];
    } m;
  } frame;
  
  _ftime64(frame.m.local_114);
  uVar3 = _ctime64(frame.m.local_114);
  sprintf(frame.m.local_104,"%.19s.%hu",uVar3,frame.m.local_10c);
  pcVar4 = frame.m.local_104;
  cVar2 = *pcVar4;
  while (cVar2 != '\0') {
    if (((cVar2 == ' ') || (cVar2 == ':')) || (cVar2 == '.')) {
      *pcVar4 = '_';
    }
    cVar2 = pcVar4[1];
    pcVar4 = pcVar4 + 1;
  }
  rwstrcat(frame.m.local_104,".bmp");
  raster_write_image_r(param_1,frame.m.local_104);
  return;
}


