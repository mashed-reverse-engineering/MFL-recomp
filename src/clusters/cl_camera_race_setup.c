// cluster-entries: camera_race_setup
/* Installed cluster camera_race_setup (tools/cluster_install.py from output/cluster_pilot_ql3/camera_race_setup.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/*0044a6a0*/ extern int camera_editor_setup_parallel(void *camera_record);
/*004d1da0*/ extern void *RwCameraSetProjection(void *camera, int projection);
/*0040fdc0*/ extern int race_get_phase(void);
/*0040fe20*/ extern int race_setup_slot_used(int slot);
/*0047a830*/ extern int car_is_active(int slot);
/*0047ac30*/ extern void car_get_mode(int slot, int *mode, int *extra);
/*0040ff70*/ extern int race_get_last_active_player(void);
/*0047b520*/ extern void car_current_matrix(RwMatrix **matrix, int player);
/*00479ae0*/ extern void car_get_bounds_point(RwV3d *point, int player, int index);
/*0047b590*/ extern void car_get_forward(RwV3d *forward, int player);
/*00409eb0*/ extern float player_rank_key(int player);
/*0040a8d0*/ extern int course_token_list_skip_to_entry(int index, int arg);
/*00409f60*/ extern void grid_position(int entry, int arg_a, int arg_b, RwV3d *position, int *aux);
/*0047b480*/ extern float car_probe_ground_distance(float x, float y, float z, void *probe);
/*0040ff60*/ extern int race_get_phase_timer(void);
/*004d53a0*/ extern float RwV3dLength(RwV3d *vector);
/*0044c2c0*/ extern void camera_record_set_path(void *camera_record, float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, float speed, float lag);
/*00421fe0*/ extern void vehicle_set_field_198(int slot, int value);
/*0044fa30*/ extern int camera_update(void *camera_record, int arg);
/*00481fd0*/ extern float random_range(float low, float high);
/*0042ebe0*/ extern int time_get_minutes(int arg);
/*0042ebf0*/ extern int time_get_seconds(int arg);
/*0042ebd0*/ extern float time_get_fraction(int arg);
/*004b2a60*/ extern int __cdecl sprintf(char *buffer, const char *format, ...);
/*007f0238*/ extern int g_player_slot3_extra;           /* 1: the AI editor camera */
/*008c74e4*/ extern void *g_camera_race_8c74e4;
/*008c74cc*/ extern float g_camera_race_8c74cc;
/*008c74d0*/ extern float g_camera_race_8c74d0;
/*008c7460*/ extern char g_car_8c7460[];                /* the camera record of the race camera */
/*008c5928*/ extern int g_car_8c5928;
/*008c7e08*/ extern int g_camera_race_8c7e08;
/*008c7e14*/ extern int g_camera_race_8c7e14;
/*00801b0c*/ extern char g_car_801b0c[];                /* ground probe data */
/*0069cbd4*/ extern int g_event_type;
/*008c86e0*/ extern char g_camera_race_8c86e0[];        /* the race code: 2 check letters, 2 letters, '-', 14 letters / digits */

#define PROJECTION_PERSPECTIVE 1
#define MAX_PLAYER_SLOTS 4
#define RACE_PHASE_RACING 6
#define RACE_PHASE_STANDINGS 7
#define RACE_PHASE_NONE -1
#define CAR_MODE_DRIVING 0
#define EDITOR_SLOT_FLAG 1
#define CAMERA_LIFT 0.15f                 /* the camera looks 0.15 above the car */
#define RESULT_BEHIND_TIMER_START 200     /* camera drifts up over 200 ticks after this */
#define RESULT_DRIFT_UP_TICKS 200
#define RESULT_DRIFT_UP_RATE 0.01f
#define RESULT_HEIGHT_OFFSET 8.0f
#define RESULT_SINK_TICKS 400
#define RESULT_SINK_RATE 6.6666666e-4f
#define RESULT_TARGET_LIFT 0.5f
#define PATH_SPEED 1.0f
#define PATH_LAG 0.2f
#define CODE_ALPHABET 26                  /* letters of the race code */
#define CODE_FIRST_LETTER 'A'
#define CODE_RANDOM_LETTER_MAX 25.0f      /* random_range(0, 25) truncated: letters A..Y */
#define CODE_TIME_SCALE 100.0f
#define CODE_CHECK_MODULUS 0x2a4          /* 26 * 26 */
#define CODE_KEY_LENGTH 10
#define CODE_KEY_OFFSET 0x82              /* 130: keeps (key + code letter - offset) near 'A'.. in range */
#define CODE_DIVISOR_1 456976             /* 26^4 .. 26^2 split a number into base 26 letters */
#define CODE_DIVISOR_2 17576
#define CODE_DIVISOR_3 676

/* The frame of the original (0x58 bytes): the objects whose addresses are passed to callees */
typedef struct CameraFrame {
  int mode;                       /* car_get_mode out values */
  RwMatrix *matrix;               /* car_current_matrix out value */
  int grid_aux;                   /* grid_position out value */
  int mode_extra;
  RwV3d from_point;               /* the interpolated look at point, later the new camera target */
  RwV3d to_point;                 /* second grid position, then the camera to target vector */
  RwV3d position;                 /* camera position: the car position raised */
  RwV3d forward;
  RwV3d bounds_far;
  RwV3d bounds_near;              /* z is reused for the centre of the car and the fraction of the rank key */
} CameraFrame;

/* Builds the short race code (time and a few random letters in base 26, a check sum and a key scramble) into g_camera_race_8c86e0 for the
   event types 0 .. 2.  Returns the last scrambled character / quotient the way the original leaves it in EAX (g_event_type for the others). */
static void ui_get_triple_r(float * p0, int * p1, int * p2, int p3, int p4);
extern void ui_get_triple(void);
static int race_code_generate(void)
{
  char time_code[6];              /* base 26 letters of the time of day */
  char triple_code[6];            /* base 26 letters of the ui triple */
  char prefix[3];
  char random_letter[2];
  char key_0[11] = "TKGFWNCIKR";        /* the key is picked by the last two bits of the third code letter */
  char key_1[11] = "LFDGUYVDEU";
  char key_2[11] = "MDXKHDIGNX";
  char key_3[11] = "PSOLPEBCYB";
  int minutes, seconds, number, rest, i, value, sum, triple_a, triple_b, result;
  float triple_fraction;
  char *key;
  int digit;

  if (g_event_type != 0 && g_event_type != 1 && g_event_type != 2) {
    return g_event_type;
  }
  minutes = time_get_minutes(0);
  seconds = time_get_seconds(0);
  number = (minutes * 100 + seconds) * 100 + (int)(time_get_fraction(0) * CODE_TIME_SCALE);
  digit = number / CODE_DIVISOR_1;
  time_code[0] = (char)(digit + CODE_FIRST_LETTER);
  rest = number - digit * CODE_DIVISOR_1;
  digit = rest / CODE_DIVISOR_2;
  time_code[1] = (char)(digit + CODE_FIRST_LETTER);
  rest -= digit * CODE_DIVISOR_2;
  digit = rest / CODE_DIVISOR_3;
  time_code[2] = (char)(digit + CODE_FIRST_LETTER);
  rest -= digit * CODE_DIVISOR_3;
  digit = rest / CODE_ALPHABET;
  time_code[3] = (char)(digit + CODE_FIRST_LETTER);
  time_code[4] = (char)(rest - digit * CODE_ALPHABET + CODE_FIRST_LETTER);
  time_code[5] = 0;
  ui_get_triple_r(&triple_fraction, &triple_b, &triple_a, 0, 1);
  value = (int)((float)(triple_a * 10000) + (float)(triple_b * 100) + triple_fraction * CODE_TIME_SCALE);
  digit = value / CODE_DIVISOR_1;
  triple_code[0] = (char)(digit + CODE_FIRST_LETTER);
  rest = value - digit * CODE_DIVISOR_1;
  digit = rest / CODE_DIVISOR_2;
  triple_code[1] = (char)(digit + CODE_FIRST_LETTER);
  rest -= digit * CODE_DIVISOR_2;
  digit = rest / CODE_DIVISOR_3;
  triple_code[2] = (char)(digit + CODE_FIRST_LETTER);
  rest -= digit * CODE_DIVISOR_3;
  digit = rest / CODE_ALPHABET;
  triple_code[3] = (char)(digit + CODE_FIRST_LETTER);
  triple_code[4] = (char)(rest - digit * CODE_ALPHABET + CODE_FIRST_LETTER);
  triple_code[5] = 0;
  random_letter[0] = (char)((int)random_range(0.0f, CODE_RANDOM_LETTER_MAX) + CODE_FIRST_LETTER);
  random_letter[1] = 0;
  prefix[0] = CODE_FIRST_LETTER;
  prefix[1] = CODE_FIRST_LETTER;
  prefix[2] = 0;
  sprintf(g_camera_race_8c86e0, "%s%s%s%s%s%s%d", prefix, random_letter, "-", triple_code, time_code, "-", number);
  sum = 0;
  for (i = 0; i < 15; i += 5) {
    sum += g_camera_race_8c86e0[4 + i] + g_camera_race_8c86e0[5 + i] + g_camera_race_8c86e0[6 + i] + g_camera_race_8c86e0[7 + i] + g_camera_race_8c86e0[8 + i];
  }
  if (sum >= CODE_CHECK_MODULUS) {
    sum = (unsigned int)sum % CODE_CHECK_MODULUS;
  }
  g_camera_race_8c86e0[0] = (char)(sum / CODE_ALPHABET + CODE_FIRST_LETTER);
  g_camera_race_8c86e0[1] = (char)(sum % CODE_ALPHABET + CODE_FIRST_LETTER);
  i = 0;
  do {
    key = key_0;
    if ((g_camera_race_8c86e0[2] & 3) == 1) {
      key = key_1;
    } else if ((g_camera_race_8c86e0[2] & 3) == 2) {
      key = key_2;
    } else if ((g_camera_race_8c86e0[2] & 3) == 3) {
      key = key_3;
    }
    result = key[i];
    value = result + g_camera_race_8c86e0[4 + i] - CODE_KEY_OFFSET;
    if (value >= CODE_ALPHABET) {
      result = value * 0x4ec4ec4f;
      value = (unsigned int)value % CODE_ALPHABET;
    }
    g_camera_race_8c86e0[4 + i] = (char)(value + CODE_FIRST_LETTER);
    i++;
  } while (i < CODE_KEY_LENGTH);
  return result;
}

/* camera_race_setup: sets up the race camera for one frame: the AI editor camera when slot 3 is the editor; the normal camera update while a
   car drives (or the race phase says so); otherwise (all cars out of the race / no race) a flyover path along the track at the rank position
   of the last active player.  Then the race code is regenerated. */
int camera_race_setup(void *camera, int unused, int arg)
{
  CameraFrame frame;
  int slot, phase, player, rank_int, entry, timer, driving;
  float rank_key, fraction, weight, dist;

  g_camera_race_8c74e4 = camera;
  g_camera_race_8c74cc = 1.0f;
  g_camera_race_8c74d0 = 1.0f;
  if (g_player_slot3_extra == EDITOR_SLOT_FLAG) {
    return camera_editor_setup_parallel(g_car_8c7460);
  }
  RwCameraSetProjection(camera, PROJECTION_PERSPECTIVE);
  phase = race_get_phase();
  if (phase == RACE_PHASE_RACING) {
    driving = 0;
    for (slot = 0; slot < MAX_PLAYER_SLOTS; slot++) {
      if (race_setup_slot_used(slot) != 0 && car_is_active(slot) == 1) {
        car_get_mode(slot, &frame.mode, &frame.mode_extra);
        if (frame.mode == CAR_MODE_DRIVING) {
          driving++;
        }
      }
    }
    if (driving != 0) {
      g_car_8c5928 = 0;
      g_camera_race_8c7e08 = 0;
      g_camera_race_8c7e14 = 0;
      return camera_update(g_car_8c7460, arg);
    }
  } else {
    if (phase == RACE_PHASE_STANDINGS) {
      return race_code_generate();
    }
    if (phase != RACE_PHASE_NONE) {
      g_car_8c5928 = 0;
      g_camera_race_8c7e08 = 0;
      g_camera_race_8c7e14 = 0;
      return camera_update(g_car_8c7460, arg);
    }
  }
  player = race_get_last_active_player();
  if (player == -1) {
    player = 0;
  }
  car_current_matrix(&frame.matrix, player);
  frame.position.x = frame.matrix->pos.x;
  frame.position.z = frame.matrix->pos.z;
  frame.position.y = frame.matrix->pos.y + CAMERA_LIFT;
  car_get_bounds_point(&frame.bounds_near, player, 4);
  car_get_bounds_point(&frame.bounds_far, player, 6);
  frame.bounds_near.z = (frame.bounds_far.z + frame.bounds_near.z) * 0.5f;
  car_get_forward(&frame.forward, player);
  g_car_8c5928 = 0;
  frame.position.x = frame.forward.x * frame.bounds_near.z + frame.position.x;
  frame.position.z = frame.forward.z * frame.bounds_near.z + frame.position.z;
  rank_key = player_rank_key(player);
  rank_int = (int)rank_key;
  frame.bounds_near.z = rank_key - (float)rank_int;
  entry = course_token_list_skip_to_entry(rank_int, 0);
  grid_position(entry, 0, 1, &frame.from_point, &frame.grid_aux);
  entry = course_token_list_skip_to_entry(entry, 0);
  grid_position(entry, 0, 1, &frame.to_point, &frame.grid_aux);
  weight = 1.0f - frame.bounds_near.z;
  frame.from_point.x = frame.to_point.x * frame.bounds_near.z + frame.from_point.x * weight;
  frame.from_point.y = frame.to_point.y * frame.bounds_near.z + frame.from_point.y * weight;
  frame.from_point.z = frame.to_point.z * frame.bounds_near.z + frame.from_point.z * weight;
  dist = car_probe_ground_distance(frame.from_point.x, frame.from_point.y, frame.from_point.z, g_car_801b0c);
  frame.from_point.y = frame.from_point.y - dist;
  timer = race_get_phase_timer() - RESULT_BEHIND_TIMER_START;
  if (timer > RESULT_DRIFT_UP_TICKS) {
    timer = RESULT_DRIFT_UP_TICKS;
  }
  frame.from_point.y = (float)timer * RESULT_DRIFT_UP_RATE + frame.from_point.y + RESULT_HEIGHT_OFFSET;
  frame.to_point.x = frame.from_point.x - frame.position.x;
  frame.to_point.y = frame.from_point.y - frame.position.y;
  frame.to_point.z = frame.from_point.z - frame.position.z;
  RwV3dLength(&frame.to_point);
  timer = race_get_phase_timer();
  if (timer > RESULT_SINK_TICKS) {
    timer = RESULT_SINK_TICKS;
  }
  frame.from_point.y = frame.from_point.y - (float)timer * RESULT_SINK_RATE;
  frame.to_point.x = frame.from_point.x - frame.position.x;
  frame.to_point.y = frame.from_point.y - frame.position.y;
  frame.to_point.z = frame.from_point.z - frame.position.z;
  RwV3dLength(&frame.to_point);
  frame.from_point.x = frame.to_point.x + frame.position.x;
  g_camera_race_8c74e4 = camera;
  g_camera_race_8c74cc = 1.0f;
  frame.from_point.z = frame.to_point.z + frame.position.z;
  g_camera_race_8c74d0 = 1.0f;
  frame.from_point.y = frame.to_point.y + frame.position.y + RESULT_TARGET_LIFT;
  camera_record_set_path(g_car_8c7460, frame.position.x, frame.position.y, frame.position.z, frame.from_point.x, frame.from_point.y, frame.from_point.z, PATH_SPEED, PATH_LAG);
  race_get_phase_timer();
  for (slot = 0; slot < MAX_PLAYER_SLOTS; slot++) {
    vehicle_set_field_198(slot, 0);
  }
  return race_code_generate();
}

/* ui_get_triple_r: C -> original ui_get_triple (registers s0,s1,s2,eax,ecx) */
__declspec(naked) void ui_get_triple_r(float * p0, int * p1, int * p2, int p3, int p4)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 28]
        mov ecx, dword ptr [esp + 32]
        call ui_get_triple
        add esp, 12
        ret
    }
}
