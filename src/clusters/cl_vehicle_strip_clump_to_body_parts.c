// cluster-entries: vehicle_strip_clump_to_body_parts
/* Installed cluster vehicle_strip_clump_to_body_parts (tools/cluster_install.py from output/cluster_pilot9/vehicle_strip_clump_to_body_parts.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00425dc0*/ extern undefined __cdecl bbox_init(undefined4 *p0, struct RpClump *p1);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6bf0*/ extern int FUN_004f6bf0(undefined4 p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f70f0*/ extern struct RpClump * RpClumpRemoveAtomic(struct RpClump *p0, struct RpAtomic *p1);
/* vehicle_strip_clump_to_body_parts */

/* WARNING: Unknown calling convention */
/* "removes all atomics except ids 0x25,0x27,0x29,0x2a,0x2b,0x2d and fits the bounding box" */

void vehicle_strip_clump_to_body_parts_n(VehicleRecord *vehicle_record)

{
  RpAtomic *atomic;
  RpClump *clump;
  int atomic_count;
  undefined4 part_id;
  int i;
  struct {
    uint unused_count;
    undefined4 atomics [128];
  } frame_obj;
  
  clump = (RpClump *)FUN_004f6bf0(vehicle_record->frame);
  atomic_count = clump_get_atomics(clump,frame_obj.atomics);
  i = 0;
  if (0 < atomic_count) {
    do {
      atomic = (RpAtomic *)frame_obj.atomics[i];
      part_id = FUN_004c52b0(atomic,0,0);
      switch(part_id) {
      case 0x25:
      case 0x27:
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2d:
        break;
      default:
        RpClumpRemoveAtomic(clump,atomic);
        RpAtomicDestroy(atomic);
        frame_obj.atomics[i] = 0;
      }
      i = i + 1;
    } while (i < atomic_count);
  }
  frame_obj.unused_count = (uint)(vehicle_record->matrix6[3] == vehicle_record->matrix6[4]);
  frame_obj.unused_count = (uint)((float)frame_obj.unused_count == vehicle_record->matrix6[5]);
  frame_obj.unused_count = (uint)((float)frame_obj.unused_count == vehicle_record->matrix6[0]);
  frame_obj.unused_count = (uint)((float)frame_obj.unused_count == vehicle_record->matrix6[1]);
  if ((float)frame_obj.unused_count == vehicle_record->matrix6[2]) {
    bbox_init(vehicle_record->matrix6,clump);
  }
  RpClumpDestroy(clump);
  if ((RpAtomic_interpolator_flags(((RpAtomic *)vehicle_record->part_frames[0x3b])) & 2) != 0) {
    _rpAtomicResyncInterpolatedSphere((RpAtomic *)vehicle_record->part_frames[0x3b]);
  }
  atomic_count = vehicle_record->part_frames[0x3b];
  vehicle_record->local_sphere[0] = *(float *)(atomic_count + 0x1c);
  vehicle_record->local_sphere[1] = *(float *)(atomic_count + 0x20);
  vehicle_record->local_sphere[2] = *(float *)(atomic_count + 0x24);
  vehicle_record->local_sphere[3] = *(float *)(atomic_count + 0x28);
  vehicle_record->world_sphere[0] = *(float *)(atomic_count + 0x1c);
  vehicle_record->world_sphere[1] = *(float *)(atomic_count + 0x20);
  vehicle_record->world_sphere[2] = *(float *)(atomic_count + 0x24);
  vehicle_record->world_sphere[3] = *(float *)(atomic_count + 0x28);
  return;
}

/* entry thunk at the original address of vehicle_strip_clump_to_body_parts: register inputs (eax) -> natural C vehicle_strip_clump_to_body_parts_n; ret 0 */
__declspec(naked) void vehicle_strip_clump_to_body_parts(void)
{
    __asm {
        push eax
        call vehicle_strip_clump_to_body_parts_n
        add esp, 4
        ret
    }
}
