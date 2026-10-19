// cluster-entries: weapon_oil_car_slide_random
/* Installed cluster weapon_oil_car_slide_random (tools/cluster_install.py from output/cluster_pilot6/weapon_oil_car_slide_random.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster weapon_oil_car_slide_random: an oil slick under a car: in 5 % of the calls the car gets a sideways push (velocity length * +-0.0001 on y, the sign
   from whether 1 - random is above 0.025), then its field 6 is set to (100, 2400.0). */
extern float k_f_0_95;
extern float float_1_0;
extern float k_f_0_025000006;
extern float k_f_1_0en4;
extern float k_f_n1_0en4;
extern float random_range(float low, float high);
extern void car_get_velocity(float *velocity, int car);
extern void car_set_field_144(float *velocity, int car);
extern void car_set_field_set(int car, int field, int value, float scale);
extern float RwV3dLength(RwV3d *v);

/* was: ESI = car */
void weapon_oil_car_slide_random_n(int car)
{
    struct {
        float random;          /* S+0x00 */
        float vector[3];       /* S+0x04 */
    } frame;
    float scale;

    frame.random = random_range(0.0f, 1.0f);
    if (frame.random > k_f_0_95) {
        car_get_velocity(frame.vector, car);
        scale = (float_1_0 - frame.random > k_f_0_025000006) ? k_f_1_0en4 : k_f_n1_0en4;
        frame.vector[1] = RwV3dLength((RwV3d *)frame.vector) * scale;
        frame.vector[0] = 0;
        frame.vector[2] = 0;
        car_set_field_144(frame.vector, car);
    }
    car_set_field_set(car, 6, 100, 2400.0f);
}

/* entry thunk at the original address of weapon_oil_car_slide_random: register inputs (esi) -> natural C weapon_oil_car_slide_random_n; ret 0 */
__declspec(naked) void weapon_oil_car_slide_random(void)
{
    __asm {
        push esi
        call weapon_oil_car_slide_random_n
        add esp, 4
        ret
    }
}
