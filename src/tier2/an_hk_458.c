#include "mfl_tier2.h"
/*004f8140*/ extern struct RpMaterial * RpMaterialCreate();
/*004f81d0*/ extern RwBool RpMaterialDestroy(struct RpMaterial *p0);
/*004f8800*/ extern struct RpMorphTarget * RpMorphTargetCalcBoundingSphere(struct RpMorphTarget *p0, struct RwSphere *p1);
/*004f8b00*/ extern struct RpGeometry * RpGeometryTriangleSetMaterial(struct RpGeometry *p0, struct RpTriangle *p1, struct RpMaterial *p2);
/*004f8bd0*/ extern struct RpGeometry * RpGeometryUnlock(struct RpGeometry *p0);
/*004f8e30*/ extern struct RpGeometry * RpGeometryCreate(RwInt32 p0, RwInt32 p1, RwUInt32 p2);

/* RpGeometryCreate counts for the quad: 4 vertices, 2 triangles */
#define QUAD_VERTEX_COUNT (4)
#define QUAD_TRIANGLE_COUNT (2)
/* RpGeometry format flags (inferred): tristrip | positions | prelit | modulate material colour; no NORMALS or TEXTURED bit */
#define GEOMETRY_FLAGS (0x4b)
/* full intensity colour channel, used for the white material colour and the prelit vertex colours */
#define COLOR_CHANNEL_FULL (0xff)
/* material ambient / diffuse coefficients (ambient + diffuse = 1.0) */
#define MATERIAL_AMBIENT (0.3)
#define MATERIAL_DIFFUSE (0.7)

/* geometry_build_pointer
 * Builds a small RpGeometry: a quad in the XZ plane at y = 0 made of two triangles (0,1,3) and (3,1,2),
 * with one white material. Sets the morph target's bounding sphere from the four vertices.
 * Parameters: none.
 * Returns the new geometry, or NULL if RpGeometryCreate fails.
 * Optional slots (normals, prelit colours, uvs) are written only when the geometry has them; with
 * flags 0x4b they are NULL (inferred), so those writes are skipped.
 */

/* "second small RpGeometry builder (triangles, material)" */

RpGeometry * geometry_build_pointer(void)

{
  RpMorphTarget *morphTarget;
  RwV3d *positions;
  RpTriangle *triangle;
  RpMaterial *material;
  RpGeometry *geometry;
  RwV3d *normal;
  RwRGBA *prelit;
  RwTexCoords *uv;
  struct {
    RwSphere bounding_sphere;
    RwReal v0_x;
    RwReal v0_y;
    RwReal v0_z;
    RwReal v1_x;
    RwReal v1_y;
    RwReal v1_z;
    RwReal v2_x;
    RwReal v2_y;
    RwReal v2_z;
    RwReal v3_x;
    RwReal v3_y;
    RwReal v3_z;
  } frame;

  frame.v0_x = -1.0;
  frame.v0_y = 0.0;
  frame.v0_z = -4.0;
  frame.v1_x = -1.0;
  frame.v1_y = 0.0;
  frame.v1_z = 4.0;
  frame.v2_x = 1.0;
  frame.v2_y = 0.0;
  frame.v2_z = 4.0;
  frame.v3_x = 1.0;
  frame.v3_y = 0.0;
  frame.v3_z = -4.0;
  /* white material; RpMaterialCreate's result is not NULL-checked before use */
  material = RpMaterialCreate();
  (RpMaterial_color(material)).red = COLOR_CHANNEL_FULL;
  (RpMaterial_color(material)).green = COLOR_CHANNEL_FULL;
  (RpMaterial_color(material)).blue = COLOR_CHANNEL_FULL;
  (RpMaterial_color(material)).alpha = COLOR_CHANNEL_FULL;
  geometry = RpGeometryCreate(QUAD_VERTEX_COUNT,QUAD_TRIANGLE_COUNT,GEOMETRY_FLAGS);
  /* RpGeometryCreate failed. The material is not destroyed on this path, so it leaks. */
  if (geometry == (RpGeometry *)0x0) {
    return (RpGeometry *)0x0;
  }
  /* lighting: ambient and diffuse, no specular */
  RpMaterial_surfaceProps_ambient(material) = MATERIAL_AMBIENT;
  RpMaterial_surfaceProps_specular(material) = 0.0;
  RpMaterial_surfaceProps_diffuse(material) = MATERIAL_DIFFUSE;
  /* fill the vertex arrays. normal, prelit and uv are write cursors: each one advances per
     vertex, and the write is skipped while its array is NULL */
  morphTarget = RpGeometry_morphTarget(geometry);
  positions = RpMorphTarget_verts(morphTarget);
  prelit = RpGeometry_preLitLum(geometry);
  uv = RpGeometry_texCoords(geometry)[0];
  triangle = RpGeometry_triangles(geometry);
  normal = RpMorphTarget_normals(morphTarget);
  /* vertex 0: (-1, 0, -4) */
  positions->x = frame.v0_x;
  positions->y = frame.v0_y;
  positions->z = frame.v0_z;
  if (normal != (RwV3d *)0x0) {
    normal->x = 0.0;
    normal->y = 1.0;
    normal->z = 0.0;
    normal = normal + 1;
  }
  if (prelit != (RwRGBA *)0x0) {
    prelit->red = COLOR_CHANNEL_FULL;
    prelit->green = COLOR_CHANNEL_FULL;
    prelit->blue = COLOR_CHANNEL_FULL;
    prelit->alpha = COLOR_CHANNEL_FULL;
    prelit = prelit + 1;
  }
  if (uv != (RwTexCoords *)0x0) {
    uv->u = 1.0;
    uv->v = 1.0;
    uv = uv + 1;
  }
  /* vertex 1: (-1, 0, 4) */
  positions[1].x = frame.v1_x;
  positions[1].y = frame.v1_y;
  positions[1].z = frame.v1_z;
  if (normal != (RwV3d *)0x0) {
    normal->x = 0.0;
    normal->y = 1.0;
    normal->z = 0.0;
    normal = normal + 1;
  }
  if (prelit != (RwRGBA *)0x0) {
    prelit->red = COLOR_CHANNEL_FULL;
    prelit->green = COLOR_CHANNEL_FULL;
    prelit->blue = COLOR_CHANNEL_FULL;
    prelit->alpha = COLOR_CHANNEL_FULL;
    prelit = prelit + 1;
  }
  if (uv != (RwTexCoords *)0x0) {
    uv->u = 1.0;
    uv->v = 1.0;
    uv = uv + 1;
  }
  /* vertex 2: (1, 0, 4) */
  positions[2].x = frame.v2_x;
  positions[2].y = frame.v2_y;
  positions[2].z = frame.v2_z;
  if (normal != (RwV3d *)0x0) {
    normal->x = 0.0;
    normal->y = 1.0;
    normal->z = 0.0;
    normal = normal + 1;
  }
  if (prelit != (RwRGBA *)0x0) {
    prelit->red = COLOR_CHANNEL_FULL;
    prelit->green = COLOR_CHANNEL_FULL;
    prelit->blue = COLOR_CHANNEL_FULL;
    prelit->alpha = COLOR_CHANNEL_FULL;
    prelit = prelit + 1;
  }
  if (uv != (RwTexCoords *)0x0) {
    uv->u = 1.0;
    uv->v = 1.0;
    uv = uv + 1;
  }
  /* vertex 3: (1, 0, -4) */
  positions[3].x = frame.v3_x;
  positions[3].y = frame.v3_y;
  positions[3].z = frame.v3_z;
  if (normal != (RwV3d *)0x0) {
    normal->x = 0.0;
    normal->y = 1.0;
    normal->z = 0.0;
  }
  if (prelit != (RwRGBA *)0x0) {
    prelit->red = COLOR_CHANNEL_FULL;
    prelit->green = COLOR_CHANNEL_FULL;
    prelit->blue = COLOR_CHANNEL_FULL;
    prelit->alpha = COLOR_CHANNEL_FULL;
  }
  if (uv != (RwTexCoords *)0x0) {
    uv->u = 1.0;
    uv->v = 1.0;
  }
  /* two triangles, both using the material: (0,1,3) and (3,1,2) */
  RpGeometryTriangleSetVertexIndices(geometry,triangle,0,1,3);
  RpGeometryTriangleSetMaterial(geometry,triangle,material);
  RpGeometryTriangleSetVertexIndices(geometry,triangle + 1,3,1,2);
  RpGeometryTriangleSetMaterial(geometry,triangle + 1,material);
  /* bounding sphere from the vertices, copied into the morph target */
  RpMorphTargetCalcBoundingSphere(morphTarget,&frame.bounding_sphere);
  (RpMorphTarget_boundingSphere(morphTarget)).center.x = frame.bounding_sphere.center.x;
  (RpMorphTarget_boundingSphere(morphTarget)).center.y = frame.bounding_sphere.center.y;
  (RpMorphTarget_boundingSphere(morphTarget)).center.z = frame.bounding_sphere.center.z;
  (RpMorphTarget_boundingSphere(morphTarget)).radius = frame.bounding_sphere.radius;
  /* finish the geometry (inferred: RpGeometryUnlock), then release our material reference
     (inferred: the geometry keeps its own reference through the triangle set calls) */
  RpGeometryUnlock(geometry);
  RpMaterialDestroy(material);
  return geometry;
}


