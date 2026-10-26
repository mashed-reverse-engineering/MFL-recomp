#include "mfl_tier2.h"
/*00406950*/ extern undefined __cdecl world_for_all_materials_cb_406910(struct RpWorld *p0, void *p1);
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00451d10*/ extern int camera_create_a();
/*00451dc0*/ extern int camera_create_b();
/*00475730*/ extern int get_viewport_camera();
/*00495b60*/ extern undefined __cdecl viewport_raster_create(uint p0, RwInt32 p1, RwInt32 p2, uint p3);
/*00495c10*/ extern char *viewport_get_field_702254();
/*0049a9b0*/ extern int fx_slot_find_free();
/*004a0e40*/ extern int rain_splash_add();
/*004a0f20*/ extern int rain_ptank_create();
/*004a1860*/ extern int game_set_option_flags();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f6bf0*/ extern int FUN_004f6bf0(undefined4 p0);
/*005dd7e8*/ extern float k_f_60;
/*006a0770*/ extern undefined4 g_course_arctic_6a0770;
/*006a0774*/ extern undefined4 g_course_arctic_6a0774;
/*006a0778*/ extern int g_course_arctic_6a0778;
/*008c5730*/ extern float g_course_arctic_8c5730;
/*008c5734*/ extern float g_course_arctic_8c5734;
/*008c5738*/ extern float g_course_arctic_8c5738;
/*008c573c*/ extern float g_course_arctic_8c573c;
/*008c5740*/ extern undefined4 g_course_arctic_8c5740;
/*008c5840*/ extern RpClump *g_course_arctic_8c5840;
/*008c5844*/ extern undefined4 g_course_arctic_8c5844[0x1000];
/* course_arctic_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_arctic_cb0(int course)

{
  void *texture_dict;
  RpAtomic *atomic;
  int row_or_clump;
  undefined4 smoke_texture;
  RwCamera *camera;
  int col_i;
  int *clump_slot;
  int grid_i;
  RwReal farClip;
  RwV3d grid_pos;
  struct {
    undefined4 vel_x;
    undefined4 vel_y;
    undefined4 vel_z;
    undefined4 emit_x;
    undefined4 emit_y;
    undefined4 emit_z;
  } frame;
  
  g_course_arctic_6a0770 =
       RwTexDictionaryFindNamedTexture((struct RwTexDictionary *)(*(RwTexDictionary **)(course + 0xc9d0)),(RwChar *)("lightning"));
  viewport_raster_create(0,0x80,0x80,0x600);
  texture_dict = (void *)viewport_get_field_702254(0);
  world_for_all_materials_cb_406910(*(RpWorld **)(course + 0xc9d4),texture_dict);
  camera_create_a(course);
  camera_create_b(course);
  RwFrameSetIdentity(*(RwFrame **)(*(char **)(course + 0xc118) + 4));
  g_course_arctic_8c5840 = *(RpClump **)(course + 0xc118);
  grid_pos.x = 0.0;
  grid_pos.y = -4.1;
  grid_pos.z = 0.0;
  atomic = (RpAtomic *)clump_first_atomic(g_course_arctic_8c5840);
  if ((RpAtomic_interpolator_flags(atomic) & 2) != 0) {
    _rpAtomicResyncInterpolatedSphere(atomic);
  }
  g_course_arctic_8c5730 = RpAtomic_boundingSphere(atomic).center.x;
  g_course_arctic_8c5734 = RpAtomic_boundingSphere(atomic).center.y;
  g_course_arctic_8c5738 = RpAtomic_boundingSphere(atomic).center.z;
  g_course_arctic_8c573c = RpAtomic_boundingSphere(atomic).radius;
  RpWorldRemoveClump(*(RpWorld **)(course + 0xc9d4),g_course_arctic_8c5840);
  RwFrameSetIdentity(RpClump_object_parent(g_course_arctic_8c5840));
  clump_slot = g_course_arctic_8c5844;
  do {
    row_or_clump = FUN_004f6bf0((undefined4)g_course_arctic_8c5840);
    *clump_slot = row_or_clump;
    RwFrameSetIdentity(*(RwFrame **)(row_or_clump + 4));
    clump_slot = clump_slot + 1;
  } while ((int)clump_slot < (int)g_course_arctic_8c5844 + 0x60);
  grid_pos.x = -150.0;
  grid_i = 0;
  row_or_clump = 5;
  do {
    grid_pos.z = -150.0;
    col_i = 5;
    do {
      RwFrameTranslate(*(RwFrame **)((char *)(&g_course_arctic_8c5840)[grid_i] + 4),&grid_pos,
                       rwCOMBINEREPLACE);
      grid_pos.z = grid_pos.z + k_f_60;
      grid_i = grid_i + 1;
      col_i = col_i + -1;
    } while (col_i != 0);
    row_or_clump = row_or_clump + -1;
    grid_pos.x = grid_pos.x + k_f_60;
  } while (row_or_clump != 0);
  frame.emit_x = 0xc23af5c3;
  frame.emit_y = 0x3f3f3b64;
  frame.emit_z = 0xc28c245a;
  frame.vel_x = 0xbc03126f;
  frame.vel_y = 0x3ca3d70a;
  frame.vel_z = 0xbc03126f;
  grid_pos.x = 0.008;
  grid_pos.y = 0.08;
  grid_pos.z = 0.008;
  smoke_texture = find_texture_dict_b("smoke");
  rain_ptank_create(smoke_texture);
  rain_splash_add(&frame.emit_x,0x3c23d70a,&frame.vel_x,&grid_pos);
  row_or_clump = 0;
  if (0 < *(int *)(course + 0xc004)) {
    grid_i = course + 0xc30;
    do {
      smoke_texture = fx_slot_find_free(0xc,grid_i,0,0x14);
      (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = smoke_texture;
      g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
      row_or_clump = row_or_clump + 1;
      grid_i = grid_i + 0x40;
    } while (row_or_clump < *(int *)(course + 0xc004));
  }
  row_or_clump = 0;
  if (0 < *(int *)(course + 0xc008)) {
    grid_i = course + 0x1830;
    do {
      smoke_texture = fx_slot_find_free(7,grid_i,0,0x14);
      (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = smoke_texture;
      g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
      row_or_clump = row_or_clump + 1;
      grid_i = grid_i + 0x40;
    } while (row_or_clump < *(int *)(course + 0xc008));
  }
  row_or_clump = 0;
  if (0 < *(int *)(course + 0xc00c)) {
    grid_i = course + 0x2430;
    do {
      smoke_texture = fx_slot_find_free(2,grid_i,0,0x14);
      (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = smoke_texture;
      g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
      row_or_clump = row_or_clump + 1;
      grid_i = grid_i + 0x40;
    } while (row_or_clump < *(int *)(course + 0xc00c));
  }
  frame.emit_x = 0xc204199a;
  frame.emit_y = 0x401147ae;
  frame.emit_z = 0xc2286666;
  g_course_arctic_6a0774 = g_course_arctic_6a0778;
  smoke_texture = fx_slot_find_free(0xc,&frame.emit_x,0,0x14);
  (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = smoke_texture;
  g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
  if (*(float *)(course + 0xc9f8) == *(float *)(course + 0xc9fc)) {
    farClip = 800.0;
    camera = (RwCamera *)get_viewport_camera(0);
    RwCameraSetFarClipPlane(camera,farClip);
    row_or_clump = get_viewport_camera(0);
    *(undefined4 *)(row_or_clump + 0x88) = 0x43480000;
    get_viewport_camera(0);
    game_set_option_flags(0x28,0x2c,0x30);
  }
  return;
}


