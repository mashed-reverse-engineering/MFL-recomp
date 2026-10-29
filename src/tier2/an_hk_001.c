#include "mfl_tier2.h"
extern undefined1 g_car_8bd794[];
extern undefined1 g_collision_triangles[];
/*0040e680*/ extern int race_setup_get_course();
/*0040fdc0*/ extern int race_get_phase();
/*004162f0*/ extern int trail_set_param();
/*00421e20*/ extern int vehicle_set_active();
/*00422090*/ extern int vehicle_get_matrix();
/*00426670*/ extern undefined __cdecl player_stat_add_turbo_used(undefined4 p0, undefined4 p1);
/*0042b0c0*/ extern int course_get_ai_polygon_count();
/*0042b210*/ extern int course_get_record_point();
/*00434bb0*/ extern int get_event_type();
/*00435ec0*/ extern int event_type_uses_standard_menu();
/*00435f10*/ extern int get_career_row();
/*004379a0*/ extern int menu_event_course_index_get();
/*00473ac0*/ extern int __cdecl course_bbox_transform_and_test(struct RwBBox *p0, RwMatrix *p1, RwReal *p2, RwMatrix *p3, struct RwV3d *p4);
/*00474230*/ extern undefined course_object_pair_build_clump(undefined4 p0, undefined4 p1, int p2, int p3);
/*00474910*/ extern int spatial_grid_clear();
/*00474ac0*/ extern int __cdecl spatial_grid_get(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*00474ba0*/ extern undefined __cdecl spatial_grid_set(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*00474bc0*/ extern int course_ray_hit_surfaces_collect(undefined4 p0);
/*0047b480*/ extern double car_probe_ground_distance();
/*0047b520*/ extern int car_current_matrix();
/*0047bb00*/ extern int car_world_collision_query();
/*0047d840*/ extern undefined car_wheel_contact_update(undefined4 p0, undefined4 p1, int p2);
/*0047fc10*/ extern undefined hud_label_draw_text(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, float p7, int p8);
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e650*/ extern int rwp_object_set_in_world();
/*004904a0*/ extern undefined __cdecl car_set_physics_matrix(int p0, RwMatrix *p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15, undefined4 p16);
/*00490c10*/ extern int __cdecl physics_body_get_position(undefined4 p0, undefined4 p1);
/*00491d70*/ extern int car_reset_physics();
/*00492b50*/ extern undefined __cdecl car_add_velocity_impl(undefined4 p0, undefined4 p1);
/*00492d10*/ extern int rwp_object_set_active();
/*004a5da0*/ extern int object_destroy_frames();
/*004b2b48*/ extern int _ftol();
/*004b3850*/ extern char * __cdecl _strstr(char *p0, char *p1);
/*004b4ba7*/ extern int entry();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd370*/ extern float k_f_0_3;
/*005dd61c*/ extern float k_f_0_001;
/*005dd62c*/ extern float k_f_100;
/*005ddafc*/ extern float k_f_0_05;
/*005e64a4*/ extern float k_f_0;
/*005de1b4*/ extern float k_f_0_0625;
/*005de47c*/ extern float k_f_n0_25;
/*005deb34*/ extern float k_f_300;
/*005df300*/ extern float k_f_0_333333;
/*005df304*/ extern float k_f_n10000;
/*005df458*/ extern float k_f_200;
/*005dfd0c*/ extern float k_f_0_0019569471;
/*005dfd14*/ extern float k_f_0_041666668;
/*005dfd60*/ extern float k_f_160;
/*005dfd64*/ extern float k_f_0_2777778;
/*005dfd68*/ extern float k_f_3_6;
/*005dfd70*/ extern float k_f_2_0en7;
/*005dfd78*/ extern float k_f_1_05;
/*006292b8*/ extern float g_car_handling_param_b8;
/*006292c0*/ extern undefined4 g_car_init_6292c0;
/*006292c4*/ extern float g_car_handling_param_c4;
/*006292c8*/ extern undefined4 g_car_init_6292c8;
/*006292cc*/ extern undefined4 g_car_init_6292cc;
/*006292d0*/ extern undefined4 g_car_init_6292d0;
/*006292d4*/ extern undefined4 g_car_init_6292d4;
/*006292d8*/ extern undefined4 g_car_init_6292d8;
/*006292dc*/ extern undefined4 g_car_init_6292dc;
/*006292e0*/ extern float g_car_handling_scale;
/*006292ec*/ extern float g_car_default_speed_scale;
/*006292f0*/ extern undefined4 car_handling_table;
/*006292f8*/ extern undefined4 g_car_6292f8;
/*0062ac74*/ extern float g_car_62ac74;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*006af5c0*/ extern undefined1 g_car_6af5c0;
/*006af5e4*/ extern undefined4 g_car_6af5e4;
/*006af6e4*/ extern undefined4 g_car_6af6e4;
/*006af6ec*/ extern undefined4 g_car_6af6ec;
/*006c2658*/ extern undefined4 g_car_6c2658;
/*006c2680*/ extern undefined4 g_course_object_count;
/*006c2684*/ extern undefined4 g_course_object_pair_count;
/*006c2884*/ extern undefined4 g_car_6c2884;
/*006c2888*/ extern undefined4 g_car_6c2888;
/*006c288c*/ extern undefined4 g_car_6c288c;
/*006c2894*/ extern float g_car_6c2894;
/*006c2898*/ extern undefined4 g_course_open_6c2898;
/*006c289c*/ extern float g_car_6c289c;
/*006c28a0*/ extern float g_car_6c28a0;
/*006c28a4*/ extern float g_car_6c28a4;
/*006c28a8*/ extern float g_car_6c28a8;
/*006c28ac*/ extern float g_car_6c28ac;
/*006c28b0*/ extern undefined4 g_car_6c28b0;
/*006c28b4*/ extern undefined4 g_car_6c28b4;
/*006c28b8*/ extern float g_car_6c28b8;
/*006c28bc*/ extern float g_car_6c28bc;
/*006c2944*/ extern undefined4 g_car_6c2944;
/*006c29f4*/ extern undefined4 g_car_6c29f4;
/*006c29f8*/ extern undefined g_app_state_6c29f8;
/*006c2ee0*/ extern undefined4 g_rwp_trigger_group_count;
/*006c2ee4*/ extern undefined4 g_car_6c2ee4;
/*006c2f24*/ extern undefined g_car_6c2f24;
/*006c2f64*/ extern undefined4 g_car_6c2f64;
/*006c2f68*/ extern undefined4 g_gfx_6c2f68;
/*006c3348*/ extern undefined1 g_text_pages_6c3348;
/*006c33c8*/ extern undefined4 g_gfx_6c33c8;
/*006c3d9c*/ extern undefined4 g_text_pages_6c3d9c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef760*/ extern float g_car_init_7ef760;
/*007ef764*/ extern undefined4 g_car_init_7ef764;
/*007ef768*/ extern undefined4 g_car_init_7ef768;
/*007ef76c*/ extern undefined4 g_car_init_7ef76c;
/*007ef7bc*/ extern undefined4 g_cars_7ef7bc;
/*007ef7c4*/ extern undefined4 g_cars_7ef7c4;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/*0089de20*/ extern undefined1 g_physics_scenarios;
/*0089de60*/ extern undefined4 g_gfx_89de60;
/*008b06e0*/ extern struct Car player_car[16];
/*008bd720*/ extern undefined4 g_car_8bd720;
/*008bd74c*/ extern undefined4 g_collision_triangle_count;
/*008bd764*/ extern undefined4 g_car_8bd764;

#define CAR_POINT_CONTACT_OFFSET (0x19c) /* first dword of the per-body-point record in car (inferred) */
#define CAR_POINT_CONTACT_STRIDE (0x31)  /* per-body-point record stride, in dwords (inferred) */
#define CAR_UP_OFFSET (0x9c8)            /* car up vector, 3 floats (inferred) */
#define BODY_POINT_STRIDE (3)            /* floats per body point (x, y, z) */
#define TRIANGLE_STRIDE (0x24)           /* floats per collision triangle record */
#define TRI_EDGE_NORMAL_0 (0x19)         /* edge 0 normal, 3 floats, indexed from tri (inferred) */
#define TRI_EDGE_NORMAL_1 (0x1c)         /* edge 1 normal, 3 floats (inferred) */
#define TRI_EDGE_NORMAL_2 (0x1f)         /* edge 2 normal, 3 floats (inferred) */

/* car_resolve_triangle_contacts */

/*
 * Contact response of the car body points against the collision triangles (inferred).
 * car: byte pointer to the car struct; body points are the g_car_8bd764 array, one per car_point_contact record.
 * triangles: the triangle list at &g_collision_triangles (collected by car_world_collision_query), TRIANGLE_STRIDE floats per
 *   record. tri points at float index 2 of a record: vertices at tri[-2..8], normal at tri[7..9], edge normals at TRI_EDGE_NORMAL_0..2.
 * Returns the number of body points newly flagged in this call (inferred).
 */
int car_resolve_triangle_contacts(char *car,char *triangles)

{
  float v0_x;
  float v0_y;
  float v0_z;
  float v1_x;
  float v1_y;
  float v1_z;
  float v2_x;
  float v2_y;
  float v2_z;
  float n_x;
  float n_y;
  float n_z;
  float point_x;
  float point_y;
  float point_z;
  float *tri;
  float *body_point;
  int contact_count;
  int *point_hit;
  undefined4 *car_point_contact;
  uint triangle_index;

  contact_count = 0;
  triangle_index = 0;
  if (g_collision_triangle_count != 0) {
    tri = (float *)(triangles + 8);
    /* for each collision triangle */
    do {
      v1_x = tri[1];
      v0_x = tri[-2];
      v1_y = tri[2];
      v0_y = tri[-1];
      v0_z = *tri;
      v1_z = tri[3];
      v2_x = tri[4];
      v2_y = tri[5];
      v2_z = tri[6];
      n_x = tri[7];
      n_y = tri[8];
      n_z = tri[9];
      point_hit = &g_car_8bd720;
      body_point = (float *)&g_car_8bd764;
      car_point_contact = (undefined4 *)(car + CAR_POINT_CONTACT_OFFSET);
      /* for each body point not yet flagged */
      do {
        if (*point_hit == 0) {
          point_x = body_point[-1];
          point_y = *body_point;
          point_z = body_point[1];
          /* point on the inner side of all three edge planes (inferred) */
          if ((k_f_0 <= (point_x - v0_x) * tri[TRI_EDGE_NORMAL_0] +
                        (point_y - v0_y) * tri[TRI_EDGE_NORMAL_0 + 1] + (point_z - v0_z) * tri[TRI_EDGE_NORMAL_0 + 2]) &&
             (k_f_0 <= (point_x - v1_x) * tri[TRI_EDGE_NORMAL_1] +
                       (point_y - v1_y) * tri[TRI_EDGE_NORMAL_1 + 1] + (point_z - v1_z) * tri[TRI_EDGE_NORMAL_1 + 2])) {
            /* point_x is reused for the plane distance (inferred): within [-0.25, 0.05) of the triangle plane,
               and the triangle normal points up relative to the car (dot with car up > 0.3) */
            if ((k_f_0 <= (point_x - v2_x) * tri[TRI_EDGE_NORMAL_2] +
                          (point_y - v2_y) * tri[TRI_EDGE_NORMAL_2 + 1] + (point_z - v2_z) * tri[TRI_EDGE_NORMAL_2 + 2]) &&
               (((point_x = (point_x - v2_x) * n_x +
                           (point_y - v2_y) * n_y + (point_z - v2_z) * n_z,
                 !(k_f_n0_25 > point_x) && (point_x < k_f_0_05)) &&
                (k_f_0_3 < n_x * *(float *)(car + CAR_UP_OFFSET) +
                           n_y * *(float *)(car + CAR_UP_OFFSET + 4) +
                           n_z * *(float *)(car + CAR_UP_OFFSET + 8))))) {
              *point_hit = 1;
              contact_count = contact_count + 1;
              *car_point_contact = 1;
            }
          }
        }
        body_point = body_point + BODY_POINT_STRIDE;
        point_hit = point_hit + 1;
        car_point_contact = car_point_contact + CAR_POINT_CONTACT_STRIDE;
      } while (body_point < (float *)&g_car_8bd794);
      triangle_index = triangle_index + 1;
      tri = tri + TRIANGLE_STRIDE;
    } while (triangle_index < g_collision_triangle_count);
  }
  return contact_count;
}
