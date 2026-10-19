// cluster-entries: car_damage_update
/* Installed cluster car_damage_update (tools/cluster_install.py from output/cluster_pilot6/car_damage_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster car_damage_update: the 4 damage records (0x28 bytes each) of the cars.  A record holds the damage of two car parts (0x33 / 0x34): when a part reaches
   17.5 damage for the first time it is flagged and a smoke puff is spawned at the part, at 10.0 the smoke flag is set. */
typedef struct CarDamage {
    float damage[2];       /* 0x00 */
    int hit[2];            /* 0x08: part damaged (smoke puff spawned) */
    int smoke[2];          /* 0x10 */
    int car;               /* 0x18 */
    int timer;             /* 0x1c: counts down to 0 */
    char unknown_20[8];
} CarDamage;

extern CarDamage car_damage[4];
extern float k_f_17_5;
extern float float_10_0;
extern void car_set_field_set(int car, int field, int value, float scale);
extern void vehicle_part_frame(int car, int part);
extern void vehicle_part_matrix(int car, int part, float *matrix);
extern void fx_smoke_particle_spawn_grey(float *position, float size);
extern void vehicle_set_flag_10(int car, int a, int b);
extern void vehicle_set_flag_1000(int car, int a, int b);

/* was: ESI = record */
void car_damage_update_n(CarDamage *rec)
{
    struct {
        float damage;          /* S+0x00 */
        float position[3];     /* S+0x04 */
        float matrix[16];      /* S+0x10: the part matrix (RwMatrix), translation at +0x30 */
    } frame;

    rec->timer = rec->timer - 1;
    if (rec->timer < 0) {
        rec->timer = 0;
    }
    frame.damage = rec->damage[0];
    car_set_field_set(rec->car, 5, (int)frame.damage, -1.0f);
    if (rec->hit[0] != 0 || frame.damage < k_f_17_5) {
        if (rec->smoke[0] == 0 && float_10_0 <= frame.damage) {
            rec->smoke[0] = 1;
        }
    }
    else {
        rec->hit[0] = 1;
        rec->smoke[0] = 1;
        vehicle_part_frame(rec->car, 0x33);
        vehicle_part_matrix(rec->car, 0x33, frame.matrix);
        frame.position[0] = frame.matrix[12];
        frame.position[1] = frame.matrix[13];
        frame.position[2] = frame.matrix[14];
        fx_smoke_particle_spawn_grey(frame.position, 2.0f);
    }
    frame.damage = rec->damage[1];
    car_set_field_set(rec->car, 4, (int)frame.damage, -1.0f);
    if (rec->hit[1] != 0 || frame.damage < k_f_17_5) {
        if (rec->smoke[1] == 0 && float_10_0 <= frame.damage) {
            rec->smoke[1] = 1;
        }
    }
    else {
        rec->hit[1] = 1;
        rec->smoke[1] = 1;
        vehicle_part_frame(rec->car, 0x34);
        vehicle_part_matrix(rec->car, 0x34, frame.matrix);
        frame.position[0] = frame.matrix[12];
        frame.position[1] = frame.matrix[13];
        frame.position[2] = frame.matrix[14];
        fx_smoke_particle_spawn_grey(frame.position, 2.0f);
    }
    vehicle_set_flag_10(rec->car, rec->hit[0], rec->hit[1]);
    vehicle_set_flag_1000(rec->car, rec->smoke[0], rec->smoke[1]);
}

/* entry thunk at the original address of car_damage_update: register inputs (esi) -> natural C car_damage_update_n; ret 0 */
__declspec(naked) void car_damage_update(void)
{
    __asm {
        push esi
        call car_damage_update_n
        add esp, 4
        ret
    }
}
