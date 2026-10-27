#include "mfl_tier2.h"
/*0040ad10*/ extern int course_editor_read_chunk();
/*0040ae20*/ extern char *gamesave_ptr_to_line_exceptions();
/*0042b1d0*/ extern char *course_get_record_base();
/*0042b210*/ extern char *course_get_record_point();
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac74*/ extern float g_car_62ac74;
/* camera_pick_ground_point */

/* "ray from the camera down (0, -1, 0) for the replay / editor view" */

/* (inferred names) out_dir = unit view direction of course record `index` (rotated down vector by the yaw / pitch of its line exception row when it has one, else the record base rotated by -25 degrees around the first edge), out_offset = distance override (row +8, 0 otherwise) */
void __cdecl camera_pick_ground_point(int index,RwV3d *out_dir,float *out_offset)

{
  char *view_exceptions;
  RwReal *record_base;
  float *exception_row;
  struct {
    RwV3d edge_axis;
    RwV3d down_dir;
    undefined1 pad_4c[8];
    float edge_z;
    RwMatrix rot_matrix;
  } frame;
  
  frame.down_dir.x = 0.0;
  frame.down_dir.y = -1.0;
  frame.down_dir.z = 0.0;
  *out_offset = 0.0;
  course_editor_read_chunk();
  view_exceptions = gamesave_ptr_to_line_exceptions();
  if (view_exceptions != 0) {
    exception_row = (float *)(view_exceptions + index * 0xc);
    if (*(unsigned int *)&exception_row[2] != 0xbf800000) {      /* integer compare with the bits of -1.0f, as the original */
      *out_offset = exception_row[2];
    }
    if (*(unsigned int *)exception_row != 0xbf800000) {
      RwMatrixRotate(&frame.rot_matrix,(RwV3d *)&g_car_62ac68,k_f_90 - *exception_row,rwCOMBINEREPLACE);
      RwV3dTransformVectors(out_dir,&frame.down_dir,1,&frame.rot_matrix);
      RwMatrixRotate(&frame.rot_matrix,(RwV3d *)&g_car_62ac74,exception_row[1] + k_f_180,rwCOMBINEREPLACE);
      RwV3dTransformVectors(out_dir,out_dir,1,&frame.rot_matrix);
      return;
    }
  }
  record_base = (RwReal *)course_get_record_base(index);
  out_dir->x = *record_base;
  out_dir->y = record_base[1];
  out_dir->z = record_base[2];
  exception_row = (float *)course_get_record_point(index,0);
  frame.edge_axis.x = *exception_row;
  frame.edge_axis.y = exception_row[1];
  frame.edge_axis.z = exception_row[2];
  exception_row = (float *)course_get_record_point(index,3);
  frame.edge_z = exception_row[2];
  frame.edge_axis.x = frame.edge_axis.x - *exception_row;
  frame.edge_axis.y = frame.edge_axis.y - exception_row[1];
  frame.edge_axis.z = frame.edge_axis.z - frame.edge_z;
  RwMatrixRotate(&frame.rot_matrix,&frame.edge_axis,-25.0,rwCOMBINEREPLACE);
  RwV3dTransformVectors(out_dir,out_dir,1,&frame.rot_matrix);
  return;
}


