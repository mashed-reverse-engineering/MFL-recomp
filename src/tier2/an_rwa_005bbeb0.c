/* rwa_rw_stream_open_005bbeb0 (0x005bbeb0): RwStream glue (_rwaRwStreamOpen): an RwStream over RWA's async file reader */
#include "mfl_tier2.h"

/*005bf6f0*/ extern void *FUN_005bf6f0(const char *name, const char *mode, void *callback, void *data);      /* async file open */
/*005bbe60*/ extern void gap_5bbe60(void);      /* the read completion callback */
/*005e0450*/ extern char g_piz_5e0450[];        /* "rb" */

typedef struct RwaRwStreamExtra {
  void *stream;            /* +0x00 the RwStream */
  RwUInt32 w4, w8;
  RwUInt32 open;           /* +0x0c */
  RwUInt32 w10;
  char pad14[0x24 - 0x14];
  RwUInt32 w24;
  char pad28[0x38 - 0x28];
  void *callback;          /* +0x38 */
  void *callbackData;      /* +0x3c */
  RwUInt32 w40;            /* +0x40 */
  void *user;              /* +0x44 */
  void *user2;             /* +0x48 */
} RwaRwStreamExtra;

#define rwSTREAMFILENAME 2

/* stream: the RwStream (+0 type, +4 access, +8 position); only file name streams are opened (type 2).  Returns stream. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 *rwa_rw_stream_open_005bbeb0(RwUInt32 *stream, RwaRwStreamExtra *extra, RwUInt32 type, RwUInt32 access,
                                         const char *name, void *user, void *user2)
{
  stream[0] = type;
  stream[2] = 0;
  stream[1] = access;
  if (type == rwSTREAMFILENAME) {
    extra->w8 = 0;
    extra->w4 = 0;
    extra->w10 = 0;
    extra->w24 = 0;
    extra->open = 1;
    extra->w40 = 1;
    extra->user = user;
    extra->callback = (void *)gap_5bbe60;
    extra->callbackData = extra;
    extra->stream = stream;
    extra->user2 = user2;
    FUN_005bf6f0(name, g_piz_5e0450, (void *)gap_5bbe60, extra);
  }
  return stream;
}
