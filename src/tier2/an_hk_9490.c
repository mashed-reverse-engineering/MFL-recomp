#include "mfl_tier2.h"
extern char UNK_0045d5b0[];
extern char UNK_005df51c[];
/*0045c6b0*/ extern int course_atomics_clone_3();
/*0045ca00*/ extern int anim_set_callback();
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e6f0*/ extern char *rwp_object_get_matrix();
/*00492d10*/ extern int rwp_object_set_active();
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/*005dda24*/ extern float k_f_0_75;
/*0060d9c8*/ extern undefined4 g_course_60d9c8;
/*0060d9cc*/ extern undefined4 g_course_60d9cc;
/*006a27a0*/ extern undefined4 g_course_warzone_6a27a0;
/*006a2888*/ extern undefined g_course_6a2888;
/*006a2998*/ extern undefined4 g_course_warzone_6a2998;
/*006a299c*/ extern undefined4 g_course_warzone_6a299c;
/*006a29a0*/ extern undefined4 g_course_warzone_6a29a0;
/*006a29a4*/ extern undefined4 g_course_warzone_6a29a4;
/*008bf040*/ extern undefined4 g_course_warzone_8bf040;
/*008bf044*/ extern undefined4 g_course_warzone_8bf044;
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/*006a2950*/ extern char g_course_get_6a2950[];
/* course_warzone_cb0 */


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/* Warzone course init: clones the atomics, builds the object-frame sets for the four course lists (offsets 0x1800/0x2400/0x0/0x4800), then plants a tree sprite
   ("OakTreeSprite") for every record of the two tree lists (0xc008 and 0xc00c counts; position from the record, radius = length of its first vector), and
   copies the matrices of world objects 1000 and 2000 into the course globals before deactivating them. */
void course_warzone_cb0(char *course)

{
  undefined4 handle;
  RwTexture *tree_tex;
  undefined4 *src;
  RwV3d *tree_rec;
  int i;
  undefined4 *dst;
  RwReal tree_radius;
  struct {
    RwReal pos_x;
    float pos_y;
    RwReal pos_z;
  } v;
  
  course_atomics_clone_3(course);
  handle = (undefined4)(clump_first_atomic((RpClump *)(*(char **)(course + 0xc11c))));
  g_course_warzone_6a2998 =
       object_frames_create_with_matrices(handle,*(undefined4 *)(course + 0xc008),course + 0x1800);
  g_course_warzone_6a299c =
       object_frames_create_with_matrices(handle,*(undefined4 *)(course + 0xc00c),course + 0x2400);
  handle = (undefined4)(clump_first_atomic((RpClump *)(*(char **)(course + 0xc118))));
  g_course_warzone_6a29a0 =
       object_frames_create_with_matrices(handle,*(undefined4 *)(course + 0xc000),course);
  handle = (undefined4)(clump_first_atomic((RpClump *)(*(char **)(course + 0xc168))));
  g_course_warzone_6a29a4 =
       object_frames_create_with_matrices(handle,*(undefined4 *)(course + 0xc018),course + 0x4800);
  src = (undefined4 *)&g_course_warzone_8bf060;
  do {
    handle = func_0x0045d750(*(undefined4 *)(course + 0xc154),0x10);
    *src = handle;
    src = src + 1;
  } while ((int)src < (int)&g_course_warzone_8bf060 + 0x20);
  g_course_warzone_8bf044 = func_0x0045d6b0(*(undefined4 *)(course + 0xc010),course + 0x3000);
  func_0x0045dc10(course);
  v.pos_x = *(RwReal *)(course + 0x30);
  v.pos_z = *(RwReal *)(course + 0x38);
  v.pos_y = *(float *)(course + 0x34) + k_f_0_75;
  tree_tex = RwTexDictionaryFindNamedTexture(COURSE_TEXDICT(course),"OakTreeSprite");
  func_0x0048d900(tree_tex);
  i = 0;
  if (0 < COURSE_FX_SOURCE_COUNT(course, 1)) {
    tree_rec = (RwV3d *)(course + 0x1820);
    do {
      v.pos_x = tree_rec[1].y;
      v.pos_y = tree_rec[1].z;
      v.pos_z = tree_rec[2].x;
      tree_radius = RwV3dLength(tree_rec);
      func_0x0048db30(&v,*(undefined4 *)(course + 0xc004),course + 0xc00,tree_radius);
      g_course_warzone_8bf040 = g_course_warzone_8bf040 + 1;
      i = i + 1;
      tree_rec = (RwV3d *)&tree_rec[5].y;
    } while (i < COURSE_FX_SOURCE_COUNT(course, 1));
  }
  i = 0;
  if (0 < COURSE_FX_SOURCE_COUNT(course, 2)) {
    tree_rec = (RwV3d *)(course + 0x2420);
    do {
      v.pos_x = tree_rec[1].y;
      v.pos_y = tree_rec[1].z;
      v.pos_z = tree_rec[2].x;
      tree_radius = RwV3dLength(tree_rec);
      func_0x0048db30(&v,*(undefined4 *)(course + 0xc004),course + 0xc00,tree_radius);
      g_course_warzone_8bf040 = g_course_warzone_8bf040 + 1;
      i = i + 1;
      tree_rec = (RwV3d *)&tree_rec[5].y;
    } while (i < COURSE_FX_SOURCE_COUNT(course, 2));
  }
  FID_conflict__wprintf("Created %d trees\n",g_course_warzone_8bf040);
  g_course_60d9c8 = rwp_object_find_index_by_id(1000);
  g_course_60d9cc = rwp_object_find_index_by_id(2000);
  func_0x0045ca50(&g_course_6a2888,*(undefined4 *)(course + 0xc114),
                  *(undefined4 *)(course + 0xc9b0));
  anim_set_callback(&g_course_6a2888,&UNK_0045d5b0);
  src = (undefined4 *)rwp_object_get_matrix(g_course_60d9c8);
  handle = g_course_60d9cc;
  dst = (undefined4 *)((char *)&g_course_get_6a2950 + 0x8);
  for (i = 0x10; i != 0; i = i + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  src = (undefined4 *)rwp_object_get_matrix(handle);
  dst = (undefined4 *)((char *)&g_course_warzone_6a27a0 + 0x8);
  for (i = 0x10; i != 0; i = i + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  rwp_object_set_active(g_course_60d9c8,0);
  rwp_object_set_active(g_course_60d9cc,0);
  g_course_warzone_6a27a0 = func_0x004849e0(&UNK_005df51c,*(undefined4 *)(course + 0xc9d0));
  return;
}





