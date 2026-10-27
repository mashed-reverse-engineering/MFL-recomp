#include "mfl_tier2.h"
/*004d5e40*/ extern struct RwTexture * RwTextureRead(RwChar *p0, RwChar *p1);
/*004ddb80*/ extern RwChar * RwImageSetPath(RwChar *p0);
/*0069bb34*/ extern undefined4 g_speaker_69bb34;
/*0069bb38*/ extern undefined4 g_speaker_69bb38;
/*0069bb3c*/ extern undefined4 g_speaker_69bb3c;
/*0069bb40*/ extern undefined4 g_hud_69bb40;
/*0076e7f0*/ extern undefined4 g_hud_76e7f0;


/* speaker_mode_names */

/* "DTSScreen MashedFullyLoaded proLogicII DolbyDig" */

void speaker_mode_names(void)
{
  RwImageSetPath("");
  g_speaker_69bb34 = RwTextureRead("DTSScreen",(RwChar *)0x0);
  g_speaker_69bb34 = RwTextureRead("MashedFullyLoaded",(RwChar *)0x0);
  g_hud_69bb40 = RwTextureRead("bigE",(RwChar *)0x0);
  g_speaker_69bb38 = RwTextureRead("proLogicII",(RwChar *)0x0);
  g_speaker_69bb3c = RwTextureRead("DolbyDig",(RwChar *)0x0);
  g_hud_76e7f0 = g_speaker_69bb34;
  return;
}
