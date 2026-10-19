#include "mfl_tier2.h"
/*00423fe0*/ extern undefined damage_add_clamped(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*00424230*/ extern int car_damage_smoke_choose();
/*0042a3c0*/ extern int course_race_start();
/*0042b030*/ extern undefined __cdecl course_unload(undefined4 p0);
/*00481fd0*/ extern double random_range();
/*00482010*/ extern int random_int_range();
/*00486710*/ extern int particles_destroy();
/*004b2b48*/ extern int _ftol();
/*004b4ba7*/ extern int entry();
/*005dd370*/ extern float k_f_0_3;
/*005dd374*/ extern float k_f_0_2;
/*00603fc8*/ extern struct CourseEntry g_course_table[28];
/*006047a8*/ extern int g_course_table_count;
/*0060761c*/ extern undefined4 g_particles_60761c;
/*00607620*/ extern undefined4 g_particles_607620;
/*00607624*/ extern undefined4 g_game_607624;
/*00659d04*/ extern undefined4 g_current_course_entry;
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/*0065a9b8*/ extern undefined4 g_game_65a9b8;
/*0065a9c0*/ extern undefined4 g_game_65a9c0;
/*0065a9c4*/ extern undefined4 g_game_65a9c4;
/*0065a9d8*/ extern undefined1 g_car_damage_65a9d8;
/*0065a9e8*/ extern undefined4 car_damage;
/*0065a9ec*/ extern undefined4 g_car_damage_65a9ec;
/*0065aa88*/ extern undefined g_car_damage_65aa88;
/*0065aac8*/ extern undefined4 g_car_damage_65aac8;
/*0065aacc*/ extern undefined4 g_car_damage_65aacc;
/*0065aad0*/ extern undefined4 g_car_damage_65aad0;
/*0065aad4*/ extern undefined4 g_car_damage_65aad4;
/*0065aad8*/ extern undefined4 g_car_damage_65aad8;
/*0065aadc*/ extern undefined4 g_car_damage_65aadc;
/*0065aae0*/ extern undefined4 g_car_damage_65aae0;
/*0065aae4*/ extern undefined4 g_car_damage_65aae4;
/*0065aae8*/ extern undefined4 g_car_damage_65aae8;
/*0065aaec*/ extern undefined4 g_car_damage_65aaec;
/*0065aaf0*/ extern undefined g_car_damage_65aaf0;
/*0065c0c8*/ extern undefined4 g_car_damage_65c0c8;


extern void __cdecl course_call_cb2(char *param_1);


extern void __cdecl course_call_cb3(char *param_1);


extern void __cdecl course_call_cb4(char *param_1);


extern void __cdecl course_call_cb5(char *param_1);


extern void course_call_cb6(void);


extern void __cdecl course_call_cb7(char *param_1);


extern void course_call_cb8(void);


extern void course_call_cb9(void);


extern void course_call_cb10(void);


extern void course_call_cb11(void);


extern void __cdecl course_call_world_setup(char *param_1);


extern CourseEntry * course_table_find_entry(char *param_1);


extern undefined4 course_has_cb0(void);


extern undefined4 course_has_cb1(void);


extern undefined4 course_has_cb2(void);


extern undefined4 course_has_cb3(void);


extern undefined4 course_has_cb4(void);


extern undefined4 course_has_cb5(void);


extern undefined4 course_has_cb6(void);


extern undefined4 course_has_cb7(void);


extern undefined4 course_has_cb8(void);


extern undefined4 course_has_cb9(void);


extern undefined4 course_has_cb10(void);


extern undefined4 course_has_cb11(void);


extern undefined4 course_has_world_setup(void);


extern void game_set_particles_60761c(undefined4 param_1);


extern void game_set_particles_607620(undefined4 param_1);


extern void game_set_65a9b8_607624(char *param_1,char *param_2);


extern void game_set_65a9c0_65a9c4(char *param_1,char *param_2);

#define VEHICLE_MATRIX_FLOATS (6)


/* vehicle_get_matrix */

/* (vehicle_index, out_matrix): copies the VEHICLE_MATRIX_FLOATS floats of
   g_vehicle_table[vehicle_index].matrix6 (+0x230) to out_matrix. Returns nothing.
   out_matrix must have room for six floats (inferred). */

/* TIER2: validated 2026-10-18 authority=diffemu */
typedef struct { float f[6]; } M6;
extern void vehicle_get_matrix(int vehicle_index,float *out_matrix);


/* car_damage_sum_front_back */

/* "car_damage[car].front + back (two floats of the 0x28 byte record)" */

extern float10 car_damage_sum_front_back(char *param_1);


/* car_damage_smoke_destroy */

/* "destroys the damage smoke particle system" */

extern void car_damage_smoke_destroy(void);


/* car_damage_smoke_spawn */

/* "adds one smoke puff (position, random colour/size/rotation) to the ring of 128" */

extern void __cdecl car_damage_smoke_spawn (undefined4 *param_1,char *param_2,char *param_3,char *param_4, char *param_5,char *param_6,char *param_7,char *param_8, char *param_9,char *param_10);


/* car_damage_smoke_destroy_004246c0 */

/* duplicate name: was car_damage_smoke_destroy (first address 00424220) */

extern void car_damage_smoke_destroy_004246c0(void);


/* car_set_damage */

/* "(car, value, side)" */

extern void car_set_damage(char *param_1,float param_2,char *param_3);


