// cluster-entries: rwp_world_initialize
/* Installed cluster rwp_world_initialize (tools/cluster_install.py from output/cluster_pilot_md/rwp_world_initialize.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern int _g_physics_000000;
/*0042a3f0*/ extern int course_get_rpworld();
/*0042a400*/ extern int course_get_collision_bsp();
/*0042a430*/ extern int course_get_ambient_light();
/*0042a440*/ extern int course_get_directional_light();
/*00458270*/ extern int course_6a2634_clear();
/*00488190*/ extern int __cdecl matrix_list_load(void *p0, RwMatrix *p1);
/*00488230*/ extern int __cdecl clump_load_logged(void *p0, RwChar *p1);
/*004912f0*/ extern int rwp_world_create_once();
/*004913f0*/ extern undefined __cdecl rwp_objects_create_cylinder_group(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15);
/*004916a0*/ extern undefined __cdecl rwp_objects_create_box_group(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15);
/*00491820*/ extern int rwp_objects_create_sphere_group();
/*00491960*/ extern undefined __cdecl rwp_objects_create_capsule_group(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*00492e00*/ extern int rwp_objects_reset_all();
/*004a5cd0*/ extern int object_frames_create_forward();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004c3f20*/ extern int FUN_004c3f20(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*0051be9c*/ extern int __stdcall Initialize(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0[0x1000];
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*006fd0f8*/ extern undefined4 g_rw_6fd0f8[0x1000];
/*006fd418*/ extern undefined4 g_physics_6fd418;
/*006fea90*/ extern undefined4 g_rw_6fea90;
/*00701c94*/ extern int g_rw_701c94;
/*00701c98*/ extern int g_physics_701c98;
/*00701c9c*/ extern undefined4 g_rw_701c9c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/* rwp_world_initialize */

/* "prints 'Initialize CALLED' and creates the physics world, collision world and car objects" */

/* Builds the RWP physics world from the object table g_rwp_objects (stride 0x110, rec = field_0x10c of the current record): clears two 200 entry id tables to -1, then for each of the 0x21 (inferred from the loop bound) records with a shape type (rec[-0x43]) loads its matrix list (.mts, rec-0x22) and clump (.dff, rec-0x32), creates the object group by shape (1 box, 2 cylinder, 3 sphere, 4 capsule, from the debug prints), logs its settings and finally resets all RWP objects. */
void rwp_world_initialize(void)

{
  size_t name_len;
  int atomic;
  undefined4 *tbl;
  undefined4 ambient_light;
  int count;
  int j;
  int *rec;
  int obj_index;
  int mat_slot;
  undefined4 *src;
  undefined4 *dst;
  char *fmt_dff;
  int *dff_name;
  char *fmt_mts;
  char *fmt_fatness;
  int idx_fatness;
  int *mts_name;
  undefined4 uVar18;
  double fatness;
  struct {
    int mat_offset;            /* running index into the matrix table g_rw_6fea90 (the orig keeps it at [esp+0x10]) */
    undefined4 pad_110;
    int world;                 /* course_get_rpworld() (orig [esp+0x18]) */
    undefined1 pad_108[4];
    int mat_ptrs [64];
  } frame;
  
  FID_conflict__wprintf("!!!!!!!!!!!!!!!!!!!! Initialize CALLED !!!!!!!!!!!!!!!!!!!!!\n");
  frame.world = course_get_rpworld();
  rwp_world_create_once(frame.world,course_get_collision_bsp());
  /* first id table: 200 entries set to -1 */
  tbl = g_rw_6fd0f8;
  for (count = 200; count != 0; count = count + -1) {
    *tbl = 0xffffffff;
    tbl = tbl + 1;
  }
  obj_index = 0;
  mat_slot = 0;
  frame.mat_offset = 0;
  g_rw_6fa7d0 = 0;
  tbl = g_rw_6fa4b0;
  for (count = 200; count != 0; count = count + -1) {
    *tbl = 0xffffffff;
    tbl = tbl + 1;
  }
  rec = (int *)&g_rwp_objects[0].field_0x10c;
  do {
    *rec = 0;
    course_6a2634_clear(1);
    /* record in use */
    if (rec[-0x43] != 0) {
      name_len = rwstrlen((RwChar *)(rec + -0x22));
      /* no MTS name: take the matrices from the clump atomic's geometry (FUN_004c3f20) and copy 16 words each into g_rw_6fea90 */
      if (name_len == 0) {
        mts_name = rec + -0x32;
        uVar18 = 0;
        ambient_light = clump_load_logged(mts_name,(RwChar *)0x0);
        (&g_physics_6fd418)[g_physics_701c98] = ambient_light;
        atomic = (int)(clump_first_atomic((RpClump *)ambient_light));
        count = FUN_004c3f20(*(undefined4 *)(atomic + 4),(undefined4)frame.mat_ptrs,ambient_light,(undefined4)mts_name,uVar18);
        j = 0;
        *rec = count;
        if (0 < count) {
          tbl = &g_rw_6fea90 + mat_slot * 0x10;
          do {
            src = (undefined4 *)(frame.mat_ptrs[j] + 0x10);
            dst = tbl;
            for (mat_slot = 0x10; mat_slot != 0; mat_slot = mat_slot + -1) {
              *dst = *src;
              src = src + 1;
              dst = dst + 1;
            }
            j = j + 1;
            tbl = tbl + 0x10;
          } while (j < count);
        }
      }
      /* MTS name given: load the matrix list from file (max 0x30 matrices, else log "Too many matrices") */
      else {
        count = matrix_list_load(rec + -0x22,(RwMatrix *)((char *)&g_rw_6fea90 + mat_slot * 0x40));
        if (0x30 < count) {
          FID_conflict__wprintf("Too many matrices in mts file %d\n");
          _g_physics_000000 = 0;
        }
        *rec = count;
        ambient_light = clump_load_logged(rec + -0x32,(RwChar *)0x0);
        (&g_physics_6fd418)[g_physics_701c98] = ambient_light;
        atomic = (int)(clump_first_atomic((RpClump *)ambient_light));
      }
      course_get_directional_light();
      ambient_light = course_get_ambient_light();
      ambient_light = object_frames_create_forward(atomic,count,ambient_light);
      (&g_rw_6fa7d8)[g_rw_701c94] = ambient_light;
      mat_slot = rec[-0x43];
      /* cylinder */
      if (mat_slot == 2) {
        FID_conflict__wprintf("making cylinder\n");
        FID_conflict__wprintf("%d - type=%d\n");
        FID_conflict__wprintf("%d - cylinderdimensions=%f,%f\n");
        FID_conflict__wprintf("%d - mass=%f\n");
        FID_conflict__wprintf
                  ("%d - COMoffset=%f,%f,%f\n",obj_index,(double)*(float *)&rec[-9],
                   (double)*(float *)&rec[-8],(double)*(float *)&rec[-7]);
        FID_conflict__wprintf("%d - bouncefriction=%f\n");
        FID_conflict__wprintf("%d - slidefriction=%f\n");
        fatness = (double)*(float *)&rec[-4];
        fmt_fatness = "%d - fatness=%f\n";
        idx_fatness = obj_index;
        FID_conflict__wprintf("%d - fatness=%f\n");
        mts_name = rec + -0x22;
        fmt_mts = "%d - MTSname=%s\n";
        j = obj_index;
        FID_conflict__wprintf("%d - MTSname=%s\n");
        dff_name = rec + -0x32;
        fmt_dff = "%d - dffname=%s\n";
        mat_slot = obj_index;
        FID_conflict__wprintf("%d - dffname=%s\n");
        rwp_objects_create_cylinder_group
                  (count,(undefined4)frame.mat_offset,g_rw_701c9c,(undefined4)frame.world,obj_index,atomic,(undefined4)fmt_dff,mat_slot,(undefined4)dff_name,(undefined4)fmt_mts,
                   j,(undefined4)mts_name,(undefined4)fmt_fatness,idx_fatness,SUB84(fatness,0),(int)((ulonglong)fatness >> 0x20));
      }
      else if (mat_slot == 1) {
        FID_conflict__wprintf("making box\n");
        FID_conflict__wprintf("%d - type=%d\n");
        FID_conflict__wprintf
                  ("%d - boxdimensions=%f,%f,%f\n",obj_index,(double)*(float *)&rec[-0x12],
                   (double)*(float *)&rec[-0x11],(double)*(float *)&rec[-0x10]);
        FID_conflict__wprintf("%d - mass=%f\n");
        FID_conflict__wprintf
                  ("%d - COMoffset=%f,%f,%f\n",obj_index,(double)*(float *)&rec[-9],
                   (double)*(float *)&rec[-8],(double)*(float *)&rec[-7]);
        FID_conflict__wprintf("%d - bouncefriction=%f\n");
        FID_conflict__wprintf("%d - slidefriction=%f\n");
        fatness = (double)*(float *)&rec[-4];
        fmt_fatness = "%d - fatness=%f\n";
        idx_fatness = obj_index;
        FID_conflict__wprintf("%d - fatness=%f\n");
        mts_name = rec + -0x22;
        fmt_mts = "%d - MTSname=%s\n";
        j = obj_index;
        FID_conflict__wprintf("%d - MTSname=%s\n");
        dff_name = rec + -0x32;
        fmt_dff = "%d - dffname=%s\n";
        mat_slot = obj_index;
        FID_conflict__wprintf("%d - dffname=%s\n");
        rwp_objects_create_box_group
                  (count,(undefined4)frame.mat_offset,g_rw_701c9c,(undefined4)frame.world,obj_index,atomic,(undefined4)fmt_dff,mat_slot,(undefined4)dff_name,(undefined4)fmt_mts,
                   j,(undefined4)mts_name,(undefined4)fmt_fatness,idx_fatness,SUB84(fatness,0),(int)((ulonglong)fatness >> 0x20));
      }
      else if (mat_slot == 3) {
        rwp_objects_create_sphere_group(count,frame.mat_offset,g_rw_701c9c,frame.world,obj_index);
      }
      else if (mat_slot == 4) {
        rwp_objects_create_capsule_group(count,frame.mat_offset,g_rw_701c9c,frame.world,obj_index,atomic);
      }
      FID_conflict__wprintf("%d - number=%d\n",obj_index);
      FID_conflict__wprintf("%d - matnum=%d\n");
      mat_slot = frame.mat_offset + count;
      g_rw_701c94 = g_rw_701c94 + 1;
      g_physics_701c98 = g_physics_701c98 + 1;
      frame.mat_offset = mat_slot;
    }
    rec = rec + 0x44;
    obj_index = obj_index + 1;
  } while ((int)rec < (int)&g_rwp_objects + 0x230c);
  rwp_objects_reset_all();
  FID_conflict__wprintf("numscenePobjects=%d\n",g_rw_6fa7d0);
  return;
}

