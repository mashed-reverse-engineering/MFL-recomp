/* RenderWare Audio (Criterion Software) DirectSound rows, MFL.exe 0x005cd320 (_rwaDSBufferDuplicate), 0x005cc330 (the output's
 * speaker mode) and 0x005d0ef0 (_rwaDSOutputPinCompleteRequest).  Written from the MFL.exe disassembly (objdump).  COM methods
 * are stdcall: HS env STDCALL_IND (s_rwa_au_w8.py). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

/*005cd290*/ extern RwBool _rwaDSBufferGetInterfaces(void **set, RwBool want_3d, RwBool want_props);

typedef long (__stdcall *ComMethod1)(void *self);
typedef long (__stdcall *ComMethod2)(void *self, void *arg);
typedef long (__stdcall *ComMethod3)(void *self, void *arg, void *out);
#define VTBL(obj) (*(void ***)(obj))

/* duplicates src's DirectSound buffer through ds (IDirectSound::DuplicateSoundBuffer) into the interface set dst {buffer, 3D
   buffer, property set} and fetches the same interfaces src has.  1: ok */
/* TIER2: unvalidated 2026-11-14 authority=static */
RwBool _rwaDSBufferDuplicate(void *ds, void **src, void **dst)
{
  dst[0] = 0;
  dst[1] = 0;
  dst[2] = 0;
  if (((ComMethod3)VTBL(ds)[5])(ds, src[0], dst) < 0)
    return 0;
  if (!_rwaDSBufferGetInterfaces(dst, src[1] != 0, src[2] != 0))
    return 0;
  return 1;
}

/* the output's speaker mode from IDirectSound::GetSpeakerConfig (low byte): 1 mono -> 3, 2 headphone -> 2, 5 quad -> 4,
   6 surround -> 5, 7 5.1 -> 6, else 1.  Like the original, the configuration word is stored in the argument's own slot. */
/* TIER2: unvalidated 2026-11-14 authority=static */
RwUInt32 rwa_dxaudio_output_speaker_mode_005cc330(char *output)
{
  void *ds = *(void **)(output + 0x7c);

  RwUInt32 config;

  ((ComMethod2)VTBL(ds)[8])(ds, &output);
  config = *(RwUInt32 *)&output & 0xff;      /* the word GetSpeakerConfig stored in the argument's slot.  (An if chain below:
                                                diffemu cannot follow a switch jump table of our object.) */
  if (config == 1)
    return 3;
  if (config == 2)
    return 2;
  if (config == 5)
    return 4;
  if (config == 6)
    return 5;
  if (config == 7)
    return 6;
  return 1;
}

/* a pin request is finished: signals the pin's semaphore (+0x150) and drops one reference (IUnknown::Release) */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
long _rwaDSOutputPinCompleteRequest(char *pin)
{
  ReleaseSemaphore(*(void **)(pin + 0x150), 1, 0);
  return ((ComMethod1)VTBL(pin)[2])(pin);
}
