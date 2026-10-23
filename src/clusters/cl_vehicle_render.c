// cluster-entries: vehicle_render
/* Installed cluster vehicle_render (tools/cluster_install.py from output/cluster_pilot_uc3/vehicle_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef struct RpWorld RpWorld;
typedef struct RwFrame RwFrame;
typedef struct RpClump RpClump;
typedef struct RpLight RpLight;
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(RwCamera *camera, RwSphere *sphere);
/*004d1b80*/ extern RwCamera *RwCameraEndUpdate(RwCamera *camera);
/*004d1b90*/ extern RwCamera *RwCameraBeginUpdate(RwCamera *camera);
/*004d15a0*/ extern RwFrame *RwFrameTransform(RwFrame *frame, RwMatrix *matrix, enum RwOpCombineType combine);
/*004f4470*/ extern RpWorld *RpWorldRemoveCamera(RpWorld *world, RwCamera *camera);
/*004f4440*/ extern RpWorld *RpWorldAddCamera(RpWorld *world, RwCamera *camera);
/*004f4a20*/ extern RpLight *RpLightSetColor(RpLight *light, RwRGBAReal *colour);
/*004f67c0*/ extern RpClump *RpClumpRender(RpClump *clump);
/*0042a3f0*/ extern RpWorld *course_get_rpworld(void);
/*0042a450*/ extern int course_is_outdoor(void);
/*004263f0*/ extern void vehicle_tyre_marks_render(int vehicle_index, RwCamera *camera);
/*00425b20*/ extern void vehicle_skidmarks_render(int vehicle_index);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00659f00*/ extern VehicleRecord g_vehicle_table[4];
extern int g_game_65a9b8;
extern float g_game_607624;
extern int g_game_65a9c0;
extern char *g_game_65a9c4;
extern int g_vehicle_render_65a9bc;
extern char *g_particles_659d70;      /* the scene RpLight: +4 its frame, +0x18 its colour */
extern int g_game_64d88c;
extern int g_particles_60761c;
extern int g_particles_607620;

#define LIGHT_FRAME(light) (*(char **)((light) + 4))
#define LIGHT_COLOUR(light) ((RwRGBAReal *)((light) + 0x18))

/* Renders vehicle `vehicle_index` into `camera` (when it is active and its bounding sphere is in the frustum): the camera is taken out of the world for the
   updates of the environment map frame (from the scene light's frame or g_game_65a9c4), the shadow camera, the exhaust flames and the headlight, then put
   back; the vehicle clump is rendered with the scene light's colour scaled by the vehicle's size factor (restored afterwards), then the front or rear
   part callback (+0x48 of the part frame 0x4c / 0x4d) and, with the z-buffer write off, the tyre marks (outdoor courses) or skid marks. */
static void vehicle_set_scale_for_course_r(VehicleRecord * p0);
extern void vehicle_set_scale_for_course(void);
static void vehicle_shadow_camera_render_setup_r(VehicleRecord * p0);
extern void vehicle_shadow_camera_render_setup(void);
static void vehicle_exhaust_flame_particle_add_r(VehicleRecord * p0, RwCamera * p1);
extern void vehicle_exhaust_flame_particle_add(void);
static void vehicle_headlight_particle_add_r(VehicleRecord * p0);
extern void vehicle_headlight_particle_add(void);
void vehicle_render(int vehicle_index, RwCamera *camera)
{
  VehicleRecord *vehicle;
  RwRGBAReal saved;
  RwRGBAReal scaled;
  char *source;
  int part;

  vehicle = &g_vehicle_table[vehicle_index];
  if ((vehicle->flags & VEHICLE_FLAG_ACTIVE) == 0) {
    return;
  }
  if (RwCameraFrustumTestSphere(camera, (RwSphere *)vehicle->world_sphere) == rwSPHEREOUTSIDE) {
    return;
  }
  vehicle->render_camera = (char *)camera;
  RwCameraEndUpdate(camera);
  RpWorldRemoveCamera(course_get_rpworld(), camera);
  if (g_game_65a9b8 != 0) {
    vehicle->size_factor = g_game_607624;
  }
  if (g_game_65a9c0 != 0) {
    if (g_game_65a9c4 != 0) {
      source = g_game_65a9c4;
      vehicle->flags = vehicle->flags | 4;
      RwFrameTransform((RwFrame *)vehicle->envmap_frame, (RwMatrix *)(source + 0x10), rwCOMBINEREPLACE);
      vehicle_set_scale_for_course_r(vehicle);
    }
  } else if (g_vehicle_render_65a9bc != 0 && g_particles_659d70 != 0) {
    source = LIGHT_FRAME(g_particles_659d70);
    vehicle->flags = vehicle->flags | 4;
    RwFrameTransform((RwFrame *)vehicle->envmap_frame, (RwMatrix *)(source + 0x10), rwCOMBINEREPLACE);
    vehicle_set_scale_for_course_r(vehicle);
  }
  if (g_game_64d88c == 0 && course_is_outdoor() != 0) {
    vehicle_shadow_camera_render_setup_r(vehicle);
  }
  if (g_particles_60761c != 0) {
    vehicle_exhaust_flame_particle_add_r(vehicle, camera);
  }
  if (g_particles_607620 != 0) {
    vehicle_headlight_particle_add_r(vehicle);
  }
  RpWorldAddCamera(course_get_rpworld(), camera);
  RwCameraBeginUpdate(camera);
  if (g_particles_659d70 != 0) {
    saved = *LIGHT_COLOUR(g_particles_659d70);
    scaled.red = saved.red * vehicle->size_factor;
    scaled.green = saved.green * vehicle->size_factor;
    scaled.blue = saved.blue * vehicle->size_factor;
    scaled.alpha = saved.alpha * vehicle->size_factor;
    RpLightSetColor((RpLight *)g_particles_659d70, &scaled);
  }
  RpClumpRender((RpClump *)vehicle->frame);
  if ((vehicle->flags & VEHICLE_FLAG_400) == 0) {
    if ((vehicle->flags & (VEHICLE_FLAG_FRONT | VEHICLE_FLAG_BACK)) == 0 && (vehicle->flags & (VEHICLE_FLAG_1000 | VEHICLE_FLAG_2000)) == 0) {
      part = vehicle->part_frames[0x4c];
    } else {
      part = vehicle->part_frames[0x4d];
    }
    if (part != 0) {
      (*(void (__cdecl **)(int))(part + 0x48))(part);
    }
  }
  if (g_particles_659d70 != 0) {
    RpLightSetColor((RpLight *)g_particles_659d70, &saved);
  }
  if (g_game_64d88c == 0) {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
    if (course_is_outdoor() == 0) {
      if ((vehicle->flags & VEHICLE_FLAG_FRONT) == 0 && g_particles_60761c != 0 && (vehicle->flags & VEHICLE_FLAG_40) != 0) {
        vehicle_skidmarks_render(vehicle_index);
      }
    } else if (g_particles_607620 != 0) {
      vehicle_tyre_marks_render(vehicle_index, camera);
    }
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
  }
}

/* vehicle_set_scale_for_course_r: C -> original vehicle_set_scale_for_course (registers esi) */
__declspec(naked) void vehicle_set_scale_for_course_r(VehicleRecord * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call vehicle_set_scale_for_course
        pop esi
        ret
    }
}

/* vehicle_shadow_camera_render_setup_r: C -> original vehicle_shadow_camera_render_setup (registers esi) */
__declspec(naked) void vehicle_shadow_camera_render_setup_r(VehicleRecord * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call vehicle_shadow_camera_render_setup
        pop esi
        ret
    }
}

/* vehicle_exhaust_flame_particle_add_r: C -> original vehicle_exhaust_flame_particle_add (registers esi,s0) */
__declspec(naked) void vehicle_exhaust_flame_particle_add_r(VehicleRecord * p0, RwCamera * p1)
{
    __asm {
        push esi
        push dword ptr [esp + 12]
        mov esi, dword ptr [esp + 12]
        call vehicle_exhaust_flame_particle_add
        add esp, 4
        pop esi
        ret
    }
}

/* vehicle_headlight_particle_add_r: C -> original vehicle_headlight_particle_add (registers edi) */
__declspec(naked) void vehicle_headlight_particle_add_r(VehicleRecord * p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call vehicle_headlight_particle_add
        pop edi
        ret
    }
}
