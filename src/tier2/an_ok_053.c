#include "mfl_tier2.h"
/*00475760*/ extern int aux_camera_get_position();
/*0047a470*/ extern int car_set_wheel_contact();
/*00481620*/ extern undefined __cdecl atomic_render_with_culling(struct RpAtomic *p0);
/*004b38f7*/ extern int finite();
/*004d18d0*/ extern struct RwBBox * RwBBoxCalculate(struct RwBBox *p0, struct RwV3d *p1, RwInt32 p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004dbfc0*/ extern struct RwStream * RwStreamWrite(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *p0, RpMaterialCallBack p1, void *p2);
/*00544e80*/ extern int RpRandom();
/*00555ff0*/ extern struct RwV3d * RpSplineFindPosition(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, struct RwV3d *p4);
/*005625f0*/ extern RwBool __cdecl RpMaterialUVAnimExists(struct RpMaterial *p0);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dd61c*/ extern float k_f_0_001;
/*005de498*/ extern float k_f_4_656613en10;
/*005df83c*/ extern float k_f_1_0en6;
/*006c3f30*/ extern undefined4 g_car_6c3f30;
/*006c3f34*/ extern undefined4 g_car_6c3f34;
/*006c3f38*/ extern undefined4 g_gfx_6c3f38;
/*006c3f3c*/ extern undefined4 g_gfx_6c3f3c;
/*006c3f40*/ extern undefined4 g_car_6c3f40;
/*006c3f44*/ extern undefined4 g_app_boot_6c3f44;
/*006c3f48*/ extern undefined4 g_gfx_6c3f48;
/*006c3f4c*/ extern undefined4 g_gfx_6c3f4c;
/*006c3f50*/ extern undefined4 g_app_boot_6c3f50;
/*006c3f54*/ extern undefined4 g_app_boot_6c3f54;
/*006c3f58*/ extern undefined4 g_app_boot_6c3f58;
/*006c3f5c*/ extern undefined4 g_app_boot_6c3f5c;
/*006c3f60*/ extern undefined4 g_app_boot_6c3f60;
/*006c3f64*/ extern undefined4 g_app_boot_6c3f64;
/*006c3f68*/ extern undefined4 g_app_boot_6c3f68;
/*006c3f6c*/ extern undefined4 g_app_boot_6c3f6c;
/*006c3f70*/ extern undefined4 g_app_boot_6c3f70;
/*006c3f74*/ extern undefined4 g_app_boot_6c3f74;
/*006c3f78*/ extern undefined4 g_app_boot_6c3f78;
/*006c3f7c*/ extern undefined4 g_app_boot_6c3f7c;
/*006c3f80*/ extern undefined4 g_app_boot_6c3f80;
/*006c3f84*/ extern undefined4 g_app_boot_6c3f84;
/*006c3f88*/ extern undefined4 g_app_boot_6c3f88;
/*006c3f8c*/ extern undefined4 g_app_boot_6c3f8c;
/*006c3f90*/ extern undefined4 g_app_boot_6c3f90;
/*006c3f94*/ extern undefined4 g_app_boot_6c3f94;
/*006c3f98*/ extern undefined4 g_app_boot_6c3f98;
/*006c3f9c*/ extern undefined4 g_app_boot_6c3f9c;
/*006c3fa0*/ extern undefined4 g_app_boot_6c3fa0;
/*006c3fa4*/ extern undefined4 g_app_boot_6c3fa4;
/*006c3fa8*/ extern undefined4 g_app_boot_6c3fa8;
/*006c3fac*/ extern undefined4 g_app_boot_6c3fac;
/*006c3fb0*/ extern undefined4 g_app_boot_6c3fb0;
/*006c3fb4*/ extern undefined4 g_app_boot_6c3fb4;
/*006c3fb8*/ extern undefined4 g_app_boot_6c3fb8;
/*006c3fbc*/ extern undefined4 g_app_boot_6c3fbc;
/*006c3fc0*/ extern undefined4 g_app_boot_6c3fc0;
/*006c3fc4*/ extern undefined4 g_app_boot_6c3fc4;
/*006c3fc8*/ extern undefined4 g_app_boot_6c3fc8;
/*006c3fcc*/ extern undefined4 g_app_boot_6c3fcc;
/*0089ddfc*/ extern undefined4 g_gfx_89ddfc;
/*0089de04*/ extern undefined4 g_app_89de04;
/*0089de08*/ extern undefined4 g_gfx_89de08;
/*0089de0c*/ extern undefined4 g_gfx_89de0c;
/* atomic_bbox_calculate */

/* "RwBBoxCalculate of the atomic's geometry vertices" */

extern RwBBox * __cdecl atomic_bbox_calculate(RwBBox *boundBox,char *param_2);


/* atomic_set_render_callback */

/* WARNING: Removing unreachable block (ram,0x0048177d) */
/* "stores the old callback at g_gfx_89de08 and installs atomic_render_with_culling" */

extern void __cdecl atomic_set_render_callback(char *param_1,char *param_2);


/* atomic_plugin_get_bbox */

/* "address of the plugin bbox (+8)" */

extern int atomic_plugin_get_bbox(char *param_1);


/* atomic_plugin_get_flags */

/* "plugin data +0x20 of the atomic" */

extern undefined4 __cdecl atomic_plugin_get_flags(char *param_1);


/* atomic_plugin_set_float_24 */

/*  */

extern float10 atomic_plugin_set_float_24(char *param_1,float param_2);


/* atomic_plugin_construct */

/* "plugin constructor: bbox min (-1, 0, 10000)" */

extern int atomic_plugin_construct(char *param_1);


/* atomic_plugin_copy */

/* "plugin copy constructor" */

void __cdecl
atomic_plugin_copy(char *param_1,char *param_2,char *param_3,char *param_4,char *param_5,
                  char *param_6,char *param_7,char *param_8,char *param_9)
{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar1 = (undefined4 *)(g_gfx_89de0c + param_1);
  puVar2 = (undefined4 *)(g_gfx_89de0c + param_2);
  *puVar1 = *puVar2;
  puVar1[9] = puVar2[9];
  puVar1[5] = puVar2[5];
  puVar1[6] = puVar2[6];
  puVar1[7] = puVar2[7];
  puVar1[2] = puVar2[2];
  puVar1[3] = puVar2[3];
  puVar1[4] = puVar2[4];
  puVar1[10] = puVar2[10];
  puVar1[8] = puVar2[8];
  puVar1[1] = puVar2[1];
  return;
}


/* atomic_plugin_stream_read */

/* "reads 0x2C bytes of the atomic plugin data" */

extern RwStream * atomic_plugin_stream_read(RwStream *stream,char *param_2,char *param_3);

#define ATOMIC_PLUGIN_BBOX_OFFSET (8)
#define ATOMIC_PLUGIN_BBOX_INF_OFFSET (0x14)
#define ATOMIC_PLUGIN_STREAM_SIZE (0x2c)


/* atomic_plugin_stream_write */

/* Plugin stream writer for the atomic: copies the plugin bbox (sup vec3 at +8, inf vec3 at +0x14 (inferred))
   at plugin_offset into a 0x2c byte buffer and writes it with RwStreamWrite.
   Params: stream = output stream, param_2 = unused, plugin_offset = atomic plugin block offset from g_gfx_89de0c.
   Returns stream on success, NULL when RwStreamWrite fails. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
uint atomic_plugin_stream_write(RwStream *stream,undefined4 param_2,char *plugin_offset)
{
  undefined4 *src;
  RwStream *write_result;
  undefined4 write_buffer [11];       /* one 0x2c byte buffer written by RwStreamWrite (separate locals: the compiler drops the stores it cannot see being read) */

  /* sup vec3 (bbox +0) into buffer dwords 2-4 */
  src = (undefined4 *)(g_gfx_89de0c + ATOMIC_PLUGIN_BBOX_OFFSET + plugin_offset);
  write_buffer[2] = *src;
  write_buffer[3] = src[1];
  write_buffer[4] = src[2];
  /* inf vec3 (bbox +0xc) into buffer dwords 5-7 */
  src = (undefined4 *)(g_gfx_89de0c + ATOMIC_PLUGIN_BBOX_INF_OFFSET + plugin_offset);
  write_buffer[5] = *src;
  write_buffer[6] = src[1];
  write_buffer[7] = src[2];
  /* write the buffer; NULL result means the write failed */
  write_result = RwStreamWrite(stream,write_buffer,ATOMIC_PLUGIN_STREAM_SIZE);
  return -(uint)(write_result != (RwStream *)0x0) & (uint)stream;
}


#define MATERIAL_PLUGIN_STREAM_SIZE (0x30)
#define MATERIAL_PLUGIN_DWORDS (0xc)


/* material_plugin_stream_read */

/* Plugin stream reader for the material: reads MATERIAL_PLUGIN_STREAM_SIZE (0x30) bytes from stream into a local
   buffer and copies them as MATERIAL_PLUGIN_DWORDS dwords to g_app_89de04 + plugin_offset.
   Params: stream = input stream, param_2 = unused, plugin_offset = material plugin block offset from g_app_89de04 (inferred).
   Returns stream on success, NULL when the read is short. */

/* TIER2: validated 2026-10-18 authority=diffemu */
typedef struct { undefined4 d[12]; } Dw12m;
extern RwStream * material_plugin_stream_read(RwStream *stream,undefined4 param_2,char *plugin_offset);


/* material_plugin_stream_write */

/*  */

extern uint material_plugin_stream_write(RwStream *stream,char *param_2,char *param_3);


/* material_plugin_copy */

/*  */

extern void material_plugin_copy(char *param_1,char *param_2);


/* material_plugin_clear_12 */

/* "clears 3 dwords of the plugin data" */

extern undefined4 material_plugin_clear_12(char *param_1);


/* surface_flag_table_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "fills the 0xff01ffff/0xff013cff... colour code pairs (two per surface id) of the surface table
   at 0x6c3f30" */

extern void surface_flag_table_init(void);


/* player_marker_get_clump */

/*  */

extern undefined4 player_marker_get_clump(char *param_1);


/* table_get_6c3f38 */

/* "int at 0x6C3F38 + i * 16" */

extern undefined4 __cdecl table_get_6c3f38(char *param_1);


/* player_marker_set_enabled */

/* "(player, flag): enables the marker of the player" */

extern void player_marker_set_enabled(char *param_1,char *param_2);


/* player_markers_render */

/* "renders the clumps of the active markers" */

extern void player_markers_render(void);
extern char g_car_6c3fdc[];

#define CAR_WHEEL_TABLE_END (((int)&g_car_6c3fdc))


/* car_wheel_contacts_clear_all */

/* Resets the wheel contact table: 10 entries of 16 bytes starting at 0x6c3f30 (inferred).
   Parameters: none. Returns nothing.
   The first two dwords of each entry are passed by address to car_set_wheel_contact, and the
   last two dwords are zeroed. */

/* "clears 10 entries and the car tables" */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void car_wheel_contacts_clear_all(void)
{
  undefined4 *slot;
  int entry_index;

  entry_index = 0;
  slot = &g_gfx_6c3f3c;
  /* walk the 10 entries (slot = last two dwords of the entry), stop at CAR_WHEEL_TABLE_END */
  do {
    slot[-1] = 0;
    *slot = 0;
    car_set_wheel_contact(slot + -3,entry_index,entry_index,0);
    car_set_wheel_contact(slot + -2,entry_index,entry_index + 10,0);
    slot = slot + 4;
    entry_index = entry_index + 1;
  } while ((LONG_PTR)slot < CAR_WHEEL_TABLE_END);
  return;
}


/* gfx_set_89ddfc */

/*  */

extern void gfx_set_89ddfc(char *param_1);


/* random_range */

/* WARNING: Removing unreachable block (ram,0x00481fe7) */
/* "(min, max) float from RpRandom" */

extern float10 __fastcall random_range(char *param_1,float param_2,float param_3);


/* dot_product_3d_clamped */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "dot product of two 3D vectors clamped to [-1, 1]" */

extern float10 dot_product_3d_clamped(float *param_1,float *param_2);


/* vec3_is_valid_small */

/* "all components finite and the squared length below 1e-6" */

extern undefined4 __cdecl vec3_is_valid_small(float *param_1);


/* matrix_is_valid_small */

/* "vec3_is_valid_small for the four rows (+0x20,+0x10,+0,+0x30)" */

extern bool __cdecl matrix_is_valid_small(char *param_1,char *param_2);


/* spline_get_length */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "samples the RpSpline at 0.001 steps and sums the segment length" */

extern float10 spline_get_length(RpSpline *spline);


/* point_lerp_to_aux_camera */

/* "(a, out, t): out = a + (aux camera position - a) * t" */

extern void point_lerp_to_aux_camera(float *param_1,float *param_2,float param_3);


/* distance_between_points */

/* "RwV3dLength of the difference of two points" */

extern float10 distance_between_points(float *param_1,float *param_2,float *param_3);


/* material_uv_anim_check */

/* "RpMaterialUVAnimExists: sets the flag when present" */

extern RpMaterial * material_uv_anim_check(RpMaterial *material,undefined4 *param_2);


/* geometry_for_all_materials_cb */

/* "RpGeometryForAllMaterials(callback from the pair)" */

extern int __cdecl geometry_for_all_materials_cb(char *param_1,undefined4 *param_2);


/* clump_for_all_materials */

/* "RpClumpForAllAtomics with geometry_for_all_materials_cb" */

extern RpClump * clump_for_all_materials(RpClump *clump,void *pData);


