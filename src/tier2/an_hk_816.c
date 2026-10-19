#include "mfl_tier2.h"
/*004a5550*/ extern int log_printf();
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*0053d050*/ extern undefined __cdecl RtBMPImageRead(undefined4 p0);
/*005e0bd0*/ extern char s_Reading_texture__s_005e0bd0[];
/* texture_read_log: logs "Reading texture %s" for texture_name, clears the current
   RwTexDictionary (RwTexDictionarySetCurrent(NULL)), then reads the BMP image
   texture_name with RtBMPImageRead. texture_name is the image file name (inferred).
   read_option is always 0 (inferred). log_format_str is the "Reading texture %s\n"
   literal, passed as RtBMPImageRead's third argument (inferred, as in the original).
   Returns nothing. */

/* "Reading texture %s" */

void texture_read_log(undefined4 texture_name)
{
  log_printf(((int)&s_Reading_texture__s_005e0bd0), texture_name);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  RtBMPImageRead(texture_name);
  return;
}


