#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*00658b50*/ extern undefined g_btb_panel_658b50;
/*00658b90*/ extern undefined g_btb_panel_658b90;
/*00658b94*/ extern undefined g_btb_panel_658b94;
/*00658b98*/ extern undefined g_btb_panel_658b98;
/*00658b9c*/ extern undefined g_btb_panel_658b9c;
/*00658ba0*/ extern undefined g_btb_panel_658ba0;
/*00658ba4*/ extern undefined g_btb_panel_658ba4;
/*00658ba8*/ extern undefined g_btb_panel_658ba8;
/*00658bd0*/ extern undefined g_btb_panel_658bd0;
/*00658bf0*/ extern undefined g_btb_panel_658bf0;
/*00658bf4*/ extern undefined g_btb_panel_658bf4;
/*00658bf8*/ extern undefined g_btb_panel_658bf8;
/*00658c00*/ extern undefined g_btb_panel_658c00;
/*00658c10*/ extern undefined g_btb_panel_658c10;
/* btb_panel_slot_update */

/*  */

void btb_panel_slot_update(int slot_or_colour)

{
  RwFrame *frame_ptr;
  undefined4 old_colour;
  undefined1 alpha_byte;
  int record_ptr;
  int slot_off;
  RwReal *angle_ptr;
  char *slot_base;      /* start of the 0xc4 byte slot */
  struct {
    RwV3d axis_x;
    RwV3d axis_y;
    RwV3d axis_z;
  } frame;
  
  slot_off = slot_or_colour * 0xc4;
  slot_base = (char *)&g_btb_panel_658b50 + slot_off;
  frame.axis_x.x = 1.0;
  frame.axis_x.y = 0.0;
  frame.axis_x.z = 0.0;
  frame.axis_y.x = 0.0;
  frame.axis_y.y = 1.0;
  frame.axis_y.z = 0.0;
  frame.axis_z.x = 0.0;
  frame.axis_z.y = 0.0;
  frame.axis_z.z = 1.0;
  if (*(int *)((char *)&g_btb_panel_658c10 + slot_off) != 0) {
    RwFrameTransform(*(RwFrame **)((char *)&g_btb_panel_658c00 + slot_off),
                     (RwMatrix *)((char *)&g_btb_panel_658b50 + slot_off),rwCOMBINEREPLACE);
    frame_ptr = *(RwFrame **)(*(char **)((char *)&g_btb_panel_658ba0 + slot_off) + 4);
    RwFrameRotate(frame_ptr,&frame.axis_z,*(RwReal *)((char *)&g_btb_panel_658bf8 + slot_off),rwCOMBINEREPLACE);
    RwFrameRotate(frame_ptr,&frame.axis_y,*(RwReal *)((char *)&g_btb_panel_658bf4 + slot_off),rwCOMBINEPOSTCONCAT);
    RwFrameRotate(frame_ptr,&frame.axis_x,*(RwReal *)((char *)&g_btb_panel_658bf0 + slot_off),rwCOMBINEPOSTCONCAT);
    angle_ptr = (RwReal *)((char *)&g_btb_panel_658bd0 + slot_off);
    record_ptr = 4;
    do {
      frame_ptr = *(RwFrame **)(*(int *)&angle_ptr[-0x10] + 4);
      RwFrameRotate(frame_ptr,&frame.axis_x,*angle_ptr,rwCOMBINEREPLACE);
      RwFrameRotate((RwFrame *)RwFrame_object_parent(frame_ptr),&frame.axis_y,angle_ptr[4],rwCOMBINEREPLACE);
      angle_ptr = angle_ptr + 1;
      record_ptr = record_ptr + -1;
    } while (record_ptr != 0);
    if (*(int *)((char *)&g_btb_panel_658b90 + slot_off) != 0) {
      record_ptr = **(int **)(*(char **)(*(char **)((char *)&g_btb_panel_658b90 + slot_off) + 0x18) + 0x20);
      old_colour = *(undefined4 *)(record_ptr + 4);
      alpha_byte = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      slot_or_colour = (slot_or_colour & 0xff000000) | ((uint)((undefined3)old_colour) & 0xffffff);
      slot_or_colour = CONCAT13(alpha_byte,(undefined3)slot_or_colour);
      *(int *)(record_ptr + 4) = slot_or_colour;
    }
    if (*(int *)((char *)&g_btb_panel_658b94 + slot_off) != 0) {
      record_ptr = **(int **)(*(char **)(*(char **)((char *)&g_btb_panel_658b94 + slot_off) + 0x18) + 0x20);
      old_colour = *(undefined4 *)(record_ptr + 4);
      alpha_byte = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      slot_or_colour = (slot_or_colour & 0xff000000) | ((uint)((undefined3)old_colour) & 0xffffff);
      slot_or_colour = CONCAT13(alpha_byte,(undefined3)slot_or_colour);
      *(int *)(record_ptr + 4) = slot_or_colour;
    }
    if (*(int *)((char *)&g_btb_panel_658b98 + slot_off) != 0) {
      record_ptr = **(int **)(*(char **)(*(char **)((char *)&g_btb_panel_658b98 + slot_off) + 0x18) + 0x20);
      old_colour = *(undefined4 *)(record_ptr + 4);
      alpha_byte = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      slot_or_colour = (slot_or_colour & 0xff000000) | ((uint)((undefined3)old_colour) & 0xffffff);
      slot_or_colour = CONCAT13(alpha_byte,(undefined3)slot_or_colour);
      *(int *)(record_ptr + 4) = slot_or_colour;
    }
    if (*(int *)((char *)&g_btb_panel_658b9c + slot_off) != 0) {
      record_ptr = **(int **)(*(char **)(*(char **)((char *)&g_btb_panel_658b9c + slot_off) + 0x18) + 0x20);
      old_colour = *(undefined4 *)(record_ptr + 4);
      alpha_byte = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      slot_or_colour = (slot_or_colour & 0xff000000) | ((uint)((undefined3)old_colour) & 0xffffff);
      slot_or_colour = CONCAT13(alpha_byte,(undefined3)slot_or_colour);
      *(int *)(record_ptr + 4) = slot_or_colour;
    }
    if (*(int *)((char *)&g_btb_panel_658ba0 + slot_off) != 0) {
      record_ptr = **(int **)(*(char **)(*(char **)((char *)&g_btb_panel_658ba0 + slot_off) + 0x18) + 0x20);
      old_colour = *(undefined4 *)(record_ptr + 4);
      alpha_byte = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      slot_or_colour = (slot_or_colour & 0xff000000) | ((uint)((undefined3)old_colour) & 0xffffff);
      slot_or_colour = CONCAT13(alpha_byte,(undefined3)slot_or_colour);
      *(int *)(record_ptr + 4) = slot_or_colour;
    }
    if (*(int *)((char *)&g_btb_panel_658ba4 + slot_off) != 0) {
      record_ptr = **(int **)(*(char **)(*(char **)((char *)&g_btb_panel_658ba4 + slot_off) + 0x18) + 0x20);
      old_colour = *(undefined4 *)(record_ptr + 4);
      alpha_byte = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      slot_or_colour = (slot_or_colour & 0xff000000) | ((uint)((undefined3)old_colour) & 0xffffff);
      slot_or_colour = CONCAT13(alpha_byte,(undefined3)slot_or_colour);
      *(int *)(record_ptr + 4) = slot_or_colour;
    }
    if (*(int *)((char *)&g_btb_panel_658ba8 + slot_off) != 0) {
      slot_off = **(int **)(*(char **)(*(char **)((char *)&g_btb_panel_658ba8 + slot_off) + 0x18) + 0x20);
      old_colour = *(undefined4 *)(slot_off + 4);
      alpha_byte = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      slot_or_colour = (slot_or_colour & 0xff000000) | ((uint)((undefined3)old_colour) & 0xffffff);
      slot_or_colour = CONCAT13(alpha_byte,(undefined3)slot_or_colour);
      *(int *)(slot_off + 4) = slot_or_colour;
    }
  }
  return;
}


