// cluster-entries: ui_rt2d_init
/* Installed cluster ui_rt2d_init (tools/cluster_install.py from output/cluster_pilot_zx/ui_rt2d_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0042f860*/ extern int __cdecl texdict_load_named_b(char *p0, undefined4 p1, undefined4 p2);
/*00563130*/ extern int Rt2dOpen();
/*005649f0*/ extern int Rt2dBrushCreate();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00567780*/ extern void FUN_00567780(undefined4 p0);
/*005de820*/ extern undefined4 g_app_boot_5de820;
/*005de824*/ extern undefined4 g_app_boot_5de824;
/*005de828*/ extern undefined2 g_app_boot_5de828;
/*005de82a*/ extern undefined1 g_app_boot_5de82a;
/*005de82c*/ extern undefined4 g_app_boot_5de82c;
/*005de830*/ extern undefined4 g_app_boot_5de830;
/*005de834*/ extern undefined2 g_app_boot_5de834;
/*005de836*/ extern undefined1 g_app_boot_5de836;
/*0069b9f8*/ extern undefined4 g_course_69b9f8;
/*0069b9fc*/ extern undefined4 g_app_boot_69b9fc;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba04*/ extern float g_ui_layout_scale_y;
/*0069ba08*/ extern undefined4 g_ui_font_texdict;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
/*0069ba14*/ extern undefined4 g_ui_brush_14;
/*0069ba18*/ extern undefined4 g_ui_brush_transparent_black;
/*0069ba1c*/ extern undefined4 g_ui_brush_white;
#define COLOUR_CHANNEL_FULL (0xff)

/* ui_rt2d_init */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Sets up the UI font and brushes: opens Rt2d, loads the font texture dict and the font,
   creates the brushes (white and opaque black, plus two more) and zeroes the course,
   g_app_boot_69b9fc and UI layout scale globals (inferred). Takes no parameters, returns nothing.
   (was: "opens Rt2d, loads the font texture dict, creates the brushes and clears the text rectangles") */

static undefined4 rt2d_font_load_r(int p0);
extern void rt2d_font_load(void);
void ui_rt2d_init(void)

{
  char *texdict_name;
  undefined4 texdict_arg_1;
  undefined4 texdict_arg_2;
  struct {
    undefined1 white_r;
    undefined1 white_g;
    undefined1 white_b;
    undefined1 white_a;
    undefined1 black_r;
    undefined1 black_g;
    undefined1 black_b;
    undefined1 black_a;
    undefined4 texdict_name;
    undefined4 texdict_name_4;
    undefined2 texdict_name_8;
    undefined1 texdict_name_10;
    undefined1 pad_11[1];
    undefined4 font_args;
    undefined4 font_args_4;
    undefined2 font_args_8;
    undefined1 font_args_10;
  } font_setup;
  
  /* Pack the font setup block: white (ff ff ff ff) at white_r, opaque black (00 00 00 ff)
     at black_r, texture dict name at texdict_name, font load args at font_args..font_args_10. */
  font_setup.black_a = COLOUR_CHANNEL_FULL;
  font_setup.white_r = COLOUR_CHANNEL_FULL;
  font_setup.white_g = COLOUR_CHANNEL_FULL;
  font_setup.white_b = COLOUR_CHANNEL_FULL;
  font_setup.white_a = COLOUR_CHANNEL_FULL;
  font_setup.font_args = g_app_boot_5de82c;
  font_setup.font_args_4 = g_app_boot_5de830;
  font_setup.font_args_8 = g_app_boot_5de834;
  font_setup.font_args_10 = g_app_boot_5de836;
  font_setup.texdict_name = g_app_boot_5de820;
  font_setup.black_r = 0;
  font_setup.black_g = 0;
  font_setup.black_b = 0;
  font_setup.texdict_name_4 = g_app_boot_5de824;
  font_setup.texdict_name_8 = g_app_boot_5de828;
  font_setup.texdict_name_10 = g_app_boot_5de82a;
  /* Open Rt2d, then load the font texture dict by name (zero extra args) */
  Rt2dOpen(0);
  texdict_arg_2 = 0;
  texdict_name = (char *)&font_setup.texdict_name;
  texdict_arg_1 = 0;
  g_ui_font_texdict = texdict_load_named_b(texdict_name,texdict_arg_1,texdict_arg_2);
  FUN_00567780(g_ui_font_texdict);
  /* Load the UI font from the packed setup block */
  g_ui_font = rt2d_font_load_r(&font_setup.font_args);
  /* Create the brushes: first gets the opaque black colour, white and text brush get white,
     brush_14 is created with no colour set */
  g_ui_brush_transparent_black = Rt2dBrushCreate();
  Rt2dBrushSetRGBA(g_ui_brush_transparent_black,&font_setup.black_r,&font_setup.black_r,&font_setup.black_r,&font_setup.black_r);
  g_ui_brush_white = Rt2dBrushCreate();
  Rt2dBrushSetRGBA(g_ui_brush_white,&font_setup.white_r,&font_setup.white_r,&font_setup.white_r,&font_setup.white_r);
  g_ui_text_brush = Rt2dBrushCreate();
  g_ui_brush_14 = Rt2dBrushCreate();
  Rt2dBrushSetRGBA(g_ui_text_brush,&font_setup.white_r,&font_setup.white_r,&font_setup.white_r,&font_setup.white_r);
  /* Reset the globals this init owns (inferred: course and layout scale defaults) */
  g_course_69b9f8 = 0;
  g_app_boot_69b9fc = 0;
  g_ui_layout_scale_x = 0;
  g_ui_layout_scale_y = 0;
  return;
}

/* rt2d_font_load_r: C -> original rt2d_font_load (registers eax) */
__declspec(naked) undefined4 rt2d_font_load_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rt2d_font_load
        ret
    }
}
