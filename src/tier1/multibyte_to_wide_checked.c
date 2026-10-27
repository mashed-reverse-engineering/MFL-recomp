#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall MultiByteToWideChar(int, int, int, int, int, int);
/* cursor_show_if_windowed */
/* "ShowCursor(1) when app_fn_4a8860() != 0" */
/* app_flag_76e7f8_set */
/* "g_app_76e7f8 = 1" */
/* app_flag_76e7fc_set */
/* "g_app_76e7fc = 1" */
/* app_state_set_1 */
/* "g_app_state = 1" */
/* texture_read_log */
/* "Reading texture %s" */
/* texture_read_png_logged */
/* "logs 'Reading texture %s' and RtPNGImageRead" */
/* MultiByteToWideChar cbMultiByte: source string is NUL-terminated, length is computed by the API */
#define MB_SRC_NUL_TERMINATED (-1)
/* multibyte_to_wide_checked */
/* "MultiByteToWideChar wrapper (dst, src, n, codepage), null dst safe" */
/*
 * Converts the NUL-terminated multibyte string src to UTF-16 in dst using code_page.
 * dst_chars is the capacity of dst in wide characters (inferred).
 * Returns dst, or NULL if dst is NULL. dst is set to an empty string before conversion.
 */
LPWSTR __stdcall multibyte_to_wide_checked(LPWSTR dst,LPCSTR src,int dst_chars,UINT code_page)
{
  /* null dst: nothing to write into, return NULL (inferred) */
  if (dst == (LPWSTR)0x0) {
    return (LPWSTR)0x0;
  }
  *dst = L'\0';
  MultiByteToWideChar(code_page,0,src,MB_SRC_NUL_TERMINATED,dst,dst_chars);
  return dst;
}
