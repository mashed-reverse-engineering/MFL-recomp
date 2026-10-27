// cluster-entries: ptank_lock_all_sub_48dc40
/* Installed cluster ptank_lock_all_sub_48dc40 (tools/cluster_install.py from output/cluster_pilot_k/ptank_lock_all_sub_48dc40.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042a460*/ extern int course_get_light_position();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd61c*/ extern float k_f_0_001;
/*006f6910*/ extern undefined4 *g_data_6f6910;
/*006f6914*/ extern undefined4 g_data_6f6914;
/*006f6918*/ extern undefined4 *g_data_6f6918;
/*006f691c*/ extern undefined4 g_data_6f691c;
/*006f6920*/ extern undefined4 *g_data_6f6920;
/*006f6924*/ extern undefined4 g_data_6f6924;
/*006f6928*/ extern undefined4 *g_data_6f6928;
/*006f692c*/ extern undefined4 g_data_6f692c;
/*006f6934*/ extern undefined4 g_data_6f6934;
/* ptank_lock_all_sub_48dc40 */

/* single caller ptank_lock_all */

void ptank_lock_all_sub_48dc40(char *ptank, undefined4 *src)

{
  byte count_byte;
  float tmp_f;
  float *centre_ptr;
  float uv_v;
  undefined4 tmp_w;
  undefined4 *vert_words;
  undefined4 *vtx_dst;
  float *src_vert;
  int emitted;
  float *dst_vert;
  int batch_end;
  int vert_count;
  RwReal dist;
  RwV3d delta;
  struct {
    int emitted_copy;
    float verts [4];
    undefined4 verts_tail [892];
  } frame;
  
  course_get_light_position();
  vert_count = *(int *)(ptank + 4);
  src_vert = (float *)*src;
  delta.x = *src_vert - *(float *)(vert_count + 0x40);
  delta.y = src_vert[1] - *(float *)(vert_count + 0x44);
  delta.z = src_vert[2] - *(float *)(vert_count + 0x48);
  RwV3dLength(&delta);
  src_vert = (float *)*src;
  vert_count = 0;
  if (*(char *)(src + 1) != '\0') {
    dst_vert = frame.verts;
    do {
      *dst_vert = *src_vert;
      tmp_f = src_vert[2];
      dst_vert[1] = src_vert[1];
      dst_vert[2] = tmp_f;
      emitted = *(int *)(ptank + 4);
      delta.x = *dst_vert - *(float *)(emitted + 0x40);
      delta.y = dst_vert[1] - *(float *)(emitted + 0x44);
      delta.z = dst_vert[2] - *(float *)(emitted + 0x48);
      dist = RwV3dLength(&delta);
      dst_vert[6] = dist;
      centre_ptr = (float *)*src;
      delta.x = *dst_vert - *centre_ptr;
      delta.y = dst_vert[1] - centre_ptr[1];
      delta.z = dst_vert[2] - centre_ptr[2];
      if (k_f_0_001 <
          delta.z * delta.z + delta.x * delta.x + delta.y * delta.y) {
        RwV3dNormalize(&delta,&delta);
      }
      tmp_f = src_vert[3];
      uv_v = src_vert[4];
      count_byte = *(byte *)(src + 1);
      *(undefined1 *)((char *)dst_vert + 0xe) = 0xc0;
      *(undefined1 *)((char *)dst_vert + 0xd) = 0xc0;
      *(undefined1 *)(dst_vert + 3) = 0xc0;
      *(undefined1 *)((char *)dst_vert + 0xf) = 0xff;
      dst_vert[4] = tmp_f;
      dst_vert[5] = uv_v;
      vert_count = vert_count + 1;
      dst_vert = dst_vert + 7;
      src_vert = src_vert + 6;
    } while (vert_count < (int)(uint)count_byte);
  }
  if (((g_data_6f6918 != (undefined4 *)0x0) && (g_data_6f6910 != (undefined4 *)0x0)) &&
     (g_data_6f6928 != (undefined4 *)0x0)) {
    emitted = 0;
    frame.emitted_copy = 0;
    if (3 < vert_count) {
      batch_end = 3;
      vert_words = frame.verts_tail;
      do {
        vtx_dst = g_data_6f6918;
        g_data_6f6934 = g_data_6f6934 + 1;
        *g_data_6f6918 = vert_words[-4];
        vtx_dst[1] = vert_words[-3];
        vtx_dst[2] = vert_words[-2];
        *g_data_6f6928 = *vert_words;
        g_data_6f6928[1] = vert_words[1];
        *g_data_6f6910 = vert_words[-1];
        *g_data_6f6920 = 0;
        g_data_6f6920[1] = 0;
        g_data_6f6920[2] = 0x3f800000;
        g_data_6f6920[3] = 0x3f800000;
        vtx_dst = (undefined4 *)((char *)g_data_6f6918 + g_data_6f691c);
        g_data_6f6928 = (undefined4 *)((char *)g_data_6f6928 + g_data_6f692c);
        g_data_6f6910 = (undefined4 *)((char *)g_data_6f6910 + g_data_6f6914);
        g_data_6f6920 = (undefined4 *)((char *)g_data_6f6920 + g_data_6f6924);
        g_data_6f6934 = g_data_6f6934 + 1;
        g_data_6f6918 = vtx_dst;
        *vtx_dst = vert_words[3];
        tmp_w = vert_words[5];
        vtx_dst[1] = vert_words[4];
        vtx_dst[2] = tmp_w;
        *g_data_6f6928 = vert_words[7];
        g_data_6f6928[1] = vert_words[8];
        *g_data_6f6910 = vert_words[6];
        *g_data_6f6920 = 0;
        g_data_6f6920[1] = 0;
        g_data_6f6920[2] = 0x3f800000;
        g_data_6f6920[3] = 0x3f800000;
        vtx_dst = (undefined4 *)((char *)g_data_6f6918 + g_data_6f691c);
        g_data_6f6928 = (undefined4 *)((char *)g_data_6f6928 + g_data_6f692c);
        g_data_6f6910 = (undefined4 *)((char *)g_data_6f6910 + g_data_6f6914);
        g_data_6f6920 = (undefined4 *)((char *)g_data_6f6920 + g_data_6f6924);
        g_data_6f6934 = g_data_6f6934 + 1;
        g_data_6f6918 = vtx_dst;
        *vtx_dst = vert_words[10];
        vtx_dst[1] = vert_words[0xb];
        vtx_dst[2] = vert_words[0xc];
        *g_data_6f6928 = vert_words[0xe];
        g_data_6f6928[1] = vert_words[0xf];
        *g_data_6f6910 = vert_words[0xd];
        *g_data_6f6920 = 0;
        g_data_6f6920[1] = 0;
        g_data_6f6920[2] = 0x3f800000;
        g_data_6f6920[3] = 0x3f800000;
        vtx_dst = (undefined4 *)((char *)g_data_6f6918 + g_data_6f691c);
        g_data_6f6928 = (undefined4 *)((char *)g_data_6f6928 + g_data_6f692c);
        g_data_6f6910 = (undefined4 *)((char *)g_data_6f6910 + g_data_6f6914);
        g_data_6f6920 = (undefined4 *)((char *)g_data_6f6920 + g_data_6f6924);
        g_data_6f6934 = g_data_6f6934 + 1;
        g_data_6f6918 = vtx_dst;
        *vtx_dst = vert_words[0x11];
        tmp_w = vert_words[0x13];
        vtx_dst[1] = vert_words[0x12];
        vtx_dst[2] = tmp_w;
        *g_data_6f6928 = vert_words[0x15];
        g_data_6f6928[1] = vert_words[0x16];
        *g_data_6f6910 = vert_words[0x14];
        *g_data_6f6920 = 0;
        g_data_6f6920[1] = 0;
        g_data_6f6920[2] = 0x3f800000;
        g_data_6f6920[3] = 0x3f800000;
        g_data_6f6918 = (undefined4 *)((char *)g_data_6f6918 + g_data_6f691c);
        g_data_6f6928 = (undefined4 *)((char *)g_data_6f6928 + g_data_6f692c);
        g_data_6f6910 = (undefined4 *)((char *)g_data_6f6910 + g_data_6f6914);
        g_data_6f6920 = (undefined4 *)((char *)g_data_6f6920 + g_data_6f6924);
        emitted = frame.emitted_copy + 4;
        batch_end = batch_end + 4;
        vert_words = vert_words + 0x1c;
        frame.emitted_copy = emitted;
      } while (batch_end < vert_count);
    }
    if (emitted < vert_count) {
      vert_words = frame.verts_tail + emitted * 7;
      vert_count = vert_count - emitted;
      do {
        vtx_dst = g_data_6f6918;
        g_data_6f6934 = g_data_6f6934 + 1;
        *g_data_6f6918 = vert_words[-4];
        tmp_w = vert_words[-2];
        vtx_dst[1] = vert_words[-3];
        vtx_dst[2] = tmp_w;
        *g_data_6f6928 = *vert_words;
        g_data_6f6928[1] = vert_words[1];
        *g_data_6f6910 = vert_words[-1];
        *g_data_6f6920 = 0;
        g_data_6f6920[1] = 0;
        g_data_6f6920[2] = 0x3f800000;
        g_data_6f6920[3] = 0x3f800000;
        g_data_6f6918 = (undefined4 *)((char *)g_data_6f6918 + g_data_6f691c);
        g_data_6f6928 = (undefined4 *)((char *)g_data_6f6928 + g_data_6f692c);
        g_data_6f6910 = (undefined4 *)((char *)g_data_6f6910 + g_data_6f6914);
        g_data_6f6920 = (undefined4 *)((char *)g_data_6f6920 + g_data_6f6924);
        vert_words = vert_words + 7;
        vert_count = vert_count + -1;
      } while (vert_count != 0);
    }
  }
  return;
}

