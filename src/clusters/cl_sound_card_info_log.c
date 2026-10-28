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
#define SOUND_MIN_VOICES_FOR_3D (0x30) /* voices left after reserving 4; below this (or software mixing) the game mutes effects in multiplayer */
/* sound_card_info_log */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "MAX voices / useSoftware flag" */

void sound_card_info_log(void)

{
  int listener_obj;
  undefined4 init_arg_zero;
  undefined4 init_arg_one;
  struct { /* sound card capabilities record filled by FUN_005ba5c0 (fields at their stack offsets) */
    undefined1 head[8];
    int use_software;       /* +8 */
    undefined1 pad_0c[14];
    ushort max_voices;      /* +0x1a */
    undefined1 pad_1c[4];
  } caps;
  CHAR msg_buf [256];
  
  init_arg_one = 1;
  init_arg_zero = 0;
  /* Open the sound system, query the card capabilities, log MAX voices / useSoftware, and set g_sound_6ae0c0 = max_voices - 4 (4 voices reserved, inferred) */
  g_sound_card_649138 = FUN_005b6800(0x0, 0x1);
  FUN_005b66b0();
  FUN_005ccee0();
  game_register_root_filesystem();
  g_sound_6ae0bc = FUN_005cb1d0(0,0,6,0,0,init_arg_zero,init_arg_one);
  init_arg_zero = FUN_005ba5c0(0,&g_sound_5f835c,0,&caps);
  FUN_005ba030(init_arg_zero,&caps);
  sprintf(msg_buf,"SOUND CARD - MAX voices %d \n",caps.max_voices);
  OutputDebugStringA(msg_buf);
  log_print(msg_buf);
  sprintf(msg_buf,"SOUND CARD - useSoftware flag = %d \n",caps.use_software);
  OutputDebugStringA(msg_buf);
  log_print(msg_buf);
  g_sound_6ae0c0 = caps.max_voices - 4;
  g_sound_mute_when_multiplayer = 0;
  if (((int)g_sound_6ae0c0 < SOUND_MIN_VOICES_FOR_3D) || (caps.use_software != 0)) {
    g_sound_mute_when_multiplayer = 1;
  }
  listener_obj = FUN_005b9500(4,0,0);
  /* listener_obj is created by FUN_005b9500(4,0,0); +0x64/+0x6c/+0x68 get 10.0f, 4.0f and ~1e-4 (0x38d1b717) with flag bits 0x10/0x40/0x20 set at +0x70 (inferred: per-parameter valid flags) */
  *(undefined4 *)(listener_obj + 100) = 0x41200000;
  *(uint *)(listener_obj + 0x70) = *(uint *)(listener_obj + 0x70) | 0x10;
  *(undefined4 *)(listener_obj + 0x6c) = 0x40800000;
  *(uint *)(listener_obj + 0x70) = *(uint *)(listener_obj + 0x70) | 0x40;
  *(undefined4 *)(listener_obj + 0x68) = 0x38d1b717;
  g_sound_6ad1e8 = listener_obj;
  *(uint *)(listener_obj + 0x70) = *(uint *)(listener_obj + 0x70) | 0x20;
  FUN_005cc2e0(g_sound_6ae0bc,listener_obj);
  FUN_005cc300(g_sound_6ae0bc,4);
  return;
}

