#include "mfl_tier2.h"
/*004dbfc0*/ extern struct RwStream * RwStreamWrite(struct RwStream *p0, void *p1, RwUInt32 p2);
/*0089de04*/ extern undefined4 g_app_89de04;


/* material_plugin_stream_write */

/*  */

uint material_plugin_stream_write(RwStream *stream,undefined4 param_2,char *param_3)

{
  RwStream *pRVar1;
  struct blk12 { undefined4 v[12]; };
  undefined4 auStack_30 [12];
  
  *(struct blk12 *)auStack_30 = *(struct blk12 *)(g_app_89de04 + param_3);
  pRVar1 = RwStreamWrite(stream,auStack_30,0x30);
  return pRVar1 != (RwStream *)0x0 ? (uint)stream : 0;
}
