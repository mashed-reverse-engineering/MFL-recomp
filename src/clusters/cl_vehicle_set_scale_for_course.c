// cluster-entries: vehicle_set_scale_for_course
/* Installed cluster vehicle_set_scale_for_course (tools/cluster_install.py from output/cluster_pilot_hb1/vehicle_set_scale_for_course.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*00550780*/ extern struct RpMaterial * __cdecl RpMatFXMaterialSetEnvMapCoefficient(struct RpMaterial *p0, RwReal p1);
/*00550720*/ extern struct RpMaterial * __cdecl RpMatFXMaterialSetEnvMapFrame(struct RpMaterial *p0, struct RwFrame *p1);
/*005506a0*/ extern struct RpMaterial * __cdecl RpMatFXMaterialSetEnvMapTexture(struct RpMaterial *p0, struct RwTexture *p1);
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*00607798*/ extern unsigned char g_vehicle_set_607798[];
/*00422440*/ extern int vehicle_part_id_has_damage_model(int part);
/*00423210*/ extern void vehicle_set_scale(int index, int scale_bits);
/* DECLS-END */
/* vehicle_set_scale_for_course */

/* "scale from the per course table at 0x607798" */

void vehicle_set_scale_for_course_n(VehicleRecord *vehicle_record)

{
  uint uVar1;
  RpMaterial *material;
  CourseId course_id;
  int iVar2;
  int iVar3;
  
  course_id = current_course_id();
  vehicle_set_scale(vehicle_record->index,*(undefined4 *)(g_vehicle_set_607798 + course_id * 4));
  uVar1 = vehicle_record->flags;
  iVar3 = 0;
  do {
    if (vehicle_record->part_frames[iVar3] != 0) {
      iVar2 = vehicle_part_id_has_damage_model(iVar3);
      if (iVar2 != 0) {
        material = (RpMaterial *)**(undefined4 **)(*(int *)(vehicle_record->part_frames[iVar3] + 0x18) + 0x20);
        if ((uVar1 & 2) != 0) {
          RpMatFXMaterialSetEnvMapCoefficient(material,vehicle_record->half_scale);
        }
        if ((uVar1 & 4) != 0) {
          RpMatFXMaterialSetEnvMapFrame(material,(RwFrame *)vehicle_record->envmap_frame);
        }
        if ((uVar1 & 1) != 0) {
          RpMatFXMaterialSetEnvMapTexture(material,(RwTexture *)vehicle_record->envmap_texture);
        }
      }
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 < 0x80);
  if ((uVar1 & 2) != 0) {
    vehicle_record->flags = vehicle_record->flags & 0xfffffffd;
  }
  if ((uVar1 & 4) != 0) {
    vehicle_record->flags = vehicle_record->flags & 0xfffffffb;
  }
  if ((uVar1 & 1) != 0) {
    vehicle_record->flags = vehicle_record->flags & 0xfffffffe;
  }
  return;
}

/* entry thunk at the original address of vehicle_set_scale_for_course: register inputs (esi) -> natural C vehicle_set_scale_for_course_n; ret 0 */
__declspec(naked) void vehicle_set_scale_for_course(void)
{
    __asm {
        push esi
        call vehicle_set_scale_for_course_n
        add esp, 4
        ret
    }
}
