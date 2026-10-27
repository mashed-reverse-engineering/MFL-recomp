// cluster-entries: course_city_helper_455b80
/* Installed cluster course_city_helper_455b80 (tools/cluster_install.py from output/cluster_pilot_k/course_city_helper_455b80.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00455aa0*/ extern int course_city_cb5_helper_455aa0();
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*0049a9b0*/ extern int fx_slot_find_free();
/*0049af00*/ extern int fx_slot_set_field_b8();
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*006a1f2c*/ extern char g_course_nt1_6a1f2c[];
/*0060d8e0*/ extern char g_course_city_60d8e0[];
/*0060d8d4*/ extern char g_course_city_60d8d4[];
/*0060d8c8*/ extern char g_course_city_60d8c8[];
/*0060d8bc*/ extern char g_course_nt1_60d8bc[];
/* course_city_helper_455b80 */

/* WARNING: Unknown calling convention */
/* callers course_city_cb0,course_city_cb6,course_city_cb7 */

/* Spawns the city course light effects: for each world object id range (1000+i x26, 2000+i x12, 5000+i x3, 6000+i x4 with two anchor points) the object index is looked up
   and an fx slot (type 2 scale 0.7, type 3 scale 0.4) is placed at the object's transformed anchor; the course data also carries two arrays of fixed positions
   (counts at +0xc008 / +0xc00c) that get their own fx slots.  Indices and slots go into g_course_nt1_6a1f2c's arrays. */
void course_city_helper_455b80_n(int course)

{
  int obj_or_pos;
  RwMatrix *obj_matrix;
  undefined4 fx_slot;
  int i;
  RwV3d world_pos;
  
  i = 0;
  course_city_cb5_helper_455aa0();
  do {
    obj_or_pos = rwp_object_find_index_by_id(i + 1000);
    *(int *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x14c)) = obj_or_pos;
    if (obj_or_pos != -1) {
      obj_matrix = (RwMatrix *)rwp_object_get_matrix(obj_or_pos);
      RwV3dTransformPoint(&world_pos,(RwV3d *)((int)&g_course_nt1_60d8bc + 0x4),obj_matrix);
      fx_slot = fx_slot_find_free(2,&world_pos,0,0x14);
      *(undefined4 *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0xb4)) = fx_slot;
      fx_slot_set_field_b8(fx_slot,0x3f333333);
    }
    i = i + 1;
  } while (i < 0x1a);
  i = 0;
  do {
    obj_or_pos = rwp_object_find_index_by_id(i + 2000);
    *(int *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x11c)) = obj_or_pos;
    if (obj_or_pos != -1) {
      obj_matrix = (RwMatrix *)rwp_object_get_matrix(obj_or_pos);
      RwV3dTransformPoint(&world_pos,(RwV3d *)((int)&g_course_city_60d8c8 + 0x4),obj_matrix);
      fx_slot = fx_slot_find_free(3,&world_pos,0,0x14);
      *(undefined4 *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x64)) = fx_slot;
      fx_slot_set_field_b8(fx_slot,0x3ecccccd);
    }
    i = i + 1;
  } while (i < 0xc);
  i = 0;
  if (0 < *(int *)(course + 0xc008)) {
    obj_or_pos = course + 0x1830;
    do {
      fx_slot = fx_slot_find_free(2,obj_or_pos,0,0x14);
      *(undefined4 *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x14)) = fx_slot;
      fx_slot_set_field_b8(fx_slot,0x3f333333);
      i = i + 1;
      obj_or_pos = obj_or_pos + 0x40;
    } while (i < *(int *)(course + 0xc008));
  }
  i = 0;
  if (0 < *(int *)(course + 0xc00c)) {
    obj_or_pos = course + 0x2430;
    do {
      fx_slot = fx_slot_find_free(3,obj_or_pos,0,0x14);
      *(undefined4 *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x1b4)) = fx_slot;
      fx_slot_set_field_b8(fx_slot,0x3ecccccd);
      i = i + 1;
      obj_or_pos = obj_or_pos + 0x40;
    } while (i < *(int *)(course + 0xc00c));
  }
  i = 0;
  do {
    obj_or_pos = rwp_object_find_index_by_id(i + 5000);
    *(int *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x20c)) = obj_or_pos;
    if (obj_or_pos != -1) {
      obj_matrix = (RwMatrix *)rwp_object_get_matrix(obj_or_pos);
      RwV3dTransformPoint(&world_pos,(RwV3d *)((int)&g_course_city_60d8d4 + 0x4),obj_matrix);
      fx_slot = fx_slot_find_free(3,&world_pos,0,0x14);
      *(undefined4 *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x218)) = fx_slot;
      fx_slot_set_field_b8(fx_slot,0x3ecccccd);
    }
    i = i + 1;
  } while (i < 3);
  i = 0;
  do {
    obj_or_pos = rwp_object_find_index_by_id(i + 6000);
    *(int *)(i * 4 + ((int)&g_course_nt1_6a1f2c + 0x4)) = obj_or_pos;
    if (obj_or_pos != -1) {
      obj_matrix = (RwMatrix *)rwp_object_get_matrix(obj_or_pos);
      RwV3dTransformPoint(&world_pos,(RwV3d *)((int)&g_course_city_60d8e0 + 0x4),obj_matrix);
      fx_slot = fx_slot_find_free(3,&world_pos,0,0x14);
      *(undefined4 *)(i * 8 + ((int)&g_course_nt1_6a1f2c + 0x94)) = fx_slot;
      fx_slot_set_field_b8(fx_slot,0x3ecccccd);
      RwV3dTransformPoint(&world_pos,(RwV3d *)((int)&g_course_city_60d8e0 + 0x10),obj_matrix);
      fx_slot = fx_slot_find_free(3,&world_pos,0,0x14);
      *(undefined4 *)(i * 8 + ((int)&g_course_nt1_6a1f2c + 0x98)) = fx_slot;
      fx_slot_set_field_b8(fx_slot,0x3ecccccd);
    }
    i = i + 1;
  } while (i < 4);
  return;
}

/* entry thunk at the original address of course_city_helper_455b80: register inputs (ebx) -> natural C course_city_helper_455b80_n; ret 0 */
__declspec(naked) void course_city_helper_455b80(void)
{
    __asm {
        push ebx
        call course_city_helper_455b80_n
        add esp, 4
        ret
    }
}
