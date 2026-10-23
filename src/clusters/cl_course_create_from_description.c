// cluster-entries: course_create_from_description
/* Installed cluster course_create_from_description (tools/cluster_install.py from output/cluster_pilot_ql2/course_create_from_description.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/*004a8420*/ extern int __cdecl debug_printf(const char *format, ...);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(const char *format, ...);
/*00488d70*/ extern void __cdecl table_clear_328f(CourseWorld *world);
/*0042f860*/ extern unsigned int __cdecl texdict_load_named_b(const char *name, int unused_a, int unused_b);
/*0042f7f0*/ extern unsigned int __cdecl texdict_load_named_a(const char *name, int unused_a, int unused_b);
/*0042f9a0*/ extern unsigned int __cdecl texdict_load_named_c(const char *name, int unused_a, int unused_b);
/*0042fa10*/ extern unsigned int __cdecl texdict_load_named_d(const char *name, int unused_a, int unused_b);
/*0042f780*/ extern unsigned int __cdecl clump_load_by_name(const char *name, int *length, int *version);
/*0042f8f0*/ extern unsigned int __cdecl rtdict_load_named(const char *name, int *version, int *length);
/*004c42b0*/ extern int __cdecl FUN_004c42b0(void *texture_dictionary, int *textures);
/*004c41c0*/ extern int __cdecl FUN_004c41c0(void *world, int *materials);
/*004c3fe0*/ extern void __cdecl FUN_004c3fe0(unsigned int clump, int *lights);
/*004c3fd0*/ extern void __cdecl FUN_004c3fd0(void *clump);
/*004c3f60*/ extern void *__cdecl FUN_004c3f60(void *clump);
/*004882b0*/ extern void __cdecl course_error_exit(int code);
/*00569270*/ extern void __cdecl RtDictSchemaSetCurrentDict(void *schema, unsigned int dict);
/*0063a38c*/ extern char _rpUVAnimDictSchema[];
/*00488190*/ extern int __cdecl matrix_list_load(const char *name, void *matrices);
/*00549090*/ extern unsigned int __cdecl FUN_00549090(void *world);
/*004848d0*/ extern unsigned int __cdecl clump_has_uv_anim(unsigned int clump);
/*00496ff0*/ extern void __cdecl atomic_world_bbox_update(void *atomic, RwBBox *box);
/*00496c40*/ extern void __cdecl smplfzx_object_create_from_atomic(void *atomic, float *position, RwBBox *box, int index);
/*004d5d90*/ extern void *__cdecl RwTexDictionaryFindNamedTexture(void *dictionary, const char *name);
/*004d5e10*/ extern void __cdecl RwTexDictionarySetCurrent(void *dictionary);
/*005501d0*/ extern void __cdecl RpMatFXMaterialSetEffects(void *material, int effects);
/*004f4ef0*/ extern char *__cdecl RpLightCreate(int type);
/*004f4a20*/ extern void __cdecl RpLightSetColor(void *light, const void *colour);
/*004f4930*/ extern void __cdecl RpWorldAddLight(void *world, void *light);
/*004f4570*/ extern void __cdecl RpWorldAddClump(void *world, void *clump);
/*004f6790*/ extern int __cdecl RpClumpGetNumLights(unsigned int clump);
/*004f6ae0*/ extern void *__cdecl RpAtomicClone(void *atomic);
/*004f7f70*/ extern void __cdecl RpAtomicSetFrame(void *atomic, void *frame);
/*004f70c0*/ extern void __cdecl RpClumpAddAtomic(void *clump, void *atomic);
/*004d0c50*/ extern void *__cdecl RwFrameCreate(void);
/*004d1860*/ extern void __cdecl _rwObjectHasFrameSetFrame(void *object, void *frame);
/*004d1640*/ extern void *__cdecl RwFrameRotate(void *frame, RwV3d *axis, float angle, int combine);
/*004d1460*/ extern void *__cdecl RwFrameTranslate(void *frame, RwV3d *translation, int combine);
/*004d15a0*/ extern void *__cdecl RwFrameTransform(void *frame, const void *matrix, int combine);
/*004d1160*/ extern void __cdecl RwFrameAddChild(void *frame, void *child);
/*005fb098*/ extern int g_course_5fb098;           /* set: drop the material fx of the course textures (low detail) */
/*0064d894*/ extern int g_course_64d894;           /* set: strip the material fx of the world */
/*006294b8*/ extern char g_course_create_6294b8[];  /* default ambient light colour (RwRGBAReal) */
/*006294c8*/ extern char g_course_create_6294c8[];  /* default directional light colour */
/*006294d8*/ extern char g_course_create_6294d8[];  /* ambient colour per course id (16 bytes each) */
/*00629758*/ extern char g_course_create_629758[];  /* directional colour per course id */

#define COURSE_FILE_NAME_SIZE 0x40            /* file name slots in CourseFiles */
#define COURSE_CLUMPS 128
#define COURSE_MTS_LISTS 16
#define COURSE_MTS_LIST_BYTES 0xc00
#define COURSE_MTS_MAX_MATRICES 0x30
#define COURSE_SKY_CLUMPS 4
#define COURSE_SPLINES 16
#define COURSE_GENERAL_ANIMS 8
#define COURSE_UVA_DICTS 8
#define COURSE_OBJECT_BSPS_MAX 16
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_AMBIENT 2
#define LIGHT_FLAGS_ON 1
#define DIRECTIONAL_LIGHT_TURN 60.0f
#define TEXTURE_FORMAT_LOW 4                  /* texture raster format byte replaced when the course runs with the low detail flag */
#define TEXTURE_FORMAT_REPLACEMENT 6
#define CLUMP_MTS_NO_BBOX ((short)-1)
#define COLLISION_BSP_FLAG_MASK 0xffffffbf
#define ERROR_EXIT_WORLD 0
#define ERROR_EXIT_COLLISION 2

/* RenderWare object header fields touched directly */
#define CLUMP_STATE(world) ((unsigned int *)(world)->_gap_c7b0)          /* second per clump flag array behind clump_loaded */
#define CLUMP_UV_ANIM(world) ((unsigned int *)((world)->_gap_c320 + 0x10)) /* per clump: has a uv animation */
#define OBJECT_SUBTYPE(object) (((unsigned char *)(object))[1])
#define OBJECT_FLAGS(object) (((unsigned char *)(object))[2])
#define OBJECT_FRAME(object) (*(void **)((char *)(object) + 4))
#define TEXTURE_FORMAT(texture) (((unsigned char *)(texture))[0x50])
#define BSP_FLAGS(bsp) (*(unsigned int *)((char *)(bsp) + 8))

/* One clump / MTS pair entry of CourseCounts.clump_mts_pairs: {u8 clump, u8 mts, s16 bbox index} */
typedef struct ClumpMtsPair {
  signed char clump;
  signed char mts;
  short bbox_index;
} ClumpMtsPair;

/* The frame block of the original (0x444 bytes): the first words are loop spill slots that the code reuses as the out values of
   clump_load_by_name, the rest are the objects whose addresses are passed to callees. */
typedef struct CourseCreateFrame {
  void *list_cursor;           /* walks the file name slot / table of the running loop */
  int clump_length;            /* out values of clump_load_by_name */
  int clump_version;
  int loop_counter;
  RwV3d vector;                /* light rotation axis / clump translation */
  union {
    int lights[4 + 6 + 256];   /* the light list may run over the next two blocks (up to the number of lights of the clump) */
    struct {
      int mts_count;           /* atomics per MTS list (clump / MTS pair loop) */
      int unused;
      int uva_version;         /* out values of rtdict_load_named */
      int uva_length;
      RwBBox bbox;
      int list[256];           /* textures / materials of the dictionary / world */
    } w;
  } u;
} CourseCreateFrame;

/* course_create_from_description: "Creating course from description": loads every file named by `files` (texture dictionary, world BSP,
   collision / hoppy / object / AI BSPs, UVA dictionaries, clumps, lights, MTS matrix lists, sky domes, splines, camera path, general anims)
   into `world`, adds the lights and clumps to the RenderWare world and instances the clump MTS pairs.  Returns 0 when a clump or spline
   cannot be loaded, else 1. */
static void world_sectors_process_r(void * p0);
extern void world_sectors_process(void);
int course_create_from_description(CourseWorld *world, CourseFiles *files, CourseCounts *counts)
{
  CourseCreateFrame frame;
  unsigned int handle;
  int i, j, count;
  char *name;
  unsigned int *slot;
  void *texture;
  void *dictionary;
  char *light;

  debug_printf("Creating course from description\n");
  debug_printf("\tResetting Course vars to zero\n");
  table_clear_328f(world);
  debug_printf("\tLoading texdic [%s]\n", files->texture_dictionary);
  handle = texdict_load_named_b(files->texture_dictionary, 0, 0);
  world->texture_dictionary = (void *)handle;
  if (handle == 0) {
    debug_printf("failed to load texture dictionary\n");
  } else {
    debug_printf("\tLoaded course texture dictionary successfully\n");
  }
  if (g_course_5fb098 != 0 && world->texture_dictionary != 0) {
    count = FUN_004c42b0(world->texture_dictionary, frame.u.w.list);
    for (i = 0; i < count; i++) {
      if (TEXTURE_FORMAT(frame.u.w.list[i]) == TEXTURE_FORMAT_LOW) {
        TEXTURE_FORMAT(frame.u.w.list[i]) = TEXTURE_FORMAT_REPLACEMENT;
      }
    }
  }
  if (world->texture_dictionary != 0) {
    texture = RwTexDictionaryFindNamedTexture(world->texture_dictionary, "BarrelRed");
    dictionary = 0;
    if (texture != 0) {
      dictionary = world->texture_dictionary;
    }
    RwTexDictionarySetCurrent(dictionary);
  }
  debug_printf("\tLoading world [%s]\n", files->world_bsp);
  handle = texdict_load_named_a(files->world_bsp, 0, 0);
  world->world = (void *)handle;
  if (handle == 0) {
    debug_printf("failed to load bsp\n");
  } else {
    debug_printf("\tLoaded bsp successfully\n");
  }
  if (world->world == 0) {
    debug_printf("\nCOURSES ERROR: loading world\n");
    course_error_exit(ERROR_EXIT_WORLD);
  }
  if (g_course_64d894 != 0) {
    count = FUN_004c41c0(world->world, frame.u.w.list);
    for (i = 0; i < count; i++) {
      RpMatFXMaterialSetEffects((void *)frame.u.w.list[i], 0);
    }
  }
  debug_printf("\tloading %d collision bsps\n", counts->collision_bsp_count);
  slot = world->collision_bsps;
  name = files->collision_bsp;
  for (i = 0; i < counts->collision_bsp_count; i++, slot++, name += COURSE_FILE_NAME_SIZE) {
    if (*name == '\0') {
      debug_printf("\nCOURSES ERROR: no name for invisible collision %d\n", i);
      continue;
    }
    debug_printf("\t\tLoading collision bsp [%s]\n", name);
    handle = texdict_load_named_a(name, 0, 0);
    *slot = handle;
    if (handle == 0) {
      debug_printf("failed to load collision %d\n", i);
      course_error_exit(ERROR_EXIT_COLLISION);
      continue;
    }
    BSP_FLAGS(handle) &= COLLISION_BSP_FLAG_MASK;
    debug_printf("\t\tcollision %d loaded succsessfully\n", i);
  }
  if (world->collision_bsps[1] == 0) {
    world->collision_bsps[1] = world->collision_bsps[0];
  }
  if (world->collision_bsps[2] == 0) {
    world->collision_bsps[2] = world->collision_bsps[0];
  }
  if (world->collision_bsps[3] == 0) {
    world->collision_bsps[3] = world->collision_bsps[0];
  }
  slot = world->hoppy_bsps;
  name = files->hoppy_bsp;
  for (i = 0; i < counts->hoppy_bsp_count; i++, slot++, name += COURSE_FILE_NAME_SIZE) {
    if (*name == '\0') {
      debug_printf("\nCOURSES ERROR: no name for hoppy collision %d\n", i);
    } else {
      *slot = texdict_load_named_a(name, 0, 0);
    }
  }
  name = files->object_bsp_files;
  for (frame.loop_counter = 0; frame.loop_counter < counts->object_bsp_count; frame.loop_counter++, name += COURSE_FILE_NAME_SIZE) {
    if (*name != '\0') {
      handle = texdict_load_named_a(name, 0, 0);
      world->object_bsp_worlds[frame.loop_counter] = handle;
      if (handle != 0) {
        world->object_bsp_loaded[frame.loop_counter] = 1;
      }
    }
  }
  slot = world->ai_worlds;
  name = files->ai_bsp;
  for (i = 0; i < counts->ai_bsp_count; i++, slot++, name += COURSE_FILE_NAME_SIZE) {
    if (*name == '\0') {
      debug_printf("COURSES ERROR: no aidata name for aidata %d\n", i);
    } else {
      *slot = texdict_load_named_a(name, 0, 0);
    }
    if (*slot != 0) {
      debug_printf("Ai data %d loaded\n", i);
    }
  }
  slot = world->uva_dicts;
  name = files->uva_files;
  for (i = 0; i < COURSE_UVA_DICTS; i++, slot++, name += COURSE_FILE_NAME_SIZE) {
    if (*name != '\0') {
      FID_conflict__wprintf("uva file exists\n");
      handle = rtdict_load_named(name, &frame.u.w.uva_length, &frame.u.w.uva_version);
      *slot = handle;
      if (handle != 0) {
        RtDictSchemaSetCurrentDict(_rpUVAnimDictSchema, handle);
        FID_conflict__wprintf("Loaded uva dict length (%d), version (%d)\n", frame.u.w.uva_length, frame.u.w.uva_version);
      }
    }
  }
  if (counts->course_id == 0) {
    RtDictSchemaSetCurrentDict(_rpUVAnimDictSchema, world->uva_dicts[0]);
  }
  debug_printf("Loading %d clumps for the world\n", counts->clump_count);
  name = files->clump_files;
  for (i = 0; i < COURSE_CLUMPS; i++, name += COURSE_FILE_NAME_SIZE) {
    frame.clump_length = 0;
    frame.clump_version = 0;
    if (*name != '\0') {
      debug_printf("LOADING Clump [%s] with index of [%d]\n", name, i);
      handle = clump_load_by_name(name, &frame.clump_length, &frame.clump_version);
      world->clumps[i] = handle;
      if (handle == 0) {
        debug_printf("COURSES ERROR: Cannot load dff model %s\n", files->clump_files + i * COURSE_FILE_NAME_SIZE);
        return 0;
      }
      world->clump_loaded[i] = 1;
      CLUMP_STATE(world)[i] = 0;
      debug_printf("LOADED Clump [%s] OK. Length %d. Version %x\n", name, frame.clump_length, frame.clump_version);
    }
  }
  if (files->occluder[0] != '\0') {
    world->occluder_clump = clump_load_by_name(files->occluder, 0, 0);
  }
  if (counts->fog_range[0] != counts->fog_range[1]) {
    world->fog_a[1] = counts->fog_range[0];
    world->fog_a[2] = counts->fog_range[1];
    *(unsigned int *)&world->fog_a[0] = *(unsigned int *)counts->fog_color;
  }
  if (counts->fog_modify_a[0] != counts->fog_modify_a[1]) {
    world->fog_b[1] = counts->fog_modify_a[0];
    world->fog_b[2] = counts->fog_modify_a[1];
    *(unsigned int *)&world->fog_b[0] = *(unsigned int *)counts->fog_color_modified;
  }
  if (files->lights[0] == '\0') {
    frame.vector.x = 1.0f;
    frame.vector.y = 0.0f;
    frame.vector.z = 0.0f;
    light = RpLightCreate(LIGHT_TYPE_AMBIENT);
    world->ambient_light = (unsigned int)light;
    RpLightSetColor(light, g_course_create_6294b8);
    OBJECT_FLAGS(world->ambient_light) = LIGHT_FLAGS_ON;
    RpWorldAddLight(world->world, (void *)world->ambient_light);
    light = RpLightCreate(LIGHT_TYPE_DIRECTIONAL);
    world->directional_light = light;
    RpLightSetColor(light, g_course_create_6294c8);
    OBJECT_FLAGS(world->directional_light) = LIGHT_FLAGS_ON;
    _rwObjectHasFrameSetFrame(world->directional_light, RwFrameCreate());
    RwFrameRotate(OBJECT_FRAME(world->directional_light), &frame.vector, DIRECTIONAL_LIGHT_TURN, rwCOMBINEREPLACE);
    RpWorldAddLight(world->world, world->directional_light);
  } else {
    handle = clump_load_by_name(files->lights, 0, 0);
    world->lights_clump = handle;
    count = RpClumpGetNumLights(handle);
    FUN_004c3fe0(world->lights_clump, frame.u.lights);
    for (i = 0; i < count; i++) {
      light = (char *)frame.u.lights[i];
      if (OBJECT_SUBTYPE(light) == LIGHT_TYPE_DIRECTIONAL) {
        world->directional_light = light;
      } else if (OBJECT_SUBTYPE(light) == LIGHT_TYPE_AMBIENT) {
        world->ambient_light = (unsigned int)light;
      }
      OBJECT_FLAGS(light) = LIGHT_FLAGS_ON;
      RpWorldAddLight(world->world, light);
    }
    if (counts->ambient_rgba[0] > 0.0f || counts->ambient_rgba[1] > 0.0f || counts->ambient_rgba[2] > 0.0f) {
      if (world->ambient_light == 0) {
        light = RpLightCreate(LIGHT_TYPE_AMBIENT);
        world->ambient_light = (unsigned int)light;
        OBJECT_FLAGS(light) = LIGHT_FLAGS_ON;
        RpWorldAddLight(world->world, (void *)world->ambient_light);
      }
      RpLightSetColor((void *)world->ambient_light, counts->ambient_rgba);
    }
  }
  if (world->ambient_light != 0) {
    RpLightSetColor((void *)world->ambient_light, g_course_create_6294d8 + counts->course_id * 16);
  }
  if (world->directional_light != 0) {
    RpLightSetColor(world->directional_light, g_course_create_629758 + counts->course_id * 16);
  }
  name = files->mts_files;
  for (i = 0; i < COURSE_MTS_LISTS; i++, name += COURSE_FILE_NAME_SIZE) {
    if (*name != '\0') {
      debug_printf("LOADING MTS [%s] with index of [%d]\n", name, i);
      count = matrix_list_load(name, world->mts_matrices + i * COURSE_MTS_LIST_BYTES);
      world->mts_matrix_counts[i] = count;
      if (count > COURSE_MTS_MAX_MATRICES) {
        debug_printf("ERROR LOADING MTS [%s] with index of [%d], Too Many matrices in MTS %d.\n", name, i, count);
        *(volatile int *)0 = 0;   /* the original deliberately writes to address 0 here (crashes: the course is unusable) */
      }
    }
  }
  if (counts->course_id == 0) {
    RtDictSchemaSetCurrentDict(_rpUVAnimDictSchema, world->uva_dicts[1]);
  }
  name = files->sky_files;
  for (i = 0; i < COURSE_SKY_CLUMPS; i++, name += COURSE_FILE_NAME_SIZE) {
    if (*name != '\0') {
      debug_printf("Loading SkyDome [%s] into sky[%d]\n", name, i);
      handle = clump_load_by_name(name, 0, 0);
      world->sky_clumps[i] = handle;
      world->sky_atomics[i] = clump_has_uv_anim(handle);
    }
  }
  handle = FUN_00549090(world->world);
  world->pvs_data = handle;
  if (handle != 0) {
    debug_printf("PVS is available\n");
    world->pvs_enabled = 1;
    debug_printf("Using PVS\n");
  } else {
    world->pvs_enabled = 0;
    debug_printf("PVS not available\n");
  }
  name = files->spline_files;
  slot = world->splines;
  for (i = 0; i < COURSE_SPLINES; i++, slot++, name += COURSE_FILE_NAME_SIZE) {
    if (*name != '\0') {
      handle = texdict_load_named_c(name, 0, 0);
      if (handle == 0) {
        debug_printf("COURSES ERROR: Cannot load spline %s\n", files->spline_files + i * COURSE_FILE_NAME_SIZE);
        counts->spline_count = 0;
        return 0;
      }
      *slot = handle;
      debug_printf("Spline [%s] loaded into course spline %d, numsplines is %d\n", name, i, counts->spline_count);
    }
  }
  debug_printf("Loading Camera path [%s]\n", files->camera_anim);
  handle = texdict_load_named_d(files->camera_anim, 0, 0);
  world->camera_anim = handle;
  if (handle == 0) {
    debug_printf("cant load camera path\n");
  } else {
    debug_printf("Camera path loaded\n");
  }
  name = files->general_anim_files;
  slot = world->general_anims;
  for (i = 0; i < COURSE_GENERAL_ANIMS; i++, slot++, name += COURSE_FILE_NAME_SIZE) {
    if (*name != '\0') {
      debug_printf("\nLoading general anim [%s]\n", name);
      handle = texdict_load_named_d(name, 0, 0);
      *slot = handle;
      if (handle == 0) {
        debug_printf("cant load general anim %d\n", i);
      } else {
        debug_printf("general anim %d loaded\n", i);
      }
    }
  }
  for (frame.loop_counter = 0; frame.loop_counter < counts->clump_mts_pair_count; frame.loop_counter++) {
    ClumpMtsPair *pair = &((ClumpMtsPair *)counts->clump_mts_pairs)[frame.loop_counter];
    void *clump = (void *)world->clumps[pair->clump];
    void *atomic = FUN_004c3f60(clump);
    unsigned char *matrix;

    frame.u.w.mts_count = world->mts_matrix_counts[pair->mts];
    matrix = world->mts_matrices + pair->mts * COURSE_MTS_LIST_BYTES;
    frame.vector.x = 0.0f;
    frame.vector.y = 0.0f;
    frame.vector.z = 0.0f;
    FUN_004c3fd0(clump);
    RwFrameTranslate(OBJECT_FRAME(clump), &frame.vector, rwCOMBINEREPLACE);
    for (j = 0; j < frame.u.w.mts_count; j++, matrix += sizeof(RwMatrix)) {
      void *copy = atomic;
      void *copy_frame = OBJECT_FRAME(atomic);
      if (j != 0) {
        copy = RpAtomicClone(atomic);
        copy_frame = RwFrameCreate();
        RpAtomicSetFrame(copy, copy_frame);
        RpClumpAddAtomic(clump, copy);
        RwFrameAddChild(OBJECT_FRAME(clump), copy_frame);
      }
      RwFrameTransform(copy_frame, matrix, rwCOMBINEREPLACE);
      if (pair->bbox_index != CLUMP_MTS_NO_BBOX) {
        atomic_world_bbox_update(copy, &frame.u.w.bbox);
        smplfzx_object_create_from_atomic(copy, &frame.vector.x, &frame.u.w.bbox, pair->bbox_index);
      }
    }
  }
  slot = world->clumps;
  for (i = 0; i < COURSE_CLUMPS; i++, slot++) {
    if (*slot != 0) {
      if (counts->clump_exclude_from_world[i] == 0) {
        RpWorldAddClump(world->world, (void *)*slot);
      }
      CLUMP_UV_ANIM(world)[i] = clump_has_uv_anim(*slot);
    }
  }
  if (files->texture_a_name[0] != '\0') {
    world->texture_a = (unsigned int)RwTexDictionaryFindNamedTexture(world->texture_dictionary, files->texture_a_name);
  }
  if (files->texture_b_name[0] != '\0') {
    world->texture_b = (unsigned int)RwTexDictionaryFindNamedTexture(world->texture_dictionary, files->texture_b_name);
  }
  world_sectors_process_r(world->world);
  switch (counts->course_id) {
  case 0:
  case 6:
  case 8:
  case 0xc:
  case 0xe:
  case 0xf:
  case 0x10:
  case 0x19:
  case 0x1a:
  case 0x24:
    world->flag_outdoor = 0;
    return 1;
  default:
    world->flag_outdoor = 1;
    return 1;
  }
}

/* world_sectors_process_r: C -> original world_sectors_process (registers eax) */
__declspec(naked) void world_sectors_process_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call world_sectors_process
        ret
    }
}
