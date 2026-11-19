/* an_h_rppatch.h: the RenderWare patch plugin (rppatch) types and the exe functions / data the hand-written rppatch
   files (an_h_patchmesh.c, an_h_patchtransfer.c) share.  Layouts from the disassembly of MFL.exe. */
#ifndef AN_H_RPPATCH_H
#define AN_H_RPPATCH_H
#include "mfl_tier2.h"

#define rwID_PATCHPLUGIN 0x123
#define E_RW_NOMEM 0x80000013
#define PATCH_MEMORY_HINT 0x30028
#define rpPATCHMESHLOCKALL 0xffff
#define rwMAXTEXTURECOORDS 8
#define rwID_MATLIST 8

enum {                                    /* RpPatchMeshDefinition.flag */
  rpPATCHMESHPOSITIONS = 0x01,
  rpPATCHMESHNORMALS = 0x02,
  rpPATCHMESHPRELIGHTS = 0x04,
  rpPATCHMESHSMOOTHNORMALS = 0x40         /* normals are recomputed, not copied */
};

enum {                                    /* RpPatchMesh.lock_mode */
  rpPATCHMESHLOCKPATCHES = 0x0001,
  rpPATCHMESHLOCKPOSITIONS = 0x0002,
  rpPATCHMESHLOCKNORMALS = 0x0004,
  rpPATCHMESHLOCKPRELIGHTS = 0x0008,
  rpPATCHMESHLOCKTEXCOORDS1 = 0x0100      /* .. TEXCOORDS8 = 0x8000 */
};

typedef struct RpPatchMeshDefinition {
  RwUInt32 flag;
  RwUInt32 num_control_points;
  RwUInt32 num_tri_patches;
  RwUInt32 num_quad_patches;
  RwUInt32 num_tex_coord_sets;
} RpPatchMeshDefinition;

typedef struct RpMaterialList {
  void **materials;
  RwInt32 num_materials;
  RwInt32 space;
} RpMaterialList;

typedef struct QuadPatch { RwUInt32 cpt_idx[16]; RwUInt32 mat_index; } QuadPatch;      /* 0x44 bytes */
typedef struct TriPatch { RwUInt32 cpt_idx[10]; RwUInt32 mat_index; } TriPatch;         /* 0x2c bytes */

typedef struct RpPatchMesh {              /* 0x60 bytes, followed by the patch and control point arrays */
  RwV3d *positions;                       /* 0x00 */
  RwV3d *normals;
  RwRGBA *pre_light_colors;
  RwTexCoords *tex_coords[rwMAXTEXTURECOORDS];      /* 0x0c */
  RpPatchMeshDefinition definition;       /* 0x2c */
  RwUInt32 lock_mode;                     /* 0x40 */
  RwInt32 ref_count;                      /* 0x44 */
  RpMaterialList mat_list;                /* 0x48 */
  void *geometry;                         /* 0x54: RpGeometry built from the patches, NULL until needed */
  QuadPatch *quad_patches;                /* 0x58 */
  TriPatch *tri_patches;                  /* 0x5c */
} RpPatchMesh;

typedef struct RwError { RwInt32 pluginID; RwInt32 errorCode; } RwError;

typedef struct RpMorphTarget {
  void *parent_geometry;
  RwSphere bounding_sphere;               /* 0x04 */
  RwV3d *verts;                           /* 0x14 */
  RwV3d *normals;                         /* 0x18 */
} RpMorphTarget;

typedef RwUInt16 RxVertexIndex;
typedef struct RpMesh {                   /* 12 bytes */
  RxVertexIndex *indices;
  RwUInt32 num_indices;
  void *material;
} RpMesh;
typedef struct RpMeshHeader {             /* 0x10 bytes, followed by the RpMesh array and the indices */
  RwUInt32 flags;
  RwUInt16 num_meshes;
  RwUInt16 serial_num;
  RwUInt32 total_indices_in_mesh;
  RwUInt32 first_mesh_offset;
} RpMeshHeader;
#define rpMESHHEADERPRIMMASK 0xff

typedef struct RpGeometryPart {           /* the RpGeometry fields used here */
  char pad00[0x20];
  RpMaterialList mat_list;                /* 0x20 */
  void *triangles;                        /* 0x2c */
  RwRGBA *pre_lit_lum;                    /* 0x30 */
  RwTexCoords *tex_coords[rwMAXTEXTURECOORDS];      /* 0x34 */
  RpMeshHeader *mesh;                     /* 0x54 */
  RwInt32 num_morph_targets;              /* 0x58 */
  RpMorphTarget *morph_target;            /* 0x5c */
} RpGeometryPart;

typedef struct PatchGlobals {             /* the start of _rpPatchGlobals */
  RwInt32 engine_offset;
  RwInt32 atomic_offset;
  RwInt32 geometry_offset;
} PatchGlobals;

typedef struct PatchGeometryData {        /* the patch extension of a geometry */
  RpPatchMesh *patch_mesh;
  void *reserved;
} PatchGeometryData;

/*007d2128*/ extern char *RwEngineInstance;
/*007da860*/ extern PatchGlobals _rpPatchGlobals;
#define GEOMETRY_PATCH_DATA(geometry) ((PatchGeometryData *)((char *)(geometry) + _rpPatchGlobals.geometry_offset))

/*00559c10*/ extern void *_rpPatchMeshGetGeometry(RpPatchMesh *mesh);
/*00559c30*/ extern RpPatchMesh *rpPatchMeshAddRef(RpPatchMesh *mesh);
/*00503d90*/ extern void *_rpMaterialListDeinitialize(RpMaterialList *list);
/*00503e10*/ extern RwInt32 _rpMaterialListAppendMaterial(RpMaterialList *list, void *material);
#endif
