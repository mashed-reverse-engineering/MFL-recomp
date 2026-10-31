#include "mfl_tier2.h"
/* The game's 3D debug drawing helpers over the RenderWare Im3D layer: a line, the three axes of a matrix and a wire box.
   All vertices are pre-lit (rwIM3D_VERTEXRGBA); an alpha of 0xff in the colour marks the lines as opaque so no alpha
   blending is switched on.  Colours are r,g,b,a bytes, default opaque white.  0x4c5b10..0x4c6210. */
/*004d49c0*/ extern RwMatrix *RwMatrixTransform(RwMatrix *matrix, RwMatrix *transform, enum RwOpCombineType combineOp);
/*004dc920*/ extern void *RwIm3DTransform(RwIm3DVertex *pVerts, RwUInt32 numVerts, RwMatrix *ltm, RwUInt32 flags);
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combineOp);
/*004dc9f0*/ extern RwBool RwIm3DEnd(void);
/*004dca20*/ extern RwBool __cdecl RwIm3DRenderIndexedPrimitive(enum RwPrimitiveType primType, unsigned short *indices, RwInt32 numIndices);
/*00771f78*/ extern RwIm3DVertex g_data_771f78[];   /* 0x2000 scratch vertices of the wire mesh drawer */
/*007b9f78*/ extern unsigned short g_data_7b9f78[]; /* its line list indices (6 per triangle) */
/*004dcce0*/ extern RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define COLOR_OPAQUE 0xff       /* alpha value that needs no alpha blending */
/* A colour given as four bytes r,g,b,a packed to the 0xAARRGGBB vertex colour. */
#define PACK_ARGB(c) ((((((c)[3] << 8) | (c)[0]) << 8 | (c)[1]) << 8) | (c)[2])
#define IM3D_LINE_FLAGS (rwIM3D_VERTEXRGBA)
#define IM3D_LINE_FLAGS_OPAQUE (rwIM3D_VERTEXRGBA | rwIM3D_ALLOPAQUE)
/* rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL */
#define MATRIX_FLAGS_IDENTITY 0x20003
/* Sets the identity (the game does not clear flags first: the identity bits are only or-ed in). */
#define MATRIX_SET_IDENTITY(m) do { \
    (m)->flags |= MATRIX_FLAGS_IDENTITY; \
    (m)->at.z = 1.0f; (m)->up.y = 1.0f; (m)->right.x = 1.0f; \
    (m)->up.x = 0.0f; (m)->right.z = 0.0f; (m)->right.y = 0.0f; \
    (m)->at.y = 0.0f; (m)->at.x = 0.0f; (m)->up.z = 0.0f; \
    (m)->pos.z = 0.0f; (m)->pos.y = 0.0f; (m)->pos.x = 0.0f; } while (0)

/* Draws a line from `from` to `to` (default both the origin) in the colour rgba (default white). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void im3d_draw_line(float *from, float *to, unsigned char *rgba)
{
  float origin[6];
  unsigned char white[4];
  RwIm3DVertex v[2];
  RwUInt32 flags;

  origin[0] = 0.0f;
  origin[1] = 0.0f;
  origin[2] = 0.0f;
  origin[3] = 0.0f;
  origin[4] = 0.0f;
  origin[5] = 0.0f;
  white[0] = 0xff;
  white[1] = 0xff;
  white[2] = 0xff;
  white[3] = 0xff;
  flags = IM3D_LINE_FLAGS;
  if (from == 0) {
    from = &origin[0];
  }
  if (to == 0) {
    to = &origin[3];
  }
  if (rgba == 0) {
    rgba = white;
  }
  if (rgba[3] == COLOR_OPAQUE) {
    flags = IM3D_LINE_FLAGS_OPAQUE;
  }
  v[0].objVertex.x = from[0];
  v[0].objVertex.y = from[1];
  v[0].objVertex.z = from[2];
  v[1].objVertex.x = to[0];
  v[1].objVertex.y = to[1];
  v[1].objVertex.z = to[2];
  v[0].color = PACK_ARGB(rgba);
  v[1].color = v[0].color;
  if (rgba[3] != COLOR_OPAQUE) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
  if (RwIm3DTransform(v, 2, 0, flags)) {
    RwIm3DRenderLine(0, 1);
    RwIm3DEnd();
  }
  if (rgba[3] != COLOR_OPAQUE) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0);
  }
}

/* Draws the three axes of `matrix` (right, up, at; each `scale` long) from its position: red, green, blue, or all in
   the one colour rgba when given. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void im3d_draw_matrix_axes(RwMatrix *matrix, unsigned char *rgba, float scale)
{
  unsigned char axisColors[3][4];
  RwIm3DVertex v[6];
  RwUInt32 flags;
  int i;

  axisColors[0][0] = 0xff;
  axisColors[0][1] = 0;
  axisColors[0][2] = 0;
  axisColors[0][3] = 0xff;
  axisColors[1][0] = 0;
  axisColors[1][1] = 0xff;
  axisColors[1][2] = 0;
  axisColors[1][3] = 0xff;
  axisColors[2][0] = 0;
  axisColors[2][1] = 0;
  axisColors[2][2] = 0xff;
  axisColors[2][3] = 0xff;
  flags = IM3D_LINE_FLAGS_OPAQUE;
  if (rgba == 0) {
    for (i = 0; i < 3; i++) {
      v[2 * i].color = PACK_ARGB(axisColors[i]);
      v[2 * i + 1].color = v[2 * i].color;
    }
  } else {
    v[0].color = PACK_ARGB(rgba);
    v[1].color = v[0].color;
    v[2].color = v[0].color;
    v[3].color = v[0].color;
    v[4].color = v[0].color;
    v[5].color = v[0].color;
  }
  v[0].objVertex.x = matrix->pos.x;
  v[0].objVertex.y = matrix->pos.y;
  v[0].objVertex.z = matrix->pos.z;
  v[1].objVertex.x = scale * matrix->right.x + matrix->pos.x;
  v[1].objVertex.y = matrix->right.y * scale + matrix->pos.y;
  v[1].objVertex.z = matrix->right.z * scale + matrix->pos.z;
  v[2].objVertex.x = matrix->pos.x;
  v[2].objVertex.y = matrix->pos.y;
  v[2].objVertex.z = matrix->pos.z;
  v[3].objVertex.x = scale * matrix->up.x + matrix->pos.x;
  v[3].objVertex.y = matrix->up.y * scale + matrix->pos.y;
  v[3].objVertex.z = matrix->up.z * scale + matrix->pos.z;
  v[4].objVertex.x = matrix->pos.x;
  v[4].objVertex.y = matrix->pos.y;
  v[4].objVertex.z = matrix->pos.z;
  v[5].objVertex.x = scale * matrix->at.x + matrix->pos.x;
  v[5].objVertex.y = matrix->at.y * scale + matrix->pos.y;
  v[5].objVertex.z = matrix->at.z * scale + matrix->pos.z;
  if (rgba != 0 && rgba[3] != COLOR_OPAQUE) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
    flags = IM3D_LINE_FLAGS;
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
  if (RwIm3DTransform(v, 6, 0, flags)) {
    RwIm3DRenderLine(0, 1);
    RwIm3DRenderLine(2, 3);
    RwIm3DRenderLine(4, 5);
    RwIm3DEnd();
  }
  if (rgba != 0 && rgba[3] != COLOR_OPAQUE) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0);
  }
}

/* Draws the 12 edges of the box [-half, +half] (default half extents 0.5) in the colour rgba (default white),
   transformed by `matrix` when given.  Corner i has the bits x = 1, y = 2, z = 4 for the positive side. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void im3d_draw_wire_box(float *half, RwMatrix *matrix, unsigned char *rgba)
{
  float defaultHalf[3];
  unsigned char white[4];
  RwMatrix ltm;
  RwIm3DVertex v[8];
  RwUInt32 flags;

  white[0] = 0xff;
  white[1] = 0xff;
  white[2] = 0xff;
  white[3] = 0xff;
  defaultHalf[0] = 0.5f;
  defaultHalf[1] = 0.5f;
  defaultHalf[2] = 0.5f;
  flags = IM3D_LINE_FLAGS;
  if (rgba == 0) {
    rgba = white;
  }
  if (rgba[3] == COLOR_OPAQUE) {
    flags = IM3D_LINE_FLAGS_OPAQUE;
  }
  if (half == 0) {
    half = defaultHalf;
  }
  v[0].objVertex.x = -half[0];
  v[0].objVertex.y = -half[1];
  v[0].objVertex.z = -half[2];
  v[1].objVertex.x = half[0];
  v[1].objVertex.y = v[0].objVertex.y;
  v[1].objVertex.z = v[0].objVertex.z;
  v[0].color = PACK_ARGB(rgba);
  v[2].objVertex.x = v[0].objVertex.x;
  v[2].objVertex.y = half[1];
  v[2].objVertex.z = v[0].objVertex.z;
  v[3].objVertex.x = half[0];
  v[3].objVertex.y = half[1];
  v[3].objVertex.z = v[0].objVertex.z;
  v[4].objVertex.x = v[0].objVertex.x;
  v[4].objVertex.y = v[0].objVertex.y;
  v[4].objVertex.z = half[2];
  v[5].objVertex.x = half[0];
  v[5].objVertex.y = v[0].objVertex.y;
  v[5].objVertex.z = half[2];
  v[6].objVertex.x = v[0].objVertex.x;
  v[6].objVertex.y = half[1];
  v[6].objVertex.z = half[2];
  v[7].objVertex.x = half[0];
  v[7].objVertex.y = half[1];
  v[7].objVertex.z = half[2];
  v[1].color = v[0].color;
  v[2].color = v[0].color;
  v[3].color = v[0].color;
  v[4].color = v[0].color;
  v[5].color = v[0].color;
  v[6].color = v[0].color;
  v[7].color = v[0].color;
  MATRIX_SET_IDENTITY(&ltm);
  if (matrix != 0) {
    RwMatrixTransform(&ltm, matrix, rwCOMBINEREPLACE);
  }
  if ((flags & rwIM3D_ALLOPAQUE) == 0) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
  if (RwIm3DTransform(v, 8, &ltm, flags)) {
    RwIm3DRenderLine(0, 1);
    RwIm3DRenderLine(1, 3);
    RwIm3DRenderLine(3, 2);
    RwIm3DRenderLine(2, 0);
    RwIm3DRenderLine(4, 5);
    RwIm3DRenderLine(5, 7);
    RwIm3DRenderLine(7, 6);
    RwIm3DRenderLine(6, 4);
    RwIm3DRenderLine(0, 4);
    RwIm3DRenderLine(2, 6);
    RwIm3DRenderLine(1, 5);
    RwIm3DRenderLine(3, 7);
    RwIm3DEnd();
  }
  if ((flags & rwIM3D_ALLOPAQUE) == 0) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0);
  }
}

/* Draws the box with corners bbox->sup and bbox->inf as wire frame in the colour rgba (default white). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void im3d_draw_bbox(RwBBox *bbox, unsigned char *rgba)
{
  RwV3d half;
  RwV3d center;
  RwMatrix ltm;

  half.x = (bbox->sup.x - bbox->inf.x) * 0.5f;
  half.y = (bbox->sup.y - bbox->inf.y) * 0.5f;
  half.z = (bbox->sup.z - bbox->inf.z) * 0.5f;
  center.x = half.x + bbox->inf.x;
  center.y = half.y + bbox->inf.y;
  center.z = half.z + bbox->inf.z;
  MATRIX_SET_IDENTITY(&ltm);
  RwMatrixTranslate(&ltm, &center, rwCOMBINEREPLACE);
  im3d_draw_wire_box(&half.x, &ltm, rgba);
}

#define WIRE_MESH_MAX 0x2000    /* size of the scratch vertex buffer / triangle limit */
#define wire_mesh_vertices g_data_771f78
#define wire_mesh_indices g_data_7b9f78
/* The geometry data the wire mesh drawer reads: triangles (three vertex indices and a material) and vertex positions. */
struct WireTriangle {
  unsigned short v[3];
  unsigned short material;
};
struct WireMeshView {
  char pad0[4];
  struct WireTriangle *triangles;
  RwV3d *vertices;
  char pad1[0x76];
  unsigned short numVertices;
  unsigned short numTriangles;
};

/* Draws the triangle edges of the mesh (at most 0x2000 vertices / triangles) in the colour rgba (default white). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void im3d_draw_wire_mesh(struct WireMeshView *mesh, unsigned char *rgba)
{
  RwV3d *vertices = mesh->vertices;
  int numTriangles = mesh->numTriangles;
  int numVertices = mesh->numVertices;
  struct WireTriangle *tri = mesh->triangles;
  int numIndices = 0;
  int i;
  unsigned char white[4];
  RwUInt32 flags;

  white[0] = 0xff;
  white[1] = 0xff;
  white[2] = 0xff;
  white[3] = 0xff;
  flags = IM3D_LINE_FLAGS;
  if (numVertices != 0) {
    if (numVertices > WIRE_MESH_MAX) {
      numVertices = WIRE_MESH_MAX;
    }
    if (numTriangles > WIRE_MESH_MAX) {
      numTriangles = WIRE_MESH_MAX;
    }
    if (rgba == 0) {
      rgba = white;
    }
    if (rgba[3] == COLOR_OPAQUE) {
      flags = IM3D_LINE_FLAGS_OPAQUE;
    }
    for (i = 0; i < numTriangles; i++) {
      unsigned short a = tri[i].v[0];
      unsigned short b = tri[i].v[1];
      unsigned short c = tri[i].v[2];
      wire_mesh_indices[numIndices] = a;
      wire_mesh_indices[numIndices + 1] = b;
      wire_mesh_indices[numIndices + 2] = b;
      wire_mesh_indices[numIndices + 3] = c;
      wire_mesh_indices[numIndices + 4] = c;
      wire_mesh_indices[numIndices + 5] = a;
      numIndices += 6;
    }
    i = 0;
    if (numVertices >= 4) {
      for (; i + 3 < numVertices; i += 4) {
        wire_mesh_vertices[i].objVertex = vertices[i];
        wire_mesh_vertices[i].color = PACK_ARGB(rgba);
        wire_mesh_vertices[i + 1].objVertex = vertices[i + 1];
        wire_mesh_vertices[i + 1].color = PACK_ARGB(rgba);
        wire_mesh_vertices[i + 2].objVertex = vertices[i + 2];
        wire_mesh_vertices[i + 2].color = PACK_ARGB(rgba);
        wire_mesh_vertices[i + 3].objVertex = vertices[i + 3];
        wire_mesh_vertices[i + 3].color = PACK_ARGB(rgba);
      }
    }
    for (; i < numVertices; i++) {
      wire_mesh_vertices[i].objVertex = vertices[i];
      wire_mesh_vertices[i].color = PACK_ARGB(rgba);
    }
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
    if (rgba[3] != COLOR_OPAQUE) {
      RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
    }
    if (RwIm3DTransform(wire_mesh_vertices, numVertices, 0, flags)) {
      RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, wire_mesh_indices, numIndices);
      RwIm3DEnd();
    }
    if (rgba[3] != COLOR_OPAQUE) {
      RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0);
    }
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);
  }
}
