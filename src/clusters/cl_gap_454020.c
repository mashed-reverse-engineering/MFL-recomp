// cluster-entries: gap_454020
/* Installed cluster gap_454020 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_454020.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*00486b50*/ extern int particles_add();
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d3cc0*/ extern RwMatrix * RwMatrixUpdate(RwMatrix *p0);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*005dd338*/ extern float k_f_0_01;
/*005de490*/ extern float k_f_0_02;
/*005df514*/ extern float k_f_1_15;
/*005df518*/ extern float k_f_1_225;
/*006a1860*/ extern undefined4 g_course_car_6a1860;
/*008c3f64*/ extern undefined4 g_course_8c3f64;
/*008c4440*/ extern undefined4 g_course_highway_8c4440;
/*008c4448*/ extern undefined4 g_course_highway_8c4448;
/* gap_454020 */

void gap_454020_n(int in_eax)

{
  struct {
    RwRGBA color;
    RwV3d axis;
    RwV3d translate;
    RwV3d scale;
    RwMatrix matrix;
  } fx;
  int slot;
  int iVar2;

  slot = *(int *)(in_eax + 0xac);
  fx.color.red = 0;
  fx.color.green = 0;
  fx.color.blue = 0;
  fx.color.alpha = 0xff;
  fx.axis.x = 1.0;
  fx.axis.y = 0.0;
  fx.axis.z = 0.0;
  iVar2 = *(int *)(*(int *)(in_eax + 0xa0) + 4);
  if (*(float *)(iVar2 + 0x44) - *(float *)&(&g_course_8c3f64)[slot * 3] <= k_f_0_01) {
    fx.scale.x = *(float *)((char *)((undefined *)&g_course_highway_8c4440) + slot * 0xc) * k_f_1_225;
    fx.scale.z = 1.0;
    fx.translate.x = 0.0;
    fx.scale.y = *(float *)((char *)((undefined *)&g_course_highway_8c4448) + slot * 0xc) * k_f_1_15;
    fx.translate.y = 0.0;
    fx.translate.z = *(float *)&(&g_course_8c3f64)[slot * 3] - k_f_0_02;
    RwMatrixTransform(&fx.matrix,(RwMatrix *)(iVar2 + 0x10),rwCOMBINEREPLACE);
    RwMatrixRotate(&fx.matrix,&fx.axis,90.0,rwCOMBINEPRECONCAT);
    RwMatrixTranslate(&fx.matrix,&fx.translate,rwCOMBINEPRECONCAT);
    RwMatrixScale(&fx.matrix,&fx.scale,rwCOMBINEPRECONCAT);
    RwMatrixUpdate(&fx.matrix);
    fx.color.alpha = 0xff;
    particles_set_color(&fx.color);
    gfx_set_fx_matrix(&fx.matrix);
    particles_add(g_course_car_6a1860);
  }
  return;
}

/* entry thunk at the original address of gap_454020: register inputs (eax) -> natural C gap_454020_n; ret 0 */
__declspec(naked) void gap_454020(void)
{
    __asm {
        push eax
        call gap_454020_n
        add esp, 4
        ret
    }
}
