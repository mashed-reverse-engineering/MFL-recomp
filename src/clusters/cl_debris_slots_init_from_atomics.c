// cluster-entries: debris_slots_init_from_atomics
/* Installed cluster debris_slots_init_from_atomics (tools/cluster_install.py from output/cluster_pilot11/debris_slots_init_from_atomics.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00424a00*/ extern int debris_body_configure(struct RpAtomic *atomic, float radius, float p2, float p3, int p4);
/*00424a60*/ extern int debris_model_get_first_clump();
/*004c52b0*/ extern int atomic_geometry_user_data_int(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*0056e5b0*/ extern void FUN_0056e5b0(undefined4 p0, undefined4 p1);
/*005dd340*/ extern float k_f_0_5;
/* debris_slots_init_from_atomics */

/* "assigns atomics to debris slots by their id range 0x50..0x59 / 0x5a..0x63" */

void debris_slots_init_from_atomics_n(undefined4 mode, int debris_set, undefined4 owner, int atomics)

{
  RpAtomic *atomic;
  int atomic_id;
  int slot;
  int i;
  
  *(undefined4 *)(debris_set + 0xf8) = mode;
  *(undefined4 *)(debris_set + 0xfc) = owner;
  i = 0;
  do {
    atomic = *(RpAtomic **)(atomics + i * 4);
    slot = 0;
    if (atomic != (RpAtomic *)0x0) {
      atomic_id = atomic_geometry_user_data_int(atomic,0,0);
      if ((((atomic_id < 0x50) || (0x59 < atomic_id)) || (*(int *)(debris_set + 0xf8) == 0)) &&
         (((atomic_id < 0x5a || (99 < atomic_id)) || (*(int *)(debris_set + 0xf8) == 1)))) {
        if (*(int *)(debris_set + 0xf8) == 0) {
          slot = atomic_id + -0x50;
        }
        else if (*(int *)(debris_set + 0xf8) == 1) {
          slot = atomic_id + -0x5a;
        }
        slot = debris_set + slot * 0x18;
        *(RpAtomic **)(slot + 0xc) = atomic;
        RwFrameRemoveChild(RpAtomic_object_object_parent(atomic));
        if ((RpAtomic_interpolator_flags(atomic) & 2) != 0) {
          _rpAtomicResyncInterpolatedSphere(atomic);
        }
        atomic_id = debris_body_configure
                          (atomic,(RpAtomic_boundingSphere(atomic)).radius * k_f_0_5,0.1f,1.0f,0)
        ;
        *(int *)(*(int *)(atomic_id + 0x10) + 8) = (int)RpAtomic_object_object_parent(atomic) + 0x10;
        FUN_0056e5b0(atomic_id,0);
        *(int *)(slot + 0x10) = atomic_id;
        debris_model_get_first_clump(atomic);
      }
    }
    i = i + 1;
  } while (i < 0x14);
  return;
}

/* entry thunk at the original address of debris_slots_init_from_atomics: register inputs (eax,ebx,ecx,s0) -> natural C debris_slots_init_from_atomics_n; ret 0 */
__declspec(naked) void debris_slots_init_from_atomics(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        push ebx
        push eax
        call debris_slots_init_from_atomics_n
        add esp, 16
        ret
    }
}
