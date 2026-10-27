// cluster-entries: course_destroy_world
/* Installed cluster course_destroy_world (tools/cluster_install.py from output/cluster_pilot_kb/course_destroy_world.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00496c10*/ extern int particles_lookup_by_key();
/*00496f50*/ extern undefined __cdecl smplfzx_retire_object(undefined4 p0, undefined4 p1);
/*004a8420*/ extern int debug_printf();
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c4270*/ extern void FUN_004c4270(undefined4 p0);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*004f4980*/ extern struct RpWorld * RpWorldRemoveLight(struct RpWorld *p0, struct RpLight *p1);
/*004f5820*/ extern RwBool RpWorldDestroy(struct RpWorld *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00556950*/ extern RwBool __cdecl RpSplineDestroy(struct RpSpline *p0);
/*00569300*/ extern RwBool __cdecl RtDictDestroy(struct RtDict *p0);
/*00573f50*/ extern RwBool RtAnimAnimationDestroy(struct RtAnimAnimation *p0);
/* course_destroy_world */

/* "'World Destroyed' / 'TextureDictionary Destroyed': destroys world, splines, clumps" */

void course_destroy_world(CourseWorld *world, CourseCounts *counts)

{
  RpClump *clump;
  int atomic_count;
  undefined4 particle_slot;
  RpWorld *world_00;
  byte *pair_ptr;
  int i;
  int j;
  uint *slot_ptr;
  undefined4 atomic;
  undefined4 atomics [48];
  
  i = 0;
  if (0 < counts->clump_mts_pair_count) {
    pair_ptr = counts->clump_mts_pairs;
    do {
      if (*(short *)(pair_ptr + 2) != -1) {
        atomic_count = clump_get_atomics(world->clumps[(char)*pair_ptr],atomics);
        j = 0;
        if (0 < atomic_count) {
          do {
            atomic = atomics[j];
            particle_slot = particles_lookup_by_key(atomic);
            smplfzx_retire_object(particle_slot,atomic);
            j = j + 1;
          } while (j < atomic_count);
        }
      }
      i = i + 1;
      pair_ptr = pair_ptr + 4;
    } while (i < counts->clump_mts_pair_count);
  }
  i = 0;
  if (0 < counts->collision_bsp_count) {
    slot_ptr = world->collision_bsps;
    do {
      if ((RpWorld *)*slot_ptr != (RpWorld *)0x0) {
        RpWorldDestroy((RpWorld *)*slot_ptr);
        *slot_ptr = 0;
      }
      i = i + 1;
      slot_ptr = slot_ptr + 1;
    } while (i < counts->collision_bsp_count);
  }
  i = 0;
  if (0 < counts->hoppy_bsp_count) {
    slot_ptr = world->hoppy_bsps;
    do {
      if ((RpWorld *)*slot_ptr != (RpWorld *)0x0) {
        RpWorldDestroy((RpWorld *)*slot_ptr);
        *slot_ptr = 0;
      }
      i = i + 1;
      slot_ptr = slot_ptr + 1;
    } while (i < counts->hoppy_bsp_count);
  }
  slot_ptr = world->splines;
  i = 0x10;
  do {
    if ((RpSpline *)*slot_ptr != (RpSpline *)0x0) {
      RpSplineDestroy((RpSpline *)*slot_ptr);
      *slot_ptr = 0;
    }
    slot_ptr = slot_ptr + 1;
    i = i + -1;
  } while (i != 0);
  slot_ptr = world->sky_clumps;
  i = 8;
  do {
    if ((RpClump *)*slot_ptr != (RpClump *)0x0) {
      RpClumpDestroy((RpClump *)*slot_ptr);
      *slot_ptr = 0;
    }
    slot_ptr = slot_ptr + 1;
    i = i + -1;
  } while (i != 0);
  if ((RpClump *)world->occluder_clump != (RpClump *)0x0) {
    RpClumpDestroy((RpClump *)world->occluder_clump);
  }
  if ((RpLight *)world->ambient_light != (RpLight *)0x0) {
    RpWorldRemoveLight((RpWorld *)world->world,(RpLight *)world->ambient_light);
  }
  if ((RpLight *)world->directional_light != (RpLight *)0x0) {
    RpWorldRemoveLight((RpWorld *)world->world,(RpLight *)world->directional_light);
  }
  if ((RpClump *)world->lights_clump != (RpClump *)0x0) {
    RpClumpDestroy((RpClump *)world->lights_clump);
  }
  slot_ptr = world->clumps;
  i = 0x80;
  do {
    clump = (RpClump *)*slot_ptr;
    if (clump != (RpClump *)0x0) {
      world_00 = (RpWorld *)clump_get_world(clump);
      if (world_00 != (RpWorld *)0x0) {
        RpWorldRemoveClump(world_00,clump);
      }
      RpClumpDestroy(clump);
    }
    slot_ptr = slot_ptr + 1;
    i = i + -1;
  } while (i != 0);
  if ((RtAnimAnimation *)world->camera_anim != (RtAnimAnimation *)0x0) {
    RtAnimAnimationDestroy((RtAnimAnimation *)world->camera_anim);
  }
  slot_ptr = world->general_anims;
  i = 8;
  do {
    if ((RtAnimAnimation *)*slot_ptr != (RtAnimAnimation *)0x0) {
      RtAnimAnimationDestroy((RtAnimAnimation *)*slot_ptr);
    }
    slot_ptr = slot_ptr + 1;
    i = i + -1;
  } while (i != 0);
  if (world->world != 0) {
    FUN_004c4270(world->world);
    RpWorldDestroy((RpWorld *)world->world);
    world->world = 0;
    debug_printf("World Destroyed\n");
  }
  i = 0;
  if (0 < counts->object_bsp_count) {
    slot_ptr = world->object_bsp_worlds;
    do {
      RpWorldDestroy((RpWorld *)*slot_ptr);
      i = i + 1;
      slot_ptr = slot_ptr + 1;
    } while (i < counts->object_bsp_count);
  }
  RpWorldDestroy((RpWorld *)world->ai_worlds[0]);
  slot_ptr = world->uva_dicts;
  i = 8;
  do {
    if ((RtDict *)*slot_ptr != (RtDict *)0x0) {
      RtDictDestroy((RtDict *)*slot_ptr);
      *slot_ptr = 0;
    }
    slot_ptr = slot_ptr + 1;
    i = i + -1;
  } while (i != 0);
  if ((RwTexDictionary *)world->texture_dictionary != (RwTexDictionary *)0x0) {
    RwTexDictionaryDestroy_004d5ac0((RwTexDictionary *)world->texture_dictionary);
    debug_printf("TextureDictionary Destroyed\n");
  }
  return;
}

