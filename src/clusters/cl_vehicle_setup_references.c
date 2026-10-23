// cluster-entries: vehicle_setup_references
/* Installed cluster vehicle_setup_references (tools/cluster_install.py from output/cluster_pilot_zx/vehicle_setup_references.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00425790*/ extern undefined __cdecl vehicle_skid_marks_collect_triangles(int p0, RwMatrix *p1, undefined4 p2);
/*0042a400*/ extern int course_get_collision_bsp();
/*0042a430*/ extern int course_get_ambient_light();
/*0042a440*/ extern int course_get_directional_light();
/*0042a450*/ extern int course_is_outdoor();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005dd330*/ extern float float_1_0;
/*005ddb38*/ extern float k_f_25;
/*005ddc28*/ extern float k_f_0;
/*005de418*/ extern float k_f_3_0en4;
/*00659d0c*/ extern undefined4 g_game_659d0c;
/*00659d60*/ extern undefined4 g_game_659d60;
/*00659d70*/ extern undefined4 g_particles_659d70;
/*00659e00*/ extern undefined g_vehicle_setup_659e00;
/*00659e30*/ extern undefined g_vehicle_setup_659e30;
/*00659e34*/ extern undefined g_vehicle_setup_659e34;
/*00659e38*/ extern undefined g_vehicle_setup_659e38;
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];







/* Per-vehicle setup constants. Values from the code; the names are inferred (see comments). */
#define VEHICLE_SETUP_STRIDE (0x40) /* bytes per vehicle in g_vehicle_setup_659e00 (16 floats) */
#define RWMATRIX_FLOAT_COUNT (0x10) /* floats copied from the transform into the setup table */
#define VEHICLE_PART_LAST (0x7f) /* last part index scanned (inferred) */

/* vehicle_setup_references */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Per-frame update for one vehicle slot, given the slot's new transform.
 *   index_or_morph:    on entry the slot index (int bits); on exit the proximity factor, clamped to 0..1
 *                      and passed to vehicle_marker_morph_update
 *   wheel_spin_arg:    passed to vehicle_wheels_spin_update
 *   vehicle_transform: the vehicle's new transform; set on the frame and copied into g_vehicle_setup_659e00
 * Stores the course directional / ambient light, refreshes the world sphere centre from the part 0x13 LTM,
 * advances the envmap angle, runs the per-vehicle sub-updates, and sets the render flag of every part.
 * (was: "stores the world / course pointers used by the vehicle records (inferred)") */

static int vehicle_ground_probe_update_r(void * p0);
extern void vehicle_ground_probe_update(void);
static int vehicle_marker_icon_select_r(void * p0);
extern void vehicle_marker_icon_select(void);
static int vehicle_marker_morph_update_r(void * p0, float p1);
extern void vehicle_marker_morph_update(void);
static int vehicle_marker_steering_wheel_update_r(void * p0);
extern void vehicle_marker_steering_wheel_update(void);
static int vehicle_register_trigger_sphere_r(void * p0);
extern void vehicle_register_trigger_sphere(void);
static int vehicle_set_scale_for_course_r(void * p0);
extern void vehicle_set_scale_for_course(void);
static int vehicle_wheels_spin_update_r(void * p0, int p1);
extern void vehicle_wheels_spin_update(void);
void vehicle_setup_references(float index_or_morph,int wheel_spin_arg,RwMatrix *vehicle_transform)

{
  int vehicle_state;
  byte masked_flag;
  uint masked_flags;
  float delta_z;
  float delta_y;
  RwMatrix *part_ltm;
  int setup_offset;
  int part_index;
  undefined4 collision_bsp;
  uint flag_word;
  int vehicle_index;
  VehicleRecord *vehicle_record;
  RwReal *setup_dst;
  bool render_flag;
  struct {
    RwV3d local_18;
    float local_c [2];
    float local_4;
  } frame_obj;
  float angle_or_delta_x;
  
  vehicle_index = *(int *)&index_or_morph;
  g_particles_659d70 = course_get_directional_light();
  g_game_659d0c = course_get_ambient_light();
  vehicle_record = g_vehicle_table + *(int *)&index_or_morph;
  RwFrameTransform(*(RwFrame **)(g_vehicle_table[*(int *)&index_or_morph].frame + 4),vehicle_transform,rwCOMBINEREPLACE);
  part_ltm = RwFrameGetLTM(*(RwFrame **)(g_vehicle_table[*(int *)&index_or_morph].part_frames[0x13] + 4));
  g_vehicle_table[*(int *)&index_or_morph].world_sphere[0] = (part_ltm->pos).x;
  g_vehicle_table[*(int *)&index_or_morph].world_sphere[1] = (part_ltm->pos).y;
  g_vehicle_table[*(int *)&index_or_morph].world_sphere[2] = (part_ltm->pos).z;
  /* Envmap (inferred): the envmap angle advances with the car velocity; the envmap frame is
     rotated about the frame's right vector. setup_offset is reused here as a frame pointer. */
  if (g_vehicle_table[*(int *)&index_or_morph].envmap_frame == (&g_game_659d60)[*(int *)&index_or_morph]) {
    car_get_velocity_any_mode(frame_obj.local_c,*(int *)&index_or_morph);
    setup_offset = (int)(g_vehicle_table[*(int *)&index_or_morph].frame);
    angle_or_delta_x = g_vehicle_table[*(int *)&index_or_morph].envmap_angle - (frame_obj.local_4 + frame_obj.local_c[0]) * k_f_3_0en4;
    g_vehicle_table[*(int *)&index_or_morph].envmap_angle = angle_or_delta_x;
    setup_offset = *(int *)(setup_offset + 4);
    frame_obj.local_18.x = *(RwReal *)(setup_offset + 0x10);
    frame_obj.local_18.y = *(RwReal *)(setup_offset + 0x14);
    frame_obj.local_18.z = *(RwReal *)(setup_offset + 0x18);
    RwFrameRotate((RwFrame *)g_vehicle_table[*(int *)&index_or_morph].envmap_frame,&frame_obj.local_18,angle_or_delta_x,
                  rwCOMBINEREPLACE);
  }
  /* Proximity: squared distance from the new position to the slot's setup point (x, y, z in
     g_vehicle_setup_659e30/34/38), scaled by k_f_25 and clamped to 0..1 into index_or_morph. */
  setup_offset = *(int *)&index_or_morph * VEHICLE_SETUP_STRIDE;
  angle_or_delta_x = (vehicle_transform->pos).x - *(float *)((char *)&g_vehicle_setup_659e30 + setup_offset);
  delta_y = (vehicle_transform->pos).y - *(float *)((char *)&g_vehicle_setup_659e34 + setup_offset);
  delta_z = (vehicle_transform->pos).z - *(float *)((char *)&g_vehicle_setup_659e38 + setup_offset);
  index_or_morph = (delta_z * delta_z + angle_or_delta_x * angle_or_delta_x + delta_y * delta_y) * k_f_25;
  if ((float_1_0 <= index_or_morph) || (k_f_0 <= index_or_morph)) {
    if (float_1_0 <= index_or_morph) {
      index_or_morph = 1.0;
    }
  }
  else {
    index_or_morph = 0.0;
  }
  /* Per-vehicle sub-updates, each takes vehicle_record. */
  vehicle_wheels_spin_update_r(vehicle_record,wheel_spin_arg);
  vehicle_marker_steering_wheel_update_r(vehicle_record);
  vehicle_ground_probe_update_r(vehicle_record);
  vehicle_register_trigger_sphere_r(vehicle_record);
  vehicle_marker_morph_update_r(vehicle_record,index_or_morph);
  vehicle_set_scale_for_course_r(vehicle_record);
  vehicle_marker_icon_select_r(vehicle_record);
  /* Skid marks: indoor courses only, and only while VEHICLE_FLAG_FRONT is clear (inferred). */
  part_index = course_is_outdoor();
  if ((part_index == 0) && ((g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT) == 0)) {
    collision_bsp = course_get_collision_bsp();
    vehicle_skid_marks_collect_triangles
              (vehicle_index,(RwMatrix *)(*(char **)(g_vehicle_table[vehicle_index].frame + 4) + 0x10),collision_bsp
              );
  }
  /* Part loop: for each part slot (part_index) decide whether its atomic is rendered. The switch
     key is the part id; field_298 is the vehicle state (0/1/2) and the flag bits pick the variant.
     Parts without a frame (part_frames == 0) are skipped. VEHICLE_FLAG_400 forces a part hidden. */
  part_index = 0;
  do {
    render_flag = false;
    if (vehicle_record->part_frames[part_index] == 0) goto LAB_00423aad;
    switch(part_index) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 8:
    case 9:
    case 0x10:
    case 0x12:
    case 0x14:
      render_flag = g_vehicle_table[vehicle_index].field_298 == 0;
      goto switchD_004239c0_caseD_e;
    case 4:
    case 6:
      if ((g_vehicle_table[vehicle_index].field_298 != 0) || ((g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT) != 0))
      goto LAB_00423a10;
      goto LAB_00423a83;
    case 5:
    case 7:
      if (g_vehicle_table[vehicle_index].field_298 != 0) goto LAB_00423a10;
      render_flag = (g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT) == 0;
      goto LAB_00423a81;
    case 10:
    case 0xc:
      if (g_vehicle_table[vehicle_index].field_298 == 0) {
        masked_flag = (byte)g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_BACK;
joined_r0x00423a49:
        if (masked_flag == 0) goto LAB_00423a83;
      }
      goto LAB_00423a10;
    case 0xb:
    case 0xd:
      vehicle_state = g_vehicle_table[vehicle_index].field_298;
      goto LAB_00423a79;
    default:
      goto switchD_004239c0_caseD_e;
    case 0x11:
      flag_word = g_vehicle_table[vehicle_index].flags;
      masked_flags = flag_word & 0x800;
      goto LAB_00423b6a;
    case 0x15:
      if ((g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_40) != 0) {
        flag_word = g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT;
        goto LAB_00423b6e;
      }
      goto LAB_00423b06;
    case 0x17:
      render_flag = g_vehicle_table[vehicle_index].field_298 == 0;
      goto LAB_00423b25;
    case 0x18:
      flag_word = g_vehicle_table[vehicle_index].flags;
      if (-1 < (char)flag_word) goto LAB_00423b06;
      goto LAB_00423b6c;
    case 0x21:
    case 0x22:
    case 0x29:
    case 0x2a:
    case 0x4b:
      render_flag = g_vehicle_table[vehicle_index].field_298 == 1;
      goto switchD_004239c0_caseD_e;
    case 0x25:
    case 0x27:
      if (g_vehicle_table[vehicle_index].field_298 == 1) {
        masked_flag = (byte)g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT;
        goto joined_r0x00423a49;
      }
      goto LAB_00423a10;
    case 0x26:
    case 0x28:
      if (g_vehicle_table[vehicle_index].field_298 == 1) {
        render_flag = (g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT) == 0;
        goto LAB_00423a81;
      }
      goto LAB_00423a10;
    case 0x2b:
    case 0x2d:
      if (g_vehicle_table[vehicle_index].field_298 == 1) {
        masked_flag = (byte)g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_BACK;
        goto joined_r0x00423a49;
      }
      goto LAB_00423a10;
    case 0x2c:
    case 0x2e:
      vehicle_state = g_vehicle_table[vehicle_index].field_298 + -1;
LAB_00423a79:
      if (vehicle_state == 0) {
        render_flag = (g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_BACK) == 0;
LAB_00423a81:
        if (!render_flag) {
LAB_00423a83:
          render_flag = true;
          goto switchD_004239c0_caseD_e;
        }
      }
LAB_00423a10:
      render_flag = false;
switchD_004239c0_caseD_e:
      if ((part_index != 0x17) && (part_index != 0x32)) break;
      goto LAB_00423aa3;
    case 0x31:
    case 0x47:
      render_flag = g_vehicle_table[vehicle_index].field_298 == 1;
      break;
    case 0x32:
      render_flag = g_vehicle_table[vehicle_index].field_298 == 1;
LAB_00423b25:
      if ((!render_flag) || (render_flag = true, (g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_400) == 0)) {
        render_flag = false;
      }
      goto LAB_00423aa3;
    case 0x46:
      render_flag = g_vehicle_table[vehicle_index].field_298 == 0;
      break;
    case 100:
      if ((g_vehicle_table[vehicle_index].field_298 != 2) || ((g_vehicle_table[vehicle_index].flags & (VEHICLE_FLAG_FRONT | VEHICLE_FLAG_BACK)) != 0))
      goto LAB_00423b06;
      render_flag = true;
      break;
    case 0x65:
      if (g_vehicle_table[vehicle_index].field_298 != 2) goto LAB_00423b06;
      flag_word = g_vehicle_table[vehicle_index].flags;
      masked_flags = flag_word & VEHICLE_FLAG_FRONT;
LAB_00423b6a:
      if (masked_flags != 0) {
LAB_00423b6c:
        flag_word = flag_word & VEHICLE_FLAG_BACK;
LAB_00423b6e:
        if (flag_word == 0) {
          render_flag = true;
          break;
        }
      }
LAB_00423b06:
      render_flag = false;
      break;
    case 0x66:
      if (((g_vehicle_table[vehicle_index].field_298 != 2) || ((g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT) != 0)
          ) || ((g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_BACK) == 0)) goto LAB_00423b06;
      render_flag = true;
      break;
    case 0x67:
      if (((g_vehicle_table[vehicle_index].field_298 != 2) || ((g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_FRONT) == 0)
          ) || (render_flag = true, (g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_BACK) == 0)) goto LAB_00423b06;
    }
    render_flag = (bool)(render_flag & ~-((g_vehicle_table[vehicle_index].flags & VEHICLE_FLAG_400) != 0));
LAB_00423aa3:
    atomic_set_render_flag((RpAtomic *)vehicle_record->part_frames[part_index],render_flag);
LAB_00423aad:
    part_index = part_index + 1;
    /* Past the last part: copy the transform's floats into the setup table, then return. */
    if (VEHICLE_PART_LAST < part_index) {
      setup_dst = (RwReal *)((char *)&g_vehicle_setup_659e00 + setup_offset);
      for (vehicle_index = RWMATRIX_FLOAT_COUNT;vehicle_index != 0; vehicle_index = vehicle_index + -1) {
        *setup_dst = (vehicle_transform->right).x;
        vehicle_transform = (RwMatrix *)&(vehicle_transform->right).y;
        setup_dst = setup_dst + 1;
      }
      return;
    }
  } while( true );
}

/* vehicle_ground_probe_update_r: C -> original vehicle_ground_probe_update (registers esi) */
__declspec(naked) int vehicle_ground_probe_update_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call vehicle_ground_probe_update
        pop esi
        ret
    }
}

/* vehicle_marker_icon_select_r: C -> original vehicle_marker_icon_select (registers edi) */
__declspec(naked) int vehicle_marker_icon_select_r(void * p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call vehicle_marker_icon_select
        pop edi
        ret
    }
}

/* vehicle_marker_morph_update_r: C -> original vehicle_marker_morph_update (registers eax,s0) */
__declspec(naked) int vehicle_marker_morph_update_r(void * p0, float p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov eax, dword ptr [esp + 8]
        call vehicle_marker_morph_update
        add esp, 4
        ret
    }
}

/* vehicle_marker_steering_wheel_update_r: C -> original vehicle_marker_steering_wheel_update (registers edi) */
__declspec(naked) int vehicle_marker_steering_wheel_update_r(void * p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call vehicle_marker_steering_wheel_update
        pop edi
        ret
    }
}

/* vehicle_register_trigger_sphere_r: C -> original vehicle_register_trigger_sphere (registers eax) */
__declspec(naked) int vehicle_register_trigger_sphere_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call vehicle_register_trigger_sphere
        ret
    }
}

/* vehicle_set_scale_for_course_r: C -> original vehicle_set_scale_for_course (registers esi) */
__declspec(naked) int vehicle_set_scale_for_course_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call vehicle_set_scale_for_course
        pop esi
        ret
    }
}

/* vehicle_wheels_spin_update_r: C -> original vehicle_wheels_spin_update (registers edi,s0) */
__declspec(naked) int vehicle_wheels_spin_update_r(void * p0, int p1)
{
    __asm {
        push edi
        push dword ptr [esp + 12]
        mov edi, dword ptr [esp + 12]
        call vehicle_wheels_spin_update
        add esp, 4
        pop edi
        ret
    }
}
