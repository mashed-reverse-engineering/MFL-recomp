#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall LoadLibraryA(int);
/*00771ab4*/ extern undefined4 g_data_771ab4;
/* FUN_004b0f30 */

/* Lazily loads OLEAUT32.DLL the first time it is needed. Does nothing once g_data_771ab4 holds a module handle.
 * Takes no parameters and returns nothing; the handle is stored in g_data_771ab4 (inferred). */
void FUN_004b0f30(void)

{
  if (g_data_771ab4 == (HMODULE)0x0) {
    g_data_771ab4 = LoadLibraryA("OleAut32.dll");
  }
  return;
}


