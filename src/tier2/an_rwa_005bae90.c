/* rwa_obj_calc_size_005bae90 (0x005bae90): objects (_rwaObjCalcSize): size of an object of definition DEF: 0x28 byte header, the definition's data aligned to its alignment (stored to *DATAOFFSET), +4 with flag 2, + the parameter tables with flag 1 */
#include "mfl_tier2.h"

/*007dae30*/ extern undefined g_data_7dae30;      /* the object list */
/*0062e594*/ extern char g_data_62e594;           /* default alignment shift + 1 */
/*005be5c0*/ extern int rwa_llist_005be5c0(void *list, void *callback, void *data);      /* nonzero: callback matched an entry */
/*005bad50*/ extern int rwa_obj_005bad50(void);

#define OBJDEF_DATA_SIZE(d) (*(RwUInt32 *)((d) + 0x40))      /* low 28 bits: data size, top 4 bits: log2 of the data alignment */
#define OBJDEF_PARAMS(d) (*(char **)((d) + 0x4c))

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
RwUInt32 rwa_obj_calc_size_005bae90(char *def, RwUInt32 flags, RwUInt32 *dataOffset)
{
  RwUInt32 size = 0x28, align, off;
  RwUInt32 countA, countB;

  if (!dataOffset)
    dataOffset = (RwUInt32 *)&dataOffset;      /* the original writes into its own argument slot */
  if (OBJDEF_DATA_SIZE(def) & 0x0fffffff) {
    if (!rwa_llist_005be5c0(&g_data_7dae30, rwa_obj_005bad50, def) && (1 << (OBJDEF_DATA_SIZE(def) >> 28)) == 1) {
      OBJDEF_DATA_SIZE(def) &= 0x0fffffff;      /* no alignment set and no object of it exists: default alignment */
      OBJDEF_DATA_SIZE(def) |= (RwUInt32)(g_data_62e594 - 1) << 28;
    }
    align = 1 << (OBJDEF_DATA_SIZE(def) >> 28);
    off = (align + 0x27) & ~(align - 1);
    *dataOffset = off;
    size = (OBJDEF_DATA_SIZE(def) & 0x0fffffff) + off;
  } else
    *dataOffset = 0;
  if ((unsigned char)flags & 2)
    size += 4;
  if ((unsigned char)flags & 1) {
    countA = *(unsigned short *)(OBJDEF_PARAMS(def) + 0xc);      /* the two parameter table counts (see 5bd700) */
    countB = *(unsigned short *)(OBJDEF_PARAMS(def) + 0xe);
    size = size + countA * 12 + countB * 8 + 8;      /* 12 / 8 bytes of object data per entry */
  }
  return size;
}
