// cluster-entries: trail_particle_add
/* Installed cluster trail_particle_add (tools/cluster_install.py from output/cluster_pilot_uc3/trail_particle_add.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd330*/ extern float float_1_0;
/*005ddb18*/ extern float float_0_8;
/*005e64a4*/ extern float k_f_0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00843ea0*/ extern undefined4 g_collision_triangles;
/*00843ea4*/ extern undefined4 g_trail_843ea4;
/*00843ea8*/ extern undefined4 g_camera_843ea8;
/*00843eac*/ extern undefined4 g_trail_843eac;
/*00843eb0*/ extern undefined4 g_trail_843eb0;
/*00843eb4*/ extern undefined g_trail_particle_843eb4;
/*00843eb8*/ extern undefined g_trail_particle_843eb8;
/*00843ebc*/ extern undefined4 g_trail_843ebc;
/*00843ec0*/ extern undefined4 g_trail_843ec0;
/*00843ec4*/ extern undefined4 g_trail_843ec4;
/*00843ec8*/ extern undefined4 g_trail_843ec8;
/*00843ecc*/ extern undefined4 g_trail_843ecc;
/*00843ed0*/ extern undefined g_trail_particle_843ed0;
/*00843ed4*/ extern undefined g_trail_particle_843ed4;
/*00843ed8*/ extern undefined4 g_trail_843ed8;
/*00843edc*/ extern undefined4 g_trail_843edc;
/*00843ee0*/ extern undefined g_trail_particle_843ee0;
/*00843ee4*/ extern undefined g_trail_particle_843ee4;
/*00843ee8*/ extern undefined g_trail_particle_843ee8;
/*00843eec*/ extern undefined g_trail_particle_843eec;
/*00843ef0*/ extern undefined g_trail_particle_843ef0;
/*00843ef4*/ extern undefined g_trail_particle_843ef4;
/*00843ef8*/ extern undefined g_trail_particle_843ef8;
/*00843efc*/ extern undefined g_trail_843efc;
/*00843f00*/ extern undefined g_trail_particle_843f00;
/*00843f04*/ extern undefined g_trail_particle_843f04;
/*00843f08*/ extern undefined g_trail_particle_843f08;
/*00843f0c*/ extern undefined g_trail_particle_843f0c;
/*008d8b00*/ extern undefined4 g_trail_8d8b00;
/* trail_particle_add */

/* "adds one trail particle to the PTank buffer (inferred)" */

void trail_particle_add_n(RwV3d *particle, int camera, float view_scale)

{
  float near_z;
  float far_z;
  float depth_a;
  float depth_b;
  float screen_w;
  float screen_h;
  float y_min;
  float half_h;
  float half_w;
  float x_min;
  int quad_index;
  float half_w_base;
  float inv_scale;
  float inv_z;
  undefined4 packed_color;
  int quad_byte_off;
  struct {
    float pad_lo[11];
    RwV3d view_pos;
    float uv_u1;
    float uv_u0;
    float uv_u1_b;
    float uv_u0_b;
    undefined1 pad_30[4];
    float uv_v1;
    undefined1 pad_28[4];
    float uv_v0;
    float pad_20[3];
    float y_clip_min;
    float x_clip_min;
    undefined1 pad_c[4];
    float x_screen;
    float y_screen;
  } frame;
  
  *(int *)&particle[1].y = 1;
  /* (inferred) project one trail particle to the screen: transform by the camera matrix, keep it when between the near / far plane */
  quad_index = g_trail_8d8b00;
  near_z = *(float *)(camera + 0x80);
  far_z = *(float *)(camera + 0x84);
  quad_byte_off = g_trail_8d8b00 * 0x70;
  depth_a = RW_GLOBAL(0x18, float);
  depth_b = RW_GLOBAL(0x1c, float);
  screen_w = (float)*(int *)(*(int *)(camera + 0x60) + 0xc);
  screen_h = (float)*(int *)(*(int *)(camera + 0x60) + 0x10);
  RwV3dTransformPoints(&frame.view_pos,particle,1,(RwMatrix *)(camera + 0x20));
  if ((frame.view_pos.z > near_z) && (frame.view_pos.z < far_z)) {
    /* screen rectangle (x_screen, y_screen +- half size), clipped to the screen; the uv range is cut by the same fraction */
    inv_z = float_1_0 / frame.view_pos.z;
    frame.x_screen = inv_z * screen_w;
    frame.x_screen = frame.x_screen * frame.view_pos.x;
    frame.y_screen = inv_z * screen_h;
    frame.y_screen = frame.y_screen * frame.view_pos.y;
    /* one multiplication per statement: cl would share (1/view_scale * p1x * 1/z * 0.8) between half_w and half_h, the original rounds w*iv and iv*h first */
    inv_scale = float_1_0 / view_scale;
    half_w_base = screen_w * inv_scale;
    half_w_base = half_w_base * float_0_8;
    half_w_base = half_w_base * particle[1].x;
    half_w_base = half_w_base * inv_z;
    half_w = half_w_base * (screen_w / screen_h);
    half_h = inv_scale * screen_h;
    half_h = half_h * float_0_8;
    half_h = half_h * particle[1].x;
    half_h = half_h * inv_z;
    if ((-half_w < frame.x_screen) &&
       (((frame.x_screen < half_w + screen_w && (-half_h < frame.y_screen)) && (frame.y_screen < half_h + screen_h)))) {
      frame.uv_u1 = particle[2].x;
      x_min = frame.x_screen - half_w;
      y_min = frame.y_screen - half_h;
      frame.uv_v1 = particle[2].z;
      frame.uv_u0_b = particle[1].z;
      frame.x_screen = half_w + frame.x_screen;
      frame.uv_u1_b = particle[2].x;
      frame.y_screen = half_h + frame.y_screen;
      frame.uv_u0 = particle[1].z;
      frame.uv_v0 = particle[2].y;
      frame.y_clip_min = y_min;
      if (y_min < k_f_0) {
        frame.y_clip_min = 0.0;
        frame.uv_v1 = particle[2].y -
                   (particle[2].y - particle[2].z) * (((half_h + half_h) - -y_min) / (half_h + half_h));
      }
      if (screen_h < frame.y_screen) {
        frame.uv_v0 = particle[2].y -
                   (particle[2].y - particle[2].z) * ((frame.y_screen - screen_h) / (half_h + half_h));
        frame.y_screen = screen_h;
      }
      if (screen_w < frame.x_screen) {
        frame.uv_u0 = particle[1].z -
                   (particle[1].z - particle[2].x) * ((frame.x_screen - screen_w) / (half_w + half_w));
        frame.uv_u0_b = frame.uv_u0;
        frame.x_screen = screen_w;
      }
      frame.x_clip_min = x_min;
      if (x_min < k_f_0) {
        frame.x_clip_min = 0.0;
        frame.uv_u1 = particle[1].z -
                   (particle[1].z - particle[2].x) *
                   (((half_w + half_w) - -x_min) / (half_w + half_w));
        frame.uv_u1_b = frame.uv_u1;
        x_min = k_f_0;
      }
      /* write the quad (4 vertices x 28 bytes) into the trail vertex buffer slot quad_index */
      *(float *)((char *)((undefined *)&g_collision_triangles) + quad_byte_off) = x_min;
      *(float *)((char *)((undefined *)&g_trail_843ea4) + quad_byte_off) = frame.y_clip_min;
      *(float *)((char *)((undefined *)&g_trail_843ebc) + quad_byte_off) = frame.x_screen;
      *(float *)((char *)((undefined *)&g_trail_843ec0) + quad_byte_off) = frame.y_clip_min;
      *(float *)((char *)((undefined *)&g_trail_843edc) + quad_byte_off) = frame.y_screen;
      *(float *)((char *)((undefined *)&g_trail_843ed8) + quad_byte_off) = frame.x_clip_min;
      *(float *)((char *)&g_trail_particle_843ef8 + quad_byte_off) = frame.y_screen;
      *(float *)((char *)&g_trail_particle_843ef4 + quad_byte_off) = frame.x_screen;
      *(float *)((char *)&g_trail_particle_843eb8 + quad_byte_off) = frame.uv_v1;
      *(float *)((char *)&g_trail_particle_843eb4 + quad_byte_off) = frame.uv_u1;
      *(float *)((char *)&g_trail_particle_843ed4 + quad_byte_off) = frame.uv_v1;
      *(float *)((char *)&g_trail_particle_843ed0 + quad_byte_off) = frame.uv_u0;
      *(float *)((char *)&g_trail_particle_843ef0 + quad_byte_off) = frame.uv_v0;
      *(float *)((char *)&g_trail_particle_843eec + quad_byte_off) = frame.uv_u1_b;
      *(float *)((char *)&g_trail_particle_843f0c + quad_byte_off) = frame.uv_v0;
      *(float *)((char *)&g_trail_particle_843f08 + quad_byte_off) = frame.uv_u0_b;
      packed_color = CONCAT31(CONCAT21(CONCAT11(*(undefined1 *)((int)&particle[3].x + 3),
                                          *(undefined1 *)&particle[3].x),
                                 *(undefined1 *)((int)&particle[3].x + 1)),
                        *(undefined1 *)((int)&particle[3].x + 2));
      (&g_trail_843eb0)[quad_index * 0x1c] = packed_color;
      *(float *)&(&g_trail_843eac)[quad_index * 0x1c] = (float_1_0 / frame.view_pos.z);
      depth_a = ((depth_b - depth_a) / (far_z - near_z)) * (far_z / *(volatile float *)&frame.view_pos.z) * (frame.view_pos.z - near_z) +
              depth_a;
      *(float *)&(&g_camera_843ea8)[quad_index * 0x1c] = depth_a;
      (&g_trail_843ecc)[quad_index * 0x1c] = packed_color;
      *(float *)&(&g_trail_843ec4)[quad_index * 0x1c] = depth_a;
      *(float *)&(&g_trail_843ec8)[quad_index * 0x1c] = (float_1_0 / frame.view_pos.z);
      *(float *)((char *)&g_trail_particle_843ee0 + quad_byte_off) = depth_a;
      *(undefined4 *)((char *)&g_trail_particle_843ee8 + quad_byte_off) = packed_color;
      *(float *)((char *)&g_trail_particle_843ee4 + quad_byte_off) = (float_1_0 / frame.view_pos.z);
      *(undefined4 *)((char *)&g_trail_particle_843f04 + quad_byte_off) = packed_color;
      *(float *)((char *)&g_trail_843efc + quad_byte_off) = depth_a;
      *(float *)((char *)&g_trail_particle_843f00 + quad_byte_off) = (float_1_0 / frame.view_pos.z);
      g_trail_8d8b00 = g_trail_8d8b00 + 1;
    }
  }
  return;
}

/* entry thunk at the original address of trail_particle_add: register inputs (edi,eax,s0) -> natural C trail_particle_add_n; ret 0 */
__declspec(naked) void trail_particle_add(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        push edi
        call trail_particle_add_n
        add esp, 12
        ret
    }
}
