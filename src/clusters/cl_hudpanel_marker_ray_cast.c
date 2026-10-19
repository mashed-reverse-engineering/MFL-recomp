// cluster-entries: hudpanel_marker_ray_cast
/* Installed cluster hudpanel_marker_ray_cast (tools/cluster_install.py from output/cluster_pilot10/hudpanel_marker_ray_cast.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c4720*/ extern void FUN_004c4720(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void FUN_004c51a0(undefined4 p0, undefined4 p1, undefined4 p2);
/*005ddafc*/ extern float k_f_0_05;
/* hudpanel_marker_ray_cast */

/* WARNING: Unknown calling convention */
/* "casts a ray from the marker and stores the hit point matrix (surface id check against 0xFEFEFF)"
    */

undefined4 hudpanel_marker_ray_cast_n(undefined4 *in_eax, int in_edx, undefined4 in_ecx)

{
  int iVar1;
  undefined4 *puVar2;
  struct {
    float local_8c;
    float local_88;
    float local_84;
    float local_80;
    float local_7c;
    float local_78;
    int local_74;
    undefined1 local_70 [36];
    int local_4c;
    undefined4 local_48;
    int local_44;
    undefined4 local_40 [16];
  } frame;
  
  iVar1 = world_intersect_line(in_ecx,in_edx,&frame.local_80);
  if ((iVar1 != 0) &&
     (*(int *)(*(int *)(*(int *)(frame.local_44 + 0x10) +
                       ((uint)*(ushort *)(*(char **)(frame.local_4c + 4) + 6 + frame.local_74 * 8) +
                       (uint)*(ushort *)(frame.local_4c + 0x80)) * 4) + 4) != -0xfefeff)) {
    FUN_004c4720(&frame.local_8c,in_edx,in_edx + 0xc,frame.local_48);
    frame.local_8c = frame.local_80 * k_f_0_05 + frame.local_8c;
    frame.local_88 = frame.local_7c * k_f_0_05 + frame.local_88;
    frame.local_84 = frame.local_78 * k_f_0_05 + frame.local_84;
    FUN_004c51a0(frame.local_40,frame.local_70,&frame.local_8c);
    if (in_eax != (undefined4 *)0x0) {
      puVar2 = frame.local_40;
      for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
        *in_eax = *puVar2;
        puVar2 = puVar2 + 1;
        in_eax = in_eax + 1;
      }
    }
    return 1;
  }
  return 0;
}

/* entry thunk at the original address of hudpanel_marker_ray_cast: register inputs (eax,edx,ecx) -> natural C hudpanel_marker_ray_cast_n; ret 0 */
__declspec(naked) void hudpanel_marker_ray_cast(void)
{
    __asm {
        push ecx
        push edx
        push eax
        call hudpanel_marker_ray_cast_n
        add esp, 12
        ret
    }
}
