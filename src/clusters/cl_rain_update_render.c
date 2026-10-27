// cluster-entries: rain_update_render
/* Installed cluster rain_update_render (tools/cluster_install.py from output/cluster_pilot_k/rain_update_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd374*/ extern float k_f_0_2;
/*005de490*/ extern float k_f_0_02;
/*005e0b24*/ extern float k_f_75;
/*0076e3e8*/ extern undefined4 g_app_76e3e8;
/*0076e5a0*/ extern undefined4 g_app_76e5a0;
/*0076e5a4*/ extern undefined4 g_rain_update_76e5a4;
/*0076e5a8*/ extern undefined4 g_app_76e5a8;
/*0076e5ac*/ extern undefined4 g_app_76e5ac;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*00885820*/ extern undefined4 g_app_885820;
/* one rain drop of the pool (0x20 bytes): alive flag, age in frames, position, velocity */
typedef struct RainDrop { int active; int age; float x, y, z; float vx, vy, vz; } RainDrop;
#define RAIN_DROP_COUNT 0x200
#define RAIN_DROP_LIFETIME 0x40 /* frames */

/* rain_update_render */

/* "updates the rain particles visible in the camera frustum" */

static int rain_emit_random_r(int p0, int p1);
extern void rain_emit_random(void);
void rain_update_render(RwCamera *camera)

{
  uint *puVar1;
  float fVar2;
  void *pvVar3;
  RainDrop *drop;
  int age;
  RwFrustumTestResult sphere_visibility;
  int iVar6;
  float *pfVar7;
  int iVar8;
  RwReal length;
  struct {
    char *local_24;
    int local_20;
    float *local_1c;
    int local_18;
    float *local_14;
    int local_10;
    RwV3d local_c;
  } frame;
  
  iVar8 = g_app_885820;
  iVar6 = 0;
  if ((g_rain_update_76e5a4 != 0) && (0 < (int)g_app_76e5a0)) {
    pfVar7 = (float *)&g_app_76e3e8;
    do {
      pvVar3 = RwCamera_object_object_parent(camera);
      frame.local_c.x = pfVar7[-2] - *(float *)((char *)pvVar3 + 0x40);
      frame.local_c.y = pfVar7[-1] - *(float *)((char *)pvVar3 + 0x44);
      frame.local_c.z = *pfVar7 - *(float *)((char *)pvVar3 + 0x48);
      length = RwV3dLength(&frame.local_c);
      if ((length < k_f_75) &&
         (sphere_visibility = RwCameraFrustumTestSphere(camera,(RwSphere *)(pfVar7 + 8)),
         sphere_visibility == rwSPHEREINSIDE)) {
        rain_emit_random_r(iVar8,(int)(pfVar7 + -2));
      }
      iVar6 = iVar6 + 1;
      pfVar7 = pfVar7 + 0xe;
    } while (iVar6 < (int)g_app_76e5a0);
  }
  g_rain_update_76e5a4 = g_rain_update_76e5a4 + 1;
  g_app_76e5a8 = 0;
  RpPTankAtomicLock_00545d10(g_app_76e5ac,&frame.local_1c,1,0x40000000);
  RpPTankAtomicLock_00545d10(g_app_76e5ac,&frame.local_24,2,0x40000000);
  RpPTankAtomicLock_00545d10(g_app_76e5ac,&frame.local_14,4,0x40000000);
  if (((frame.local_1c != (float *)0x0) && (frame.local_24 != (char *)0x0)) && (frame.local_14 != (float *)0x0)) {
    drop = (RainDrop *)iVar8;
    for (iVar8 = RAIN_DROP_COUNT; iVar8 != 0; iVar8--, drop++) {
      if (drop->active == 1) {
        age = drop->age;
        fVar2 = drop->x;
        drop->x = drop->vx + fVar2;
        drop->age = age + 1;
        drop->y = drop->vy + drop->y;
        drop->z = drop->vz + drop->z;
        if (age + 1 < RAIN_DROP_LIFETIME) {
          *frame.local_1c = drop->vx + fVar2;
          frame.local_1c[1] = drop->y;
          frame.local_1c[2] = drop->z;
          frame.local_1c = (float *)((int)frame.local_1c + frame.local_18);
          *frame.local_24 = (char)(drop->age / 2) + -0x40;
          frame.local_24[1] = (char)(drop->age / 2) + '@';
          frame.local_24[2] = '\b';
          iVar6 = drop->age * -4 + 0xff;
          if (iVar6 < 0) {
            iVar6 = 0;
          }
          else if (0xff < iVar6) {
            iVar6 = 0xff;
          }
          frame.local_24[3] = (char)iVar6;
          frame.local_24 = frame.local_24 + frame.local_20;
          fVar2 = (float)drop->age * k_f_0_02 + k_f_0_2;
          *frame.local_14 = fVar2;
          frame.local_14[1] = fVar2;
          frame.local_14 = (float *)((int)frame.local_14 + frame.local_10);
          g_app_76e5a8 = g_app_76e5a8 + 1;
        }
        else {
          drop->active = 0;
          drop->age = 0;
        }
      }
    }
    RpPTankAtomicUnlock_00545f20(g_app_76e5ac);
  }
  iVar8 = g_app_76e5ac;
  puVar1 = (uint *)(*(char **)(g_ptank_plugin_offset + g_app_76e5ac) + 0x40);
  *puVar1 = *puVar1 | 0x800000;
  *(int *)(*(char **)(g_ptank_plugin_offset + iVar8) + 4) = g_app_76e5a8;
  return;
}

/* rain_emit_random_r: C -> original rain_emit_random (registers eax,edi) */
__declspec(naked) int rain_emit_random_r(int p0, int p1)
{
    __asm {
        push edi
        mov eax, dword ptr [esp + 8]
        mov edi, dword ptr [esp + 12]
        call rain_emit_random
        pop edi
        ret
    }
}
