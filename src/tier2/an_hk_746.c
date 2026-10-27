#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd40c*/ extern float k_f_1_5;
/*005de288*/ extern float k_f_1_125;
/*00659b30*/ extern undefined4 g_load_659b30;
/*00659ce0*/ extern char *g_load_659ce0;
/*00659cf8*/ extern undefined4 g_game_659cf8;
/*00659cfc*/ extern undefined4 g_game_659cfc;
static int hudpanel_set_icon_texture_per_part_r(void *reg_eax);
extern void hudpanel_set_icon_texture_per_part(void);
__declspec(naked) int hudpanel_set_icon_texture_per_part_r(void *reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_set_icon_texture_per_part
        ret
    }
}

static int time_trial_panel_times_update_r(void *reg_ebx);
extern void time_trial_panel_times_update(void);
__declspec(naked) int time_trial_panel_times_update_r(void *reg_ebx)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call time_trial_panel_times_update
        pop ebx
        ret
    }
}

/* rwMATRIXTYPEMASK (3, orthonormal) | rwMATRIXINTERNALIDENTITY (0x20000) */
#define MATRIX_FLAGS_ORTHONORMAL_IDENTITY (0x20003)
/* race phase whose placement is offset_phase7 with the 1.5 scale (per the original comment below) */
#define LOAD_INDICATOR_RACE_PHASE (7)

/* hudpanel_load_indicator_update */

/* "places the load indicator for phase 7 (0.35,0.2, scale 1.5) or otherwise (-0.375,-0.3, 1.125)"
    */
/* Positions the race load indicator frame (g_load_659ce0) for the current race phase, then refreshes the HUD
   parts that use g_load_659b30. Takes no parameters and returns nothing. Phase 7 gets offset_phase7 and scale
   1.5 (k_f_1_5); any other phase gets offset_default and scale 1.125 (k_f_1_125). */

void hudpanel_load_indicator_update(void)

{
  int race_phase;
  struct {
    RwV3d scale;
    RwV3d offset_default;
    RwV3d offset_phase7;
    RwMatrix matrix;
  } frame;
  
  frame.matrix.flags = frame.matrix.flags | MATRIX_FLAGS_ORTHONORMAL_IDENTITY;
  frame.offset_default.x = -0.375;
  frame.offset_default.y = -0.3;
  frame.offset_default.z = 1.0;
  frame.offset_phase7.x = 0.35;
  frame.offset_phase7.y = 0.2;
  frame.offset_phase7.z = 1.0;
  frame.scale.x = 1.0;
  frame.scale.y = 1.0;
  frame.scale.z = 1.0;
  g_game_659cfc = g_game_659cf8;
  frame.matrix.at.z = 1.0;
  frame.matrix.up.y = 1.0;
  frame.matrix.right.x = 1.0;
  frame.matrix.up.x = 0.0;
  frame.matrix.right.z = 0.0;
  frame.matrix.right.y = 0.0;
  frame.matrix.at.y = 0.0;
  frame.matrix.at.x = 0.0;
  frame.matrix.up.z = 0.0;
  frame.matrix.pos.z = 0.0;
  frame.matrix.pos.y = 0.0;
  frame.matrix.pos.x = 0.0;
  /* Choose the placement by race phase: LOAD_INDICATOR_RACE_PHASE uses offset_phase7 and the 1.5 scale. */
  race_phase = race_get_phase();
  if (race_phase == LOAD_INDICATOR_RACE_PHASE) {
    RwMatrixTranslate(&frame.matrix,&frame.offset_phase7,rwCOMBINEREPLACE);
    frame.scale.x = frame.scale.x * k_f_1_5;
    frame.scale.y = frame.scale.y * k_f_1_5;
    frame.scale.z = frame.scale.z * k_f_1_5;
  }
  else {
    RwMatrixTranslate(&frame.matrix,&frame.offset_default,rwCOMBINEREPLACE);
    frame.scale.x = frame.scale.x * k_f_1_125;
    frame.scale.y = frame.scale.y * k_f_1_125;
    frame.scale.z = frame.scale.z * k_f_1_125;
  }
  /* Apply the placement matrix to the indicator frame, then scale it by the chosen factor. */
  RwFrameTransform((struct RwFrame *)g_load_659ce0,(RwMatrix *)&frame.matrix,rwCOMBINEREPLACE);
  RwFrameScale((struct RwFrame *)g_load_659ce0,(struct RwV3d *)&frame.scale,rwCOMBINEPRECONCAT);
  /* Refresh the HUD parts that read g_load_659b30 (inferred from the callee names). */
  time_trial_panel_times_update_r((void *)&g_load_659b30);
  hudpanel_set_icon_texture_per_part_r((void *)&g_load_659b30);
  return;
}


