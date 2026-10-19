#include "mfl_tier2.h"
/*004a5550*/ extern int log_printf();
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*00524880*/ extern undefined __cdecl RtPNGImageRead(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005e0bd0*/ extern char s_Reading_texture__s_005e0bd0[];
/* texture_read_png_logged */

/* "logs 'Reading texture %s' and RtPNGImageRead" */

/*
 * Logs "Reading texture <texture_name>" and then reads the PNG texture with RtPNGImageRead.
 * texture_name: name of the texture, used as the %s argument of the log line and as the
 * first argument of RtPNGImageRead. Returns nothing.
 * The current texture dictionary is cleared (set to NULL) before the read.
 */

void texture_read_png_logged(undefined4 texture_name)

{
  undefined4 read_flags;
  char *format_string;
  undefined4 texture_name_copy;

  format_string = "Reading texture %s\n";
  texture_name_copy = texture_name;
  log_printf(((int)&s_Reading_texture__s_005e0bd0), texture_name);
  read_flags = 0;
  /* no current texture dictionary during the read (inferred) */
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  RtPNGImageRead(texture_name,read_flags,(undefined4)format_string,texture_name_copy);
  return;
}


