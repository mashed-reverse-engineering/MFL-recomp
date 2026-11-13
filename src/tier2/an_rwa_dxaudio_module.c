/* RenderWare Audio (Criterion Software) DirectSound output module open / close, MFL.exe 0x005cbac0 and 0x005cbb00.  Written from
 * the MFL.exe disassembly (objdump).  COM and DirectSound are set up with the first output object and shut down with the last;
 * The result of RwaObjAddObjToExecutionList (0x5bad70) for the object's definition decides: 0 = the first instance (open),
 * <= 1 = the last one (close). */
#include "mfl_tier2.h"

/*005bad70*/ extern RwUInt32 RwaObjAddObjToExecutionList(void *def);
/*005cdc20*/ extern RwBool _rwaDSOpen(void);
/*005cdc40*/ extern void _rwaDSClose(void);
/*005dd2f0*/ extern __declspec(dllimport) int __stdcall CoInitialize(int);
/*005dd2e4*/ extern __declspec(dllimport) void __stdcall CoUninitialize(void);

/* module open (obj: the output object, its first word = definition).  1: ok; 0: DirectSound could not be opened. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwBool rwa_dxaudio_module_open_005cbac0(void **obj)
{
  if (RwaObjAddObjToExecutionList(*obj))
    return 1;
  CoInitialize(0);
  if (_rwaDSOpen())
    return 1;
  CoUninitialize();
  return 0;
}

/* module close: the last instance closes DirectSound and COM. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void rwa_dxaudio_module_close_005cbb00(void **obj)
{
  if (RwaObjAddObjToExecutionList(*obj) <= 1) {
    _rwaDSClose();
    CoUninitialize();
  }
}
