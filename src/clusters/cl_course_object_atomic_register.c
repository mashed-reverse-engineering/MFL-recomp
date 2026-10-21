// cluster-entries: course_object_atomic_register
/* Installed cluster course_object_atomic_register (tools/cluster_install.py from output/cluster_pilot_uc3/course_object_atomic_register.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef struct RpAtomic RpAtomic;
typedef struct RpWorld RpWorld;
typedef struct RwFrame RwFrame;
/*004d15a0*/ extern RwFrame *RwFrameTransform(RwFrame *frame, RwMatrix *matrix, enum RwOpCombineType combine);
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, RwV3d *translation, enum RwOpCombineType combine);
/*004f4560*/ extern RpWorld *RpAtomicGetWorld(RpAtomic *atomic);
/*004f44d0*/ extern RpWorld *RpWorldRemoveAtomic(RpWorld *world, RpAtomic *atomic);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(RpAtomic *atomic);
/*00499dc0*/ extern int shadow_project_to_ground_00499dc0(int param, RwMatrix *matrix, float *position, RwMatrix *shadow_matrix, int *out);
extern void LAB_00473d50(void);
extern int g_gfx_89de0c;
extern int g_course_object_count;

/* One entry (0x130 bytes) of the course object table. */
typedef struct CourseObject {
  RwV3d pos;                 /* +0x00 world position (matrix->pos) */
  int atomic_field_28;       /* +0x0c copied from the atomic */
  int kind;                  /* +0x10 = 0x1d */
  RwV3d *pos_ptr;            /* +0x14 -> pos */
  int index;                 /* +0x18 own table index */
  void (*callback)(void);    /* +0x1c */
  char pad_20[0x6c];
  RpAtomic *atomic;          /* +0x8c */
  int field_90, field_94;
  RwMatrix frame_matrix;     /* +0x98 copy of the frame's modelling matrix */
  RwFrame *frame;            /* +0xd8 */
  char pad_dc[0x10];
  int param;                 /* +0xec */
  RwMatrix shadow_matrix;    /* +0xf0 projected shadow transform */
} CourseObject;
typedef char CourseObject_size_is_0x130[sizeof(CourseObject) == 0x130 ? 1 : -1];
extern CourseObject g_car_6af658[];   /* the table: record 0 starts at its position vector */

/* Registers `atomic` (placed by `matrix`) as the next course object: stores its position, frame and a copy of the frame matrix, detaches it from its world,
   and builds the shadow transform with shadow_project_to_ground (identity moved 100000 units away when no ground is found). `param` goes to the entry and the projector. */
void course_object_atomic_register_n(char *atomic, RwMatrix *matrix, int shadow_param, int param)
{
  CourseObject *obj;
  RpWorld *world;
  RwFrame *frame;
  int shadow_out[32];      /* only [6] is preset; the callee writes more than the 7 words the original reserved (it clobbers saved registers there), so the buffer is generous */
  RwV3d away;

  obj = &g_car_6af658[g_course_object_count];
  obj->atomic = (RpAtomic *)atomic;
  obj->kind = 0x1d;
  obj->callback = LAB_00473d50;
  obj->index = g_course_object_count;
  obj->pos_ptr = &obj->pos;
  if ((atomic[0x4c] & 2) != 0) {
    _rpAtomicResyncInterpolatedSphere((RpAtomic *)atomic);
  }
  obj = &g_car_6af658[g_course_object_count];
  obj->atomic_field_28 = *(int *)(atomic + 0x28);
  obj->pos = matrix->pos;
  RwFrameTransform(*(RwFrame **)(atomic + 4), matrix, rwCOMBINEREPLACE);
  world = RpAtomicGetWorld(g_car_6af658[g_course_object_count].atomic);
  if (world != 0) {
    RpWorldRemoveAtomic(world, g_car_6af658[g_course_object_count].atomic);
  }
  obj = &g_car_6af658[g_course_object_count];
  obj->param = param;
  obj->field_90 = 0;
  obj->field_94 = 0;
  frame = *(RwFrame **)(atomic + 4);
  obj->frame = frame;
  obj->frame_matrix = *(RwMatrix *)((char *)frame + 0x10);
  shadow_out[6] = -1;
  if (shadow_project_to_ground_00499dc0(shadow_param, &obj->frame_matrix, (float *)((char *)obj->atomic + g_gfx_89de0c + 8), &obj->shadow_matrix, shadow_out) == 0) {
    obj = &g_car_6af658[g_course_object_count];
    obj->shadow_matrix.right.x = 1.0f;
    obj->shadow_matrix.up.y = 1.0f;
    obj->shadow_matrix.at.z = 1.0f;
    obj->shadow_matrix.right.y = 0.0f;
    obj->shadow_matrix.right.z = 0.0f;
    obj->shadow_matrix.up.x = 0.0f;
    obj->shadow_matrix.up.z = 0.0f;
    obj->shadow_matrix.at.x = 0.0f;
    obj->shadow_matrix.at.y = 0.0f;
    obj->shadow_matrix.pos.x = 0.0f;
    obj->shadow_matrix.pos.y = 0.0f;
    obj->shadow_matrix.pos.z = 0.0f;
    obj->shadow_matrix.flags |= 0x20003;
    away.x = 0.0f;
    away.y = 100000.0f;
    away.z = 0.0f;
    RwMatrixTranslate(&obj->shadow_matrix, &away, rwCOMBINEREPLACE);
  }
  g_course_object_count++;
}

/* entry thunk at the original address of course_object_atomic_register: register inputs (ecx,eax,s0,s1) -> natural C course_object_atomic_register_n; ret 0 */
__declspec(naked) void course_object_atomic_register(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        push ecx
        call course_object_atomic_register_n
        add esp, 16
        ret
    }
}
