#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*0048e3d0*/ extern int rwp_object_get_user_data();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*0049a980*/ extern int fx_slot_release();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd36c*/ extern float k_f_0_4;
/*005df540*/ extern float k_f_21_829;
/*005df544*/ extern float k_f_85_38;
/*0060d8c8*/ extern float g_course_city_60d8c8;
/*0060d8d4*/ extern float g_course_city_60d8d4;
/*0060d8e0*/ extern float g_course_city_60d8e0;
/*007ef804*/ extern undefined4 g_tick_number;
/*006a1f2c*/ extern char g_course_nt1_6a1f2c[];
/*0060d8fc*/ extern char g_course_spline_60d8fc[];

#define TICK_PARITY_MASK (0x80000001)
#define FLOAT_BITS_NEG_ONE (0xbf800000)
#define FLOAT_BITS_ONE (0x3f800000)
#define SLOT_EMPTY (-1)
#define MATRIX_POS_X (0x30)
#define MATRIX_POS_Y (0x34)
#define MATRIX_POS_Z (0x38)
#define NT1_A_FX_BASE (0xb4)
#define NT1_A_OBJ_BASE (0x14c)
#define NT1_A_BYTES (0x68)
#define NT1_B_FX_BASE (0x64)
#define NT1_B_OBJ_BASE (0x11c)
#define NT1_B_BYTES (0x30)
#define NT1_C_FX_BASE (0x218)
#define NT1_C_OBJ_BASE (0x20c)
#define NT1_C_BYTES (0xc)
#define NT1_D_OBJ_BASE (0x4)
#define NT1_D_FX_BASE (0x94)
#define NT1_D_FX_END (0xb4)
#define SPLINE_REF_OFFSET (0xc)
#define SPLINE_REF_END (0x8)
/* func_0x00455ee0 / func_0x00455fd0 take the effect position (a vector in the caller's frame) in EDI,
 * func_0x00455e60 in ESI (the original: `lea edi|esi,[esp+..]` before each call); adapters load the
 * register and keep the caller's value. */
extern void func_0x00455ee0(void);
extern void func_0x00455fd0(void);
extern void func_0x00455e60(void);
static void func_0x00455ee0_r(RwV3d *reg_edi);
static void func_0x00455fd0_r(RwV3d *reg_edi);
static void func_0x00455e60_r(RwV3d *reg_esi);
__declspec(naked) void func_0x00455ee0_r(RwV3d *reg_edi)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call func_0x00455ee0
        pop edi
        ret
    }
}
__declspec(naked) void func_0x00455fd0_r(RwV3d *reg_edi)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call func_0x00455fd0
        pop edi
        ret
    }
}
__declspec(naked) void func_0x00455e60_r(RwV3d *reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call func_0x00455e60
        pop esi
        ret
    }
}

/* course_city_cb1 */


/*
 * Per-frame effect update for the city course (inferred).
 * Spawns an effect at a random point around (k_f_85_38, 0.99, k_f_21_829) on
 * even ticks, then walks the four object-slot tables in g_course_nt1_6a1f2c.
 * Every live slot whose object has flag 8 set gets its fx slot released and
 * is marked SLOT_EMPTY; the effect is placed at the object's matrix position.
 * Takes no parameters and returns nothing.
 * The local slot_offset is a byte offset into the tables in the first three
 * loops and holds a matrix pointer in the last loop.
 */
void course_city_cb1(void)

{
  uint user_data;
  char *matrix;
  float *ref_height_ptr;
  undefined4 *object_handle_ptr;
  int slot_offset;
  int *fx_slot_ptr;
  bool tick_is_even;
  double rand_value;
  RwV3d dir;
  RwV3d pos;
  RwV3d fx_pos;

  /* tick_is_even (inferred): bit 0 of the tick count is clear */
  user_data = g_tick_number & TICK_PARITY_MASK;
  tick_is_even = user_data == 0;
  pos.x = 85.38;
  pos.y = 0.99;
  pos.z = 21.829;
  dir.x = 0.0;
  dir.y = 0.0;
  dir.z = 0.0;
  if ((int)user_data < 0) {
    tick_is_even = (user_data - 1 | 0xfffffffe) == 0xffffffff;
  }
  if (tick_is_even) {
    func_0x00455ee0_r(&pos);
  }
  /* random unit vector in the xz plane, scaled and offset from the base point */
  rand_value = (double)random_range(FLOAT_BITS_NEG_ONE,FLOAT_BITS_ONE);
  dir.x = (RwReal)rand_value;
  rand_value = (double)random_range(FLOAT_BITS_NEG_ONE,FLOAT_BITS_ONE);
  dir.z = (RwReal)rand_value;
  RwV3dNormalize(&dir,&dir);
  dir.x = dir.x * k_f_0_4;
  dir.y = 0.0;
  pos.y = 0.1;
  dir.z = dir.z * k_f_0_4;
  pos.x = dir.x + k_f_85_38;
  pos.z = dir.z + k_f_21_829;
  func_0x00455fd0_r(&pos);
  /* table A: 26 slots, fx slot at NT1_A_FX_BASE, object at NT1_A_OBJ_BASE; y offset g_course_city_60d8c8 */
  slot_offset = 0;
  do {
    if (SLOT_EMPTY < *(int *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_A_FX_BASE))) {
      user_data = rwp_object_get_user_data(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_A_OBJ_BASE)));
      if ((user_data & 8) != 0) {
        matrix = rwp_object_get_matrix(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_A_OBJ_BASE)));
        fx_pos.x = *(float *)(matrix + MATRIX_POS_X);
        fx_pos.y = *(float *)(matrix + MATRIX_POS_Y) - g_course_city_60d8c8;
        fx_pos.z = *(float *)(matrix + MATRIX_POS_Z);
        func_0x00455e60_r(&fx_pos);
        fx_slot_release(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_A_FX_BASE)));
        *(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_A_FX_BASE)) = SLOT_EMPTY;
      }
    }
    slot_offset = slot_offset + 4;
  } while (slot_offset < NT1_A_BYTES);
  /* table B: 12 slots, same logic; y offset g_course_city_60d8d4 */
  slot_offset = 0;
  do {
    if (SLOT_EMPTY < *(int *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_B_FX_BASE))) {
      user_data = rwp_object_get_user_data(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_B_OBJ_BASE)));
      if ((user_data & 8) != 0) {
        matrix = rwp_object_get_matrix(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_B_OBJ_BASE)));
        fx_pos.x = *(float *)(matrix + MATRIX_POS_X);
        fx_pos.y = *(float *)(matrix + MATRIX_POS_Y) - g_course_city_60d8d4;
        fx_pos.z = *(float *)(matrix + MATRIX_POS_Z);
        func_0x00455e60_r(&fx_pos);
        fx_slot_release(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_B_FX_BASE)));
        *(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_B_FX_BASE)) = SLOT_EMPTY;
      }
    }
    slot_offset = slot_offset + 4;
  } while (slot_offset < NT1_B_BYTES);
  /* table C: 3 slots, same logic; y offset g_course_city_60d8e0 */
  slot_offset = 0;
  do {
    if (SLOT_EMPTY < *(int *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_C_FX_BASE))) {
      user_data = rwp_object_get_user_data(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_C_OBJ_BASE)));
      if ((user_data & 8) != 0) {
        matrix = rwp_object_get_matrix(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_C_OBJ_BASE)));
        fx_pos.x = *(float *)(matrix + MATRIX_POS_X);
        fx_pos.y = *(float *)(matrix + MATRIX_POS_Y) - g_course_city_60d8e0;
        fx_pos.z = *(float *)(matrix + MATRIX_POS_Z);
        func_0x00455e60_r(&fx_pos);
        fx_slot_release(*(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_C_FX_BASE)));
        *(undefined4 *)(slot_offset + ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_C_FX_BASE)) = SLOT_EMPTY;
      }
    }
    slot_offset = slot_offset + 4;
  } while (slot_offset < NT1_C_BYTES);
  /*
   * table D: 8 fx slots (NT1_D_FX_BASE..NT1_D_FX_END) paired with object
   * handles from NT1_D_OBJ_BASE; the y offset is taken per slot from the
   * spline reference table, 3 floats apart.
   */
  object_handle_ptr = (undefined4 *)((char *)&g_course_nt1_6a1f2c + NT1_D_OBJ_BASE);
  fx_slot_ptr = (int *)((char *)&g_course_nt1_6a1f2c + NT1_D_FX_BASE);
  do {
    ref_height_ptr = (float *)((char *)&g_course_city_60d8e0 + SPLINE_REF_OFFSET);
    do {
      if (SLOT_EMPTY < *fx_slot_ptr) {
        user_data = rwp_object_get_user_data(*object_handle_ptr);
        if ((user_data & 8) != 0) {
          slot_offset = rwp_object_get_matrix(*object_handle_ptr);
          fx_pos.x = *(float *)(slot_offset + MATRIX_POS_X);
          fx_pos.y = *(float *)(slot_offset + MATRIX_POS_Y) - *ref_height_ptr;
          fx_pos.z = *(float *)(slot_offset + MATRIX_POS_Z);
          func_0x00455e60_r(&fx_pos);
          fx_slot_release(*fx_slot_ptr);
          *fx_slot_ptr = SLOT_EMPTY;
        }
      }
      ref_height_ptr = ref_height_ptr + 3;
      fx_slot_ptr = fx_slot_ptr + 1;
    } while ((LONG_PTR)ref_height_ptr < ((LONG_PTR)&g_course_spline_60d8fc + SPLINE_REF_END));
    object_handle_ptr = object_handle_ptr + 1;
  } while ((LONG_PTR)fx_slot_ptr < ((LONG_PTR)&g_course_nt1_6a1f2c + NT1_D_FX_END));
  return;
}


