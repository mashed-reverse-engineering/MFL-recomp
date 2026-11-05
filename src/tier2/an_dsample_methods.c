/* Small COM methods in the D3DX code range (0x5d12a0, 0x5d2990, 0x5d2ad0-0x5d2b00): a not-implemented stub and IMediaSample methods
   (GetSize, GetActualDataLength, SetActualDataLength; GetMediaType 0x5d2b00 stays original: it calls the trapped CreateMediaType 0x5cdc50) of the RenderWare Audio DirectShow media sample (DirectShow base
   class CMediaSample layout).  __stdcall, HRESULT results; called only through the vtables. */
#include "mfl_tier2.h"
typedef long HRESULT;

#define E_NOTIMPL_ 0x80004001
#define VFW_E_BUFFER_OVERFLOW_ 0x8004020d

typedef struct MflMediaSample {
  void *vtbl;
  unsigned char pad04[0x1c];
  unsigned int flags;   /* 0x20 */
  int actual;   /* 0x24: valid data length */
  unsigned char pad28[0x14];
  void *media_type;   /* 0x3c */
  unsigned char *buffer;   /* 0x40 */
  int size;   /* 0x44: buffer size */
} MflMediaSample;


/* a vtable slot that is not implemented */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
HRESULT __stdcall dsample_not_implemented(void *self, void *arg)
{
  return E_NOTIMPL_;
}

/* IMediaSample::GetSize */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int __stdcall dsample_get_size(MflMediaSample *s)
{
  return s->size;
}

/* IMediaSample::GetActualDataLength */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int __stdcall dsample_get_actual_length(MflMediaSample *s)
{
  return s->actual;
}

/* IMediaSample::SetActualDataLength: at most the buffer size */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
HRESULT __stdcall dsample_set_actual_length(MflMediaSample *s, int len)
{
  if (len > s->size)
    return VFW_E_BUFFER_OVERFLOW_;
  s->actual = len;
  return 0;
}
