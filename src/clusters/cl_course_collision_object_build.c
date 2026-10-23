// cluster-entries: course_collision_object_build
/* Installed cluster course_collision_object_build (tools/cluster_install.py from output/cluster_pilot_pg2/course_collision_object_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c54f0*/ extern void __cdecl FUN_004c54f0(float *bounds, float *points, int point_count);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *frame);
/*004d5670*/ extern void __cdecl RwV3dTransformPoints(float *points_out, float *points_in, int count, RwMatrix *matrix);
/*007d2128*/ extern char *RwEngineInstance;

#define COLLISION_MAX_FACES 12
#define COLLISION_MAX_POINTS 8
#define COLLISION_MAX_EDGES 18

/* collision data of one object (RpClump mesh), built from its triangle list */
typedef struct CollisionRecord {
  float face_normals[COLLISION_MAX_FACES][3];        /* 0x00 */
  float points[COLLISION_MAX_POINTS][3];             /* 0x90: the mesh points in world space */
  unsigned char edges[COLLISION_MAX_EDGES][4];       /* 0xf0: per edge: face a, face b, vertex a, vertex b */
  float bounds[4];                                   /* 0x138 */
  int edge_count;                                    /* 0x148 */
  int point_count;                                   /* 0x14c */
  int face_count;                                    /* 0x150 */
  int face_count_copy;                               /* 0x154 */
} CollisionRecord;

/* the mesh data object reached from the owner (+0x18) */
typedef struct CollisionMesh {
  unsigned char unknown_00[0x10];
  int face_count;                                    /* 0x10 */
  int point_count;                                   /* 0x14 */
  unsigned char unknown_18[0x14];
  unsigned short *triangles;                         /* 0x2c */
  unsigned char unknown_30[0x2c];
  unsigned char *clump;                              /* 0x5c: its frame is at +0x14 */
} CollisionMesh;

typedef struct CollisionOwner {
  int unknown_00;
  struct RwFrame *frame;                             /* 0x04 */
  unsigned char unknown_08[0x10];
  CollisionMesh *mesh;                               /* 0x18 */
} CollisionOwner;

/* one edge as course_collision_edges_build produces it (stride 0x18) */
typedef struct CollisionEdge {
  int unknown_00[2];
  int vertex_a;                                      /* 0x08 */
  int vertex_b;                                      /* 0x0c */
  int face_a;                                        /* 0x10 */
  int face_b;                                        /* 0x14 */
} CollisionEdge;

/* what course_collision_edges_build returns: a malloc'ed edge array and its length */
typedef struct EdgeInfo {
  CollisionEdge *edges;
  int count;
} EdgeInfo;

/* course_collision_object_build: builds the collision `record` of the object `owner->mesh` (its points moved by the owner's frame): face normals, the
   points transformed into world space, the edge list (4 bytes per edge) and the bounds */
static void course_mesh_triangle_normals_build_r(unsigned short * p0, float * p1, float * p2, int p3);
extern void course_mesh_triangle_normals_build(void);
static void course_collision_edges_build_r(int p0, void * p1, unsigned short * p2);
extern void course_collision_edges_build(void);
void course_collision_object_build(CollisionRecord *record, CollisionOwner *owner)
{
  CollisionMesh *mesh = owner->mesh;
  float *vertices = *(float **)(mesh->clump + 0x14);      /* the mesh points in object space */
  unsigned short *triangles = mesh->triangles;
  int face_count = mesh->face_count;
  int point_count = mesh->point_count;
  EdgeInfo edge_info;
  float *normals;
  int i;

  edge_info.edges = 0;
  edge_info.count = 0;
  normals = (float *)RwMalloc(face_count * 12, 0);
  course_mesh_triangle_normals_build_r(triangles, normals, vertices, face_count);
  course_collision_edges_build_r(face_count, &edge_info, triangles);
  record->face_count_copy = face_count;
  record->point_count = point_count;
  RwV3dTransformPoints(&record->points[0][0], vertices, point_count, RwFrameGetLTM(owner->frame));
  record->face_count = face_count;
  for (i = 0; i < face_count * 12; i++) {
    ((unsigned char *)record)[i] = ((unsigned char *)normals)[i];
  }
  record->edge_count = edge_info.count;
  for (i = 0; i < edge_info.count; i++) {
    record->edges[i][0] = (unsigned char)edge_info.edges[i].face_a;
    record->edges[i][1] = (unsigned char)edge_info.edges[i].face_b;
    record->edges[i][2] = (unsigned char)edge_info.edges[i].vertex_a;
    record->edges[i][3] = (unsigned char)edge_info.edges[i].vertex_b;
  }
  RwFree(normals);
  RwFree(edge_info.edges);
  FUN_004c54f0(record->bounds, &record->points[0][0], record->point_count);
}

/* course_mesh_triangle_normals_build_r: C -> original course_mesh_triangle_normals_build (registers edx,ecx,ebx,s0) */
__declspec(naked) void course_mesh_triangle_normals_build_r(unsigned short * p0, float * p1, float * p2, int p3)
{
    __asm {
        push ebx
        push dword ptr [esp + 20]
        mov edx, dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 16]
        mov ebx, dword ptr [esp + 20]
        call course_mesh_triangle_normals_build
        add esp, 4
        pop ebx
        ret
    }
}

/* course_collision_edges_build_r: C -> original course_collision_edges_build (registers eax,s0,s1) */
__declspec(naked) void course_collision_edges_build_r(int p0, void * p1, unsigned short * p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call course_collision_edges_build
        add esp, 8
        ret
    }
}
