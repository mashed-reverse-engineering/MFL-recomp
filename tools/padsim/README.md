# padsim: simulated DirectInput 8 game controllers for Wine tests

The test container has no input devices and no `/dev/uinput` (see output/padsim_2026-11-22.md), so the rig sits one level higher:
`padsim8.dll` is a DirectInput 8 proxy in the pattern of tools/d3dtrace. The exe's `dinput8.dll` import is renamed to `padsim8.dll`; the DLL
loads the real dinput8, wraps `IDirectInput8A` and puts simulated devices in front of the real ones. The system keyboard stays real, so the
harness's xdotool keys work as before. Without a `padsim.cfg` the DLL passes everything through.

| File | What |
|---|---|
| padsim.c | the proxy; the header comment is the reference for the cfg and the commands |
| patch_import.py | `patch_import.py <exe> [old] [new]`: rename the import (default dinput8.dll -> padsim8.dll), in place, idempotent |
| padsim_test.c | self-check exe (25 checks: enumeration, caps, objects, state, live commands, press timing, unplug / replug, effect, keyboards) |
| Makefile | `make` = padsim8.dll, `make test` = padsim_test.exe (i686-w64-mingw32-gcc) |

## Use in a run
```
make -C tools/padsim
python3 tools/padsim/patch_import.py <game>/App_Executables/MFL.exe      # a run copy (clone.sh / prepare.py), never the install
cp tools/padsim/padsim8.dll <game>/App_Executables/
cp my.cfg <game>/App_Executables/padsim.cfg
```
Then start the run as usual (play.sh / scen_play.sh). Result: `<game>/App_Executables/padsim.log`.

## padsim.cfg
```
dev=xbox                 # device 0; types: xbox ps4 generic joystick wheel keyboard; an optional name follows the type
dev=generic My Pad       # device 1
real=0                   # hide real game controllers (default 1)
axis 0 x -1000           # a command without `at`: the start state
at 600 press 0 0 30      # at tick 600 (tick = GetDeviceState calls on device 0 = game frames): button 0 down for 30 ticks
at 700 pov 0 27000
```
Commands: `button D B 0|1`, `press D B [ticks=6]`, `axis D x|y|z|rx|ry|rz|s0|s1 -1000..1000`, `pov D <degrees * 100 | -1>`, `plug D 0|1`.
Axis values are the raw DirectInput sense: `x -1000` = left, `y -1000` = stick forward = menu up. POV: 0 up, 9000 right, 18000 down, 27000 left.

**Live commands:** complete lines appended to `App_Executables/padsim.live` are executed at the next poll. For play_inner.sh / scen_inner.sh this is
one more step (not in the tracked scripts yet; agents/pad_w17/pad_test.sh adds it to its private copy):
```
    pad) echo "$a $b $c $d $e" >> $GAME/App_Executables/padsim.live ;;
```
so a script can mix memory-gated key steps with `pad press 0 3 30`.

## What the devices look like
| Type | Name | Axes | Buttons | POV | FF |
|---|---|---|---|---|---|
| xbox | Controller (XBOX 360 For Windows) | X Y Z Rx Ry (Z = both triggers, as Windows shows an XInput pad through DirectInput) | 10 | 1 | yes |
| ps4 | Wireless Controller | X Y Z Rx Ry Rz | 14 | 1 | yes |
| generic | USB Gamepad | X Y Z Rz | 12 | 1 | no |
| joystick | Logitech Extreme 3D | X Y Rz slider | 12 | 1 | no |
| wheel | Logitech Driving Force USB | X Y | 12 | 1 | yes |
| keyboard | Keyboard 2 | - | 256 keys (button = DIK code) | - | no |

Unplugged device (`plug D 0`): not enumerated with DIEDFL_ATTACHEDONLY, Poll = DIERR_INPUTLOST once, then DIERR_NOTACQUIRED, Acquire = DIERR_UNPLUGGED.
Force feedback: `CreateEffect` / `SetParameters` / `Start` / `Stop` / `Release` and `SendForceFeedbackCommand` are logged as `FF <tick> ...` lines
(type, duration, gain, direction, magnitude); nothing is played.

## Limits
- ANSI interface only (`IID_IDirectInput8A`, what the game uses); a W request is passed through without simulated devices.
- `GetDeviceState` knows `c_dfDIJoystick` (0x50), `c_dfDIJoystick2` (0x110) and the 256 byte keyboard format; custom data formats and buffered data
  (`GetDeviceData` returns 0 events) are not simulated. One DIPROP_RANGE per device (not per axis).
- The tick counter counts `GetDeviceState` on device 0; two live device objects of device 0 would count double.
- This is not XInput: an XInput backend needs its own proxy (xinput1_4 / xinput9_1_0: `XInputGetState`, `XInputSetState`, `XInputGetCapabilities`),
  best fed from the same cfg and command files.
- Real kernel devices (uinput) are a separate, optional path: see the report.
