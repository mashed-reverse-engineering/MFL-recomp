// cluster-entries: joypads_search_cb_4a5030
/* Installed cluster joypads_search_cb_4a5030 (tools/cluster_install.py from output/cluster_pilot_md/joypads_search_cb_4a5030.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a4670*/ extern struct DInputObject * app_get_dinput_76ed10(void);
/*004a8420*/ extern int __cdecl debug_printf(const char *format, ...);
/*00770f68*/ extern void *g_app_770f68;
/*005e1c4c*/ extern unsigned char g_data_5e1c4c[];
/* joypads_search_cb_4a5030 */

#define JOYPAD_INFO_SIZE 0x244      /* sizeof(DIDEVICEINSTANCE) */
#define DISCL_FOREGROUND_NONEXCLUSIVE 5   /* DISCL_FOREGROUND | DISCL_NONEXCLUSIVE */

struct DInputDevice;

/* the slots of IDirectInputDevice that are used here (vtable byte offsets 0x2c, 0x34, 0x3c) */
typedef struct DInputDeviceVtbl {
  void *_gap_0[11];
  int (__stdcall *SetDataFormat)(struct DInputDevice *self, const void *format);              /* +0x2c */
  void *_gap_30;
  int (__stdcall *SetCooperativeLevel)(struct DInputDevice *self, void *window, unsigned int flags);   /* +0x34 */
  void *_gap_38;
  int (__stdcall *GetDeviceInfo)(struct DInputDevice *self, void *info);                       /* +0x3c */
} DInputDeviceVtbl;

typedef struct DInputDevice {
  const DInputDeviceVtbl *vtbl;
} DInputDevice;

/* IDirectInput: CreateDevice at vtable byte offset 0xc */
typedef struct DInputObject {
  const struct DInputObjectVtbl *vtbl;
} DInputObject;

typedef struct DInputObjectVtbl {
  void *_gap_0[3];
  int (__stdcall *CreateDevice)(DInputObject *self, const void *guid, DInputDevice **device, void *outer);   /* +0xc */
} DInputObjectVtbl;

/* a DIDEVICEINSTANCE (0x244 bytes) as the enumeration callback gets it */
typedef struct DeviceInstance {
  unsigned int size;                 /* +0x00 */
  unsigned char guid_instance[16];   /* +0x04 */
  unsigned char guid_product[16];    /* +0x14 */
  unsigned int device_type;          /* +0x24 */
  char instance_name[260];           /* +0x28 */
  char product_name[260];            /* +0x12c */
  unsigned char tail[20];            /* +0x230 */
} DeviceInstance;

/* one joypad slot (0x448 bytes): the device and the copy of its DIDEVICEINSTANCE */
typedef struct Joypad {
  DInputDevice *device;              /* +0x00 */
  DeviceInstance info;               /* +0x04 */
  unsigned char rest[0x200];
} Joypad;

extern Joypad g_dinput_76ed20[];     /* 0x76ed20 */

/* "function pointer pushed/stored by joypads_search (PUSH 0x4a5030): DirectInput EnumDevices callback; creates the device of every found
   joypad, reads its device info and sets data format and cooperative level" */
int joypads_search_cb_4a5030_n(const DeviceInstance *found, int *joypad_count)
{
  int index;
  Joypad *joypad;
  const DInputObjectVtbl *object_vtbl;
  DInputDevice *device;

  index = *joypad_count;
  joypad = &g_dinput_76ed20[index];
  object_vtbl = app_get_dinput_76ed10()->vtbl;
  if (object_vtbl->CreateDevice(app_get_dinput_76ed10(), found->guid_instance, &joypad->device, 0) >= 0) {
    debug_printf("Found joypad %d\n", index);
    device = joypad->device;
    debug_printf("Getting joypad %d device info\n", index);
    joypad->info.size = JOYPAD_INFO_SIZE;
    if (device->vtbl->GetDeviceInfo(device, &joypad->info) < 0) {
      debug_printf("Failed\n");
    }
    debug_printf("Joypad %d [%s][%s]\n", index, joypad->info.instance_name, joypad->info.product_name);
    debug_printf("Setting joypad %d data format\n", index);
    if (device->vtbl->SetDataFormat(device, g_data_5e1c4c) < 0) {
      debug_printf("Failed\n");
    }
    debug_printf("Setting joypad %d cooperative level\n", index);
    if (device->vtbl->SetCooperativeLevel(device, g_app_770f68, DISCL_FOREGROUND_NONEXCLUSIVE) < 0) {
      debug_printf("Failed\n");
    }
    *joypad_count = *joypad_count + 1;
  }
  return 1;
}

/* entry thunk at the original address of joypads_search_cb_4a5030: register inputs (s0,s1) -> natural C joypads_search_cb_4a5030_n; ret 8 */
__declspec(naked) void joypads_search_cb_4a5030(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        call joypads_search_cb_4a5030_n
        add esp, 8
        ret 8
    }
}
