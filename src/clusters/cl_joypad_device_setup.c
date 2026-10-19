// cluster-entries: joypad_device_setup
/* Installed cluster joypad_device_setup (tools/cluster_install.py from output/cluster_pilot_h3/joypad_device_setup.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c6600*/ extern void FUN_004c6600(void *p0, int p1);      /* bzero-like: (buffer, byte count) */
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005e12e0*/ extern float k_f_0_13333334;
/*00770f60*/ extern float g_app_770f60;
extern double fabs(double);
extern double sqrt(double);
/* joypad_device_setup */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "DirectInput joypad: creates the device, sets data format, axis ranges and dead zones (1.5 KB)"
    */

typedef struct { float x, y; } V2;
typedef int (__stdcall *DiDeviceCall1)(int *self);
typedef int (__stdcall *DiDeviceGetState)(int *self, int size, void *state);

/* DirectInput DIJOYSTATE (0x50 bytes) as the device returns it */
struct JoyState {
  int lX;
  int lY;
  int lZ;
  int lRx;
  int lRy;
  int lRz;
  int slider[2];
  uint pov[4];
  char buttons[32];
};

void joypad_device_setup_n(undefined4 *in_esi)

{
  struct JoyState js;
  V2 v;
  float *pf;
  float *px;
  float *py;
  float fVar3;
  float fVar4;
  float fVar5;
  int *dev;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *puVar11;
  float *pfVar10;
  int i;
  undefined4 uVar9;

  pf = (float *)in_esi;
  dev = (int *)*in_esi;
  iVar6 = ((DiDeviceCall1)*(int *)(*dev + 100))(dev);      /* Poll */
  if (iVar6 < 0) {
    iVar6 = ((DiDeviceCall1)*(int *)(*dev + 0x1c))(dev);      /* Acquire */
    while (iVar6 == -0x7ff8ffe2) {
      iVar6 = ((DiDeviceCall1)*(int *)(*dev + 0x1c))(dev);
    }
  }
  iVar6 = ((DiDeviceGetState)*(int *)(*dev + 0x24))(dev, 0x50, &js);      /* GetDeviceState */
  if (iVar6 < 0) {
    return;
  }
  puVar7 = in_esi + 0xd0;
  puVar11 = in_esi + 0x111;
  iVar6 = 0xf;
  do {
    *puVar11 = puVar11[-1];
    puVar11[-0x10] = puVar11[-0x11];
    *puVar7 = puVar7[-2];
    puVar7[1] = puVar7[-1];
    puVar7[0x20] = puVar7[0x1e];
    puVar7[0x21] = puVar7[0x1f];
    puVar11 = puVar11 + -1;
    puVar7 = puVar7 + -2;
    iVar6 = iVar6 + -1;
  } while (iVar6 != 0);
  in_esi[0xf2] = 0;
  in_esi[0xb3] = 0;
  in_esi[0xb2] = 0;
  in_esi[0xd3] = 0;
  in_esi[0xd2] = 0;
  FUN_004c6600(in_esi + 0x102,0x20);
  for (i = 0; i < 16; i++) {
    if (js.buttons[i] < 0) {
      ((byte *)in_esi)[0x408 + (i >> 3)] |= (byte)(1 << (i & 7));
    }
  }
  pf[0xb2] = (float)(js.lX >> 12) * k_f_0_13333334 - float_1_0;
  pf[0xb3] = float_1_0 - (float)(js.lY >> 12) * k_f_0_13333334;
  if ((short)js.pov[0] != -1) {
    switch (js.pov[0]) {
    case 0:
      in_esi[0xb2] = 0;
      in_esi[0xb3] = 0x3f800000;
      break;
    case 4500:
      in_esi[0xb2] = 0x3f800000;
      in_esi[0xb3] = 0x3f800000;
      break;
    case 9000:
      in_esi[0xb2] = 0x3f800000;
      in_esi[0xb3] = 0;
      break;
    case 13500:
      in_esi[0xb2] = 0x3f800000;
      in_esi[0xb3] = 0xbf800000;
      break;
    case 18000:
      in_esi[0xb2] = 0;
      in_esi[0xb3] = 0xbf800000;
      break;
    case 22500:
      in_esi[0xb2] = 0xbf800000;
      in_esi[0xb3] = 0xbf800000;
      break;
    case 27000:
      in_esi[0xb2] = 0xbf800000;
      in_esi[0xb3] = 0;
      break;
    case 31500:
      in_esi[0xb2] = 0xbf800000;
      in_esi[0xb3] = 0x3f800000;
      break;
    }
  }
  fVar5 = g_app_770f60;
  fVar3 = (fabs(pf[0xb2]) < fVar5) ? k_f_0 : pf[0xb2];
  pf[0xb2] = fVar3;
  fVar4 = (fabs(pf[0xb3]) < fVar5) ? k_f_0 : pf[0xb3];
  pf[0xb3] = fVar4;
  v = *(V2 *)&pf[0xb2];
  fVar5 = v.x * v.x + v.y * v.y;
  if (fVar5 > float_1_0) {
    fVar5 = float_1_0 / sqrt(fVar5);
    pf[0xb2] = fVar3 * fVar5;
    pf[0xb3] = fVar4 * fVar5;
  }
  iVar6 = 7;
  pfVar10 = (float *)(in_esi + 0xa0);
  do {
    *pfVar10 = pfVar10[-2];
    pfVar10[1] = pfVar10[-1];
    pfVar10[0x10] = pfVar10[0xe];
    pfVar10[0x11] = pfVar10[0xf];
    pfVar10 = pfVar10 + -2;
    iVar6 = iVar6 + -1;
  } while (iVar6 != 0);
  uVar9 = in_esi[0xb2];
  in_esi[0x93] = in_esi[0xb3];
  in_esi[0x92] = uVar9;
  in_esi[0xb2] = 0;
  in_esi[0xb3] = 0;
  in_esi[0xb2] = uVar9;
  in_esi[0xb3] = in_esi[0x93];
  px = &pf[0xb2];
  py = &pf[0xb3];
  px[0] = pf[0x94] + px[0];
  py[0] = pf[0x95] + py[0];
  px[0] = pf[0x96] + px[0];
  py[0] = pf[0x97] + py[0];
  px[0] = pf[0x98] + px[0];
  py[0] = pf[0x99] + py[0];
  px[0] = pf[0x9a] + px[0];
  py[0] = pf[0x9b] + py[0];
  px[0] = pf[0x9c] + px[0];
  py[0] = pf[0x9d] + py[0];
  px[0] = pf[0x9e] + px[0];
  py[0] = pf[0x9f] + py[0];
  px[0] = px[0] + pf[0xa0];
  py[0] = pf[0xa1] + py[0];
  pf[0xb2] = pf[0xb2] * k_f_0_125;
  pf[0xb3] = pf[0xb3] * k_f_0_125;
  return;
}

/* entry thunk at the original address of joypad_device_setup: register inputs (esi) -> natural C joypad_device_setup_n; ret 0 */
__declspec(naked) void joypad_device_setup(void)
{
    __asm {
        push esi
        call joypad_device_setup_n
        add esp, 4
        ret
    }
}
