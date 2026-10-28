// cluster-entries: vehicle_skid_marks_collect_triangles
/* Installed cluster vehicle_skid_marks_collect_triangles (tools/cluster_install.py from output/cluster_pilot_j1/vehicle_skid_marks_collect_triangles.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481e80*/ extern int player_marker_get_colour();
/*00487ee0*/ extern undefined __cdecl triangle_normal_cross(struct RwV3d *p0, float *p1, float *p2, float *p3);
/*00487fd0*/ extern int plane_classify_point();
/*004c4be0*/ extern int FUN_004c4be0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd340*/ extern float k_f_0_5;
/*005dda24*/ extern float k_f_0_75;
/*005de398*/ extern float k_f_0_35;
/*0065c8f8*/ extern undefined g_game_65c8f8;
/*0065c920*/ extern undefined g_game_65c920;
/*0065d7f8*/ extern undefined g_vehicle_setup_65d7f8;
/*0065d7fc*/ extern undefined g_vehicle_setup_65d7fc;
/*0065d800*/ extern undefined g_vehicle_setup_65d800;
/*0065d804*/ extern undefined g_vehicle_setup_65d804;
/*0065d80c*/ extern undefined g_game_65d80c;
/*0065d810*/ extern undefined g_game_65d810;
/* vehicle_skid_marks_collect_triangles */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* "collects up to 0x40 course triangles near the car (excluding marker colours) into the 0xF1C byte
   skid record" */

void __cdecl vehicle_skid_marks_collect_triangles(int car_index,RwMatrix *car_matrix,undefined4 world)

{
  float at_z;
  float *out_tri;
  int tmp_i;
  int record_off;
  float *tri_ptr;
  RwMatrix *copy_src;
  int *surface_color_ptr;
  float *copy_src_f;
  RwReal *uv_cursor;
  RwMatrix *copy_dst;
  float *copy_dst_f;
  RwV3d *car_pos;
  struct {
    int tris_left;
    float plane_dist;
    RwV3d *car_pos_saved;
    RwV3d shift_vec;
    RwV3d scale_vec;
    RwV3d projected_pt;
    RwReal projected_v1_x;
    undefined1 pad_10a4[4];
    RwReal projected_v1_z;
    RwReal projected_v2_x;
    undefined1 pad_1098[4];
    RwReal projected_v2_z;
    RwV3d tri_normal;
    undefined1 pad_1084[4];
    RwMatrix to_uv_matrix;
    RwMatrix inv_car_matrix;
    undefined1 tri_query_hdr [16];
    float tri_records [1019];
    undefined4 uStack_4;
  } frame;
  
  frame.uStack_4 = 0x42579a;
  /* per car skid record (0xf1c bytes): triangles of 60 bytes; query the world for triangles around the car and keep the usable ones */
  record_off = car_index * 0xf1c;
  frame.scale_vec.y = 1.0;
  frame.shift_vec.y = 0.0;
  frame.scale_vec.x = 1.0;
  frame.scale_vec.z = 0.75;
  frame.shift_vec.x = 0.5;
  frame.shift_vec.z = -0.5;
  RwMatrixInvert(&frame.inv_car_matrix,car_matrix);
  copy_src = &frame.inv_car_matrix;
  copy_dst = &frame.to_uv_matrix;
  for (tmp_i = 0x10; tmp_i != 0; tmp_i = tmp_i + -1) {
    (copy_dst->right).x = (copy_src->right).x;
    copy_src = (RwMatrix *)&(copy_src->right).y;
    copy_dst = (RwMatrix *)&(copy_dst->right).y;
  }
  RwMatrixScale(&frame.to_uv_matrix,&frame.scale_vec,rwCOMBINEPOSTCONCAT);
  RwMatrixTranslate(&frame.to_uv_matrix,&frame.shift_vec,rwCOMBINEPOSTCONCAT);
  tri_ptr = (float *)((char *)&g_vehicle_setup_65d7f8 + record_off);
  car_pos = &car_matrix->pos;
  *tri_ptr = car_pos->x;
  *(RwReal *)((char *)&g_vehicle_setup_65d7fc + record_off) = (car_matrix->pos).y;
  *(RwReal *)((char *)&g_vehicle_setup_65d800 + record_off) = (car_matrix->pos).z;
  *(undefined4 *)((char *)&g_vehicle_setup_65d804 + record_off) = 0x3fa00000;
  *tri_ptr = (car_matrix->at).x + *tri_ptr;
  *(float *)((char *)&g_vehicle_setup_65d7fc + record_off) =
       (car_matrix->at).y + *(float *)((char *)&g_vehicle_setup_65d7fc + record_off);
  at_z = (car_matrix->at).z;
  *(undefined4 *)((char *)&g_game_65d80c + record_off) = 0;
  *(float *)((char *)&g_vehicle_setup_65d800 + record_off) = at_z + *(float *)((char *)&g_vehicle_setup_65d800 + record_off);
  frame.car_pos_saved = car_pos;
  frame.tris_left = FUN_004c4be0(world,(undefined4)tri_ptr,(undefined4)frame.tri_query_hdr,0x40);
  if (frame.tris_left != 0) {
    if (0 < frame.tris_left) {
      /* skip surfaces with the 3 special colours and the 5 player marker colours, steep faces (< 0.75) and triangles by plane_classify_point distance (limits 0.5 / 0.35, inferred) */
      tri_ptr = frame.tri_records;
      do {
        tmp_i = *(int *)(*(int *)(*(int *)&tri_ptr[0xb] + 0x10) +
                        ((uint)*(ushort *)(*(int *)(*(int *)&tri_ptr[9] + 4) + 6 + *(int *)&tri_ptr[-1] * 8) +
                        (uint)*(ushort *)(*(int *)&tri_ptr[9] + 0x80)) * 4);
        surface_color_ptr = (int *)(tmp_i + 4);
        tmp_i = *(int *)(tmp_i + 4);
        if (((tmp_i != -0xfefeff) && (tmp_i != -0xffff01)) && (tmp_i != -0xff80)) {
          tmp_i = player_marker_get_colour(0);
          if (*surface_color_ptr != tmp_i) {
            tmp_i = player_marker_get_colour(1);
            if (*surface_color_ptr != tmp_i) {
              tmp_i = player_marker_get_colour(2);
              if (*surface_color_ptr != tmp_i) {
                tmp_i = player_marker_get_colour(3);
                if (*surface_color_ptr != tmp_i) {
                  tmp_i = player_marker_get_colour(4);
                  if ((*surface_color_ptr != tmp_i) && !(tri_ptr[-3] < k_f_0_75)) {
                    triangle_normal_cross(&frame.tri_normal,tri_ptr,tri_ptr + 3,tri_ptr + 6);
                    tmp_i = plane_classify_point(&frame.tri_normal,car_pos,&frame.plane_dist);
                    if (tmp_i == 0) {
                      if (frame.plane_dist > k_f_0_5) goto LAB_00425a9c;
                    }
                    else if (tmp_i == 2) {
                      if (frame.plane_dist > k_f_0_35) goto LAB_00425a9c;
                    }
                    out_tri = (float *)(&g_game_65c8f8 +
                                      *(int *)((char *)&g_game_65d80c + record_off) * 0x3c + record_off);
                    *(int *)((char *)&g_game_65d80c + record_off) = *(int *)((char *)&g_game_65d80c + record_off) + 1;
                    copy_src_f = tri_ptr;
                    copy_dst_f = out_tri;
                    for (tmp_i = 9; tmp_i != 0; tmp_i = tmp_i + -1) {
                      *copy_dst_f = *copy_src_f;
                      copy_src_f = copy_src_f + 1;
                      copy_dst_f = copy_dst_f + 1;
                    }
                    out_tri[1] = out_tri[1] + *(float *)((char *)&g_game_65d810 + record_off);
                    out_tri[4] = out_tri[4] + *(float *)((char *)&g_game_65d810 + record_off);
                    out_tri[7] = out_tri[7] + *(float *)((char *)&g_game_65d810 + record_off);
                    car_pos = frame.car_pos_saved;
                  }
                }
              }
            }
          }
        }
LAB_00425a9c:
        tri_ptr = tri_ptr + 0x10;
        frame.tris_left = frame.tris_left + -1;
      } while (frame.tris_left != 0);
    }
    /* transform the kept triangles into the car local texture space (to_uv_matrix) and store their uv */
    tmp_i = 0;
    if (0 < *(int *)((char *)&g_game_65d80c + record_off)) {
      uv_cursor = (RwReal *)((char *)&g_game_65c920 + record_off);
      do {
        RwV3dTransformPoints(&frame.projected_pt,(RwV3d *)(uv_cursor + -10),3,&frame.to_uv_matrix);
        uv_cursor[-1] = frame.projected_pt.x;
        *uv_cursor = frame.projected_pt.z;
        uv_cursor[1] = frame.projected_v1_x;
        uv_cursor[2] = frame.projected_v1_z;
        uv_cursor[3] = frame.projected_v2_x;
        uv_cursor[4] = frame.projected_v2_z;
        tmp_i = tmp_i + 1;
        uv_cursor = uv_cursor + 0xf;
      } while (tmp_i < *(int *)((char *)&g_game_65d80c + record_off));
    }
  }
  return;
}

