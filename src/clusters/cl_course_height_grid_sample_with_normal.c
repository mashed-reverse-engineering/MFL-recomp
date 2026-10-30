// cluster-entries: course_height_grid_sample_with_normal
/* Installed cluster course_height_grid_sample_with_normal (tools/cluster_install.py from output/cluster_pilot_j1/course_height_grid_sample_with_normal.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* Height query on the course height grid: a grid of cells (each cell a 25 x 25 height map packed in 9 bit steps) laid over the course. */
typedef struct HeightQuery {
    float x;
    float height;          /* out */
    float z;
    float *normal;         /* out, optional (NULL = not wanted) */
} HeightQuery;

/*006c2884*/ extern char **g_car_6c2884;       /* cell table, g_car_6c28b4 x g_car_6c28b0 pointers (NULL = no cell) */
/*006c2894*/ extern float g_car_6c2894;        /* grid origin x */
/*006c289c*/ extern float g_car_6c289c;        /* grid origin z */
/*006c28a8*/ extern float g_car_6c28a8;        /* cells per unit x */
/*006c28ac*/ extern float g_car_6c28ac;        /* cells per unit z */
/*006c28b0*/ extern int g_car_6c28b0;          /* cell rows (z) */
/*006c28b4*/ extern int g_car_6c28b4;          /* cell columns (x) */
/*005dd364*/ extern float k_f_n1;
/*005e64a4*/ extern float k_f_0;
/*004d5290*/ extern RwReal RwV3dNormalize(RwV3d *out, RwV3d *in);

#define CF(off) (*(float *)(cell + (off)))
#define CI(off) (*(int *)(cell + (off)))
/* height at map position (i, j): base - sample * (1/511) * scale */
#define CELL_H_SET(out, i, j) { float s_ = (float)(*(unsigned int *)(cell + ((i) * 0x19 + (j)) * 4) >> 0x17); s_ = s_ * 0.0019569471f; s_ = s_ * CF(0xa0c); out = CF(0x9f8) - s_; }      /* one product per statement: cl would multiply sample * scale first */

/* rounding point: the original stores the value to a stack float and reloads it */
static float float_barrier(float v) { return v; }

int course_height_grid_sample_with_normal(HeightQuery *q)
{
    float x = q->x, z = q->z;
    int cx, cz, ix, iz, nx, nz;
    char *cell;
    float x0, z0, gx, gz, fx, fz, h00, h0z, height;
    double hx0;      /* the only height the original keeps unrounded */
    RwV3d dx, dz;

    cx = (int)((x - g_car_6c2894) * g_car_6c28a8 * (float)g_car_6c28b4);
    cz = (int)((z - g_car_6c289c) * g_car_6c28ac * (float)g_car_6c28b0);
    if (cx >= g_car_6c28b4 || cz >= g_car_6c28b0 || cx < 0 || cz < 0) return 0;
    cell = g_car_6c2884[g_car_6c28b4 * cz + cx];
    if (cell == 0) return 0;
    x0 = CF(0xa00);
    z0 = CF(0xa08);
    ix = (int)((x - x0) * CF(0x9dc) * (float)(CI(0x9c4) - 1) + 0.5f);
    iz = (int)((z - z0) * CF(0x9e0) * (float)(CI(0x9c8) - 1) + 0.5f);
    gx = (float)ix * CF(0x9e4) + x0;
    fx = (x - gx) * CF(0x9ec);
    gz = (float)iz * CF(0x9e8) + z0;
    fz = (z - gz) * CF(0x9f0);
    if (!(fx > k_f_0)) { nx = ix - 1; fx = fx * k_f_n1; } else nx = ix + 1;
    if (!(fz > k_f_0)) { fz = fz * k_f_n1; nz = iz - 1; } else nz = iz + 1;
    CELL_H_SET(h00, ix, iz); h00 = float_barrier(h00);
    CELL_H_SET(h0z, ix, nz); h0z = float_barrier(h0z);
    CELL_H_SET(hx0, nx, iz);
    dx.x = ((float)nx * CF(0x9e4) + x0) - gx;
    dx.y = hx0 - h00;
    dx.z = gz - *(volatile float *)&gz;      /* 0, or NaN for an infinite gz: the original subtracts two loads */
    dz.x = gx - *(volatile float *)&gx;
    dz.y = h0z - h00;
    dz.z = float_barrier((float)nz * CF(0x9e8) + z0) - gz;
    height = dx.y * fx + h00;      /* source order of the original: (dx.y * fx + h00) + dz.y * fz */
    height = height + dz.y * fz;
    if (q->normal != 0) {
        RwV3dNormalize(&dx, &dx);
        RwV3dNormalize(&dz, &dz);
        q->normal[0] = dz.z * dx.y - dz.y * dx.z;
        q->normal[1] = dz.x * dx.z - dz.z * dx.x;
        q->normal[2] = dz.y * dx.x - dz.x * dx.y;
    }
    q->height = height;
    return 2;
}

