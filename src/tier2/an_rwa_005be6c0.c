/* rwa_uuid_serialize_005be6c0 (0x005be6c0): UUIDs (_rwaUUIDSerialize): copy a 16-byte UUID, byte-swapping its first three fields when asked */
#include "mfl_tier2.h"

/*005bf3e0*/ extern void FUN_005bf3e0(void *cursor, const void *src, RwUInt32 bytes);      /* copy to *cursor byte-swapped, advance *cursor */

typedef struct RwaUUID { RwUInt32 data1; unsigned short data2, data3; RwUInt32 data4[2]; } RwaUUID;

/* swap != 0: data1 / data2 / data3 byte-swapped (endian change), data4 as is.  Returns uuid. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
const RwaUUID *rwa_uuid_serialize_005be6c0(const RwaUUID *uuid, RwaUUID *out, RwUInt32 swap)
{
  RwaUUID tmp;

  if (swap) {
#define cursor (*(RwUInt32 **)&swap)      /* the original keeps the write cursor in the (then dead) swap argument */
    cursor = (RwUInt32 *)&tmp;
    FUN_005bf3e0(&cursor, &uuid->data1, 4);
    FUN_005bf3e0(&cursor, &uuid->data2, 2);
    FUN_005bf3e0(&cursor, &uuid->data3, 2);
    cursor[0] = uuid->data4[0];
    cursor[1] = uuid->data4[1];
#undef cursor
    *out = tmp;
  } else
    *out = *uuid;
  return uuid;
}
