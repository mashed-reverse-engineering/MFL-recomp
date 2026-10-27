// cluster-entries: ai_polygons_log_count
/* Installed cluster ai_polygons_log_count (tools/cluster_install.py from output/cluster_pilot_j1/ai_polygons_log_count.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004884a0*/ extern undefined __cdecl course_path_segments_distance_and_heading(undefined4 p0);
/*004a8420*/ extern int debug_printf();
/*004c4620*/ extern void FUN_004c4620(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c4780*/ extern int FUN_004c4780(undefined4 p0, undefined4 p1);
/*004c5150*/ extern int FUN_004c5150(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005ddc28*/ extern float k_f_0;
static int triangle_normal_cross_array_r(float * reg_eax, RwV3d * reg_ecx, int param_3);
extern void triangle_normal_cross_array(void);
__declspec(naked) int triangle_normal_cross_array_r(float * reg_eax, RwV3d * reg_ecx, int param_3)
{
    __asm {
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        call triangle_normal_cross_array
        add esp, 4
        ret
    }
}

/* ai_polygons_log_count */

/* "'Number of AI polygons is %d' and builds the AI polygon records" */

void ai_polygons_log_count(CourseLap *lap,int param_2)

{
  int hit;
  RwReal *src_f;
  int cnt;
  int poly_index;
  RwReal *poly_rec;
  RwV3d *dst_v3;
  RwReal *dst_f;
  struct {
    undefined1 uStack_178;
    undefined1 uStack_177;
    undefined1 uStack_176;
    undefined1 uStack_175;
  } frame;
  RwV3d pts[5];      /* the 3 triangle corners, then two more points (0xc apart on the stack) */
  RwV3d out[4];      /* RwV3dTransformPoints result */
  RwMatrix RStack_108;
  int local_c8;
  RwReal aRStack_c4 [9];
  RwMatrix RStack_a0;
  RwReal aRStack_60 [24];     /* two 12 float records written by FUN_004c5150 (the second one is aRStack_60 + 12) */
  
  cnt = *(int *)(param_2 + 0x14);
  lap->ai_polygon_count = cnt;
  local_c8 = cnt;
  debug_printf("Number of AI polygons is %d\n",cnt);
  poly_index = 0;
  if (0 < cnt) {
    poly_rec = (RwReal *)&lap->field_0xc1d8;
    do {
      frame.uStack_177 = (undefined1)((uint)poly_index >> 8);
      frame.uStack_176 = 0;
      frame.uStack_175 = 0xff;
      frame.uStack_178 = (undefined1)poly_index;
      FUN_004c5150(param_2,(undefined4)&frame.uStack_178,aRStack_60,2);
      src_f = aRStack_60;
      dst_v3 = &pts[0];
      for (cnt = 9; cnt != 0; cnt = cnt + -1) {
        dst_v3->x = *src_f;
        src_f = src_f + 1;
        dst_v3 = (RwV3d *)&dst_v3->y;
      }
      src_f = aRStack_60 + 12;
      dst_f = aRStack_c4;
      for (cnt = 9; cnt != 0; cnt = cnt + -1) {
        *dst_f = *src_f;
        src_f = src_f + 1;
        dst_f = dst_f + 1;
      }
      cnt = 0;
      src_f = aRStack_c4;
      do {
        pts[4].x = *src_f;
        pts[4].y = src_f[1];
        pts[4].z = src_f[2];
        hit = FUN_004c4780((undefined4)&pts[4],(undefined4)&pts[0]);
        if (((hit == 0) && (hit = FUN_004c4780((undefined4)&pts[4],(undefined4)&pts[1]), hit == 0)) &&
           (hit = FUN_004c4780((undefined4)&pts[4],(undefined4)&pts[2]), hit == 0)) {
          pts[3].x = pts[4].x;
          pts[3].y = pts[4].y;
          pts[3].z = pts[4].z;
          break;
        }
        cnt = cnt + 1;
        src_f = src_f + 3;
      } while (cnt < 3);
      triangle_normal_cross_array_r((float *)&pts[0],(RwV3d *)(poly_rec + -6),1);
      FUN_004c4620((undefined4)(poly_rec + -3),(undefined4)&pts[0],4);
      RStack_108.pos.y = poly_rec[-2];
      RStack_108.pos.z = poly_rec[-1];
      RStack_108.pos.x = poly_rec[-3];
      RStack_108.at.y = poly_rec[-5];
      RStack_108.at.z = poly_rec[-4];
      RStack_108.right.x = RStack_108.at.z - RStack_108.at.y * k_f_0;
      RStack_108.at.x = poly_rec[-6];
      RStack_108.right.y = RStack_108.at.x * k_f_0 - RStack_108.at.z * k_f_0;
      RStack_108.right.z = RStack_108.at.y * k_f_0 - RStack_108.at.x;
      RwV3dNormalize(&RStack_108.right,&RStack_108.right);
      RStack_108.up.x = RStack_108.right.z * RStack_108.at.y - RStack_108.right.y * RStack_108.at.z;
      RStack_108.up.y = RStack_108.right.x * RStack_108.at.z - RStack_108.right.z * RStack_108.at.x;
      RStack_108.up.z = RStack_108.right.y * RStack_108.at.x - RStack_108.right.x * RStack_108.at.y;
      RwV3dNormalize(&RStack_108.up,&RStack_108.up);
      RwMatrixUpdate(&RStack_108);
      RwMatrixInvert(&RStack_a0,&RStack_108);
      RwV3dTransformPoints(out,&pts[0],4,&RStack_a0);
      if (k_f_0 <= out[0].x) {
LAB_00488a2c:
        if (k_f_0 < out[0].x) {
          if (out[0].y <= k_f_0) {
            if ((k_f_0 < out[0].x) && (out[0].y < k_f_0)) {
              src_f = poly_rec + 9;
              goto LAB_00488a8c;
            }
          }
          else {
            poly_rec[6] = pts[0].x;
            poly_rec[7] = pts[0].y;
            poly_rec[8] = pts[0].z;
          }
        }
      }
      else {
        src_f = poly_rec;
        if (k_f_0 <= out[0].y) {
          if ((k_f_0 <= out[0].x) || (out[0].y <= k_f_0)) goto LAB_00488a2c;
          poly_rec[3] = pts[0].x;
          poly_rec[4] = pts[0].y;
          poly_rec[5] = pts[0].z;
        }
        else {
LAB_00488a8c:
          *src_f = pts[0].x;
          src_f[1] = pts[0].y;
          src_f[2] = pts[0].z;
        }
      }
      if (k_f_0 <= out[1].x) {
LAB_00488b04:
        if (k_f_0 < out[1].x) {
          if (out[1].y <= k_f_0) {
            if ((k_f_0 < out[1].x) && (out[1].y < k_f_0)) {
              src_f = poly_rec + 9;
              goto LAB_00488b64;
            }
          }
          else {
            poly_rec[6] = pts[1].x;
            poly_rec[7] = pts[1].y;
            poly_rec[8] = pts[1].z;
          }
        }
      }
      else {
        src_f = poly_rec;
        if (k_f_0 <= out[1].y) {
          if ((k_f_0 <= out[1].x) || (out[1].y <= k_f_0)) goto LAB_00488b04;
          poly_rec[3] = pts[1].x;
          poly_rec[4] = pts[1].y;
          poly_rec[5] = pts[1].z;
        }
        else {
LAB_00488b64:
          *src_f = pts[1].x;
          src_f[1] = pts[1].y;
          src_f[2] = pts[1].z;
        }
      }
      if (k_f_0 <= out[2].x) {
LAB_00488bdc:
        if (k_f_0 < out[2].x) {
          if (out[2].y <= k_f_0) {
            if ((k_f_0 < out[2].x) && (out[2].y < k_f_0)) {
              src_f = poly_rec + 9;
              goto LAB_00488c3c;
            }
          }
          else {
            poly_rec[6] = pts[2].x;
            poly_rec[7] = pts[2].y;
            poly_rec[8] = pts[2].z;
          }
        }
      }
      else {
        src_f = poly_rec;
        if (k_f_0 <= out[2].y) {
          if ((k_f_0 <= out[2].x) || (out[2].y <= k_f_0)) goto LAB_00488bdc;
          poly_rec[3] = pts[2].x;
          poly_rec[4] = pts[2].y;
          poly_rec[5] = pts[2].z;
        }
        else {
LAB_00488c3c:
          *src_f = pts[2].x;
          src_f[1] = pts[2].y;
          src_f[2] = pts[2].z;
        }
      }
      if (k_f_0 <= out[3].x) {
LAB_00488cb4:
        if (k_f_0 < out[3].x) {
          if (out[3].y <= k_f_0) {
            if ((k_f_0 < out[3].x) && (out[3].y < k_f_0)) {
              src_f = poly_rec + 9;
              goto LAB_00488d14;
            }
          }
          else {
            poly_rec[6] = pts[3].x;
            poly_rec[7] = pts[3].y;
            poly_rec[8] = pts[3].z;
          }
        }
      }
      else {
        src_f = poly_rec;
        if (k_f_0 <= out[3].y) {
          if ((k_f_0 <= out[3].x) || (out[3].y <= k_f_0)) goto LAB_00488cb4;
          poly_rec[3] = pts[3].x;
          poly_rec[4] = pts[3].y;
          poly_rec[5] = pts[3].z;
        }
        else {
LAB_00488d14:
          *src_f = pts[3].x;
          src_f[1] = pts[3].y;
          src_f[2] = pts[3].z;
        }
      }
      poly_index = poly_index + 1;
      poly_rec = poly_rec + 0x13;
    } while (poly_index < local_c8);
  }
  course_path_segments_distance_and_heading((undefined4)lap);
  return;
}

