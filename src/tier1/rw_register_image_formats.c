#include "mfl_tier2.h"
extern char PTR_g_data_706d62_005e0be8[];
extern char LAB_0053da80[];
extern char PTR_g_course_lap_data_field6152_0x1808_005e0be4[];
/*004a29d0*/ extern int texture_read_log();
/*004a2a00*/ extern int texture_read_png_logged();
/*004de040*/ extern RwBool __cdecl RwImageRegisterImageFormat(RwChar *p0, RwImageCallBackRead p1, RwImageCallBackWrite p2);
/*005244e0*/ extern int RtPNGImageWrite();
/* rw_register_image_formats */
/* "registers the log and the png image formats" */
/* Registers two RenderWare image formats: the log format (texture_read_log, write callback LAB_0053da80)
   and the png format (texture_read_png_logged, RtPNGImageWrite). Takes no parameters.
   Returns true only if both registrations succeed; stops after the first failure (inferred). */
int rw_register_image_formats(void)
{
  RwBool registered;

  /* log format: first registration; on failure return false without trying png (inferred) */
  registered = RwImageRegisterImageFormat
                    ((RwChar *)&PTR_g_data_706d62_005e0be8,texture_read_log,
                     (RwImageCallBackWrite)&LAB_0053da80);
  if (registered == 0) {
    return registered;
  }
  /* png format: second registration; its result is the return value */
  registered = RwImageRegisterImageFormat
                    ((RwChar *)&PTR_g_course_lap_data_field6152_0x1808_005e0be4,
                     texture_read_png_logged,RtPNGImageWrite);
  return registered != 0;
}
