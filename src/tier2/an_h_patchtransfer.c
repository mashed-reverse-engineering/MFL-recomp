#include "an_h_rppatch.h"
/* RenderWare patch plugin (rppatch): the transfer of a locked patch mesh into its geometry (0x55a150) and the
   normal smoothing it uses (0x55b970, 0x55bc60).  Written from the disassembly of MFL.exe. */

extern void *memset(void *dest, int c, size_t count);
extern void *memcpy(void *dest, const void *src, size_t count);

#define PATCH_INDICES 16                  /* indices per patch in the mesh; a triangle patch uses 10, padded with 0 */
#define TRI_PATCH_TEX_MARK 10000.0f       /* tex coord written at control point 5 of a triangle patch */

/*00947778*/ extern RwInt32 meshModule;                   /* RwModuleInfo; the first field is the globals offset */
#define MESH_TRILIST_FLAGS RW_GLOBAL(meshModule + 0x2d, RwUInt8)      /* mesh globals: header flags of a triangle list */

/*004f8ba0*/ extern RpGeometryPart *RpGeometryLock(RpGeometryPart *geometry, RwInt32 lock_mode);
/*004f8bd0*/ extern RpGeometryPart *RpGeometryUnlock(RpGeometryPart *geometry);
/*004f8800*/ extern RpMorphTarget *RpMorphTargetCalcBoundingSphere(const RpMorphTarget *morph, RwSphere *sphere);
/*00500ba0*/ extern RpMeshHeader *_rpMeshHeaderCreate(RwUInt32 size);
/*0053e3b0*/ extern void _rpMeshHeaderDestroy(RpMeshHeader *header);
/*00501360*/ extern RwUInt16 _rpMeshGetNextSerialNumber(void);
/*004d53a0*/ extern RwReal RwV3dLength(const RwV3d *v);

#define LOCK_BIT(mode, from, to) ((((mode) >> (from)) & 1) << (to))
#define SMOOTH_MEMORY_HINT 0x30123
#define SMOOTH_IDX_GROW 8

typedef struct PatchSmoothIdxTable {      /* per control point: the other control points at the same position */
  RwUInt32 count;
  RwUInt32 capacity;
  RwUInt16 *idx;
} PatchSmoothIdxTable;

/* Appends value to the list of entry i (grown by 8 when full; the allocation is not checked, as in the exe). */
PatchSmoothIdxTable *PatchSmoothIdxTableAddIdx(PatchSmoothIdxTable *table, RwUInt32 i, RwUInt32 value)
{
  PatchSmoothIdxTable *entry = &table[i];
  RwUInt32 capacity;
  RwUInt16 *buffer, *old;

  if (entry->count == entry->capacity) {
    old = entry->idx;
    capacity = entry->capacity + SMOOTH_IDX_GROW;
    buffer = (RwUInt16 *)RwMalloc(capacity * sizeof(RwUInt16), SMOOTH_MEMORY_HINT);
    if (old != NULL) {
      memcpy(buffer, old, entry->count * sizeof(RwUInt16));
      RwFree(old);
    }
    entry->capacity = capacity;
    entry->idx = buffer;
  }
  entry->idx[entry->count] = (RwUInt16)value;
  entry->count++;
  return table;
}

/* Smooths the normals across control points that share a position (each coordinate within +-tolerance): every
   such normal whose dot product with this one is at least min_dot is added, the sum normalised.  in / out NULL =
   the mesh's own normals.  A point without partners keeps its normal (copied when in != out). */
RpPatchMesh *_rpPatchSmoothNormal(RpPatchMesh *mesh, RwReal tolerance, RwReal min_dot, RwV3d *in, RwV3d *out)
{
  RwUInt32 num_points, i, j, k;
  PatchSmoothIdxTable *table;
  RwV3d *positions, sum;
  RwReal dx, dy, dz, dot, length, inv;
  RwUInt16 *idx;

  if (in == NULL) {
    in = mesh->normals;
  }
  if (out == NULL) {
    out = mesh->normals;
  }
  num_points = mesh->definition.num_control_points;
  table = (PatchSmoothIdxTable *)RwMalloc(num_points * sizeof(PatchSmoothIdxTable), SMOOTH_MEMORY_HINT);
  memset(table, 0, num_points * sizeof(PatchSmoothIdxTable));
  positions = mesh->positions;

  for (i = 0; i < num_points; i++) {
    for (j = i + 1; j < num_points; j++) {
      dx = positions[i].x - positions[j].x;
      dy = positions[i].y - positions[j].y;
      dz = positions[i].z - positions[j].z;
      if (dx >= -tolerance && dx <= tolerance && dy >= -tolerance && dy <= tolerance && dz >= -tolerance && dz <= tolerance) {
        table = PatchSmoothIdxTableAddIdx(table, i, j);
        table = PatchSmoothIdxTableAddIdx(table, j, i);
      }
    }
  }

  for (i = 0; i < num_points; i++) {
    if (table[i].count > 0) {
      sum = in[i];
      idx = table[i].idx;
      for (k = table[i].count; k != 0; k--, idx++) {
        dot = in[*idx].y * in[i].y;      /* in the exe's order: (y*y + x*x) + z*z */
        dot += in[*idx].x * in[i].x;
        dot += in[*idx].z * in[i].z;
        if (dot >= min_dot) {
          sum.x += in[*idx].x;
          sum.y += in[*idx].y;
          sum.z += in[*idx].z;
        }
      }
      length = RwV3dLength(&sum);
      if (length > 0.0f) {
        inv = 1.0f / length;
        out[i].x = sum.x * inv;
        out[i].y = sum.y * inv;
        out[i].z = sum.z * inv;
      }
    } else if (out != in) {
      out[i] = in[i];
    }
  }

  for (i = 0; i < num_points; i++) {
    if (table[i].idx != NULL) {
      RwFree(table[i].idx);
    }
    table[i].idx = NULL;
    table[i].count = 0;
    table[i].capacity = 0;
  }
  RwFree(table);
  return mesh;
}

/* Copies the parts of the patch mesh named by its lock mode into its geometry: the patches become one mesh per
   material (16 indices per patch), positions (plus a new bounding sphere), normals (copied or smoothed),
   pre-light colours and tex coords.  Returns the mesh (the one _rpPatchSmoothNormal hands back). */
RpPatchMesh *_rpPatchGeometryTransferFromPatchMesh(RpPatchMesh *mesh)
{
  RwUInt32 num_points = mesh->definition.num_control_points;
  RpGeometryPart *geometry = (RpGeometryPart *)_rpPatchMeshGetGeometry(mesh);
  RpMorphTarget *morph = geometry->morph_target;
  RwUInt32 mode = mesh->lock_mode;
  RwUInt32 i, m, p, k, count, num_materials, num_indices;
  RpMeshHeader *header;
  RpMesh *entry;
  RxVertexIndex *index;
  RwSphere sphere;
  RwTexCoords *tex;

  geometry = RpGeometryLock(geometry, LOCK_BIT(mode, 15, 11) | LOCK_BIT(mode, 14, 10) | LOCK_BIT(mode, 13, 9)
                                    | LOCK_BIT(mode, 12, 8) | LOCK_BIT(mode, 11, 7) | LOCK_BIT(mode, 10, 6)
                                    | LOCK_BIT(mode, 9, 5) | LOCK_BIT(mode, 8, 4) | LOCK_BIT(mode, 3, 3)
                                    | LOCK_BIT(mode, 2, 2) | LOCK_BIT(mode, 1, 1) | (mode & 1));

  if (mesh->lock_mode & rpPATCHMESHLOCKPATCHES) {
    if (geometry->mesh != NULL) {
      _rpMeshHeaderDestroy(geometry->mesh);
      geometry->mesh = NULL;
    }
    _rpMaterialListDeinitialize(&geometry->mat_list);

    num_materials = mesh->mat_list.num_materials;
    num_indices = (mesh->definition.num_tri_patches + mesh->definition.num_quad_patches) * PATCH_INDICES;
    header = _rpMeshHeaderCreate(sizeof(RpMeshHeader) + num_materials * sizeof(RpMesh) + num_indices * sizeof(RxVertexIndex));
    header->flags = 0;
    header->num_meshes = 0;
    header->serial_num = 0;
    header->total_indices_in_mesh = 0;
    header->first_mesh_offset = 0;
    header->flags = (header->flags & ~rpMESHHEADERPRIMMASK) | MESH_TRILIST_FLAGS;
    entry = (RpMesh *)(header + 1);
    header->num_meshes = (RwUInt16)num_materials;
    index = (RxVertexIndex *)(entry + num_materials);
    header->serial_num = _rpMeshGetNextSerialNumber();
    header->total_indices_in_mesh = num_indices;
    header->first_mesh_offset = 0;

    for (m = 0; m < num_materials; m++) {
      count = 0;
      entry->indices = index;
      entry->material = mesh->mat_list.materials[m];
      for (p = 0; p < mesh->definition.num_quad_patches; p++) {
        QuadPatch *quad = &mesh->quad_patches[p];
        if (quad->mat_index == m) {
          for (k = 0; k < 16; k++) {
            index[k] = (RxVertexIndex)quad->cpt_idx[k];
          }
          index += PATCH_INDICES;
          count += PATCH_INDICES;
        }
      }
      for (p = 0; p < mesh->definition.num_tri_patches; p++) {
        TriPatch *tri = &mesh->tri_patches[p];
        if (tri->mat_index == m) {
          for (k = 0; k < 10; k++) {
            index[k] = (RxVertexIndex)tri->cpt_idx[k];
          }
          for (k = 10; k < PATCH_INDICES; k++) {
            index[k] = 0;
          }
          index += PATCH_INDICES;
          count += PATCH_INDICES;
        }
      }
      entry->num_indices = count;
      entry++;
    }
    geometry->mesh = header;
    for (i = 0; i < (RwUInt32)mesh->mat_list.num_materials; i++) {
      _rpMaterialListAppendMaterial(&geometry->mat_list, mesh->mat_list.materials[i]);
    }
  }

  if ((mesh->lock_mode & rpPATCHMESHLOCKPOSITIONS) && (mesh->definition.flag & rpPATCHMESHPOSITIONS)) {
    for (i = 0; i < num_points; i++) {
      morph->verts[i] = mesh->positions[i];
    }
    RpMorphTargetCalcBoundingSphere(morph, &sphere);
    morph->bounding_sphere = sphere;
  }

  if ((mesh->lock_mode & rpPATCHMESHLOCKNORMALS) && (mesh->definition.flag & rpPATCHMESHNORMALS)) {
    if (mesh->definition.flag & rpPATCHMESHSMOOTHNORMALS) {
      mesh = _rpPatchSmoothNormal(mesh, 0.0001f, 0.5f, mesh->normals, morph->normals);
    } else {
      for (i = 0; i < num_points; i++) {
        morph->normals[i] = mesh->normals[i];
      }
    }
  }

  if ((mesh->lock_mode & rpPATCHMESHLOCKPRELIGHTS) && (mesh->definition.flag & rpPATCHMESHPRELIGHTS)) {
    for (i = 0; i < num_points; i++) {
      geometry->pre_lit_lum[i] = mesh->pre_light_colors[i];
    }
  }

  if (mesh->definition.num_tex_coord_sets > 0) {
    for (k = 0; k < rwMAXTEXTURECOORDS; k++) {
      if ((mesh->lock_mode & (rpPATCHMESHLOCKTEXCOORDS1 << k)) && mesh->tex_coords[k] != NULL) {
        for (i = 0; i < num_points; i++) {
          geometry->tex_coords[k][i] = mesh->tex_coords[k][i];
        }
      }
    }
  }

  if ((mesh->lock_mode & rpPATCHMESHLOCKPATCHES) || (mesh->lock_mode & rpPATCHMESHLOCKTEXCOORDS1)) {
    tex = geometry->tex_coords[0];
    if (tex != NULL) {
      for (p = 0; p < mesh->definition.num_tri_patches; p++) {
        k = mesh->tri_patches[p].cpt_idx[5];
        tex[k].u = TRI_PATCH_TEX_MARK;
        tex[k].v = TRI_PATCH_TEX_MARK;
      }
      for (p = 0; p < mesh->definition.num_quad_patches; p++) {
        k = mesh->quad_patches[p].cpt_idx[5];
        tex[k].u = 0.0f;
        tex[k].v = 0.0f;
      }
    }
  }

  RpGeometryUnlock(geometry);
  return mesh;
}
