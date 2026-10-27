// cluster-entries: sound_card_info_log
/* Installed cluster sound_card_info_log (tools/cluster_install.py from output/cluster_pilot_kc/sound_card_info_log.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall OutputDebugStringA(int);
/*0046ec40*/ extern int game_register_root_filesystem();
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004b2a60*/ extern int sprintf();
/*005b66b0*/ extern void FUN_005b66b0(void);
/*005b6800*/ extern int FUN_005b6800();
/*005b9500*/ extern int FUN_005b9500(undefined4 p0, undefined4 p1, undefined4 p2);
/*005ba030*/ extern void FUN_005ba030(undefined4 p0, void *p1); /* reads a second stack word: the caller leaves &local_12c there from the previous call */
/*005ba5c0*/ extern int FUN_005ba5c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005cb1d0*/ extern int FUN_005cb1d0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6);
/*005cc2e0*/ extern void FUN_005cc2e0(undefined4 p0, undefined4 p1);
/*005cc300*/ extern void FUN_005cc300(undefined4 p0, undefined4 p1);
/*005ccee0*/ extern void FUN_005ccee0(void);
/*005f835c*/ extern undefined g_sound_5f835c;
/*00649138*/ extern undefined4 g_sound_card_649138;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*006ad1e8*/ extern undefined4 g_sound_6ad1e8;
/*006ae0bc*/ extern undefined4 g_sound_6ae0bc;
/*006ae0c0*/ extern undefined4 g_sound_6ae0c0;
/* sound_card_info_log */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "MAX voices / useSoftware flag" */

void sound_card_info_log(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  struct { /* sound card capabilities record filled by FUN_005ba5c0 (fields at their stack offsets) */
    undefined1 head[8];
    int use_software;       /* +8 */
    undefined1 pad_0c[14];
    ushort max_voices;      /* +0x1a */
    undefined1 pad_1c[4];
  } caps;
  CHAR local_104 [256];
  
  uVar3 = 1;
  uVar2 = 0;
  g_sound_card_649138 = FUN_005b6800(0x0, 0x1);
  FUN_005b66b0();
  FUN_005ccee0();
  game_register_root_filesystem();
  g_sound_6ae0bc = FUN_005cb1d0(0,0,6,0,0,uVar2,uVar3);
  uVar2 = FUN_005ba5c0(0,&g_sound_5f835c,0,&caps);
  FUN_005ba030(uVar2,&caps);
  sprintf(local_104,"SOUND CARD - MAX voices %d \n",caps.max_voices);
  OutputDebugStringA(local_104);
  log_print(local_104);
  sprintf(local_104,"SOUND CARD - useSoftware flag = %d \n",caps.use_software);
  OutputDebugStringA(local_104);
  log_print(local_104);
  g_sound_6ae0c0 = caps.max_voices - 4;
  g_sound_mute_when_multiplayer = 0;
  if (((int)g_sound_6ae0c0 < 0x30) || (caps.use_software != 0)) {
    g_sound_mute_when_multiplayer = 1;
  }
  iVar1 = FUN_005b9500(4,0,0);
  *(undefined4 *)(iVar1 + 100) = 0x41200000;
  *(uint *)(iVar1 + 0x70) = *(uint *)(iVar1 + 0x70) | 0x10;
  *(undefined4 *)(iVar1 + 0x6c) = 0x40800000;
  *(uint *)(iVar1 + 0x70) = *(uint *)(iVar1 + 0x70) | 0x40;
  *(undefined4 *)(iVar1 + 0x68) = 0x38d1b717;
  g_sound_6ad1e8 = iVar1;
  *(uint *)(iVar1 + 0x70) = *(uint *)(iVar1 + 0x70) | 0x20;
  FUN_005cc2e0(g_sound_6ae0bc,iVar1);
  FUN_005cc300(g_sound_6ae0bc,4);
  return;
}

