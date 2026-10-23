// cluster-entries: spring_grid_update
/* Installed cluster spring_grid_update (tools/cluster_install.py from output/cluster_pilot_j1/spring_grid_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/*0088a860*/ extern struct SpringNode g_spring_nodes[460];
/*0048a680*/ extern void spring_grid_prepare(void);
/*005de490*/ extern float k_f_0_02;
/*006f2ab4*/ extern unsigned int *g_spring_grid_6f2ab4;      /* destination of the 4x4 matrix copy */
/*006f2ad8*/ extern float g_spring_grid_6f2ad8;               /* grid time */

#define GRID_ROWS 20
#define GRID_COLS 23

/* rounding point: the original stores the value to a stack float and reloads it (cl would keep the x87 register) */
static float float_barrier(float v) { return v; }

/* One grid node, explicit Euler step: velocity += force * weight * dt, position += velocity * dt.
   The original x87 code rounds some intermediates to float (stack temporaries) and keeps others in the register;
   float_barrier marks exactly the rounded ones, the double locals the unrounded ones. */
static void spring_node_step(struct SpringNode *n, float dt)
{
    float fx_dt, force_z, vel_x, vel_y, step_y, step_z;
    double fy_dt, fz_dt, vel_z;

    fx_dt = float_barrier((n->w2c * n->w04) * dt);
    force_z = float_barrier(n->w34 * n->w04);
    fy_dt = (n->w04 * n->w30) * dt;           /* unrounded */
    fz_dt = force_z * dt;                     /* unrounded product of the rounded force */
    vel_x = float_barrier(fx_dt + n->w14);
    n->w14 = vel_x;
    vel_y = float_barrier(fy_dt + n->w18);
    n->w18 = vel_y;
    vel_z = fz_dt + n->w1c;
    n->w1c = vel_z;
    n->w08 = vel_x * dt + n->w08;             /* x product unrounded */
    step_y = float_barrier(vel_y * dt);
    n->w0c = step_y + n->w0c;
    step_z = float_barrier(vel_z * dt);       /* z uses the unrounded velocity */
    n->w10 = step_z + n->w10;
}

void spring_grid_update(float dt, const unsigned int *matrix)
{
    int row, col, i;

    g_spring_grid_6f2ad8 = g_spring_grid_6f2ad8 + k_f_0_02;
    for (i = 0; i < 16; i++) g_spring_grid_6f2ab4[i] = matrix[i];
    spring_grid_prepare();
    for (row = 0; row < GRID_ROWS; row++)
        for (col = 0; col < GRID_COLS; col++)
            spring_node_step(&g_spring_nodes[row * GRID_COLS + col], dt);
}

