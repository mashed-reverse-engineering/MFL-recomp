#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a5a78*/ extern undefined4 g_weapon_drum_6a5a78;
/*006a5c80*/ extern undefined4 g_weapon_drum_6a5c80;
/*006a5d10*/ extern undefined4 g_weapon_6a5d10;
/*006a5d14*/ extern undefined4 g_weapon_drum_6a5d14;
/*006a5c98*/ extern char g_weapon_drum_6a5c98[];
/*006a5d30*/ extern char g_weapon_6a5d30[];


/* Frees the weapon drum assets: the clump table at 0x6a5c80 (11-word records),
   the drum record table at 0x6a5a78 (17-word records; each holds an atomic and
   two frames), then the two remaining clumps. */
void weapon_drum_free_assets(void)
{
  undefined4 *entry;

  entry = &g_weapon_drum_6a5c80;
  do {
    RpClumpDestroy((RpClump *)*entry);
    entry = entry + 0xb;
  } while ((int)entry < ((int)&g_weapon_6a5d30));
  /* entry[-2] = atomic, entry[-1] and entry[0] = frames (field names unknown) */
  entry = &g_weapon_drum_6a5a78;
  do {
    RwFrameRemoveChild((RwFrame *)*entry);
    RwFrameDestroy((RwFrame *)entry[-1]);
    RwFrameDestroy((RwFrame *)*entry);
    RpAtomicDestroy((RpAtomic *)entry[-2]);
    entry = entry + 0x11;
  } while ((int)entry < ((int)&g_weapon_drum_6a5c98));
  RpClumpDestroy(g_weapon_drum_6a5d14);
  RpClumpDestroy(g_weapon_6a5d10);
  return;
}
