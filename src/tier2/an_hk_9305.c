#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall FindResourceA(int, int, char *);
extern __declspec(dllimport) int __stdcall LoadResource(int, int);
extern __declspec(dllimport) int __stdcall LockResource(int);
extern __declspec(dllimport) int __stdcall SizeofResource(int, int);
/*004a9390*/ extern int app_get_app_7e7700();
/* resource_load */

/* "(id, type): FindResourceA / LoadResource helper, 0 on failure" */

undefined4 resource_load(ushort param_1,char *param_2,undefined4 *param_3,uint *param_4)

{
  int hModule;
  int hResInfo;
  int hResData;
  
  hModule = app_get_app_7e7700();
  hResInfo = FindResourceA(hModule,(int)(uint)param_1,param_2);
  if (hResInfo == 0) {
    return 0;
  }
  hResData = LoadResource(hModule,hResInfo);
  if (hResData == 0) {
    return 0;
  }
  if (param_4 != (uint *)0x0) {
    *param_4 = SizeofResource(hModule,hResInfo);
  }
  if (param_3 != (undefined4 *)0x0) {
    *param_3 = LockResource(hResData);
  }
  return 1;
}
