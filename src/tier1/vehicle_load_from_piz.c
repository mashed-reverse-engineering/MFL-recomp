#include "mfl_tier2.h"
/*004231b0*/ extern int vehicle_model_path();
/*00423320*/ extern int vehicle_load();
/*0042fa80*/ extern char *course_content_path_build();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004a4440*/ extern int game_close_piz();


/* vehicle_load_from_piz */

/* "(model): opens the vehicle piz of vehicle_model_path and loads the vehicle (FUN_00423320)" */

void vehicle_load_from_piz(undefined4 param_1)
{
  undefined4 uVar1;
  char *_Str;
  
  uVar1 = vehicle_model_path(param_1);
  _Str = (char *)course_content_path_build(uVar1);
  game_open_piz(_Str);
  vehicle_load(0,param_1);
  game_close_piz();
  return;
}
