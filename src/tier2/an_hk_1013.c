#include "mfl_tier2.h"
/*00434100*/ extern int __cdecl menu_icon_texture_lookup(undefined4 p0);
/*0043cda0*/ extern int menu_texture_dict_load();
/*004824a0*/ extern int draw_rect();
/*004832d0*/ extern int draw_sprite_uv(int *param_3, float param_4, float param_5, float param_6, float param_7, uint param_8, RwReal param_9, RwReal param_10, RwReal param_11, RwReal param_12, int param_13, int param_14);
/*00484180*/ extern int ui_draw_sprite_oriented(int texture, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 colour, int p6, undefined4 p7, undefined4 p8);
/*004b2b48*/ extern int _ftol();
/*005dd33c*/ extern float k_f_4;
/*005dd634*/ extern float k_f_2;
/*005dec48*/ extern float k_f_36;
/*005dec4c*/ extern float k_f_272;
/*005dec50*/ extern float k_f_182;
/*00607e68*/ extern undefined g_menu_layout_607e68;
/*00608138*/ extern undefined g_menu_layout_608138;
/*00608408*/ extern undefined g_menu_layout_608408;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cc30*/ extern undefined4 g_menu_layout_69cc30;
/*0069d664*/ extern undefined4 g_hud_69d664;
/*0069d688*/ extern undefined4 g_career_row;
/*0069d690*/ extern undefined4 g_menu_event_column;
/*007eebc0*/ extern struct GameState g_game_state;
/*008c86c0*/ extern undefined4 g_menu_layout_8c86c0;
/*007ef7f8*/ extern float g_menu_wave_time;
/* menu_layout_draw_b */

/* "builds and draws the 6 entry menu layout (variant b)" */

void menu_layout_draw_b(int param_1,int param_2)

{
  undefined4 uVar1;
  bool bVar2;
  uint uVar3;
  uint uVar4;
  int sprite_texture;
  int *piVar5;
  int iVar6;
  int iVar7;
  int slot;
  float pu_edi;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  RwReal RVar12;
  RwReal RVar13;
  RwReal RVar14;
  RwReal RVar15;
  RwReal RVar16;
  struct {
    undefined4 local_64;
    int local_60;
  } frame;
  struct {
    float local_50;
    float local_4c;
    float local_48;
    float local_44;
    undefined4 local_40 [16];
  } frame2;
  
  frame2.local_40[3] = 3;
  frame2.local_40[9] = 3;
  frame2.local_40[0xf] = 3;
  frame2.local_40[0] = 0;
  frame2.local_40[1] = 1;
  frame2.local_40[2] = 2;
  frame2.local_40[4] = 4;
  frame2.local_40[5] = 5;
  frame2.local_40[6] = 0;
  frame2.local_40[7] = 1;
  frame2.local_40[8] = 2;
  frame2.local_40[10] = 4;
  frame2.local_40[0xb] = 5;
  frame2.local_40[0xc] = 0;
  frame2.local_40[0xd] = 1;
  frame2.local_40[0xe] = 2;
  frame.local_64 = 0xffffff;
  frame2.local_50 = 330.0;
  frame2.local_4c = 96.0;
  frame2.local_48 = 270.0;
  frame2.local_44 = 180.0;
  if (param_2 != 0) {
    frame2.local_50 = 321.0;
    frame2.local_4c = 65.0;
    frame2.local_48 = 255.0;
    frame2.local_44 = 200.0;
  }
  uVar3 = ((int)(g_menu_wave_time * 60.0f));
  uVar3 = uVar3 & 0x1ff;
  if (uVar3 == 0x1e0) {
    g_menu_layout_69cc30 = g_menu_layout_69cc30 + 2;
  }
  else if (uVar3 == 0xe0) {
    g_hud_69d664 = g_hud_69d664 + 2;
  }
  if ((g_hud_69d664 & 1) == 0) {
    g_hud_69d664 = g_hud_69d664 + 1;
  }
  if (uVar3 < 0xe0) {
    iVar7 = 0;
LAB_0043d357:
    iVar6 = 0xff;
  }
  else if (uVar3 < 0x100) {
    iVar7 = uVar3 - 0xe0;
    if (iVar7 < 0x10) {
      iVar7 = iVar7 * 0x10;
      goto LAB_0043d357;
    }
    iVar6 = (0x1f - iVar7) * 0x10;
    iVar7 = 0xff;
  }
  else if (uVar3 < 0x1e0) {
    iVar6 = 0;
    iVar7 = 0xff;
  }
  else {
    iVar6 = uVar3 - 0x1e0;
    if (0xf < iVar6) {
      iVar7 = (0x1f - iVar6) * 0x10;
      goto LAB_0043d357;
    }
    iVar6 = iVar6 * 0x10;
    iVar7 = 0xff;
  }
  uVar1 = frame2.local_40[g_hud_69d664 & 0xf];
  if (iVar6 != 0) {
    iVar6 = iVar6 * param_1;
    uVar4 = uVar3 + 0x20;
    frame.local_64 = CONCAT13(((char)(iVar6 / 0xff) + (char)(iVar6 >> 0x1f)) -
                        (char)((longlong)iVar6 * 0x80808081 >> 0x3f),0xffffff);
    if (0x1ff < uVar4) {
      uVar4 = uVar3 - 0x1e0;
    }
    sprite_texture = menu_texture_dict_load(0);
    ui_draw_sprite_oriented(sprite_texture,*(int *)&frame2.local_50,*(int *)&frame2.local_4c,*(int *)&frame2.local_48,*(int *)&frame2.local_44,frame.local_64,(int)(uVar4 << 8) / 0x120, frame2.local_40[g_menu_layout_69cc30 & 0xf],0);
  }
  if (iVar7 != 0) {
    iVar7 = iVar7 * param_1;
    frame.local_64 = CONCAT13(((char)(iVar7 / 0xff) + (char)(iVar7 >> 0x1f)) -
                        (char)((longlong)iVar7 * 0x80808081 >> 0x3f),(undefined3)frame.local_64);
    sprite_texture = menu_texture_dict_load(1);
    ui_draw_sprite_oriented(sprite_texture,*(int *)&frame2.local_50,*(int *)&frame2.local_4c,*(int *)&frame2.local_48,*(int *)&frame2.local_44,frame.local_64,(int)((uVar3 - 0xe0) * 0x100) / 0x120, uVar1,0);
  }
  if (param_2 == 0) {
    frame.local_60 = param_1 << 0x18;
    frame2.local_50 = frame2.local_50 - k_f_2;
    frame2.local_4c = frame2.local_4c - k_f_2;
    draw_rect(*(int *)&frame2.local_50,*(int *)&frame2.local_4c,0x40000000,frame2.local_44 + k_f_4,frame.local_60);
    draw_rect(*(int *)&frame2.local_50,*(int *)&frame2.local_4c,0x43890000,0x40000000,frame.local_60);
    frame2.local_4c = frame2.local_4c + k_f_182;
    draw_rect(*(int *)&frame2.local_50,*(int *)&frame2.local_4c,0x43890000,0x40000000,frame.local_60);
    draw_rect(frame2.local_50 + k_f_272,frame2.local_4c - k_f_182,0x40000000,0x43360000,frame.local_60);
  }
  frame2.local_50 = 330.0;
  frame2.local_4c = 100.0;
  if (param_2 != 0) {
    frame2.local_50 = 322.0;
    frame2.local_4c = 76.0;
  }
  param_2 = g_career_row * 0xc;
  frame2.local_44 = 40.0;
  frame2.local_48 = 40.0;
  frame.local_64 = CONCAT13((char)param_1,(undefined3)frame.local_64);
  if (g_menu_event_column == 5) {
    param_2 = param_2 + 8;
    goto LAB_0043d5dc;
  }
  if (g_menu_event_column != 4) {
    if (g_menu_event_column == 8) {
      param_2 = param_2 + 8;
      goto LAB_0043d5dc;
    }
    if (g_menu_event_column != 7) {
      if (g_menu_event_column == 0xd) {
        param_2 = param_2 + 8;
        goto LAB_0043d5dc;
      }
      if (g_menu_event_column != 0xc) {
        if (g_menu_event_column == 2) {
          param_2 = param_2 + 8;
          goto LAB_0043d5dc;
        }
        if (g_menu_event_column != 1) goto LAB_0043d5dc;
      }
    }
  }
  param_2 = param_2 + 4;
LAB_0043d5dc:
  slot = 0;
  g_game_state.mirror_mode = 0;
  iVar6 = g_event_type;
  pu_edi = frame2.local_50;
  do {
    if (((iVar6 == 3) || (iVar6 == 4)) || (iVar6 == 5)) {
      iVar7 = *(int *)((char *)&g_menu_layout_607e68 + (param_2 + slot) * 4);
LAB_0043d730:
      if (iVar7 != 0) {
        if (slot == 3) {
          g_game_state.mirror_mode = 1;
        }
        iVar7 = 0;
        iVar6 = 1;
        RVar15 = 1.0;
        RVar14 = 0.0;
        RVar13 = 1.0;
        RVar12 = 0.0;
        fVar8 = pu_edi;
        fVar9 = frame2.local_4c;
        fVar10 = frame2.local_48;
        fVar11 = frame2.local_44;
        uVar3 = frame.local_64;
        piVar5 = (int *)menu_icon_texture_lookup(slot);
        draw_sprite_uv(piVar5,fVar8,fVar9,fVar10,fVar11,uVar3,RVar12,RVar13,RVar14,
                       RVar15,iVar6,iVar7);
        pu_edi = frame2.local_50 + k_f_36;
        iVar6 = g_event_type;
        frame2.local_50 = pu_edi;
      }
    }
    else {
      bVar2 = false;
      if (iVar6 == 0xe) {
        if (*(int *)((char *)&g_menu_layout_608138 + (param_2 + slot) * 4) != 0) {
          if (slot == 3) {
            g_game_state.mirror_mode = 1;
          }
          iVar7 = 0;
          iVar6 = 1;
          RVar15 = 1.0;
          RVar14 = 0.0;
          RVar13 = 1.0;
          RVar12 = 0.0;
          fVar8 = pu_edi;
          fVar9 = frame2.local_4c;
          fVar10 = frame2.local_48;
          fVar11 = frame2.local_44;
          uVar3 = frame.local_64;
          piVar5 = (int *)menu_icon_texture_lookup(slot);
          draw_sprite_uv(piVar5,fVar8,fVar9,fVar10,fVar11,uVar3,RVar12,RVar13,RVar14
                         ,RVar15,iVar6,iVar7);
          pu_edi = frame2.local_50 + k_f_36;
          iVar6 = g_event_type;
          frame2.local_50 = pu_edi;
        }
        bVar2 = true;
      }
      if (iVar6 == 0xf) {
        if (slot == 0) {
          iVar6 = 1;
          RVar16 = 1.0;
          RVar15 = 1.0;
          fVar8 = pu_edi;
          fVar9 = frame2.local_4c;
          fVar10 = frame2.local_48;
          fVar11 = frame2.local_44;
          uVar3 = frame.local_64;
          RVar12 = 0.0;
          RVar13 = 0.0;
          RVar14 = 0.0;
          piVar5 = (int *)menu_icon_texture_lookup(0);
          draw_sprite_uv(piVar5,fVar8,fVar9,fVar10,fVar11,uVar3,RVar12,RVar15,RVar13,
                         RVar16,iVar6,(int)RVar14);
          iVar6 = g_event_type;
        }
        bVar2 = true;
      }
      if (iVar6 == 0x10) {
        if (slot == 0) {
          iVar6 = 1;
          RVar16 = 1.0;
          RVar15 = 1.0;
          fVar8 = pu_edi;
          fVar9 = frame2.local_4c;
          fVar10 = frame2.local_48;
          fVar11 = frame2.local_44;
          uVar3 = frame.local_64;
          RVar12 = 0.0;
          RVar13 = 0.0;
          RVar14 = 0.0;
          piVar5 = (int *)menu_icon_texture_lookup(1);
          draw_sprite_uv(piVar5,fVar8,fVar9,fVar10,fVar11,uVar3,RVar12,RVar15,RVar13,
                         RVar16,iVar6,(int)RVar14);
          iVar6 = g_event_type;
        }
        bVar2 = true;
      }
      if ((iVar6 != 0x11) && (!bVar2)) {
        iVar7 = *(int *)((char *)&g_menu_layout_608408 + (param_2 + slot) * 4);
        goto LAB_0043d730;
      }
    }
    slot = slot + 1;
    if (3 < slot) {
      g_menu_layout_8c86c0 = g_menu_layout_8c86c0 + 0x20;
      return;
    }
  } while( true );
}


