// cluster-entries: rwp_collision_mesh_to_geometry
/* Installed cluster rwp_collision_mesh_to_geometry (tools/cluster_install.py from output/cluster_pilot_j1/rwp_collision_mesh_to_geometry.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00493b90*/ extern int hsv_phase_to_rgb();
/*00493c40*/ extern int vec2_normalise_to_unit_square();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004f68f0*/ extern struct RpAtomic * RpAtomicCreate();
/*004f69e0*/ extern struct RpAtomic * RpAtomicSetGeometry(struct RpAtomic *p0, struct RpGeometry *p1, RwUInt32 p2);
/*004f8140*/ extern struct RpMaterial * RpMaterialCreate();
/*004f81d0*/ extern RwBool RpMaterialDestroy(struct RpMaterial *p0);
/*004f8800*/ extern struct RpMorphTarget * RpMorphTargetCalcBoundingSphere(struct RpMorphTarget *p0, struct RwSphere *p1);
/*004f8b00*/ extern struct RpGeometry * RpGeometryTriangleSetMaterial(struct RpGeometry *p0, struct RpTriangle *p1, struct RpMaterial *p2);
/*004f8bd0*/ extern struct RpGeometry * RpGeometryUnlock(struct RpGeometry *p0);
/*004f8e30*/ extern struct RpGeometry * RpGeometryCreate(RwInt32 p0, RwInt32 p1, RwUInt32 p2);
/*004f9060*/ extern RwBool RpGeometryDestroy(struct RpGeometry *p0);
/* rwp_collision_mesh_to_geometry */

/* "converts the Rwp collision mesh (strips/vertex tables) to an RpGeometry with normals and uv" */

/* mesh layout as read below: +4 position table (3 floats each), +0xc face normal table, +0x10 strip start indices (ushort; a strip with n entries gives n-2 triangles), +0x14 strip vertex records (0xc bytes each), +0x1c vertex count, +0x1e strip count; shared_vertices != 0: one geometry vertex per mesh vertex with accumulated normals, else 3 vertices per triangle */
/* collision mesh header (the tables are plain addresses used with int arithmetic below) */
typedef struct CollisionMesh {
    int unknown_00;
    int positions;      /* 0x04: 3 floats per vertex */
    int strip_ranges;   /* 0x08: ushort table, start index per strip (read with a byte offset) */
    int normals;        /* 0x0c: face normal table */
    int strips;         /* 0x10: ushort strip start indices */
    int tris;           /* 0x14: strip vertex records, 0xc bytes each */
    int tri_extra;      /* 0x18 */
    ushort strip_count_a; /* 0x1c */
    ushort strip_count_b; /* 0x1e */
    char unknown_20[0x24];
    float uv_scale[2];  /* 0x44 */
} CollisionMesh;
RpAtomic * rwp_collision_mesh_to_geometry(CollisionMesh *mesh,int shared_vertices)

{
  ushort *strip_end_ptr;
  uint uVar2;
  int tri_rec;
  ushort strip_begin;
  RpMorphTarget *morphTarget;
  RwV3d *verts;
  RwV3d *normals;
  RwTexCoords *uvs;
  int numTriangles;
  RpGeometry *geometry;
  RpMaterial *material;
  RpAtomic *atomic;
  ushort *strip_cursor;
  RwReal *pRVar9;
  uint uVar10;
  int strip_len;
  uint uVar12;
  undefined4 *src_normal;
  uint numVert;
  RwReal *normal_z_ptr;
  undefined4 *dst_normal;
  RwTexCoords *uv_ptr;
  RwV3d *vert_ptr;
  struct {
    uint strip_index;
    uint tri_in_strip;
    RpTriangle *tri_ptr;
    undefined1 pad_40[4];
    RwV3d *vert_offset;
    RwV3d *normal_or_uv_ptr;
  } frame;
  RwSphere bound_sphere;
  
  numVert = (uint)mesh->strip_count_b;
  numTriangles = 0;
  if (numVert != 0) {
    strip_cursor = *(ushort **)&mesh->strips;
    do {
      strip_begin = *strip_cursor;
      strip_end_ptr = strip_cursor + 1;
      strip_cursor = strip_cursor + 1;
      numVert = numVert - 1;
      numTriangles = numTriangles + -2 + ((uint)*strip_end_ptr - (uint)strip_begin);
    } while (numVert != 0);
  }
  if (shared_vertices == 0) {
    numVert = numTriangles * 3;
  }
  else {
    numVert = (uint)mesh->strip_count_a;
  }
  geometry = RpGeometryCreate(numVert,numTriangles,0x74);
  morphTarget = RpGeometry_morphTarget(geometry);
  verts = RpMorphTarget_verts(morphTarget);
  normals = RpMorphTarget_normals(morphTarget);
  uvs = RpGeometry_texCoords(geometry)[0];
  material = RpMaterialCreate();
  if ((shared_vertices != 0) && (frame.strip_index = 0, (short)mesh->strip_count_a != 0)) {
    numTriangles = 0;
    normal_z_ptr = &normals->z;
    frame.vert_offset = verts;
    frame.normal_or_uv_ptr = (RwV3d *)uvs;
    do {
      pRVar9 = (RwReal *)((int)normal_z_ptr + mesh->positions + (-8 - (int)normals));
      frame.vert_offset->x = *pRVar9;
      frame.vert_offset->y = pRVar9[1];
      frame.vert_offset->z = pRVar9[2];
      vert_ptr = (RwV3d *)(normal_z_ptr + -2);
      vert_ptr->x = 0.0;
      normal_z_ptr[-1] = 0.0;
      *normal_z_ptr = 0.0;
      vec2_normalise_to_unit_square(frame.normal_or_uv_ptr,(int)mesh->uv_scale,frame.vert_offset);
      uVar10 = (uint)*(ushort *)(mesh->strip_ranges + 2 + numTriangles) -
               (uint)*(ushort *)(mesh->strip_ranges + numTriangles);
      numVert = 0;
      uVar12 = numVert;
      if (3 < (int)uVar10) {
        do {
          strip_len = (uint)*(ushort *)
                          (mesh->tris + 10 +
                          (uint)*(ushort *)
                                 (mesh->tri_extra +
                                 (*(ushort *)(mesh->strip_ranges + numTriangles) + uVar12) * 2) *
                          0xc) * 0xc;
          vert_ptr->x = *(float *)(mesh->normals + strip_len) + vert_ptr->x;
          normal_z_ptr[-1] = *(float *)(mesh->normals + 4 + strip_len) + normal_z_ptr[-1];
          *normal_z_ptr = *(float *)(mesh->normals + 8 + strip_len) + *normal_z_ptr;
          strip_len = (uint)*(ushort *)
                          (mesh->tris + 10 +
                          (uint)*(ushort *)
                                 (mesh->tri_extra + 2 +
                                 (*(ushort *)(mesh->strip_ranges + numTriangles) + uVar12) * 2) *
                          0xc) * 0xc;
          vert_ptr->x = *(float *)(mesh->normals + strip_len) + vert_ptr->x;
          normal_z_ptr[-1] = *(float *)(mesh->normals + 4 + strip_len) + normal_z_ptr[-1];
          *normal_z_ptr = *(float *)(mesh->normals + 8 + strip_len) + *normal_z_ptr;
          strip_len = (uint)*(ushort *)
                          (mesh->tris + 10 +
                          (uint)*(ushort *)
                                 (mesh->tri_extra + 4 +
                                 (*(ushort *)(mesh->strip_ranges + numTriangles) + uVar12) * 2) *
                          0xc) * 0xc;
          vert_ptr->x = *(float *)(mesh->normals + strip_len) + vert_ptr->x;
          normal_z_ptr[-1] = *(float *)(mesh->normals + 4 + strip_len) + normal_z_ptr[-1];
          *normal_z_ptr = *(float *)(mesh->normals + 8 + strip_len) + *normal_z_ptr;
          strip_len = (uint)*(ushort *)
                          (mesh->tris + 10 +
                          (uint)*(ushort *)
                                 (mesh->tri_extra + 6 +
                                 (*(ushort *)(mesh->strip_ranges + numTriangles) + uVar12) * 2) *
                          0xc) * 0xc;
          numVert = uVar12 + 4;
          vert_ptr->x = *(float *)(mesh->normals + strip_len) + vert_ptr->x;
          normal_z_ptr[-1] = *(float *)(mesh->normals + 4 + strip_len) + normal_z_ptr[-1];
          uVar2 = uVar12 + 7;
          *normal_z_ptr = *(float *)(mesh->normals + 8 + strip_len) + *normal_z_ptr;
          uVar12 = numVert;
        } while (uVar2 < uVar10);
      }
      for (; numVert < uVar10; numVert = numVert + 1) {
        strip_len = (uint)*(ushort *)
                        (mesh->tris + 10 +
                        (uint)*(ushort *)
                               (mesh->tri_extra +
                               (*(ushort *)(mesh->strip_ranges + numTriangles) + numVert) * 2) *
                        0xc) * 0xc;
        vert_ptr->x = *(float *)(mesh->normals + strip_len) + vert_ptr->x;
        normal_z_ptr[-1] = *(float *)(mesh->normals + 4 + strip_len) + normal_z_ptr[-1];
        *normal_z_ptr = *(float *)(mesh->normals + 8 + strip_len) + *normal_z_ptr;
      }
      RwV3dNormalize(vert_ptr,vert_ptr);
      frame.vert_offset = frame.vert_offset + 1;
      frame.strip_index = frame.strip_index + 1;
      frame.normal_or_uv_ptr = (RwV3d *)((int)frame.normal_or_uv_ptr + 8);
      numTriangles = numTriangles + 2;
      normal_z_ptr = normal_z_ptr + 3;
    } while (frame.strip_index < mesh->strip_count_a);
  }
  frame.tri_ptr = RpGeometry_triangles(geometry);
  numTriangles = 0;
  frame.strip_index = 0;
  if ((short)mesh->strip_count_b != 0) {
    frame.vert_offset = (RwV3d *)0x0;
    do {
      strip_cursor = (ushort *)(mesh->strips + frame.strip_index * 2);
      strip_len = (uint)strip_cursor[1] - (uint)*strip_cursor;
      frame.tri_in_strip = 0;
      if (strip_len != 2) {
        frame.normal_or_uv_ptr = normals + numTriangles;
        uv_ptr = uvs + numTriangles + 2;
        vert_ptr = verts + numTriangles + 2;
        do {
          RpGeometryTriangleSetMaterial(geometry,frame.tri_ptr,material);
          numVert = (uint)*(ushort *)(mesh->strips + frame.strip_index * 2);
          uVar10 = (uint)*(ushort *)(mesh->tris + 4 + numVert * 0xc);
          tri_rec = mesh->tris + (numVert + frame.tri_in_strip) * 0xc;
          numVert = (uint)*(ushort *)(tri_rec + 0x10);
          uVar12 = (uint)*(ushort *)(tri_rec + 0x1c);
          if (shared_vertices == 0) {
            RpGeometryTriangleSetVertexIndices
                      (geometry,frame.tri_ptr,numTriangles,numTriangles + 1,numTriangles + 2);
            normal_z_ptr = (RwReal *)(mesh->positions + uVar10 * 0xc);
            vert_ptr[-2].x = *normal_z_ptr;
            vert_ptr[-2].y = normal_z_ptr[1];
            vert_ptr[-2].z = normal_z_ptr[2];
            normal_z_ptr = (RwReal *)(mesh->positions + numVert * 0xc);
            vert_ptr[-1].x = *normal_z_ptr;
            vert_ptr[-1].y = normal_z_ptr[1];
            vert_ptr[-1].z = normal_z_ptr[2];
            normal_z_ptr = (RwReal *)(mesh->positions + uVar12 * 0xc);
            vert_ptr->x = *normal_z_ptr;
            vert_ptr->y = normal_z_ptr[1];
            vert_ptr->z = normal_z_ptr[2];
            normal_z_ptr = (RwReal *)(mesh->normals + (int)frame.vert_offset);
            frame.normal_or_uv_ptr->x = *normal_z_ptr;
            frame.normal_or_uv_ptr->y = normal_z_ptr[1];
            frame.normal_or_uv_ptr->z = normal_z_ptr[2];
            normal_z_ptr = (RwReal *)(mesh->normals + (int)frame.vert_offset);
            frame.normal_or_uv_ptr[1].x = *normal_z_ptr;
            frame.normal_or_uv_ptr[1].y = normal_z_ptr[1];
            frame.normal_or_uv_ptr[1].z = normal_z_ptr[2];
            src_normal = (undefined4 *)(mesh->normals + (int)frame.vert_offset);
            dst_normal = (undefined4 *)(((int)normals - (int)verts) + (int)vert_ptr);
            *dst_normal = *src_normal;
            dst_normal[1] = src_normal[1];
            dst_normal[2] = src_normal[2];
            hsv_phase_to_rgb(uv_ptr + -2,0,strip_len);
            hsv_phase_to_rgb(uv_ptr + -1,frame.tri_in_strip + 1,strip_len);
            hsv_phase_to_rgb(uv_ptr,frame.tri_in_strip + 2,strip_len);
          }
          else {
            RpGeometryTriangleSetVertexIndices(geometry,frame.tri_ptr,uVar10,numVert,uVar12);
          }
          frame.tri_ptr = frame.tri_ptr + 1;
          frame.tri_in_strip = frame.tri_in_strip + 1;
          frame.normal_or_uv_ptr = frame.normal_or_uv_ptr + 3;
          numTriangles = numTriangles + 3;
          vert_ptr = vert_ptr + 3;
          uv_ptr = uv_ptr + 3;
        } while (frame.tri_in_strip < strip_len - 2U);
      }
      frame.vert_offset = (RwV3d *)((int)frame.vert_offset + 0xc);
      frame.strip_index = frame.strip_index + 1;
    } while (frame.strip_index < mesh->strip_count_b);
  }
  RpMorphTargetCalcBoundingSphere(morphTarget,&bound_sphere);
  RpMorphTarget_boundingSphere(morphTarget).center.x = bound_sphere.center.x;
  RpMorphTarget_boundingSphere(morphTarget).center.y = bound_sphere.center.y;
  RpMorphTarget_boundingSphere(morphTarget).center.z = bound_sphere.center.z;
  RpMorphTarget_boundingSphere(morphTarget).radius = bound_sphere.radius;
  RpGeometryUnlock(geometry);
  atomic = RpAtomicCreate();
  RpAtomicSetGeometry(atomic,geometry,1);
  RpMaterialDestroy(material);
  RpGeometryDestroy(geometry);
  return atomic;
}

