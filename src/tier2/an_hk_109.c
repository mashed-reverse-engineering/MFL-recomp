#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CloseHandle(int);
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*007d1f78*/ extern undefined4 g_piz_7d1f78;
/*007d1f80*/ extern undefined1 g_piz_7d1f80;
/* piz_close_file */

/* Closes the file held in g_piz_7d1f78 and releases it. No parameters, no return value.
   Bit 0 of g_piz_7d1f80 says what kind of object g_piz_7d1f78 is (inferred from the calls below). */
void piz_close_file(void)

{
  /* Set: stdio FILE * - fclose it and clear the pointer to NULL. */
  if ((g_piz_7d1f80 & 1) != 0) {
    _fclose(g_piz_7d1f78);
    g_piz_7d1f78 = (FILE *)0x0;
    return;
  }
  /* Clear: Win32 HANDLE - CloseHandle it. The stored value is left as it was. */
  CloseHandle(g_piz_7d1f78);
  return;
}


