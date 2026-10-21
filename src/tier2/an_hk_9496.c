#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*005dfcf4*/ extern float k_f_21;
/*005dfe00*/ extern float k_f_416;
/*007d2128*/ extern char *RwEngineInstance;
/*007ef7f8*/ extern float g_menu_wave_time;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
extern double sin(double);
/* build_info_display_draw_wave_grid */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "draws the animated sine wave distorted tile grid of the build info backdrop" */

void build_info_display_draw_wave_grid(void)

{
  int iVar1;
  float10 fVar2;
  float10 fVar3;
  float10 fVar4;
  float10 fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  short sVar14;
  RwReal *pRVar15;
  uint uVar16;
  int iVar17;
  uint uVar18;
  float10 fVar19;
  float10 fVar20;
  float10 fVar21;
  float10 fVar22;
  float10 fVar23;
  struct {
    int local_40;
    uint local_3c;
    undefined1 pad_38[6];
    ushort uStack_32;
  } frame;
  float local_24;
  float local_1c;
  float local_14;
  float local_c;
  float x0f;
  float x1f;
  float x2f;
  float x3f;
  float s1r;
  float i21f;
  
  frame.local_3c = 0xff808080;
  frame.local_40 = 0;
  do {
    uVar18 = 0;
    do {
      iVar17 = 0;
      do {
        iVar1 = (uVar18 & 1) * 0x15 + (iVar17 * 5 + 0x2d) * 8;
        fVar19 = (float10)(int)(uVar18 * 0x15);
        if (frame.local_40 != 0) {
          fVar19 = fVar19 + (float10)k_f_416;
        }
        fVar2 = (float10)k_f_21;
        fVar6 = g_menu_wave_time + g_menu_wave_time;
        fVar20 = (float10)sin((float10)(int)(iVar17 + uVar18) + (float10)fVar6);
        fVar3 = (float10)(4.0);
        fVar21 = (float10)sin((float10)(int)(uVar18 + 1 + iVar17) + (float10)fVar6);
        fVar4 = (float10)(4.0);
        fVar22 = (float10)sin((float10)(int)(uVar18 + 2 + iVar17) + (float10)fVar6);
        fVar5 = (float10)(4.0);
        fVar23 = (float10)sin((float10)(int)(iVar17 + uVar18 * 2) + (float10)fVar6);
        local_14 = (float)(fVar23 + fVar23 + fVar19);
        fVar23 = (float10)sin((float10)(int)(iVar17 + 1 + uVar18 * 2) + (float10)fVar6);
        local_c = (float)(fVar23 + fVar23 + fVar19);
        fVar23 = (float10)sin((float10)(int)(iVar17 + 2 + uVar18 * 2) + (float10)fVar6);
        local_24 = (float)(fVar23 + fVar23 + (float10)(float)(fVar19 + (float10)k_f_21));
        fVar23 = (float10)sin((float10)(int)(iVar17 + 3 + uVar18 * 2) + (float10)fVar6);
        local_1c = (float)(fVar23 + fVar23 + (float10)(float)(fVar19 + (float10)k_f_21));
        if (frame.local_40 == 0) {
          if (uVar18 == 2) {
            local_1c = 64.0f;
            local_24 = 64.0f;
          }
        }
        else if (uVar18 == 0) {
          local_c = 416.0f;
          local_14 = 416.0f;
        }
        frame.uStack_32 = 0xff80;
        uVar16 = frame.local_3c;
        if ((iVar17 == 0) && ((uVar18 & 1) == 0)) {
          uVar16 = 0;
          frame.uStack_32 = 0;
        }
        *(volatile float *)&x0f = fVar20 * fVar3 + (float10)iVar1;
        *(volatile float *)&s1r = fVar21 * fVar4;
        *(volatile float *)&i21f = (float10)iVar1 + fVar2;
        *(volatile float *)&x1f = fVar21 * fVar4 + ((float10)iVar1 + fVar2);
        *(volatile float *)&x2f = *(volatile float *)&s1r + (float10)iVar1;
        *(volatile float *)&x3f = fVar22 * fVar5 + *(volatile float *)&i21f;
        sVar14 = screen_width();
        fVar6 = (float)(int)sVar14 * *(volatile float *)&x0f * k_f_0_0015625;
        sVar14 = screen_height();
        fVar7 = (float)(int)sVar14 * local_14 * k_f_0_0020833334;
        sVar14 = screen_width();
        fVar8 = (float)(int)sVar14 * *(volatile float *)&x1f * k_f_0_0015625;
        sVar14 = screen_height();
        fVar9 = (float)(int)sVar14 * local_c * k_f_0_0020833334;
        sVar14 = screen_width();
        fVar10 = (float)(int)sVar14 * *(volatile float *)&x2f * k_f_0_0015625;
        sVar14 = screen_height();
        fVar11 = (float)(int)sVar14 * local_24 * k_f_0_0020833334;
        sVar14 = screen_width();
        fVar12 = (float)(int)sVar14 * *(volatile float *)&x3f * k_f_0_0015625;
        sVar14 = screen_height();
        g_im2d_vertices[3].y = (float)(int)sVar14 * local_1c * k_f_0_0020833334;
        g_im2d_vertices[0].emissiveColor =
             (((uint)(frame.uStack_32 >> 8) << 8 | uVar16 & 0xff) << 8 | uVar16 >> 8 & 0xff) << 8 |
             frame.uStack_32 & 0xff;
        g_im2d_vertices[1].emissiveColor = 0xff808080;
        g_im2d_vertices[3].emissiveColor = 0xff808080;
        pRVar15 = &g_im2d_vertices[0].rhw;
        g_im2d_vertices[0].x = fVar6;
        g_im2d_vertices[0].y = fVar7;
        g_im2d_vertices[1].x = fVar8;
        g_im2d_vertices[1].y = fVar9;
        g_im2d_vertices[2].x = fVar10;
        g_im2d_vertices[2].y = fVar11;
        g_im2d_vertices[2].emissiveColor = g_im2d_vertices[0].emissiveColor;
        g_im2d_vertices[3].x = fVar12;
        do {
          pRVar15[-1] = RwIm2DGetNearScreenZ();
          *pRVar15 = 1.0f;
          pRVar15 = pRVar15 + 7;
        } while ((int)pRVar15 < (int)&g_im2d_vertices + 0x7c);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
        RwRenderStateSet
                  (rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
        RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)0x5);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)0x6);
        RwIm2DRenderPrimitive
                  (rwPRIMTYPETRISTRIP,g_im2d_vertices,4);
        iVar17 = iVar17 + 1;
      } while (iVar17 < 7);
      uVar18 = uVar18 + 1;
    } while ((int)uVar18 < 3);
    frame.local_40 = frame.local_40 + 1;
  } while (frame.local_40 < 2);
  return;
}


