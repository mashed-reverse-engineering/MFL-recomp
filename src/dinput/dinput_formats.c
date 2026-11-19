/* dinput_formats.c (S7): the DirectInput 8 object-format tables of dinput8.lib that the game's linker put into .text at 0x4ab230..0x4ac4f0
   (4,800 bytes, layer "other"), as constant data.  Linked only with DROP_DINPUT=1 (tools/delink/link_ours.sh): mkobj then fills the old bytes
   (tools/delink/dinput_dead.txt) and the rgodf pointers of the two DIDATAFORMAT structures in .rdata (c_dfDIJoystick 0x5e1c4c, c_dfDIKeyboard
   0x5e1e54; generated as references to func_0x004ab230 / func_0x004ab4f0) resolve to these arrays.  Compiled with mingw gcc (data only).
   DirectInput compares the object GUIDs by value, so the tables point at their own copies of the dinput.h GUIDs (values checked against the
   exe's .rdata 0x5e1edc..0x5e1f6c).  Contents checked word for word against the original bytes (output/gap/gap_data.c, T-N18). */
typedef struct { unsigned long Data1; unsigned short Data2, Data3; unsigned char Data4[8]; } DiGuid;
typedef struct { const DiGuid *pguid; unsigned long dwOfs, dwType, dwFlags; } DiObjectDataFormat;     /* DIOBJECTDATAFORMAT */

#define DIGUID(n) { 0xa36d0200 + (n), 0xc9f3, 0x11cf, { 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } }
static const DiGuid GUID_XAxis = DIGUID(0xe0), GUID_YAxis = DIGUID(0xe1), GUID_ZAxis = DIGUID(0xe2), GUID_RxAxis = DIGUID(0xf4), GUID_RyAxis = DIGUID(0xf5),
                    GUID_RzAxis = DIGUID(0xe3), GUID_Slider = DIGUID(0xe4), GUID_POV = DIGUID(0xf2);
static const DiGuid GUID_Key = { 0x55728220, 0xd33c, 0x11cf, { 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };

#define DIDFT_OPTIONAL 0x80000000u
#define AXIS(g, ofs) { &g, ofs, DIDFT_OPTIONAL | 0xffff03, 0x100 }      /* DIDFT_AXIS | DIDFT_ANYINSTANCE, DIDOI_ASPECTPOSITION */
#define POV(ofs) { &GUID_POV, ofs, DIDFT_OPTIONAL | 0xffff10, 0 }       /* DIDFT_POV | DIDFT_ANYINSTANCE */
#define BTN(ofs) { 0, ofs, DIDFT_OPTIONAL | 0xffff0c, 0 }               /* DIDFT_BUTTON | DIDFT_ANYINSTANCE */
#define BTN4(o) BTN(o), BTN(o + 1), BTN(o + 2), BTN(o + 3)

/* c_rgodfDIJoystick (0x4ab230): the DIJOYSTATE layout, 6 axes, 2 sliders, 4 POVs, 32 buttons */
const DiObjectDataFormat c_rgodfDIJoystick[44] __asm__("_func_0x004ab230") = {
    AXIS(GUID_XAxis, 0x00), AXIS(GUID_YAxis, 0x04), AXIS(GUID_ZAxis, 0x08), AXIS(GUID_RxAxis, 0x0c), AXIS(GUID_RyAxis, 0x10), AXIS(GUID_RzAxis, 0x14),
    AXIS(GUID_Slider, 0x18), AXIS(GUID_Slider, 0x1c), POV(0x20), POV(0x24), POV(0x28), POV(0x2c),
    BTN4(0x30), BTN4(0x34), BTN4(0x38), BTN4(0x3c), BTN4(0x40), BTN4(0x44), BTN4(0x48), BTN4(0x4c)};

/* c_rgodfDIKeyboard (0x4ab4f0): one button per scan code: dwOfs = scan code, dwType = DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_MAKEINSTANCE(scan code) */
#define KEY(i) { &GUID_Key, (i), DIDFT_OPTIONAL | (i) << 8 | 0x0c, 0 }
#define KEY4(i) KEY(i), KEY(i + 1), KEY(i + 2), KEY(i + 3)
#define KEY16(i) KEY4(i), KEY4(i + 4), KEY4(i + 8), KEY4(i + 12)
const DiObjectDataFormat c_rgodfDIKeyboard[256] __asm__("_func_0x004ab4f0") = {
    KEY16(0x00), KEY16(0x10), KEY16(0x20), KEY16(0x30), KEY16(0x40), KEY16(0x50), KEY16(0x60), KEY16(0x70),
    KEY16(0x80), KEY16(0x90), KEY16(0xa0), KEY16(0xb0), KEY16(0xc0), KEY16(0xd0), KEY16(0xe0), KEY16(0xf0)};
