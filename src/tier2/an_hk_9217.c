#include "mfl_tier2.h"
/*0040d0f0*/ extern int __cdecl find_texture_dict_c(RwChar *p0);
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c8e0*/ extern float text_width();
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*00483130*/ extern int draw_sprite();
/*004832d0*/ extern int draw_sprite_uv(int *param_3, float param_4, float param_5, float param_6, float param_7, uint param_8, RwReal param_9, RwReal param_10, RwReal param_11, RwReal param_12, int param_13, int param_14);
/*004b2b48*/ extern int _ftol();
/*005dd334*/ extern float k_f_0_033333335;
/*005dd624*/ extern float k_f_0_0020833334;
/*005dd7f0*/ extern float k_f_30;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb50*/ extern float k_f_8;
/*005de7c0*/ extern float k_f_0_0015625;
/*005deb24*/ extern float k_f_382;
/*005deb28*/ extern float k_f_0_55625;
/*0060a5b0*/ extern undefined g_menu_page_33_bonus_features;
/*0069ca00*/ extern undefined4 g_menu_panel_state_69ca00;
/*0069ca04*/ extern undefined4 g_menu_69ca04;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*007d2128*/ extern char *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
static int menu_item_y_position_r(uint reg_eax, int reg_ecx);
extern void menu_item_y_position(void);
__declspec(naked) int menu_item_y_position_r(uint reg_eax, int reg_ecx)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call menu_item_y_position
        ret
    }
}

/* menu_panel_draw_69ca00 */

/* "panel with state flag g_menu_panel_state_69ca00 (fade counter 69ca04)" */

void menu_panel_draw_69ca00(void)

{
  short sVar1;
  undefined *puVar2;
  int iVar3;
  int *texture_dict;
  undefined4 uVar4;
  uint *puVar5;
  int reg_eax;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  uint uVar10;
  RwReal RVar11;
  RwReal RVar12;
  float fVar13;
  RwReal RVar14;
  float fVar15;
  RwReal RVar16;
  float fVar17;
  int iVar18;
  undefined4 uVar19;
  struct {
    uint uStack_20;
    float fStack_1c;
    int iStack_18;
  } frame;
  
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  iVar18 = g_menu_69ca04;
  if ((g_menu_panel_state_69ca00 == 0) && ((int)g_menu_69ca04 < 0x60)) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    return;
  }
  reg_eax = 4;
  if (g_menu_panel_state_69ca00 == 1) {
    puVar2 = (undefined *)g_menu_records[g_menu_current].item_list;
  }
  else {
    puVar2 = (undefined *)g_menu_records[g_menu_current + 1].item_list;
  }
  if (puVar2 == &g_menu_page_33_bonus_features) {
    reg_eax = 2;
  }
  iVar3 = menu_item_y_position_r(reg_eax,0x1e);
  frame.fStack_1c = (float)iVar3;
  frame.uStack_20 = iVar18 << 0x18;
  if (reg_eax != 0) {
    puVar5 = g_game_state.option_flags + (puVar2 == &g_menu_page_33_bonus_features) + 1;
    frame.iStack_18 = reg_eax;
    do {
      sVar1 = screen_width();
      fVar6 = (float)(int)sVar1 * k_f_0_55625;
      sVar1 = screen_height();
      fVar13 = (frame.fStack_1c - k_f_8) * (float)(int)sVar1 * k_f_0_0020833334;
      sVar1 = screen_width();
      fVar15 = (float)(int)sVar1 * k_f_0_025;
      sVar1 = screen_height();
      iVar3 = 0;
      iVar18 = 0;
      RVar16 = 1.0;
      RVar14 = 0.0;
      RVar12 = 0.0;
      RVar11 = 0.9999;
      fVar17 = (float)(int)sVar1 * k_f_0_033333335;
      fVar7 = fVar13;
      fVar8 = fVar15;
      fVar9 = fVar17;
      uVar10 = frame.uStack_20;
      texture_dict = (int *)find_texture_dict_c("Arrow");
      draw_sprite_uv(texture_dict,fVar6,fVar7,fVar8,fVar9,uVar10,RVar11,RVar12,
                     RVar14,RVar16,iVar18,iVar3);
      reg_eax = (*puVar5 != 0) + 0x5c;
      ui_rt2d_text_draw(reg_eax,0x43bc0000,*(int *)&frame.fStack_1c,frame.uStack_20,0x3f333333,0);
      iVar18 = (int)text_width(reg_eax,0x3f333333);
      sVar1 = screen_width();
      uVar19 = 1;
      fVar6 = ((float)iVar18 + k_f_382) * (float)(int)sVar1 * k_f_0_0015625;
      uVar10 = frame.uStack_20;
      uVar4 = find_texture_dict_c("Arrow");
      draw_sprite(uVar4,*(int *)&fVar6,*(int *)&fVar13,*(int *)&fVar15,*(int *)&fVar17,uVar10,uVar19);
      frame.fStack_1c = frame.fStack_1c + k_f_30;
      puVar5 = puVar5 + 1;
      frame.iStack_18 = frame.iStack_18 + -1;
    } while (frame.iStack_18 != 0);
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}


