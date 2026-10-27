// cluster-entries: world_sectors_process_cb_4890e0
/* Installed cluster world_sectors_process_cb_4890e0 (tools/cluster_install.py from output/cluster_pilot_kg/world_sectors_process_cb_4890e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c4620*/ extern void FUN_004c4620(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c54f0*/ extern void FUN_004c54f0(undefined4 p0, undefined4 p1, undefined4 p2);
/*00554320*/ extern int __cdecl FUN_00554320(int p0, void *p1, int p2, int p3);
/*00554340*/ extern int __cdecl FUN_00554340(int a1, int a2);
/*00554430*/ extern void __cdecl RpUserDataArraySetReal(void * p0, RwInt32 p1, RwReal p2);
/*005dfefc*/ extern undefined g_data_5dfefc;
/* world_sectors_process_cb_4890e0 */

/* function pointer pushed/stored by world_sectors_process (PUSH 0x4890e0) */

int world_sectors_process_cb_4890e0(int sector)

{
  int iVar1;
  undefined4 uVar2;
  struct {
    float x;
    float y;
    float z;
    float radius;
  } sphere; /* bounding sphere of the sector (centre + radius), stored as the user data array */
  
  iVar1 = FUN_00554340(sector,0);
  if (iVar1 == 0) {
    if (*(short *)(sector + 0x82) == 0) {
      FUN_004c4620(&sphere.x,sector + 0x60,2);
      sphere.radius = 0.0f;
    }
    else {
      FUN_004c54f0(&sphere.x,*(undefined4 *)(sector + 8),(uint)*(ushort *)(sector + 0x82));
    }
    uVar2 = FUN_00554320(sector,&g_data_5dfefc,2,4);
    uVar2 = FUN_00554340(sector,uVar2);
    RpUserDataArraySetReal(uVar2,0,sphere.x);
    RpUserDataArraySetReal(uVar2,1,sphere.y);
    RpUserDataArraySetReal(uVar2,2,sphere.z);
    RpUserDataArraySetReal(uVar2,3,sphere.radius);
  }
  return sector;
}

