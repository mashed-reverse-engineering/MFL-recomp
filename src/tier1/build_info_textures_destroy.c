#include "mfl_tier2.h"
/*004d5bf0*/ extern RwBool RwTextureDestroy(struct RwTexture *p0);
/*004ddb80*/ extern RwChar * RwImageSetPath(RwChar *p0);
/*0069bb34*/ extern undefined4 g_speaker_69bb34;
/*0069bb38*/ extern undefined4 g_speaker_69bb38;
/*0069bb3c*/ extern undefined4 g_speaker_69bb3c;
/*0069bb40*/ extern undefined4 g_hud_69bb40;
/*0076e7f0*/ extern undefined4 g_hud_76e7f0;


/* build_info_textures_destroy */

/* "destroys the speaker / hud textures of the build info screens" */

void build_info_textures_destroy(void)
{
  RwImageSetPath("");
  RwTextureDestroy(g_speaker_69bb34);
  RwTextureDestroy(g_hud_69bb40);
  RwTextureDestroy(g_speaker_69bb38);
  RwTextureDestroy(g_speaker_69bb3c);
  g_hud_76e7f0 = 0;
  return;
}
