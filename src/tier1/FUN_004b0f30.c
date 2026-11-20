#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall LoadLibraryA(int);
/*00771ab4*/ extern undefined4 g_data_771ab4;
/* FUN_004b0f30 */
/* Lazily loads OLEAUT32.DLL the first time it is needed. Does nothing once g_data_771ab4 holds a module handle.
 * Takes no parameters; the handle is stored in g_data_771ab4 (inferred) and returned (the caller at 0x4b16ff keeps it;
 * VOID_w16: was void). */
undefined4 FUN_004b0f30(void)
{
  if (g_data_771ab4 == (HMODULE)0x0) {
    g_data_771ab4 = LoadLibraryA("OleAut32.dll");
  }
  return g_data_771ab4;
}
