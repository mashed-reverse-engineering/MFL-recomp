// cluster-entries: vehicle_load
/* Installed cluster vehicle_load (tools/cluster_install.py from output/cluster_pilot_pg3/vehicle_load.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00422440*/ extern int __cdecl vehicle_part_id_has_damage_model(int part_id);
/*00423210*/ extern void __cdecl vehicle_set_scale(int vehicle, float scale);
/*00425740*/ extern void __cdecl load_groundbeam(int vehicle);
/*00425ee0*/ extern void __cdecl vehicle_render_state_init(int vehicle, void **textures, unsigned int shadow_camera);
/*0042a430*/ extern void *course_get_ambient_light(void);
/*0042a440*/ extern unsigned char *course_get_directional_light(void);
/*0042a450*/ extern int course_is_outdoor(void);
/*0042f780*/ extern void *__cdecl clump_load_by_name(const char *name, int unused_a, int unused_b);
/*0042f860*/ extern void *__cdecl texdict_load_named_b(const char *name, int unused_a, int unused_b);
/*004c3f90*/ extern int __cdecl FUN_004c3f90(void *clump, int *parts);
/*004c52b0*/ extern int __cdecl FUN_004c52b0(void *atomic, int a, int b);
/*004c53e0*/ extern void __cdecl FUN_004c53e0(void *geometry, int flags, int on);
/*004c66a0*/ extern void __cdecl FUN_004c66a0(void *memory, int size);
/*004d16e0*/ extern void __cdecl RwFrameSetIdentity(void *frame);
/*004d15a0*/ extern void * __cdecl RwFrameTransform(void *frame, void *matrix, int combine);
/*004d5d90*/ extern void *__cdecl RwTexDictionaryFindNamedTexture(void *dictionary, const char *name);
/*004d5e10*/ extern void *__cdecl RwTexDictionarySetCurrent(void *dictionary);
/*00550190*/ extern void __cdecl FUN_00550190(void *atomic);
/*005501d0*/ extern void __cdecl RpMatFXMaterialSetEffects(void *material, int effect);
/*00550350*/ extern void __cdecl RpMatFXMaterialSetupEnvMap(void *material, void *texture, void *frame, int use_frame_buffer_alpha, float coefficient);
/*0054d6a0*/ extern int __cdecl FUN_0054d6a0(void *geometry);
/*0054da20*/ extern void __cdecl RpCollisionGeometryBuildData(void *geometry, int a);
/*005542c0*/ extern int __cdecl FUN_005542c0(void *geometry);
/*00554120*/ extern void __cdecl FUN_00554120(void *geometry, const char *name, int a, int b);
/*005542f0*/ extern void *__cdecl FUN_005542f0(void *geometry, int index);
/*00554410*/ extern void __cdecl RpUserDataArraySetInt(void *array, int index, int value);
/*00555020*/ extern void __cdecl RpMorphGeometryCreateInterpolators(void *geometry, int count);
/*005550a0*/ extern void __cdecl RpMorphGeometrySetInterpolator(void *geometry, int index, int start, int end, float scale);
/*00555100*/ extern void __cdecl RpMorphAtomicSetCurrentInterpolator(void *atomic, int index);
/*007d2128*/ extern char *RwEngineInstance;
/*00659f00*/ extern VehicleRecord g_vehicle_table[];
/*00659d08*/ extern void *g_game_659d08;             /* texture dictionary of the vehicle models */
/*00659d60*/ extern void *g_game_659d60[];            /* per vehicle environment map frame */
/*00659d50*/ extern int g_game_659d50[];              /* per vehicle: loaded */
/*00659d0c*/ extern void *g_game_659d0c;              /* ambient light */
/*00659d70*/ extern void *g_particles_659d70;         /* directional light */
/*00659d74*/ extern void *g_game_659d74;              /* shadow texture */
/*0065a9b0*/ extern unsigned char *g_load_65a9b0;     /* shadow camera */
/*0065a9c8*/ extern int g_game_65a9c8;                /* number of loaded vehicles */
/*0064d88c*/ extern int g_game_64d88c;
/*005dd5a2*/ extern char g_game_5dd5a2[];             /* material effect name */
/*005de408*/ extern char g_game_5de408[];             /* ".dff" */
/*005de410*/ extern char g_game_5de410[];             /* ".txd" */

typedef unsigned int (__cdecl *VecStrlenFunc)(const char *);
#define rwstrlen RW_GLOBAL(0xf4, VecStrlenFunc)             /* stringFuncs.vecStrlen */
#define VEHICLE_RECORD_SIZE 0x2ac
#define MAX_VEHICLE_PARTS 128
#define MATFX_EFFECT_ENVMAP 2
#define VEHICLE_FLAG_ENVMAP 4
#define PART_LIGHT_FIRST 0x3b               /* parts 0x3b..0x3e: headlights (black), 0x11 0x15 0x18: white lights */
#define PART_LIGHT_LAST 0x3e
#define PART_MORPH 0x12
#define PART_COLLISION_EXTRA 0x40
#define COLOUR_BLACK 0xff000000
#define GEOMETRY_USERDATA_PART 0
#define GEOMETRY_USERDATA_PARTID 1

/* vehicle model description from the vehicle table (load_shuriken): names of the files and textures */
typedef struct VehicleModel {
  unsigned char unknown_00[0x10];
  char dff_name[0x10];                   /* 0x10 */
  char txd_name[0x10];                   /* 0x20 */
  char envmap_texture[0x10];             /* 0x30 */
  char light_texture[4][0x10];           /* 0x40 .. 0x7f */
} VehicleModel;

/* an atomic of the loaded clump */
typedef struct PartAtomic {
  unsigned char unknown_00[0x18];
  unsigned char *geometry;               /* 0x18 */
  unsigned char unknown_1c[0x2c];
  int user_data;                         /* 0x48 */
} PartAtomic;

typedef struct GeometryView {
  unsigned char unknown_00[0x20];
  void **materials;                      /* 0x20 */
  int material_count;                    /* 0x24 */
} GeometryView;

/* compile: /GS */

/* vehicle_load: (re)loads vehicle slot `vehicle` from model `model`: clears the 0x2ac byte record, loads the textures and the clump,
   registers every part atomic of the clump (user data, damage environment map, light colours, morph targets, collision data),
   strips the clump to the body parts, finds the light textures and sets up the shadow (outdoor: render state, indoor: ground beam) */
static unsigned char * load_shuriken_r(int p0);
extern void load_shuriken(void);
static void vehicle_strip_clump_to_body_parts_r(VehicleRecord * p0);
extern void vehicle_strip_clump_to_body_parts(void);
void vehicle_load(int vehicle, int model)
{
  /* one frame object, in the original's order: the loop temporaries, the four light textures, the file name buffer, the part list */
  struct {
    unsigned int colour_white;
    unsigned int colour_black;
    int part_index;
    int part_id;
    PartAtomic *part;
    void *texture[4];
    char name[64];
    int parts[MAX_VEHICLE_PARTS];
  } frame;
  VehicleRecord *record = &g_vehicle_table[vehicle];
  VehicleModel *definition;
  void *dictionary;
  void *clump;
  GeometryView *geometry;
  void *user_data;
  int material_index;
  PartAtomic *atomic;
  int part_id;
  int part_user_data;

  FUN_004c66a0(record, VEHICLE_RECORD_SIZE);
  definition = (VehicleModel *)load_shuriken_r(model);
  g_vehicle_table[vehicle].object = (int)definition;
  g_vehicle_table[vehicle].index = vehicle;
  g_vehicle_table[vehicle].field_298 = 1;
  dictionary = g_game_659d08;
  if (dictionary == 0) {
    rwstrcpy(frame.name, definition->txd_name);
    rwstrcat(frame.name, g_game_5de410);
    dictionary = texdict_load_named_b(definition->txd_name, 0, 0);
    g_game_659d08 = dictionary;
  }
  RwTexDictionarySetCurrent(dictionary);
  rwstrcpy(frame.name, ((VehicleModel *)g_vehicle_table[vehicle].object)->dff_name);
  rwstrcat(frame.name, g_game_5de408);
  clump = clump_load_by_name(((VehicleModel *)g_vehicle_table[vehicle].object)->dff_name, 0, 0);
  g_vehicle_table[vehicle].frame = (char *)clump;
  g_vehicle_table[vehicle].clump_part_count = FUN_004c3f90(clump, frame.parts);
  RwFrameSetIdentity(g_game_659d60[vehicle]);
  if (course_get_directional_light() != 0) {
    RwFrameTransform(g_game_659d60[vehicle], *(unsigned char **)(course_get_directional_light() + 4) + 0x10, 0);
  }
  g_vehicle_table[vehicle].envmap_frame = (unsigned int)g_game_659d60[vehicle];
  g_vehicle_table[vehicle].flags |= VEHICLE_FLAG_ENVMAP;
  vehicle_set_scale(vehicle, 1.0f);
  g_vehicle_table[vehicle].envmap_angle = 0.0f;
  if (rwstrlen(((VehicleModel *)g_vehicle_table[vehicle].object)->envmap_texture) != 0) {
    g_vehicle_table[vehicle].envmap_texture = (unsigned int)RwTexDictionaryFindNamedTexture(g_game_659d08, ((VehicleModel *)g_vehicle_table[vehicle].object)->envmap_texture);
  }
  for (frame.part_index = 0; frame.part_index < g_vehicle_table[vehicle].clump_part_count; frame.part_index++) {
    frame.part = (PartAtomic *)frame.parts[frame.part_index];
    atomic = frame.part;
    geometry = (GeometryView *)atomic->geometry;
    part_user_data = atomic->user_data;
    if (FUN_005542c0(geometry) == 1) {
      FUN_00554120(geometry, g_game_5dd5a2, 1, 2);
    }
    user_data = FUN_005542f0(geometry, 1);
    RpUserDataArraySetInt(user_data, GEOMETRY_USERDATA_PART, (int)record);
    RpUserDataArraySetInt(user_data, GEOMETRY_USERDATA_PARTID, part_user_data);
    part_id = FUN_004c52b0(atomic, 0, 0);
    frame.part_id = part_id;
    record->part_frames[part_id] = (int)atomic;
    if (vehicle_part_id_has_damage_model(part_id) != 0) {
      FUN_00550190(atomic);
      for (material_index = 0; material_index < geometry->material_count; material_index++) {
        void *material = geometry->materials[material_index];
        RpMatFXMaterialSetEffects(material, MATFX_EFFECT_ENVMAP);
        RpMatFXMaterialSetupEnvMap(material, (void *)record->envmap_texture, (void *)record->envmap_frame, 0, record->half_scale);
      }
    }
    if (part_id >= PART_LIGHT_FIRST && part_id <= PART_LIGHT_LAST) {
      frame.colour_black = COLOUR_BLACK;
      FUN_004c53e0(geometry, 0x20, 0);
      FUN_004c53e0(geometry, 0x40, 1);
      *(unsigned int *)(*(unsigned char **)geometry->materials + 4) = frame.colour_black;
    }
    if (part_id == 0x11 || part_id == 0x15 || part_id == 0x18) {
      frame.colour_white = COLOUR_WHITE_ARGB;
      FUN_004c53e0(geometry, 0x20, 0);
      FUN_004c53e0(geometry, 0x40, 1);
      *(unsigned int *)(*(unsigned char **)geometry->materials + 4) = frame.colour_white;
    }
    if (part_id == PART_MORPH) {
      RpMorphGeometryCreateInterpolators(geometry, 1);
      RpMorphGeometrySetInterpolator(geometry, 0, 0, 1, 1.0f);
      RpMorphAtomicSetCurrentInterpolator(atomic, 0);
    } else if (part_id >= PART_LIGHT_FIRST && (part_id <= PART_LIGHT_LAST || part_id == PART_COLLISION_EXTRA) &&
               FUN_0054d6a0(atomic->geometry) == 0) {
      RpCollisionGeometryBuildData(atomic->geometry, 0);
    }
  }
  vehicle_strip_clump_to_body_parts_r(record);
  frame.texture[0] = RwTexDictionaryFindNamedTexture(g_game_659d08, ((VehicleModel *)g_vehicle_table[vehicle].object)->light_texture[0]);
  frame.texture[1] = RwTexDictionaryFindNamedTexture(g_game_659d08, ((VehicleModel *)g_vehicle_table[vehicle].object)->light_texture[1]);
  frame.texture[2] = RwTexDictionaryFindNamedTexture(g_game_659d08, ((VehicleModel *)g_vehicle_table[vehicle].object)->light_texture[2]);
  frame.texture[3] = RwTexDictionaryFindNamedTexture(g_game_659d08, ((VehicleModel *)g_vehicle_table[vehicle].object)->light_texture[3]);
  if (g_game_64d88c == 0) {
    g_vehicle_table[vehicle].shadow_camera = (unsigned int)g_load_65a9b0;
    g_vehicle_table[vehicle].shadow_frame = *(unsigned int *)(g_load_65a9b0 + 4);
    g_vehicle_table[vehicle].shadow_texture = (unsigned int)g_game_659d74;
    frame.texture[0] = g_game_659d74;
    frame.texture[1] = g_game_659d74;
    frame.texture[2] = g_game_659d74;
    frame.texture[3] = g_game_659d74;
    if (course_is_outdoor() != 0) {
      vehicle_render_state_init(vehicle, frame.texture, g_vehicle_table[vehicle].shadow_camera);
    } else {
      load_groundbeam(vehicle);
    }
  }
  g_particles_659d70 = course_get_directional_light();
  g_game_659d0c = course_get_ambient_light();
  g_game_65a9c8++;
  g_game_659d50[vehicle] = 1;
  RwTexDictionarySetCurrent(0);
}

/* load_shuriken_r: C -> original load_shuriken (registers esi) */
__declspec(naked) unsigned char * load_shuriken_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call load_shuriken
        pop esi
        ret
    }
}

/* vehicle_strip_clump_to_body_parts_r: C -> original vehicle_strip_clump_to_body_parts (registers eax) */
__declspec(naked) void vehicle_strip_clump_to_body_parts_r(VehicleRecord * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call vehicle_strip_clump_to_body_parts
        ret
    }
}
