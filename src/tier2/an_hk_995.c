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

void btb_panel_slot_update(int param_1)

{
  RwFrame *pRVar1;
  undefined4 uVar2;
  undefined1 uVar3;
  int iVar4;
  int iVar5;
  RwReal *pRVar6;
  char *slot_base;      /* start of the 0xc4 byte slot */
  struct {
    RwV3d local_24;
    RwV3d local_18;
    RwV3d local_c;
  } frame;
  
  iVar5 = param_1 * 0xc4;
  slot_base = (char *)&g_btb_panel_658b50 + iVar5;
  frame.local_24.x = 1.0;
  frame.local_24.y = 0.0;
  frame.local_24.z = 0.0;
  frame.local_18.x = 0.0;
  frame.local_18.y = 1.0;
  frame.local_18.z = 0.0;
  frame.local_c.x = 0.0;
  frame.local_c.y = 0.0;
  frame.local_c.z = 1.0;
  if (*(int *)((char *)&g_btb_panel_658c10 + iVar5) != 0) {
    RwFrameTransform(*(RwFrame **)((char *)&g_btb_panel_658c00 + iVar5),
                     (RwMatrix *)((char *)&g_btb_panel_658b50 + iVar5),rwCOMBINEREPLACE);
    pRVar1 = *(RwFrame **)(*(int *)((char *)&g_btb_panel_658ba0 + iVar5) + 4);
    RwFrameRotate(pRVar1,&frame.local_c,*(RwReal *)((char *)&g_btb_panel_658bf8 + iVar5),rwCOMBINEREPLACE);
    RwFrameRotate(pRVar1,&frame.local_18,*(RwReal *)((char *)&g_btb_panel_658bf4 + iVar5),rwCOMBINEPOSTCONCAT);
    RwFrameRotate(pRVar1,&frame.local_24,*(RwReal *)((char *)&g_btb_panel_658bf0 + iVar5),rwCOMBINEPOSTCONCAT);
    pRVar6 = (RwReal *)((char *)&g_btb_panel_658bd0 + iVar5);
    iVar4 = 4;
    do {
      pRVar1 = *(RwFrame **)(*(int *)&pRVar6[-0x10] + 4);
      RwFrameRotate(pRVar1,&frame.local_24,*pRVar6,rwCOMBINEREPLACE);
      RwFrameRotate((RwFrame *)RwFrame_object_parent(pRVar1),&frame.local_18,pRVar6[4],rwCOMBINEREPLACE);
      pRVar6 = pRVar6 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    if (*(int *)((char *)&g_btb_panel_658b90 + iVar5) != 0) {
      iVar4 = **(int **)(*(int *)(*(int *)((char *)&g_btb_panel_658b90 + iVar5) + 0x18) + 0x20);
      uVar2 = *(undefined4 *)(iVar4 + 4);
      uVar3 = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      param_1 = (param_1 & 0xff000000) | ((uint)((undefined3)uVar2) & 0xffffff);
      param_1 = CONCAT13(uVar3,(undefined3)param_1);
      *(int *)(iVar4 + 4) = param_1;
    }
    if (*(int *)((char *)&g_btb_panel_658b94 + iVar5) != 0) {
      iVar4 = **(int **)(*(int *)(*(int *)((char *)&g_btb_panel_658b94 + iVar5) + 0x18) + 0x20);
      uVar2 = *(undefined4 *)(iVar4 + 4);
      uVar3 = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      param_1 = (param_1 & 0xff000000) | ((uint)((undefined3)uVar2) & 0xffffff);
      param_1 = CONCAT13(uVar3,(undefined3)param_1);
      *(int *)(iVar4 + 4) = param_1;
    }
    if (*(int *)((char *)&g_btb_panel_658b98 + iVar5) != 0) {
      iVar4 = **(int **)(*(int *)(*(int *)((char *)&g_btb_panel_658b98 + iVar5) + 0x18) + 0x20);
      uVar2 = *(undefined4 *)(iVar4 + 4);
      uVar3 = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      param_1 = (param_1 & 0xff000000) | ((uint)((undefined3)uVar2) & 0xffffff);
      param_1 = CONCAT13(uVar3,(undefined3)param_1);
      *(int *)(iVar4 + 4) = param_1;
    }
    if (*(int *)((char *)&g_btb_panel_658b9c + iVar5) != 0) {
      iVar4 = **(int **)(*(int *)(*(int *)((char *)&g_btb_panel_658b9c + iVar5) + 0x18) + 0x20);
      uVar2 = *(undefined4 *)(iVar4 + 4);
      uVar3 = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      param_1 = (param_1 & 0xff000000) | ((uint)((undefined3)uVar2) & 0xffffff);
      param_1 = CONCAT13(uVar3,(undefined3)param_1);
      *(int *)(iVar4 + 4) = param_1;
    }
    if (*(int *)((char *)&g_btb_panel_658ba0 + iVar5) != 0) {
      iVar4 = **(int **)(*(int *)(*(int *)((char *)&g_btb_panel_658ba0 + iVar5) + 0x18) + 0x20);
      uVar2 = *(undefined4 *)(iVar4 + 4);
      uVar3 = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      param_1 = (param_1 & 0xff000000) | ((uint)((undefined3)uVar2) & 0xffffff);
      param_1 = CONCAT13(uVar3,(undefined3)param_1);
      *(int *)(iVar4 + 4) = param_1;
    }
    if (*(int *)((char *)&g_btb_panel_658ba4 + iVar5) != 0) {
      iVar4 = **(int **)(*(int *)(*(int *)((char *)&g_btb_panel_658ba4 + iVar5) + 0x18) + 0x20);
      uVar2 = *(undefined4 *)(iVar4 + 4);
      uVar3 = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      param_1 = (param_1 & 0xff000000) | ((uint)((undefined3)uVar2) & 0xffffff);
      param_1 = CONCAT13(uVar3,(undefined3)param_1);
      *(int *)(iVar4 + 4) = param_1;
    }
    if (*(int *)((char *)&g_btb_panel_658ba8 + iVar5) != 0) {
      iVar5 = **(int **)(*(int *)(*(int *)((char *)&g_btb_panel_658ba8 + iVar5) + 0x18) + 0x20);
      uVar2 = *(undefined4 *)(iVar5 + 4);
      uVar3 = ((int)(*(float *)(slot_base + 0xbc) * 255.0f));
      param_1 = (param_1 & 0xff000000) | ((uint)((undefined3)uVar2) & 0xffffff);
      param_1 = CONCAT13(uVar3,(undefined3)param_1);
      *(int *)(iVar5 + 4) = param_1;
    }
  }
  return;
}


