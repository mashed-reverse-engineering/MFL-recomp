#include "mfl_tier2.h"
/*004f8140*/ extern struct RpMaterial * RpMaterialCreate();
/*004f81d0*/ extern RwBool RpMaterialDestroy(struct RpMaterial *p0);
/*004f8800*/ extern struct RpMorphTarget * RpMorphTargetCalcBoundingSphere(struct RpMorphTarget *p0, struct RwSphere *p1);
/*004f8b00*/ extern struct RpGeometry * RpGeometryTriangleSetMaterial(struct RpGeometry *p0, struct RpTriangle *p1, struct RpMaterial *p2);
/*004f8bd0*/ extern struct RpGeometry * RpGeometryUnlock(struct RpGeometry *p0);
/*004f8e30*/ extern struct RpGeometry * RpGeometryCreate(RwInt32 p0, RwInt32 p1, RwUInt32 p2);

#define ARROW_NUM_VERTS (3)
#define ARROW_GEOMETRY_FLAGS (0x4b) /* rpGEOMETRY* flag set (inferred) */
#define COLOR_CHANNEL_MAX (0xff) /* full intensity of one RwRGBA channel */

/* geometry_build_arrow */

/*
 * Builds the arrow model: an RpGeometry with one triangle, one white material and a
 * morph target bounding sphere. Vertices (x, y=0, z): v0 (1,-1), v1 (0,2), v2 (-1,-1).
 * Per-vertex normal (0,1,0) and UV (1,1) are (inferred).
 * Returns the geometry, or NULL if RpGeometryCreate fails.
 */

RpGeometry * geometry_build_arrow(void)

{
  RpTriangle *triangle;
  RpMorphTarget *morphTarget;
  RwV3d *verts;
  RpMaterial *material;
  RpGeometry *geometry;
  RwV3d *normals;
  RwRGBA *prelit_colors;
  RwTexCoords *tex_coords;
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
  } arrow;

  arrow.v0_x = 1.0;
  arrow.v0_y = 0.0;
  arrow.v0_z = -1.0;
  arrow.v1_x = 0.0;
  arrow.v1_y = 0.0;
  arrow.v1_z = 2.0;
  arrow.v2_x = -1.0;
  arrow.v2_y = 0.0;
  arrow.v2_z = -1.0;
  material = RpMaterialCreate();
  (RpMaterial_color(material)).red = COLOR_CHANNEL_MAX;
  (RpMaterial_color(material)).green = COLOR_CHANNEL_MAX;
  (RpMaterial_color(material)).blue = COLOR_CHANNEL_MAX;
  (RpMaterial_color(material)).alpha = COLOR_CHANNEL_MAX;
  geometry = RpGeometryCreate(ARROW_NUM_VERTS,1,ARROW_GEOMETRY_FLAGS);
  if (geometry == (RpGeometry *)0x0) {
    return (RpGeometry *)0x0;
  }
  RpMaterial_surfaceProps_ambient(material) = 0.3;
  RpMaterial_surfaceProps_specular(material) = 0.0;
  RpMaterial_surfaceProps_diffuse(material) = 0.7;
  triangle = RpGeometry_triangles(geometry);
  prelit_colors = RpGeometry_preLitLum(geometry);
  tex_coords = RpGeometry_texCoords(geometry)[0];
  morphTarget = RpGeometry_morphTarget(geometry);
  verts = RpMorphTarget_verts(morphTarget);
  normals = RpMorphTarget_normals(morphTarget);
  /* Vertex 0: write position, then normal, prelit colour and UV for each stream that exists. */
  verts->x = arrow.v0_x;
  verts->y = arrow.v0_y;
  verts->z = arrow.v0_z;
  if (normals != (RwV3d *)0x0) {
    normals->x = 0.0;
    normals->y = 1.0;
    normals->z = 0.0;
    normals = normals + 1;
  }
  if (prelit_colors != (RwRGBA *)0x0) {
    prelit_colors->red = COLOR_CHANNEL_MAX;
    prelit_colors->green = COLOR_CHANNEL_MAX;
    prelit_colors->blue = COLOR_CHANNEL_MAX;
    prelit_colors->alpha = COLOR_CHANNEL_MAX;
    prelit_colors = prelit_colors + 1;
  }
  if (tex_coords != (RwTexCoords *)0x0) {
    tex_coords->u = 1.0;
    tex_coords->v = 1.0;
    tex_coords = tex_coords + 1;
  }
  /* Vertex 1: same per-stream writes as vertex 0. */
  verts[1].x = arrow.v1_x;
  verts[1].y = arrow.v1_y;
  verts[1].z = arrow.v1_z;
  if (normals != (RwV3d *)0x0) {
    normals->x = 0.0;
    normals->y = 1.0;
    normals->z = 0.0;
    normals = normals + 1;
  }
  if (prelit_colors != (RwRGBA *)0x0) {
    prelit_colors->red = COLOR_CHANNEL_MAX;
    prelit_colors->green = COLOR_CHANNEL_MAX;
    prelit_colors->blue = COLOR_CHANNEL_MAX;
    prelit_colors->alpha = COLOR_CHANNEL_MAX;
    prelit_colors = prelit_colors + 1;
  }
  if (tex_coords != (RwTexCoords *)0x0) {
    tex_coords->u = 1.0;
    tex_coords->v = 1.0;
    tex_coords = tex_coords + 1;
  }
  /* Vertex 2: last vertex, so the pointers are not advanced past it. */
  verts[2].x = arrow.v2_x;
  verts[2].y = arrow.v2_y;
  verts[2].z = arrow.v2_z;
  if (normals != (RwV3d *)0x0) {
    normals->x = 0.0;
    normals->y = 1.0;
    normals->z = 0.0;
  }
  if (prelit_colors != (RwRGBA *)0x0) {
    prelit_colors->red = COLOR_CHANNEL_MAX;
    prelit_colors->green = COLOR_CHANNEL_MAX;
    prelit_colors->blue = COLOR_CHANNEL_MAX;
    prelit_colors->alpha = COLOR_CHANNEL_MAX;
  }
  if (tex_coords != (RwTexCoords *)0x0) {
    tex_coords->u = 1.0;
    tex_coords->v = 1.0;
  }
  /* Triangle uses vertices 0, 1, 2 and the material. */
  RpGeometryTriangleSetVertexIndices(geometry,triangle,0,1,2);
  RpGeometryTriangleSetMaterial(geometry,triangle,material);
  /* Compute the bounding sphere from the vertices, then copy it into the morph target. */
  RpMorphTargetCalcBoundingSphere(morphTarget,&arrow.bounding_sphere);
  (RpMorphTarget_boundingSphere(morphTarget)).center.x = arrow.bounding_sphere.center.x;
  (RpMorphTarget_boundingSphere(morphTarget)).center.y = arrow.bounding_sphere.center.y;
  (RpMorphTarget_boundingSphere(morphTarget)).center.z = arrow.bounding_sphere.center.z;
  (RpMorphTarget_boundingSphere(morphTarget)).radius = arrow.bounding_sphere.radius;
  RpGeometryUnlock(geometry);
  RpMaterialDestroy(material);
  return geometry;
}


