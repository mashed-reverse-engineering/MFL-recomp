// cluster-entries: spring_grid_render
/* Installed cluster spring_grid_render (tools/cluster_install.py from output/cluster_pilot_j1/spring_grid_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct SpringNode g_spring_nodes[460];
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*004b2b48*/ extern int _ftol();
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*005dd378*/ extern float k_f_0_1;
/*005dd41c*/ extern float float_5_0;
/*005dd620*/ extern float float_10_0;
/*006299d8*/ extern undefined1 g_gfx_6299d8;
/*006c2940*/ extern int g_aux_camera;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd9ac*/ extern float k_f_255;
/*005ddb38*/ extern float k_f_25;
/*006f1e48*/ extern undefined4 g_spring_grid_6f1e48;
/*006f2ab4*/ extern undefined4 g_spring_grid_6f2ab4;
/*006f2ab8*/ extern undefined4 *g_gfx_6f2ab8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define SPRING_GRID_COLS (0x17) /* nodes per grid row (g_spring_nodes row length) */
#define SPRING_NODE_FLOATS (0x11) /* floats per node when stepping along a row (node_ptr += this) */
#define SPRING_ROW_FLOATS (0x187) /* floats per grid row = SPRING_GRID_COLS * SPRING_NODE_FLOATS */
/* spring_grid_render */

/* "scales the grid positions by 0.1 and renders them with Im3D (inferred)" */

void spring_grid_render(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  RwDevice *device;
  RwMatrix *matrix;
  undefined4 *tex_ptr;
  void *im3d_ok;
  float *row_ptr;
  uint row_first_or_light;
  float *node_ptr;
  RwV3d *node_cursor;
  RwV3d *pointsOut;
  int pass_value;
  int cols_left;
  uint row_last_or_alpha;
  float *vtx_out;
  float *lightv;
  double tt;
  RwV3d edge;
  struct {
    int row_i;
    int pass;
    int col_first;
    int col_last;
    int col_span;
    int col_i;
    int col_left;
    int rows_left;
    float uv_u0;
    float uv_v0;
    float uv_u1;
    float uv_v1;
    int rows_total;
    int row_next;
  } frame;
  RwMatrix inv_matrix;
  
  /* Pass 1 of the setup: scale every node position by 0.1 (k_f_0_1) and transform it by g_spring_grid_6f2ab4 (matrix). Rows of SPRING_GRID_COLS nodes. */
  lightv = (float *)(*(int *)(g_aux_camera + 4) + 0x10);
  node_cursor = (RwV3d *)&g_spring_nodes[0].w08;
  do {
    cols_left = SPRING_GRID_COLS;
    do {
      matrix = (RwMatrix *)g_spring_grid_6f2ab4;
      node_cursor->x = node_cursor->x * k_f_0_1;
      node_cursor->y = node_cursor->y * k_f_0_1;
      node_cursor->z = node_cursor->z * k_f_0_1;
      RwV3dTransformPoints(node_cursor,node_cursor,1,matrix);
      node_cursor = (RwV3d *)&node_cursor[5].z;
      cols_left = cols_left + -1;
    } while (cols_left != 0);
  } while ((int)node_cursor < 0x891c7d);
  /* Two render passes (pass 0: whole "Checks"-textured grid; pass 1: inner window, texture g_gfx_6f2ab8 if set). Each grid row is drawn as one Im3D triangle strip; vertex colour = squared half-lambert of the surface normal against the aux camera light vector (lightv) as tt*tt*255+25 clamped to 0..255 (grey), alpha byte g_gfx_6299d8 (inferred). */
  frame.pass = 0;
  do {
    pass_value = frame.pass;
    if (frame.pass == 0) {
      frame.col_last = 0x16;
      row_first_or_light = 0;
      row_last_or_alpha = 0x13;
      frame.col_first = 0;
    }
    else {
      frame.col_first = 0xd;
      frame.col_last = 0x12;
      row_first_or_light = 5;
      row_last_or_alpha = 9;
    }
    if ((g_gfx_6f2ab8 == (undefined4 *)0x0) || (frame.pass == 0)) {
      device = &RwGlobalsRelease_dOpenDevice(RwEngineInstance);
      tex_ptr = (undefined4 *)find_texture_dict_b("Checks");
      RwDevice_fpRenderStateSet(device)(rwRENDERSTATETEXTURERASTER,(void *)*tex_ptr);
      RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,(void *)0x1);
      pass_value = frame.pass;
    }
    else {
      RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)*g_gfx_6f2ab8);
      RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,(void *)0x3);
    }
    RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)0x2);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
    if (row_first_or_light < row_last_or_alpha) {
      frame.col_span = frame.col_last - frame.col_first;
      row_ptr = &g_spring_nodes[row_first_or_light * SPRING_GRID_COLS + frame.col_first].w40;
      frame.rows_left = row_last_or_alpha - row_first_or_light;
      frame.row_i = 0;
      frame.rows_total = frame.rows_left;
      do {
        if (frame.col_first <= frame.col_last) {
          frame.col_left = frame.col_span + 1;
          frame.col_i = 0;
          node_ptr = row_ptr;
          vtx_out = (float *)&g_spring_grid_6f1e48;
          do {
            if (frame.pass == 0) {
              frame.uv_u0 = node_ptr[-1] * float_5_0;
              frame.uv_v0 = *node_ptr * float_5_0;
              frame.uv_u1 = node_ptr[0x186] * float_5_0;
              frame.uv_v1 = node_ptr[0x187] * float_5_0;
            }
            else {
              frame.row_next = frame.row_i + 1;
              frame.uv_u0 = (float)frame.col_i / (float)frame.col_span;
              frame.uv_v0 = (float)frame.row_i / (float)frame.rows_total;
              frame.uv_v1 = (float)frame.row_next / (float)frame.rows_total;
              frame.uv_u1 = frame.uv_u0;
            }
            edge.x = node_ptr[-0xe] - node_ptr[0x179];
            fVar1 = node_ptr[-0xd];
            fVar2 = node_ptr[-0xc];
            *vtx_out = node_ptr[-0xe];
            edge.y = node_ptr[-0xd] - node_ptr[0x17a];
            vtx_out[1] = fVar1;
            edge.z = node_ptr[-0xc] - node_ptr[0x17b];
            vtx_out[2] = fVar2;
            RwV3dNormalize(&edge,&edge);
            tt = ((double)edge.y * lightv[9] + (double)edge.x * lightv[8] + (double)edge.z * lightv[10] + float_1_0) * k_f_0_5;
            row_first_or_light = (int)(tt * tt * k_f_255 + k_f_25);
            if ((int)row_first_or_light < 0x100) {
              if ((int)row_first_or_light < 0) {
                row_first_or_light = 0;
              }
            }
            else {
              row_first_or_light = 0xff;
            }
            row_last_or_alpha = (uint)g_gfx_6299d8;
            edge.x = node_ptr[0x179] - node_ptr[0x300];
            vtx_out[7] = frame.uv_u0;
            fVar1 = node_ptr[0x17a];
            edge.y = node_ptr[0x17a] - node_ptr[0x301];
            *(uint *)&vtx_out[6] = ((row_last_or_alpha << 8 | row_first_or_light) << 8 | row_first_or_light) << 8 | row_first_or_light;
            fVar2 = node_ptr[0x179];
            vtx_out[8] = frame.uv_v0;
            fVar3 = node_ptr[0x17b];
            edge.z = node_ptr[0x302];
            vtx_out[9] = fVar2;
            edge.z = node_ptr[0x17b] - edge.z;
            vtx_out[10] = fVar1;
            vtx_out[0xb] = fVar3;
            RwV3dNormalize(&edge,&edge);
            tt = ((double)edge.y * lightv[9] + (double)edge.x * lightv[8] + (double)edge.z * lightv[10] + float_1_0) * k_f_0_5;
            row_first_or_light = (int)(tt * tt * k_f_255 + k_f_25);
            if ((int)row_first_or_light < 0x100) {
              if ((int)row_first_or_light < 0) {
                row_first_or_light = 0;
              }
            }
            else {
              row_first_or_light = 0xff;
            }
            row_last_or_alpha = (uint)g_gfx_6299d8;
            vtx_out[0x10] = frame.uv_u1;
            node_ptr = node_ptr + SPRING_NODE_FLOATS;
            *(uint *)&vtx_out[0xf] = ((row_last_or_alpha << 8 | row_first_or_light) << 8 | row_first_or_light) << 8 | row_first_or_light;
            vtx_out[0x11] = frame.uv_v1;
            frame.col_i = frame.col_i + 1;
            frame.col_left = frame.col_left + -1;
            vtx_out = vtx_out + 0x12;
          } while (frame.col_left != 0);
          frame.col_left = 0;
        }
        im3d_ok = RwIm3DTransform((RwIm3DVertex *)&g_spring_grid_6f1e48,frame.col_span * 2 + 2,
                                 (RwMatrix *)0x0,1);
        if (im3d_ok != (void *)0x0) {
          RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
          RwIm3DEnd();
        }
        row_ptr = row_ptr + SPRING_ROW_FLOATS;
        frame.row_i = frame.row_i + 1;
        frame.rows_left = frame.rows_left + -1;
      } while (frame.rows_left != 0);
      frame.rows_left = 0;
      pass_value = frame.pass;
    }
    frame.pass = pass_value + 1;
  } while (frame.pass < 2);
  /* Restore: transform nodes back with the inverse matrix and scale by 10 (undoing the 0.1 scale above). */
  RwMatrixInvert((RwMatrix *)&inv_matrix,(RwMatrix *)g_spring_grid_6f2ab4);
  node_cursor = (RwV3d *)&g_spring_nodes[0].w08;
  do {
    pass_value = SPRING_GRID_COLS;
    pointsOut = node_cursor;
    do {
      RwV3dTransformPoints(pointsOut,pointsOut,1,&inv_matrix);
      node_cursor = (RwV3d *)&pointsOut[5].z;
      pass_value = pass_value + -1;
      pointsOut->x = pointsOut->x * float_10_0;
      pointsOut->y = pointsOut->y * float_10_0;
      pointsOut->z = pointsOut->z * float_10_0;
      pointsOut = node_cursor;
    } while (pass_value != 0);
  } while ((int)node_cursor < 0x891c7d);
  RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,(void *)0x1);
  return;
}

