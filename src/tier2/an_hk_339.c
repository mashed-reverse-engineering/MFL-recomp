#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall ShowCursor(int);
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004a5550*/ extern int log_printf();
/*004a6910*/ extern int video_set_flag_771058_by_mode();
/*004a87d0*/ extern int video_dialog_free();
/*004a8860*/ extern int app_get_app_7712f4();
/*004a9070*/ extern int __cdecl video_config_dialog(int p0);
/*004dbca0*/ extern int FUN_004dbca0(void);
/*0062ad34*/ extern undefined4 g_app_62ad34;
/*0076e86c*/ extern undefined4 g_app_76e86c;
/*0076ece8*/ extern undefined4 g_video_select_76ece8;

/* offsets into the render object returned by FUN_004dbca0 (inferred from the log labels) */
#define VERTEX_SHADER_VERSION_OFFSET (0xc4)
#define PIXEL_SHADER_VERSION_OFFSET (0xcc)
/* shader versions are stored in the low 16 bits */
#define SHADER_VERSION_MASK (0xffff)

/* video_select_and_set_mode */

/* "logs 'VertexShaderVersion = %x' and 'PixelShader...' then sets the mode" */

/*
 * Runs the video settings dialog and, if the player confirms it, applies the selected mode.
 * Parameters: none (the dialog reads its settings from g_app_62ad34 (inferred)).
 * Returns 1 if the mode was set, 0 if the dialog was cancelled or the mode was not applied.
 */
undefined4 video_select_and_set_mode(void)

{
  int result;

  /* show the dialog; nonzero means the player confirmed */
  result = video_config_dialog(g_app_62ad34);
  if ((result != 0) && (g_app_76e86c == 0)) {
    /* hide the cursor when the app object exists (inferred: fullscreen play) */
    result = app_get_app_7712f4();
    if (result != 0) {
      ShowCursor(0);
    }
    /* create the render object and log its vertex/pixel shader versions */
    g_video_select_76ece8 = FUN_004dbca0();
    log_printf("VertexShaderVersion = %x\n",*(uint *)(g_video_select_76ece8 + VERTEX_SHADER_VERSION_OFFSET) & SHADER_VERSION_MASK);
    log_printf("PixelShaderVersion = %x\n",*(uint *)(g_video_select_76ece8 + PIXEL_SHADER_VERSION_OFFSET) & SHADER_VERSION_MASK);
    log_print("Calling PCVS_InitRenderMeshType\n");
    /* apply the chosen video mode */
    video_set_flag_771058_by_mode();
    return 1;
  }
  /* dialog cancelled or blocked: release the dialog */
  video_dialog_free();
  return 0;
}


