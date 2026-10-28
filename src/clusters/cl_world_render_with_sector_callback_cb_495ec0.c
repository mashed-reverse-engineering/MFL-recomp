// cluster-entries: world_render_with_sector_callback_cb_495ec0
/* Installed cluster world_render_with_sector_callback_cb_495ec0 (tools/cluster_install.py from output/cluster_pilot_kc/world_render_with_sector_callback_cb_495ec0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042d9a0*/ extern int store_u32();
/*0042d9b0*/ extern int rw_stream_chunk_copy();
/*0048d840*/ extern int __cdecl course_objects_test_sphere(undefined4 p0);
/*0048d8a0*/ extern int __cdecl course_objects_test_bbox(undefined4 p0);
/*00544430*/ extern int FUN_00544430(undefined4 p0);
/*00554340*/ extern int __cdecl FUN_00554340(int a1, int a2);
/*00554400*/ extern double RpUserDataArrayGetReal();
/*0068b8f8*/ extern undefined4 g_race_phase_arg;
/*00702a90*/ extern undefined4 g_physics_702a90;
/*00702a94*/ extern undefined4 g_physics_702a94;
/*007032b0*/ extern undefined4 g_particle_system_7032b0;
/*007036b0*/ extern undefined4 g_physics_7036b0;
/*007036b4*/ extern undefined4 g_physics_7036b4;
/*007036b8*/ extern undefined4 g_particle_system_7036b8;
/*007036c8*/ extern undefined4 g_data_7036c8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* world_render_with_sector_callback_cb_495ec0 */

/* function pointer pushed/stored by world_render_with_sector_callback (PUSH 0x495ec0) */

static int world_sector_list_add_r(undefined4 p0);
extern void world_sector_list_add(void);
/* Per-sector callback: reads the bounding sphere from the sector user data, counts it, and if non-empty either queues the sector in the visible list (sphere or bbox hit) or adds it to the world sector list (inferred); returns sector. */
int world_render_with_sector_callback_cb_495ec0(int sector)

{
  RwStringFunctions *str_funcs;
  undefined4 user_data;
  RwChar *string1;
  int test_result;
  RwChar *string2;
  RwSphere sphere; /* bounding sphere read from the sector's user data (x, y, z, radius) */
  
  user_data = FUN_00554340(sector,0);
  string2 = "BS";
  str_funcs = &RwGlobalsRelease_stringFuncs(RwEngineInstance);
  string1 = (RwChar *)rw_first_field(user_data);
  RwStringFunctions_vecStrcmp(str_funcs)(string1,string2);
  FUN_00544430(user_data);
  g_physics_7036b0 = g_physics_7036b0 + (uint)*(ushort *)(sector + 0x84);
  g_physics_702a94 = g_physics_702a94 + 1;
  sphere.center.x = (float)RpUserDataArrayGetReal(user_data,0);
  sphere.center.y = (float)RpUserDataArrayGetReal(user_data,1);
  sphere.center.z = (float)RpUserDataArrayGetReal(user_data,2);
  sphere.radius = (float)RpUserDataArrayGetReal(user_data,3);
  if (*(short *)(sector + 0x84) != 0) {
    test_result = course_objects_test_sphere(&sphere);
    if ((test_result != 0) || (test_result = course_objects_test_bbox(sector + 0x60), test_result != 0)) {
      (&g_particle_system_7032b0)[g_particle_system_7036b8] = sector;
      g_particle_system_7036b8 = g_particle_system_7036b8 + 1;
      return sector;
    }
    test_result = rw_first_field(&g_race_phase_arg);
    if (test_result == 1) {
      rw_stream_chunk_copy(&g_race_phase_arg,&g_data_7036c8);
      store_u32(&g_race_phase_arg,0);
    }
    if (g_data_7036c8 != 0) {
      return sector;
    }
    world_sector_list_add_r(sector);
    g_physics_702a90 = g_physics_702a90 + (uint)*(ushort *)(sector + 0x84);
  }
  g_physics_7036b4 = g_physics_7036b4 + 1;
  return sector;
}

/* world_sector_list_add_r: C -> original world_sector_list_add (registers ecx) */
__declspec(naked) int world_sector_list_add_r(undefined4 p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call world_sector_list_add
        ret
    }
}
