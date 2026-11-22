/* padsim_test.exe: self-check of padsim8.dll (run it under Wine in a directory that holds padsim8.dll; it writes its own padsim.cfg).
 * Result: padsim_test.txt (one line per check, last line PASS or FAIL) and the exit code.  Build: make test. */
#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800
#define COBJMACROS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <dinput.h>

static FILE *out; static int bad, nfound, nobj; static GUID found[8]; static char names[8][260];
#define CHECK(c) do { int ok_ = (c); fprintf(out, "%s %s\n", ok_ ? "ok  " : "FAIL", #c); fflush(out); bad += !ok_; } while (0)
static BOOL CALLBACK on_dev(const DIDEVICEINSTANCEA *d, void *ref) { if (nfound < 8) { found[nfound] = d->guidInstance; strcpy(names[nfound++], d->tszProductName); } return DIENUM_CONTINUE; }
static BOOL CALLBACK on_obj(const DIDEVICEOBJECTINSTANCEA *o, void *ref) { nobj++; return DIENUM_CONTINUE; }
static void put(const char *file, const char *mode, const char *text) { FILE *f = fopen(file, mode); fputs(text, f); fclose(f); }

int main(void) {
    IDirectInput8A *di; IDirectInputDevice8A *pad, *wheel; IDirectInputEffect *eff; DIJOYSTATE js; DIDEVCAPS caps = { sizeof caps }; DIDEVICEINSTANCEA info = { sizeof info };
    DICONSTANTFORCE cf = { 5000 }; DWORD axes[1] = { DIJOFS_X }; LONG dir[1] = { 0 }; DIEFFECT e; BYTE keys[256];
    out = fopen("padsim_test.txt", "w");
    put("padsim.cfg", "w", "dev=xbox\ndev=wheel\ndev=keyboard\nbutton 0 2 1\naxis 0 x -1000\naxis 0 y 500\npov 0 27000\nbutton 2 30 1\nat 3 plug 1 0\nat 5 plug 1 1\n");
    CHECK(DirectInput8Create(GetModuleHandleA(NULL), DIRECTINPUT_VERSION, &IID_IDirectInput8A, (void **)&di, NULL) == DI_OK);
    CHECK(IDirectInput8_EnumDevices(di, DI8DEVCLASS_GAMECTRL, on_dev, NULL, DIEDFL_ATTACHEDONLY) == DI_OK);
    CHECK(nfound >= 2 && !strcmp(names[0], "Controller (XBOX 360 For Windows)") && !strcmp(names[1], "Logitech Driving Force USB"));      /* no keyboard in the game controller class */
    CHECK(IDirectInput8_CreateDevice(di, &found[0], &pad, NULL) == DI_OK);
    CHECK(IDirectInput8_CreateDevice(di, &found[1], &wheel, NULL) == DI_OK);
    CHECK(IDirectInputDevice8_GetDeviceInfo(pad, &info) == DI_OK && (info.dwDevType & 0xff) == DI8DEVTYPE_GAMEPAD);
    CHECK(IDirectInputDevice8_GetCapabilities(pad, &caps) == DI_OK && caps.dwAxes == 5 && caps.dwButtons == 10 && caps.dwPOVs == 1 && (caps.dwFlags & DIDC_FORCEFEEDBACK));
    CHECK(IDirectInputDevice8_EnumObjects(pad, on_obj, NULL, DIDFT_ALL) == DI_OK && nobj == 16);
    CHECK(IDirectInputDevice8_SetDataFormat(pad, &c_dfDIJoystick) == DI_OK && IDirectInputDevice8_SetDataFormat(wheel, &c_dfDIJoystick) == DI_OK);
    CHECK(IDirectInputDevice8_Poll(pad) == DIERR_NOTACQUIRED);
    CHECK(IDirectInputDevice8_Acquire(pad) == DI_OK && IDirectInputDevice8_Acquire(wheel) == DI_OK);
    CHECK(IDirectInputDevice8_GetDeviceState(pad, sizeof js, &js) == DI_OK);      /* tick 1 */
    CHECK(js.lX == 0 && js.lY == 49151 && js.lZ == 32767 && js.rgdwPOV[0] == 27000 && js.rgdwPOV[1] == (DWORD)-1 && js.rgbButtons[2] == 0x80 && !js.rgbButtons[0]);
    /* live commands: a press of 2 ticks */
    put("padsim.live", "a", "press 0 5 2\naxis 0 x 1000\n");
    CHECK(IDirectInputDevice8_GetDeviceState(pad, sizeof js, &js) == DI_OK && js.rgbButtons[5] == 0x80 && js.lX == 65535);      /* tick 2 */
    CHECK(IDirectInputDevice8_GetDeviceState(pad, sizeof js, &js) == DI_OK && js.rgbButtons[5] == 0x80);      /* tick 3: the wheel is unplugged */
    CHECK(IDirectInputDevice8_Poll(wheel) == DIERR_INPUTLOST && IDirectInputDevice8_Acquire(wheel) == DIERR_UNPLUGGED && IDirectInputDevice8_GetDeviceState(wheel, sizeof js, &js) == DIERR_NOTACQUIRED);
    CHECK(IDirectInputDevice8_GetDeviceState(pad, sizeof js, &js) == DI_OK && !js.rgbButtons[5]);      /* tick 4: press over */
    nfound = 0; names[1][0] = 0; IDirectInput8_EnumDevices(di, DI8DEVCLASS_GAMECTRL, on_dev, NULL, DIEDFL_ATTACHEDONLY);
    CHECK(nfound >= 1 && strcmp(names[1], "Logitech Driving Force USB"));      /* hot-unplug: gone from the enumeration */
    IDirectInputDevice8_GetDeviceState(pad, sizeof js, &js);      /* tick 5: plugged again */
    CHECK(IDirectInputDevice8_Acquire(wheel) == DI_OK && IDirectInputDevice8_GetDeviceState(wheel, sizeof js, &js) == DI_OK && js.lX == 32767);
    /* force feedback: constant force on the wheel, none on a device without it */
    memset(&e, 0, sizeof e); e.dwSize = sizeof e; e.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS; e.dwDuration = 500000; e.dwGain = 10000; e.dwTriggerButton = DIEB_NOTRIGGER;
    e.cAxes = 1; e.rgdwAxes = axes; e.rglDirection = dir; e.cbTypeSpecificParams = sizeof cf; e.lpvTypeSpecificParams = &cf;
    CHECK(IDirectInputDevice8_CreateEffect(wheel, &GUID_ConstantForce, &e, &eff, NULL) == DI_OK);
    CHECK(IDirectInputEffect_Start(eff, 1, 0) == DI_OK && IDirectInputEffect_Stop(eff) == DI_OK && IDirectInputEffect_Release(eff) == 0);
    /* the simulated second keyboard and the real system keyboard */
    nfound = 0; IDirectInput8_EnumDevices(di, DI8DEVCLASS_KEYBOARD, on_dev, NULL, DIEDFL_ATTACHEDONLY);
    CHECK(nfound >= 2 && !strcmp(names[0], "Keyboard 2"));      /* the real keyboard follows */
    IDirectInputDevice8_Release(wheel);
    CHECK(IDirectInput8_CreateDevice(di, &found[0], &wheel, NULL) == DI_OK && IDirectInputDevice8_Acquire(wheel) == DI_OK && IDirectInputDevice8_GetDeviceState(wheel, 256, keys) == DI_OK && keys[30] == 0x80);
    IDirectInputDevice8_Release(wheel);
    CHECK(IDirectInput8_CreateDevice(di, &GUID_SysKeyboard, &wheel, NULL) == DI_OK && IDirectInputDevice8_SetDataFormat(wheel, &c_dfDIKeyboard) == DI_OK);      /* passed through */
    IDirectInputDevice8_Release(wheel); IDirectInputDevice8_Release(pad);
    CHECK(IDirectInput8_Release(di) == 0);
    fprintf(out, bad ? "FAIL %d\n" : "PASS\n", bad); fclose(out);
    Sleep(4000);      /* the wine-run harness waits for the process before it runs its steps */
    return bad != 0;
}
