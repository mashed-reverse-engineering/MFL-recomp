#include "an_h_rppatch.h"
/* RenderWare patch plugin (rppatch): patch mesh creation, reference counting and unlock (0x559a10 .. 0x559c30,
   0x559c50), the geometry built from a patch mesh (0x55a0d0), the geometry extension destructor / stream read
   (0x55a630, 0x55a6c0) and the patch mesh stream read (0x55bd70).  Written from the disassembly of MFL.exe.
   RpPatchPluginAttach (an_h_patchplugin.c) takes the extension callbacks' addresses. */

enum {                                    /* RpGeometry format flags */
  rpGEOMETRYTRISTRIP = 0x01,
  rpGEOMETRYTEXCOORDSETS_MASK = 0xff0000
};

extern void *memset(void *dest, int c, size_t count);

/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);
/*00503df0*/ extern void *_rpMaterialListInitialize(RpMaterialList *list);
/*005040c0*/ extern RpMaterialList *_rpMaterialListStreamRead(void *stream, RpMaterialList *list);
/*004f8e30*/ extern void *RpGeometryCreate(RwInt32 num_verts, RwInt32 num_triangles, RwUInt32 format);
/*004f9060*/ extern RwBool RpGeometryDestroy(void *geometry);
/*004f9050*/ extern void *RpGeometryAddRef(void *geometry);
/*004dc8d0*/ extern void *RwStreamReadInt32(void *stream, void *buffer, RwUInt32 length);
/*004dc720*/ extern RwBool RwStreamFindChunk(void *stream, RwUInt32 type, RwUInt32 *length, RwUInt32 *version);
/*0055a150*/ extern RpPatchMesh *_rpPatchGeometryTransferFromPatchMesh(RpPatchMesh *mesh);

#define FLAG_BIT(flag, from, to) ((((flag) >> (from)) & 1) << (to))

void *_rpPatchGeometryCreateFromPatchMesh(RpPatchMesh *mesh);
RpPatchMesh *_rpPatchMeshStreamRead(void *stream);

/* Allocates a patch mesh with all arrays the definition asks for in one zeroed block; NULL (error set) on failure. */
RpPatchMesh *_rpPatchMeshCreate(RpPatchMeshDefinition *definition)
{
  RwUInt32 vertex_words, size, i;
  RpPatchMesh *mesh;
  char *next;
  RwError error;

  vertex_words = ((definition->flag & rpPATCHMESHPOSITIONS) + ((definition->flag >> 1) & 1)) * 3      /* positions, normals */
               + ((definition->flag >> 2) & 1)                                                       /* pre-light colours */
               + definition->num_tex_coord_sets * 2;
  size = sizeof(RpPatchMesh) + 4 * (vertex_words * definition->num_control_points
                                    + definition->num_quad_patches * (sizeof(QuadPatch) / 4)
                                    + definition->num_tri_patches * (sizeof(TriPatch) / 4));
  mesh = (RpPatchMesh *)RwMalloc(size, PATCH_MEMORY_HINT);
  if (mesh == NULL) {
    error.pluginID = rwID_PATCHPLUGIN;
    error.errorCode = _rwerror(E_RW_NOMEM, size);
    RwErrorSet(&error);
    return NULL;
  }
  memset(mesh, 0, size);

  mesh->quad_patches = (QuadPatch *)(mesh + 1);
  mesh->tri_patches = (TriPatch *)(mesh->quad_patches + definition->num_quad_patches);
  next = (char *)(mesh->tri_patches + definition->num_tri_patches);
  if (definition->flag & rpPATCHMESHPOSITIONS) {
    mesh->positions = (RwV3d *)next;
    next += definition->num_control_points * sizeof(RwV3d);
  }
  if (definition->flag & rpPATCHMESHNORMALS) {
    mesh->normals = (RwV3d *)next;
    next += definition->num_control_points * sizeof(RwV3d);
  }
  if (definition->flag & rpPATCHMESHPRELIGHTS) {
    mesh->pre_light_colors = (RwRGBA *)next;
    next += definition->num_control_points * sizeof(RwRGBA);
  }
  for (i = 0; i < definition->num_tex_coord_sets; i++) {
    mesh->tex_coords[i] = (RwTexCoords *)next;
    next += definition->num_control_points * sizeof(RwTexCoords);
  }

  mesh->definition.flag = definition->flag;
  mesh->definition.num_quad_patches = definition->num_quad_patches;
  mesh->definition.num_tri_patches = definition->num_tri_patches;
  mesh->definition.num_control_points = definition->num_control_points;
  mesh->definition.num_tex_coord_sets = definition->num_tex_coord_sets;
  mesh->ref_count = 1;
  if (!_rpMaterialListInitialize(&mesh->mat_list)) {
    error.pluginID = rwID_PATCHPLUGIN;
    error.errorCode = _rwerror(E_RW_NOMEM, size);
    RwErrorSet(&error);
    RwFree(mesh);
    return NULL;
  }
  mesh->lock_mode = rpPATCHMESHLOCKALL;
  return mesh;
}

/* Drops a reference.  At one reference left the cached geometry goes (its destroy result is returned); at zero the
   mesh itself. */
RwBool rpPatchMeshDestroy(RpPatchMesh *mesh)
{
  RwInt32 count = --mesh->ref_count;
  void *geometry;

  if (count == 1) {
    geometry = mesh->geometry;
    if (geometry != NULL) {
      mesh->geometry = NULL;
      return RpGeometryDestroy(geometry);
    }
  } else if (count == 0) {
    _rpMaterialListDeinitialize(&mesh->mat_list);
    RwFree(mesh);
  }
  return TRUE;
}

/* The geometry the mesh refines to, created on first use. */
void *_rpPatchMeshGetGeometry(RpPatchMesh *mesh)
{
  if (mesh->geometry == NULL) {
    mesh->geometry = _rpPatchGeometryCreateFromPatchMesh(mesh);
  }
  return mesh->geometry;
}

/* Ends a lock: changed data moves to the geometry first. */
RpPatchMesh *RpPatchMeshUnlock(RpPatchMesh *mesh)
{
  if (mesh->lock_mode & rpPATCHMESHLOCKALL) {
    mesh = _rpPatchGeometryTransferFromPatchMesh(mesh);
  }
  mesh->lock_mode = 0;
  return mesh;
}

/* A geometry with one vertex per control point and the matching format flags (positions, normals, prelight,
   textured, light, modulate, tex coord set count); it holds a reference to the mesh.  Not checked for NULL, as in
   the exe. */
void *_rpPatchGeometryCreateFromPatchMesh(RpPatchMesh *mesh)
{
  RwUInt32 flag = mesh->definition.flag;
  void *geometry;

  geometry = RpGeometryCreate(mesh->definition.num_control_points, 0,
                              FLAG_BIT(flag, 5, 6) | FLAG_BIT(flag, 4, 5) | FLAG_BIT(flag, 1, 4) | FLAG_BIT(flag, 2, 3)
                              | FLAG_BIT(flag, 3, 2) | FLAG_BIT(flag, 0, 1) | (flag & rpGEOMETRYTEXCOORDSETS_MASK)
                              | rpGEOMETRYTRISTRIP);
  GEOMETRY_PATCH_DATA(geometry)->patch_mesh = mesh;
  rpPatchMeshAddRef(mesh);
  return geometry;
}

void *rpPatchGeometryDestructor(void *geometry, RwInt32 offset, RwInt32 size)
{
  PatchGeometryData *data = GEOMETRY_PATCH_DATA(geometry);

  if (data->patch_mesh != NULL) {
    rpPatchMeshDestroy(data->patch_mesh);
  }
  data->patch_mesh = NULL;
  data->reserved = NULL;
  return geometry;
}

/* Reads a patch mesh into the geometry; the mesh keeps a reference to the geometry.  Like the exe, a failed read
   is not checked. */
void *rpPatchGeometryRead(void *stream, RwInt32 length, void *geometry, RwInt32 offset, RwInt32 size)
{
  PatchGeometryData *data = GEOMETRY_PATCH_DATA(geometry);

  data->patch_mesh = _rpPatchMeshStreamRead(stream);
  data->patch_mesh->geometry = geometry;
  RpGeometryAddRef(data->patch_mesh->geometry);
  return stream;
}

/* Reads a patch mesh: definition, control point arrays (all tex coord sets in one read), quad and triangle
   patches, then the material list.  NULL when a read fails (the partly read mesh is not freed, as in the exe). */
RpPatchMesh *_rpPatchMeshStreamRead(void *stream)
{
  RpPatchMeshDefinition definition;
  RpPatchMesh *mesh;
  RwUInt32 flag, num_tex_coord_sets;

  stream = RwStreamReadInt32(stream, &definition, sizeof(definition));
  if (stream == NULL) {
    return NULL;
  }
  mesh = _rpPatchMeshCreate(&definition);
  flag = mesh->definition.flag;
  num_tex_coord_sets = mesh->definition.num_tex_coord_sets;
  if (flag & rpPATCHMESHPOSITIONS) {
    stream = RwStreamReadInt32(stream, mesh->positions, definition.num_control_points * sizeof(RwV3d));
    if (stream == NULL) {
      return NULL;
    }
  }
  if (flag & rpPATCHMESHNORMALS) {
    stream = RwStreamReadInt32(stream, mesh->normals, definition.num_control_points * sizeof(RwV3d));
    if (stream == NULL) {
      return NULL;
    }
  }
  if (flag & rpPATCHMESHPRELIGHTS) {
    stream = RwStreamReadInt32(stream, mesh->pre_light_colors, definition.num_control_points * sizeof(RwRGBA));
    if (stream == NULL) {
      return NULL;
    }
  }
  if (num_tex_coord_sets > 0) {
    stream = RwStreamReadInt32(stream, mesh->tex_coords[0], definition.num_control_points * num_tex_coord_sets * sizeof(RwTexCoords));
    if (stream == NULL) {
      return NULL;
    }
  }
  if (definition.num_quad_patches > 0) {
    stream = RwStreamReadInt32(stream, mesh->quad_patches, definition.num_quad_patches * sizeof(QuadPatch));
    if (stream == NULL) {
      return NULL;
    }
  }
  if (definition.num_tri_patches > 0) {
    stream = RwStreamReadInt32(stream, mesh->tri_patches, definition.num_tri_patches * sizeof(TriPatch));
    if (stream == NULL) {
      return NULL;
    }
  }
  RwStreamFindChunk(stream, rwID_MATLIST, NULL, NULL);      /* result not checked, as in the exe */
  _rpMaterialListStreamRead(stream, &mesh->mat_list);
  return mesh;
}
