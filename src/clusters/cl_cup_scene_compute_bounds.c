// cluster-entries: cup_scene_compute_bounds
/* Installed cluster cup_scene_compute_bounds (tools/cluster_install.py from output/cluster_pilot8/cup_scene_compute_bounds.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster cup_scene_compute_bounds: bounding box of the first atomic of a loaded cup scene model: min / max over all vertices, centre,
   size, the reciprocal of the box diagonal and the centre negated (the offset that moves the model to the origin). */
typedef struct MorphTarget {
    char unknown_00[0x14];
    float (*vertices)[3];  /* 0x14 */
} MorphTarget;
typedef struct SceneGeometry {
    char unknown_00[0x14];
    int vertex_count;      /* 0x14 */
    char unknown_18[0x44];
    MorphTarget *morph;    /* 0x5c */
} SceneGeometry;
typedef struct SceneAtomic {
    char unknown_00[0x18];
    SceneGeometry *geometry; /* 0x18 */
} SceneAtomic;
typedef struct CupSceneModel {
    void *clump;           /* 0x00 */
    float offset[3];       /* 0x04: centre of the box, negated */
    float min[3];          /* 0x10 */
    float max[3];          /* 0x1c */
    RwV3d size;            /* 0x28 */
    float inverse_diagonal; /* 0x34 */
} CupSceneModel;

extern SceneAtomic *clump_first_atomic(void *clump);
extern float RwV3dLength(RwV3d *v);
extern float float_1_0;
extern float k_f_0_5;
extern float k_f_n1;

/* was: ESI = model */
void cup_scene_compute_bounds_n(CupSceneModel *model)
{
    SceneAtomic *atomic;
    float (*vertices)[3];
    int count, i, axis;

    if (model->clump == 0)
        return;
    atomic = clump_first_atomic(model->clump);
    count = atomic->geometry->vertex_count;
    vertices = atomic->geometry->morph->vertices;
    for (axis = 0; axis < 3; axis++) {
        model->max[axis] = 1.17549435e-38f;     /* sic: the smallest positive normal float, not -FLT_MAX */
        model->min[axis] = 3.40282347e+38f;
    }
    for (i = 0; i < count; i++) {
        for (axis = 0; axis < 3; axis++) {
            if (model->max[axis] < vertices[i][axis])
                model->max[axis] = vertices[i][axis];
            if (vertices[i][axis] < model->min[axis])
                model->min[axis] = vertices[i][axis];
        }
    }
    model->offset[0] = (model->max[0] - model->min[0]) * k_f_0_5 + model->min[0];
    model->offset[1] = (model->max[1] - model->min[1]) * k_f_0_5 + model->min[1];
    model->offset[2] = (model->max[2] - model->min[2]) * k_f_0_5 + model->min[2];
    model->size.x = model->max[0] - model->min[0];
    model->size.y = model->max[1] - model->min[1];
    model->size.z = model->max[2] - model->min[2];
    model->inverse_diagonal = float_1_0 / RwV3dLength(&model->size);
    model->offset[0] = model->offset[0] * k_f_n1;
    model->offset[1] = model->offset[1] * k_f_n1;
    model->offset[2] = model->offset[2] * k_f_n1;
}

/* entry thunk at the original address of cup_scene_compute_bounds: register inputs (esi) -> natural C cup_scene_compute_bounds_n; ret 0 */
__declspec(naked) void cup_scene_compute_bounds(void)
{
    __asm {
        push esi
        call cup_scene_compute_bounds_n
        add esp, 4
        ret
    }
}
