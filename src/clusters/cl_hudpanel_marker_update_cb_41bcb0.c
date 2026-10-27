// cluster-entries: hudpanel_marker_update_cb_41bcb0
/* Installed cluster hudpanel_marker_update_cb_41bcb0 (tools/cluster_install.py from output/cluster_pilot_kg/hudpanel_marker_update_cb_41bcb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14(void);
/*00485660*/ extern int particle_pool_find_oldest_or_free(int a, int *marker, int b);
/*00487670*/ extern int effect_sprite_spawn(int a, int *marker, void *colour, float s0, float s1);
/*0049cb60*/ extern int sphere_vs_box_test(float *box, float *sphere);
/*005dd330*/ extern float float_1_0;
/* hudpanel_marker_update_cb_41bcb0 */

/* function pointer pushed/stored by hudpanel_marker_update (MOV dword ptr [ESP + 0xec],0x41bcb0) */
/* trigger callback of a hud marker: sphere = the trigger sphere (+0x14 the marker record), other = the object that entered it (+0x10 kind, +0x14 data);
   kinds 7 and 0x12 are ignored, kind 0x15 must really touch the box; then a 1.0 long ray down from the marker object's position tests the world and a
   hit spawns the (0xff,0x80,0,0x80) sprite effect; the marker goes to state 2 (exploding) either way. */
static int hudpanel_marker_ray_cast_r(int * p0, float * p1, int p2);
extern void hudpanel_marker_ray_cast(void);
void hudpanel_marker_update_cb_41bcb0(float *sphere, int other)

{
  int *marker;
  int kind;
  float *pos;
  float ray[6];
  unsigned char colour[4];

  marker = *(int **)&sphere[5];
  kind = *(int *)(other + 0x10);
  if ((kind != 7) && (kind != 0x12)) {
    if (kind == 0x15) {
      if (sphere_vs_box_test((float *)(*(int *)(other + 0x14) + 0x68), sphere) == 0) {
        return;
      }
      marker[0x11] = 2;
    }
    pos = (float *)(*(int *)(marker[0x10] + 4) + 0x40);
    ray[0] = pos[0];
    ray[1] = pos[1];
    ray[2] = pos[2];
    ray[3] = pos[0];
    ray[4] = pos[1] - float_1_0;
    ray[5] = pos[2];
    if (hudpanel_marker_ray_cast_r(marker, ray, weapon_get_rw_801b14()) != 0) {
      colour[0] = 0xff;
      colour[1] = 0x80;
      colour[2] = 0;
      colour[3] = 0x80;
      particle_pool_find_oldest_or_free(0, marker, 0);
      effect_sprite_spawn(0, marker, colour, 2.0f, 4.0f);
    }
    marker[0x11] = 2;
  }
  return;
}

/* hudpanel_marker_ray_cast_r: C -> original hudpanel_marker_ray_cast (registers eax,edx,ecx) */
__declspec(naked) int hudpanel_marker_ray_cast_r(int * p0, float * p1, int p2)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov edx, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        call hudpanel_marker_ray_cast
        ret
    }
}
