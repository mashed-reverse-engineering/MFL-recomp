#include "mfl_tier2.h"
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056e5b0*/ extern void FUN_0056e5b0(void *p0, undefined4 p1);
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*005dd364*/ extern float k_f_n1;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];
/* car_set_physics_matrix */

/* "copies the body_matrix into the physics body and translates by the field_0x138 offset" */

void __cdecl
car_set_physics_matrix
          (int car_index,RwMatrix *src_matrix,char *unused_3,char *unused_4,char *unused_5,
          char *unused_6,char *unused_7,char *unused_8,char *unused_9,
          char *unused_10,char *unused_11,char *unused_12,char *unused_13,
          char *unused_14,char *unused_15,char *unused_16,char *unused_17)

{
  RwMatrix *body_matrix;
  char *owner;
  int words_left;
  RwMatrix *src_cursor;
  RwMatrix *dst_cursor;
  RwV3d body_offset;
  
  if (((&g_rw_6fd498)[car_index] != 0) && (owner = rwp_owner_object_get((&g_rw_6fd498)[car_index]), owner != 0))
  {
    body_matrix = *(RwMatrix **)(*(char **)(owner + 0x10) + 8);
    src_cursor = src_matrix;
    dst_cursor = body_matrix;
    for (words_left = 0x10; words_left != 0; words_left = words_left + -1) {
      (dst_cursor->right).x = (src_cursor->right).x;
      src_cursor = (RwMatrix *)&(src_cursor->right).y;
      dst_cursor = (RwMatrix *)&(dst_cursor->right).y;
    }
    body_offset.x = player_car[car_index].rwp_body_offset[0] * k_f_n1;
    body_offset.y = player_car[car_index].rwp_body_offset[1] * k_f_n1;
    body_offset.z = player_car[car_index].rwp_body_offset[2] * k_f_n1;
    RwV3dTransformVectors(&body_offset,&body_offset,1,src_matrix);
    RwMatrixTranslate(body_matrix,&body_offset,rwCOMBINEPRECONCAT);
    RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fd498)[car_index] + 4));
    FUN_0056e5b0(owner,0);
    rwp_world_set_bit_5c(*(undefined4 *)(owner + 0x24),owner,1);
  }
  return;
}


