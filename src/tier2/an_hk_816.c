#include "mfl_tier2.h"
/*004a5550*/ extern int log_printf();
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*0053d050*/ extern undefined __cdecl RtBMPImageRead(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
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
  undefined4 read_option;
  char *log_format_str;
  undefined4 texture_name_copy;

  log_format_str = "Reading texture %s\n";
  texture_name_copy = texture_name;
  /* log the name first, then clear the current dictionary before loading */
  log_printf(((int)&s_Reading_texture__s_005e0bd0), texture_name);
  read_option = 0;
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  RtBMPImageRead(texture_name,read_option,(undefined4)log_format_str,texture_name_copy);
  return;
}


