// cluster-entries: car_points_vs_dynamic_objects
/* Installed cluster car_points_vs_dynamic_objects (tools/cluster_install.py from output/cluster_pilot_j1/car_points_vs_dynamic_objects.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* Collision objects of the sample world (triangles of 0x90 bytes) against the 18 collision points of a car. */
typedef struct DynTriangle {
    float corner[3][3];          /* [0..8] */
    float normal[3];             /* [9..11] */
    int pad_30[2];
    int owner;                   /* [14] car id this triangle belongs to */
    float mass;                  /* [15] */
    float push[3];               /* [16..18] */
    float impulse[3];            /* [19..21] accumulated impulse */
    float hit_point[3];          /* [22..24] */
    int hit;                     /* [25] 1 = touched this frame */
    int *info;                   /* [26] object info (position handle at +0xc) */
    int pad_6c[9];
} DynTriangle;

/*00496ed0*/ extern DynTriangle *smplfzx_get_71c1e0(void);
/*00496ec0*/ extern unsigned int particles_get_particles_72db20(void);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*00496f80*/ extern void smplfzx_object_get_position(int handle, RwV3d *out);
/*005dd364*/ extern float k_f_n1;
/*005ddc28*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005dfd68*/ extern float k_f_3_6;
/*005ddb9c*/ extern float k_f_n10;
/*005de290*/ extern float k_f_100000;
/*005de274*/ extern float k_f_n0_4;

void car_points_vs_dynamic_objects(float *car)
{
    DynTriangle *tri;
    unsigned int tri_index, count;
    RwMatrix *body_matrix;
    RwV3d spin_vel, to_obj;
    RwV3d *contact;
    float *point, *point_state;
    int point_index;
    float depth, speed, cos_in, px, py, pz;
    float ex, ey, ez, ux, uy, uz;
    float v0x, v0y, v0z, v1x, v1y, v1z, v2x, v2y, v2z, nx, ny, nz;
    float ex2, ey2, ez2, tmp;

    body_matrix = (RwMatrix *)(car + (int)car[0x26b] * 0x10 + 0x24a);
    tri = smplfzx_get_71c1e0();
    tri_index = 0;
    if (particles_get_particles_72db20() != 0) {
        do {
            if (tri->owner == *(int *)car) {
                v0x = tri->corner[0][0]; v0y = tri->corner[0][1]; v0z = tri->corner[0][2];
                v1x = tri->corner[1][0]; v1y = tri->corner[1][1]; v1z = tri->corner[1][2];
                v2x = tri->corner[2][0]; v2y = tri->corner[2][1]; v2z = tri->corner[2][2];
                nx = tri->normal[0]; ny = tri->normal[1]; nz = tri->normal[2];
                point_state = car + 0x66;
                point_index = 0;
                contact = (RwV3d *)(car + 0x130);
                point = car + 0x280;
                do {
                    if (point_index > 3 || *(int *)point_state == 0) {
                        px = point[-2]; py = point[-1]; pz = point[0];
                        depth = (pz - v0z) * nz + (py - v0y) * ny + nx * (px - v0x);
                        if (depth < k_f_0) {
                            ex = ny * (v1z - v0z) - (v1y - v0y) * nz;
                            ey = (v1x - v0x) * nz - (v1z - v0z) * nx;
                            if (k_f_0 <= (py - v0y) * ey + (px - v0x) * ex + (pz - v0z) * ((v1y - v0y) * nx - (v1x - v0x) * ny)) {
                                ex = ny * (v2z - v1z) - (v2y - v1y) * nz;
                                ey = (v2x - v1x) * nz - (v2z - v1z) * nx;
                                if (k_f_0 <= (px - v1x) * ex + (py - v1y) * ey + (pz - v1z) * ((v2y - v1y) * nx - (v2x - v1x) * ny)) {
                                    ex = ny * (v0z - v2z) - (v0y - v2y) * nz;
                                    ey = (v0x - v2x) * nz - (v0z - v2z) * nx;
                                    if ((k_f_0 <= (px - v2x) * ex + (py - v2y) * ey + (pz - v2z) * ((v0y - v2y) * nx - (v0x - v2x) * ny)) && (k_f_n0_4 <= depth)) {
                                        contact[1].x = -depth;
                                        spin_vel.x = (point[-0x268] - car[0x57]) * k_f_3_6;
                                        spin_vel.y = (point[-0x267] - car[0x58]) * k_f_3_6;
                                        spin_vel.z = (point[-0x266] - car[0x59]) * k_f_3_6;
                                        RwV3dTransformVectors((RwV3d *)&contact[1].z, &spin_vel, 1, body_matrix);
                                        contact->x = car[0x26c];
                                        spin_vel.z = 0.0f;
                                        contact->y = car[0x26d];
                                        spin_vel.y = 0.0f;
                                        contact->z = car[0x26e];
                                        spin_vel.x = 0.0f;
                                        speed = RwV3dLength(contact);
                                        if (speed <= k_f_1_0en4) goto no_slip;
                                        RwV3dNormalize(contact, contact);
                                        cos_in = tri->normal[2] * contact->z + contact->y * tri->normal[1] + tri->normal[0] * contact->x;
                                        if (k_f_0 <= cos_in) goto no_slip;
                                        if (cos_in < k_f_n1) cos_in = k_f_n1;
                                        ((float *)contact)[8] = speed * cos_in;
                                        goto slip_done;
no_slip:
                                        tri->hit = 1;
                                        tri->hit_point[0] = point[-2];
                                        tri->hit_point[1] = point[-1];
                                        tri->hit_point[2] = point[0];
                                        tri->impulse[2] = 0.0f;
                                        tri->impulse[1] = 0.0f;
                                        tri->impulse[0] = 0.0f;
                                        goto after_slip;
slip_done:
                                        ((float *)contact)[-2 * 3 + 2] = tri->normal[0];
                                        ((float *)contact)[-1 * 3 + 0] = tri->normal[1];
                                        ((float *)contact)[-1 * 3 + 1] = tri->normal[2];
                                        ((float *)contact)[-2 * 3 + 0] = depth;
                                        ((unsigned int *)contact)[-2 * 3 + 1] = 0xfffffffeu;      /* a NaN bit pattern (marker) */
                                        tri->impulse[0] = car[0x14] * car[0x26c];
                                        tri->impulse[1] = car[0x14] * car[0x26d];
                                        tri->hit = 1;
                                        tri->impulse[2] = car[0x14] * car[0x26e];
                                        tri->impulse[0] = tri->impulse[0] - tri->push[0] * tri->mass;
                                        tri->impulse[1] = tri->impulse[1] - tri->push[1] * tri->mass;
                                        tri->impulse[2] = tri->impulse[2] - tri->push[2] * tri->mass;
                                        tri->hit_point[0] = point[-2];
                                        tri->hit_point[1] = point[-1];
                                        tri->hit_point[2] = point[0];
after_slip:
                                        smplfzx_object_get_position(tri->info[3], &spin_vel);
                                        to_obj.x = spin_vel.x - body_matrix->pos.x;
                                        to_obj.y = spin_vel.y - body_matrix->pos.y;
                                        to_obj.z = spin_vel.z - body_matrix->pos.z;
                                        RwV3dNormalize(&to_obj, &to_obj);
                                        car[0x26c] = to_obj.x * k_f_n10 + car[0x26c];
                                        car[0x26d] = to_obj.y * k_f_n10 + car[0x26d];
                                        car[0x26e] = to_obj.z * k_f_n10 + car[0x26e];
                                        tri->impulse[0] = tri->impulse[0] + to_obj.x * k_f_100000;
                                        tri->impulse[1] = tri->impulse[1] + to_obj.y * k_f_100000;
                                        tri->impulse[2] = tri->impulse[2] + to_obj.z * k_f_100000;
                                    }
                                }
                            }
                        }
                    }
                    point_index++;
                    point_state += 0x31;
                    contact = (RwV3d *)&contact[5].y;
                    point += 3;
                } while (point_index < 0x12);
            }
            tri_index++;
            tri++;
            count = particles_get_particles_72db20();
        } while (tri_index < count);
    }
}

