// cluster-entries: course_rouabout_cb0
/* Installed cluster course_rouabout_cb0 (tools/cluster_install.py from output/cluster_pilot_j1/course_rouabout_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004068f0*/ extern int __cdecl clump_for_all_atomics_cb_4068a0(struct RpClump *p0, void *p1);
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00457430*/ extern int course_spline_objects_create();
/*00457620*/ extern int load_shadow_models();
/*00475730*/ extern int get_viewport_camera();
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*0049c1a0*/ extern int particles_slot_set_vectors();
/*0049c280*/ extern int fx_snow_ptank_create();
/*004a1860*/ extern int game_set_option_flags();
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004a5e50*/ extern int object_set_colour();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c5150*/ extern int FUN_004c5150(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*005dd340*/ extern float k_f_0_5;
/*005ddafc*/ extern float k_f_0_05;
/*005e64a4*/ extern float k_f_0;
/*006a2164*/ extern undefined4 g_course_rouabout_6a2164;
/*006a2168*/ extern undefined4 g_course_6a2168;
/*006a216c*/ extern undefined4 g_course_6a216c;
/*006a2170*/ extern undefined4 g_course_rouabout_6a2170;
/*006a2174*/ extern undefined4 g_course_rouabout_6a2174;
/*006a2178*/ extern undefined4 g_course_rouabout_6a2178;
/*006a217c*/ extern undefined4 g_course_rouabout_6a217c;
/*006a2180*/ extern undefined4 g_course_rouabout_6a2180;
/*006a2184*/ extern undefined4 g_course_rouabout_6a2184;
/*006a2188*/ extern undefined4 g_course_6a2188;
/*006a218c*/ extern undefined4 g_course_rouabout_6a218c;
/*006a2190*/ extern undefined4 g_course_rouabout_6a2190;
/*006a2194*/ extern undefined4 g_course_rouabout_6a2194;
/*006a2198*/ extern undefined4 g_course_rouabout_6a2198;
/*006a219c*/ extern undefined4 g_course_rouabout_6a219c;
/*006a21a0*/ extern undefined4 g_course_rouabout_6a21a0;
/*006a21a4*/ extern undefined4 g_course_rouabout_6a21a4;
/*006a21a8*/ extern undefined4 g_course_rouabout_6a21a8[0x1000];
/*006a21ac*/ extern undefined4 g_course_rouabout_6a21ac;
/*006a21b0*/ extern undefined4 g_course_rouabout_6a21b0;
/*006a21b4*/ extern undefined4 g_course_rouabout_6a21b4;
/*006a21b8*/ extern undefined4 g_course_rouabout_6a21b8;
/*006a21bc*/ extern undefined4 g_course_rouabout_6a21bc;
/*006a21c0*/ extern undefined4 g_course_rouabout_6a21c0;
/*006a21c4*/ extern undefined4 g_course_rouabout_6a21c4;
/*006a21c8*/ extern undefined4 g_course_rouabout_6a21c8[0x1000];
/*006a23cc*/ extern float g_course_rouabout_6a23cc;
/*006a23d0*/ extern undefined4 g_course_rouabout_6a23d0;
/*006a23d4*/ extern undefined4 g_course_6a23d4;
/*006a23d8*/ extern undefined4 g_course_rouabout_6a23d8;
/*006a23dc*/ extern undefined4 g_course_rouabout_6a23dc;
/*006a23e0*/ extern undefined g_course_6a23e0;
/*006a23e4*/ extern undefined g_course_rouabout_6a23e4;
/*006a23e8*/ extern undefined g_course_rouabout_6a23e8;
/*006a23ec*/ extern undefined g_course_6a23ec;
/*006a23f0*/ extern undefined g_course_rouabout_6a23f0;
/*006a23f4*/ extern undefined g_course_rouabout_6a23f4;
/*006a2614*/ extern float g_course_6a2614;
/*006a2618*/ extern float g_course_rouabout_6a2618;
/*006a261c*/ extern float g_course_rouabout_6a261c;
/*006a2620*/ extern float g_course_rouabout_6a2620;
/*006a2624*/ extern float g_course_rouabout_6a2624;
/*006a2628*/ extern float g_course_rouabout_6a2628;
/*006a262c*/ extern undefined4 g_course_rouabout_6a262c;
/*006a2630*/ extern undefined4 g_course_rouabout_6a2630;
/*006a2634*/ extern undefined4 g_course_6a2634;
/* course_rouabout_cb0 */


/* Roundabout course setup (inferred): creates the two animated object sets, sets far clip 150 when the two fog values at +0xc9f8/+0xc9fc match, creates spline objects and smoke emitters per +0xc30 record, loads shadow models, then (if g_course_6a2634) caches 8 object matrices and builds 8x2 collision triangles with their bounding box in g_course_6a2614. */
void course_rouabout_cb0(int course)

{
  float *pfVar1;
  float fVar2;
  undefined4 uVar3;
  undefined4 texture_dict;
  RwCamera *camera;
  int camera_2;
  undefined4 *src_matrix;
  int vert_i;
  int comp_i;
  undefined4 *cursor;
  undefined4 *object_ptr;
  float *tri_src;
  undefined4 *dst;
  float *pfVar11;
  float *tri;
  float *bounds;
  RwReal farClip;
  struct {
    int tri_group;
    undefined1 key_b0;
    char key_b1;
    undefined1 key_b2;
    undefined1 key_b3;
    int tri_i;
    undefined4 emit_x;
    float emit_y;
    undefined4 emit_z;
    int tri_count;
    undefined4 vec_a_x;
    undefined4 vec_a_y;
    undefined4 vec_a_z;
    undefined4 vec_b_x;
    undefined4 vec_b_y;
    undefined4 vec_b_z;
    float tris [48];
  } scan;
  
  bounds = &g_course_6a2614;
  if (*(int *)(course + 0xc154) != 0) {
    texture_dict = (undefined4)(clump_first_atomic((RpClump *)(*(int *)(course + 0xc154))));
    g_course_rouabout_6a262c =
         object_frames_create_with_matrices
                   (texture_dict,*(undefined4 *)(course + 0xc00c),course + 0x2400);
    object_set_colour(g_course_rouabout_6a262c,0xffffffff,0x3f333333,0x3f333333,0x3f333333,
                      0x3f800000);
  }
  if (*(int *)(course + 0xc15c) != 0) {
    texture_dict = (undefined4)(clump_first_atomic((RpClump *)(*(int *)(course + 0xc15c))));
    g_course_rouabout_6a2630 =
         object_frames_create_with_matrices
                   (texture_dict,*(undefined4 *)(course + 0xc010),course + 0x3000);
    object_set_colour(g_course_rouabout_6a2630,0xffffffff,0x3e4ccccd,0x3e4ccccd,0x3e4ccccd,
                      0x3f800000);
  }
  clump_for_all_atomics_cb_4068a0(*(RpClump **)(course + 0xc128),*(void **)(course + 0xc9d0));
  if (*(float *)(course + 0xc9f8) == *(float *)(course + 0xc9fc)) {
    farClip = 150.0;
    camera = (RwCamera *)get_viewport_camera(0);
    RwCameraSetFarClipPlane(camera,farClip);
    camera_2 = get_viewport_camera(0);
    *(undefined4 *)(camera_2 + 0x88) = 0x42480000;
    get_viewport_camera(0);
    game_set_option_flags(0x83,0x96,0xa3);
  }
  course_spline_objects_create(course);
  scan.emit_x = 0xc23af5c3;
  scan.emit_y = 0.747;
  scan.emit_z = 0xc28c245a;
  scan.vec_a_x = 0xbd23d70a;
  scan.vec_a_y = 0x3cf5c28f;
  scan.vec_a_z = 0xbd23d70a;
  scan.vec_b_x = 0x3d23d70a;
  scan.vec_b_y = 0x3d75c28f;
  scan.vec_b_z = 0x3d23d70a;
  texture_dict = find_texture_dict_b("smoke");
  fx_snow_ptank_create(0,texture_dict);
  camera_2 = 0;
  if (0 < *(int *)(course + 0xc004)) {
    cursor = (undefined4 *)(course + 0xc30);
    do {
      scan.emit_x = *cursor;
      scan.emit_z = cursor[2];
      scan.emit_y = *(float *)&cursor[1] - k_f_0_5;
      particles_slot_set_vectors(0,&scan.emit_x,0x3f19999a,&scan.vec_a_x,&scan.vec_b_x);
      camera_2 = camera_2 + 1;
      cursor = cursor + 0x10;
    } while (camera_2 < *(int *)(course + 0xc004));
  }
  load_shadow_models(course);
  g_course_6a2188 = 0xffff33ff;
  g_course_rouabout_6a218c = 0xffff34ff;
  g_course_rouabout_6a2190 = 0xffff35ff;
  g_course_rouabout_6a2194 = 0xffff36ff;
  g_course_rouabout_6a2198 = 0xffff37ff;
  g_course_rouabout_6a219c = 0xffff38ff;
  g_course_rouabout_6a21a0 = 0xffff39ff;
  g_course_rouabout_6a21a4 = 0xffff3aff;
  if (g_course_6a2634 != 0) {
    g_course_rouabout_6a21a8[0] = rwp_object_find_index_by_id(8000);
    g_course_rouabout_6a21ac = rwp_object_find_index_by_id(0x1f41);
    g_course_rouabout_6a21b0 = rwp_object_find_index_by_id(0x1f42);
    g_course_rouabout_6a21b4 = rwp_object_find_index_by_id(0x1f43);
    g_course_rouabout_6a21b8 = rwp_object_find_index_by_id(0x1f44);
    g_course_rouabout_6a21bc = rwp_object_find_index_by_id(0x1f45);
    g_course_rouabout_6a21c0 = rwp_object_find_index_by_id(0x1f46);
    g_course_rouabout_6a21c4 = rwp_object_find_index_by_id(0x1f47);
    g_course_6a2168 = 0;
    g_course_6a216c = 0;
    g_course_rouabout_6a2170 = 0;
    g_course_rouabout_6a2174 = 0;
    g_course_rouabout_6a2178 = 0;
    g_course_rouabout_6a217c = 0;
    g_course_rouabout_6a2180 = 0;
    g_course_rouabout_6a2184 = 0;
    g_course_rouabout_6a2164 = 0;
    g_course_rouabout_6a23cc = 0;
    g_course_rouabout_6a23d0 = 0xffffffff;
    object_ptr = g_course_rouabout_6a21a8;
    cursor = g_course_rouabout_6a21c8;
    do {
      src_matrix = (undefined4 *)rwp_object_get_matrix(*object_ptr);
      object_ptr = object_ptr + 1;
      dst = cursor;
      for (camera_2 = 0x10; camera_2 != 0; camera_2 = camera_2 + -1) {
        *dst = *src_matrix;
        src_matrix = src_matrix + 1;
        dst = dst + 1;
      }
      cursor = cursor + 0x10;
    } while ((int)object_ptr < 0x6a21c8);
    scan.tri_group = 0;
    do {
      scan.key_b1 = (char)scan.tri_group + '3';
      scan.key_b0 = 0xff;
      scan.key_b2 = 0xff;
      scan.key_b3 = 0xff;
      scan.tri_count = FUN_004c5150(*(undefined4 *)(course + 0xc090),(undefined4)&scan.key_b0,(undefined4)scan.tris,4);
      scan.tri_i = 0;
      if (0 < scan.tri_count) {
        do {
          camera_2 = scan.tri_i + scan.tri_group * 2;
          tri = (float *)&g_course_6a23d4 + camera_2 * 9;
          tri_src = scan.tris + scan.tri_i * 0xc;
          for (comp_i = 0; comp_i < 9; comp_i++) {
            *(int *)&tri[comp_i] = *(int *)&tri_src[comp_i];
          }
          if (!((tri[6] - tri[3]) * (tri[2] - tri[5]) - (tri[8] - tri[5]) * (tri[0] - tri[3]) > k_f_0)) {      /* a NaN area swaps too (test ah,0x41; je) */
            for (comp_i = 0; comp_i < 3; comp_i++) {
              int tmp = *(int *)&tri[comp_i];
              *(int *)&tri[comp_i] = *(int *)&tri[6 + comp_i];
              *(int *)&tri[6 + comp_i] = tmp;
            }
          }
          if ((scan.tri_group == 0) && (scan.tri_i == 0)) {
            for (comp_i = 0; comp_i < 3; comp_i++) {
              *(int *)&bounds[comp_i] = *(int *)&tri[comp_i];
              *(int *)&bounds[3 + comp_i] = *(int *)&tri[comp_i];
            }
          }
          for (vert_i = 0; vert_i < 3; vert_i++) {
            for (comp_i = 0; comp_i < 3; comp_i++) {
              if (bounds[comp_i] < tri[vert_i * 3 + comp_i]) bounds[comp_i] = tri[vert_i * 3 + comp_i];
              if (tri[vert_i * 3 + comp_i] < bounds[3 + comp_i]) bounds[3 + comp_i] = tri[vert_i * 3 + comp_i];
            }
          }
          scan.tri_i = scan.tri_i + 1;
        } while (scan.tri_i < scan.tri_count);
      }
      scan.tri_group = scan.tri_group + 1;
    } while (scan.tri_group < 8);
    g_course_rouabout_6a2618 = g_course_rouabout_6a2618 + k_f_0_05;
  }
  return;
}

