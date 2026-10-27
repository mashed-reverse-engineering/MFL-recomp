#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall FindResourceA(int, int, char *);
extern __declspec(dllimport) int __stdcall LoadResource(int, int);
extern __declspec(dllimport) int __stdcall LockResource(int);
extern __declspec(dllimport) int __stdcall SizeofResource(int, int);
/*004a9390*/ extern int app_get_app_7e7700();
/* resource_load */
/* "(id, type): FindResourceA / LoadResource helper, 0 on failure" */
undefined4 resource_load(ushort id,char *type,undefined4 *out_data,uint *out_size)
{
  int hModule;
  int hResInfo;
  int hResData;
  
  hModule = app_get_app_7e7700();
  hResInfo = FindResourceA(hModule,(int)(uint)id,type);
  if (hResInfo == 0) {
    return 0;
  }
  hResData = LoadResource(hModule,hResInfo);
  if (hResData == 0) {
    return 0;
  }
  if (out_size != (uint *)0x0) {
    *out_size = SizeofResource(hModule,hResInfo);
  }
  if (out_data != (undefined4 *)0x0) {
    *out_data = LockResource(hResData);
  }
  return 1;
}
