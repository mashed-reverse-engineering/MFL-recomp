/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpPoseTables { RwMatrix *matrices; int pad[3]; RtQuat *quats; } RwpPoseTables;
typedef struct RwpPoseWorld { char pad[0x10]; RwpPoseTables *tables; } RwpPoseWorld;
typedef struct RwpBodyPose { RwpPoseWorld *world; int index; RwV3d translation; } RwpBodyPose;
extern RwMatrix *RwMatrixTransform(RwMatrix *m, RwMatrix *other, int combine);
extern RwMatrix *RwMatrixTranslate(RwMatrix *m, RwV3d *t, int combine);
extern void RtQuatConvertFromMatrix(RtQuat *q, RwMatrix *m);
/* Transforms the pose matrix of a body pose by a matrix, translates it and converts it to its quaternion. (Rwp library) */
RwpBodyPose *rwp_sphere_0056bdd0(RwpBodyPose *p, RwMatrix *other)
{
  RwMatrixTransform(p->world->tables->matrices + p->index, other, 0);
  RwMatrixTranslate(p->world->tables->matrices + p->index, &p->translation, 1);
  RtQuatConvertFromMatrix(p->world->tables->quats + p->index, p->world->tables->matrices + p->index);
  return p;
}
