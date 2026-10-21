// cluster-entries: car_update
/* Installed cluster car_update (tools/cluster_install.py from output/cluster_pilot_uc3/car_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase(void);
extern unsigned int g_tick_number;

/* The driving force of the first two wheels: the current value and a ring of the last 16 values (indexed by the low 4 bits of the tick counter). */
typedef struct WheelForce { float current; float history[16]; } WheelForce;
#define WHEEL_FORCE(car, i) ((WheelForce *)((char *)&(car)->wheels[i] + 0x40))
#define CAR_INT(car, off) (*(int *)((char *)(car) + (off)))
#define CAR_FLOAT(car, off) (*(float *)((char *)(car) + (off)))
#define CAR_THROTTLE_TIME 0xb24      /* ms the accelerate input has been held */
#define CAR_BRAKE_TIME 0xb28         /* ms the brake input has been held */
#define CAR_TURBO_FLAG 0xbf0

/* Per step update of one car with the input bytes `input` (accelerate, brake, ..., [5] = analog flag): the "sliding" amount (what is left of the speed
   across the heading), the held times of accelerate and brake, the driving force of the first two wheels (their force ring is updated), then the
   force integration (car_apply_forces), the drivetrain (car_update_drivetrain) and the air control; mode 2 slows the car down by 10% per step. */
static void car_apply_forces_r(Car * p0, float p1, RwMatrix * p2);
extern void car_apply_forces(void);
static int car_update_drivetrain_r(Car * p0, int p1, float p2, RwMatrix * p3, unsigned char * p4);
extern void car_update_drivetrain(void);
static void car_air_control_r(Car * p0, RwMatrix * p1, float p2);
extern void car_air_control(void);
void car_update_n(Car *car, int arg0, float dt, unsigned char *input, int unused)
{
  RwMatrix *matrix;
  int race_phase;
  unsigned int tick;
  float dot;
  float force;
  float held;

  race_phase = race_get_phase();
  matrix = (RwMatrix *)(car->matrices + car->matrix_current * 0x10);
  CAR_INT(car, 0xb20) = 0;
  CAR_INT(car, 0xb1c) = 0;
  CAR_INT(car, 0xb18) = 0;
  CAR_INT(car, 0xb14) = 0;
  WHEEL_FORCE(car, 3)->current = 0.0f;
  WHEEL_FORCE(car, 2)->current = 0.0f;
  WHEEL_FORCE(car, 1)->current = 0.0f;
  WHEEL_FORCE(car, 0)->current = 0.0f;
  if (car->speed == 0.0f) {
    CAR_INT(car, 0xb0c) = 0;
  } else {
    dot = car->forward[0] * car->velocity[0] + car->forward[1] * car->velocity[1] + car->forward[2] * car->velocity[2];
    if (dot < 0.0f) {
      dot = -dot;
    }
    CAR_FLOAT(car, 0xb0c) = (1.0f - dot / car->speed) * car->speed;
  }
  if (input[0] == 0) {
    CAR_INT(car, CAR_THROTTLE_TIME) = 0;
  } else {
    CAR_INT(car, CAR_THROTTLE_TIME) = (int)((float)CAR_INT(car, CAR_THROTTLE_TIME) + dt);
  }
  if (input[1] == 0) {
    CAR_INT(car, CAR_BRAKE_TIME) = 0;
  } else {
    CAR_INT(car, CAR_BRAKE_TIME) = (int)((float)CAR_INT(car, CAR_BRAKE_TIME) + dt);
  }
  tick = g_tick_number & 0xf;
  force = 0.0f;
  if (input[0] != 0) {
    if (car->speed > 64.0f) {
      CAR_INT(car, 0xb20) = 1;
    }
    force = (float)input[0] * car->wheels[0].const_34 * 0.00390625f;
    if (race_phase == 7) {
      force = 0.0f;
    }
    force = force * 0.5f;
    if (CAR_INT(car, CAR_TURBO_FLAG) != 0) {
      force = force * 0.75f;
    }
    if ((float)input[5] > 128.0f) {
      force = force * 1.5f;
    } else {
      held = (float)CAR_INT(car, CAR_THROTTLE_TIME);
      if (held > 6000.0f) {
        held = 6000.0f;
      }
      force = (held + 6000.0f) * force * 0.00016666666f;
    }
    WHEEL_FORCE(car, 0)->current = force;
    WHEEL_FORCE(car, 1)->current = force;
  }
  WHEEL_FORCE(car, 0)->history[tick] = force;
  WHEEL_FORCE(car, 1)->history[tick] = force;
  if (input[1] != 0) {
    if (car->speed > 64.0f) {
      CAR_INT(car, 0xb20) = 1;
    }
    force = (float)input[1] * car->wheels[0].const_34 * 0.00390625f;
    if (race_phase == 7) {
      force = 0.0f;
    }
    force = force * 0.5f;
    if (CAR_INT(car, CAR_TURBO_FLAG) != 0) {
      force = force * 0.75f;
    }
    if ((float)input[5] > 128.0f) {
      force = force * 1.5f;
    } else {
      held = (float)CAR_INT(car, CAR_BRAKE_TIME);
      if (held > 6000.0f) {
        held = 6000.0f;
      }
      force = (held + 6000.0f) * force * 0.00016666666f;
    }
    force = -force;
    WHEEL_FORCE(car, 0)->current = force;
    WHEEL_FORCE(car, 1)->current = force;
    WHEEL_FORCE(car, 0)->history[tick] = force;
    WHEEL_FORCE(car, 1)->history[tick] = force;
  }
  car_apply_forces_r(car, dt, matrix);
  car_update_drivetrain_r(car, arg0, dt, matrix, input);
  car_air_control_r(car, matrix, dt);
  if (car->mode == 2) {
    car->velocity[0] = car->velocity[0] * 0.9f;
    car->velocity[1] = car->velocity[1] * 0.9f;
    car->velocity[2] = car->velocity[2] * 0.9f;
  }
}

/* car_apply_forces_r: C -> original car_apply_forces (registers edi,s0,s1) */
__declspec(naked) void car_apply_forces_r(Car * p0, float p1, RwMatrix * p2)
{
    __asm {
        push edi
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov edi, dword ptr [esp + 16]
        call car_apply_forces
        add esp, 8
        pop edi
        ret
    }
}

/* car_update_drivetrain_r: C -> original car_update_drivetrain (registers esi,s0,s1,s2,s3) */
__declspec(naked) int car_update_drivetrain_r(Car * p0, int p1, float p2, RwMatrix * p3, unsigned char * p4)
{
    __asm {
        push esi
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        mov esi, dword ptr [esp + 24]
        call car_update_drivetrain
        add esp, 16
        pop esi
        ret
    }
}

/* car_air_control_r: C -> original car_air_control (registers ecx,esi,s0) */
__declspec(naked) void car_air_control_r(Car * p0, RwMatrix * p1, float p2)
{
    __asm {
        push esi
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 12]
        mov esi, dword ptr [esp + 16]
        call car_air_control
        add esp, 4
        pop esi
        ret
    }
}

/* entry thunk at the original address of car_update: register inputs (eax,s0,s1,s2,s3) -> natural C car_update_n; ret 0 */
__declspec(naked) void car_update(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push eax
        call car_update_n
        add esp, 20
        ret
    }
}
