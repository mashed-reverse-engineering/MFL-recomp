// cluster-entries: course_mesh_triangle_normals_build
/* Installed cluster course_mesh_triangle_normals_build (tools/cluster_install.py from output/cluster_pilot11/course_mesh_triangle_normals_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS */
#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* course_mesh_triangle_normals_build */

/* WARNING: Unknown calling convention */
/* "builds the triangle normals (cross products) from the index list" */

void course_mesh_triangle_normals_build_n(unsigned short *in_edx, RwV3d *in_ecx, float *in_ebx, int param_4)

{
  float *pa;
  float *pb;
  float *pc;
  float ex;
  float ey;
  float ez;
  float d[2];
  char gsbuf[8];  /* a char array local makes cl /GS emit the security cookie of the original */
  float dz;
  unsigned short *idx;
  
  if (0 < param_4) {
    idx = in_edx + 2;
    do {
      pa = in_ebx + idx[0] * 3;
      pb = in_ebx + idx[-1] * 3;
      pc = in_ebx + idx[-2] * 3;
      ex = pa[0] - pb[0];
      ey = pa[1] - pb[1];
      ez = pa[2] - pb[2];
      d[0] = pc[0] - pb[0];
      d[1] = pc[1] - pb[1];
      dz = pc[2] - pb[2];
      gsbuf[0] = 0;
      in_ecx->x = dz * ey - d[1] * ez;
      in_ecx->y = d[0] * ez - dz * ex;
      in_ecx->z = d[1] * ex - d[0] * ey;
      RwV3dNormalize(in_ecx,in_ecx);
      idx = idx + 4;
      in_ecx = in_ecx + 1;
      param_4 = param_4 + -1;
    } while (param_4 != 0);
  }
  return;
}

/* entry thunk at the original address of course_mesh_triangle_normals_build: register inputs (edx,ecx,ebx,s0) -> natural C course_mesh_triangle_normals_build_n; ret 0 */
__declspec(naked) void course_mesh_triangle_normals_build(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ebx
        push ecx
        push edx
        call course_mesh_triangle_normals_build_n
        add esp, 16
        ret
    }
}
