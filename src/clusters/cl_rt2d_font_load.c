// cluster-entries: rt2d_font_load
/* Installed cluster rt2d_font_load (tools/cluster_install.py from output/cluster_pilot11/rt2d_font_load.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*004dc2a0*/ extern RwBool RwStreamClose(struct RwStream *p0, void *p1);
/*00564e40*/ extern int Rt2dFontStreamRead(struct RwStream *p0);
/* rt2d_font_load */

/* "opens a font file and reads chunk 0x199 with Rt2dFontStreamRead" */

int rt2d_font_load_n(void *in_eax)

{
  RwStream *stream;
  RwBool RVar1;
  int uVar2;
  
  uVar2 = 0;
  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,in_eax);
  if (stream != (RwStream *)0x0) {
    RVar1 = RwStreamFindChunk(stream,0x199,(RwUInt32 *)0x0,(RwUInt32 *)0x0);
    if (RVar1 != 0) {
      uVar2 = Rt2dFontStreamRead(stream);
      RwStreamClose(stream,(void *)0x0);
    }
  }
  return uVar2;
}

/* entry thunk at the original address of rt2d_font_load: register inputs (eax) -> natural C rt2d_font_load_n; ret 0 */
__declspec(naked) void rt2d_font_load(void)
{
    __asm {
        push eax
        call rt2d_font_load_n
        add esp, 4
        ret
    }
}
