// cluster-entries: camera_chase_update camera_follow_update camera_record_update
/* Installed cluster camera_chase_update (tools/cluster_install.py from output/cluster_pilot_dtrig/camera_chase_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* DECLS-BEGIN */
/* DECLS-END */
extern double atan(double);
extern double sin(double);
/*004d53a0*/ extern RwReal RwV3dLength(RwV3d *v);
/*004d5410*/ extern float _rwSqrt(float x);
/*004d43f0*/ extern RwMatrix *RwMatrixRotate(RwMatrix *matrix, RwV3d *axis, float degrees, int combine);
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, RwV3d *translation, int combine);
/*004d56d0*/ extern RwV3d *RwV3dTransformVectors(RwV3d *out, RwV3d *in, int count, RwMatrix *matrix);
/*0044a990*/ extern void camera_get_target_position(RwV3d *out, int *view);
/*0040f710*/ extern int intro_banner_get_flag(void);
/*0047b520*/ extern void car_current_matrix(float **matrix_out, int car);
/*0047b750*/ extern void car_get_speed(float *speed_out, int car);
/*004d54d0*/ extern float RwV2dLength(RwV2d *v);
/*004d5540*/ extern float RwV2dNormalize(RwV2d *out, RwV2d *in);
/*0044c560*/ extern void menu_set_car_8c7460_and_reset(int car);
/*0042b110*/ extern int current_course_id(void);
/*006a06f4*/ extern float g_camera_follow_6a06f4;       /* seconds-ish clock of the follow camera sway */
/*00409eb0*/ extern float player_rank_key(int player);
/*007ef7f0*/ extern int g_tick_count;                    /* milliseconds of the last frame */
/*007ef804*/ extern int g_tick_number;                   /* frame counter */
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*0062ac74*/ extern RwV3d g_car_62ac74;                  /* the vertical axis the yaw turns about */

/* One camera record (216 bytes): placement.pos is the camera position, orientation the look matrix built every frame
   from yaw / pitch / roll (degrees) and the matrix translation. */
typedef struct CameraRecord {
    int reserved0;
    int mode;                   /* CAMERA_MODE_* of the record */
    int fixed_orientation;      /* 1: the angles are taken as computed, without the velocity offset and the look-ahead shift */
    RwMatrix placement;
    RwMatrix orientation;
    float velocity[3];
    float reserved98[3];
    float height_target;
    int arrived;
    float pitch;
    float yaw;
    float roll;
    float pitch_velocity;
    float yaw_velocity;
    float turn_limit;           /* angle window (degrees) inside which the swing is damped */
    float distance_param;       /* > 1000: distance mode, else the follow distance; derives turn_limit */
    float blend;
    float reservedcc;
    float rank_from;            /* the camera blends while the followed player's rank key is outside [rank_from, rank_to) */
    float rank_to;
} CameraRecord;

#define CHASE_STEP_PER_MS 0.00033333333f   /* 1 / 3000 */
#define CHASE_DISTANCE_MODE 1000.0f
#define CHASE_MAX_DISTANCE 45.0f
#define CHASE_MIN_DISTANCE 7.0f
#define CHASE_LIMIT_SCALE 25.0f
#define CHASE_LIMIT_MAX 1.0f
#define CHASE_SWING_RATE 7.5f
#define CHASE_DAMPING_RATE 16.0f
#define CHASE_VELOCITY_DAMPING 4.0f
#define CHASE_BLEND_OUT_OF_RANGE 100.0f
#define CHASE_EYE_OFFSET_Y 0.1741f
#define CHASE_EYE_OFFSET_Z 0.2713f
#define RADIANS_TO_DEGREES ((double)57.2958f)
#define DEGREES_FULL 360.0f
#define DEGREES_HALF 180.0f
#define MATRIX_ORTHONORMAL_FLAGS 0x20003     /* rwMATRIXTYPEORTHONORMAL | rwMATRIXINTERNALIDENTITY */
#define COMBINE_POSTCONCAT 2                  /* rwCOMBINEPOSTCONCAT */

/* Heading of (a, b) in degrees (b is the divisor): 90 / 270 when b is 0, atan(a / b) shifted by 180 (and by another 180
   when b > 0), unwrapped. */
static double heading_degrees(double a, float b)
{
    double heading;

    if (b == 0.0f) {
        if (!(a < 0.0f))
            return 90.0f;
        return 270.0f;
    }
    heading = atan(a / b);
    heading = DEGREES_HALF - -(heading * RADIANS_TO_DEGREES);
    if (b > 0.0f)
        heading = heading + DEGREES_HALF;
    return heading;
}

/* Pitch towards (dx, dy, dz): the heading of dy against the horizontal distance. */
static double pitch_degrees(float dx, float dy, float dz)
{
    float horizontal = dx * dx + dz * dz;

    if (horizontal == 0.0f || (horizontal = _rwSqrt(horizontal)) == 0.0f)
        return !(dy < 0.0f) ? 90.0f : 270.0f;
    return heading_degrees(dy, horizontal);
}

/* The x87 keeps float products in extended precision; the original spills (rounds) some of them.  Passing a value
   through this (not inlined) function rounds it to a float at exactly that point. */
static float float_barrier(float value)
{
    return value;
}

/* An angle difference in degrees folded into (-180, 180]. */
static float wrap_degrees(float angle)
{
    while (angle >= DEGREES_FULL)
        angle = angle - DEGREES_FULL;
    while (angle < 0.0f)
        angle = angle + DEGREES_FULL;
    if (angle > DEGREES_HALF)
        angle = -(DEGREES_FULL - angle);
    return angle;
}

/* The look matrix is rebuilt every frame: identity first, then yaw about the vertical axis, pitch about its own right
   vector and roll about its at vector. */
static void camera_orientation_reset(CameraRecord *rec)
{
    RwMatrix *matrix = &rec->orientation;

    matrix->at.z = 1.0f;
    matrix->up.y = 1.0f;
    matrix->right.x = 1.0f;
    matrix->up.x = 0.0f;
    matrix->right.z = 0.0f;
    matrix->right.y = 0.0f;
    matrix->up.z = 0.0f;
    matrix->at.y = 0.0f;
    matrix->at.x = 0.0f;
    matrix->pos.z = 0.0f;
    matrix->pos.y = 0.0f;
    matrix->pos.x = 0.0f;
    matrix->flags = matrix->flags | MATRIX_ORTHONORMAL_FLAGS;
}

/* Rebuilds the look matrix from yaw / pitch / roll and moves it to `translation`. */
static void camera_orientation_apply(CameraRecord *rec, RwV3d *translation)
{
    RwMatrix *matrix = &rec->orientation;

    camera_orientation_reset(rec);
    RwMatrixRotate(matrix, &g_car_62ac74, rec->yaw, COMBINE_POSTCONCAT);
    RwMatrixRotate(matrix, &matrix->right, rec->pitch, COMBINE_POSTCONCAT);
    RwMatrixRotate(matrix, &matrix->at, rec->roll, COMBINE_POSTCONCAT);
    RwMatrixTranslate(matrix, translation, COMBINE_POSTCONCAT);
}

#define FOLLOW_STEP_PER_MS CHASE_STEP_PER_MS

/* Chase camera: follows the target position `view` behind and above it, with smoothed yaw / pitch. */
void camera_chase_update(CameraRecord *rec, int *view)
{
    float step = (float)g_tick_count * CHASE_STEP_PER_MS;
    RwV3d target;
    RwV3d position;
    RwV3d offset_to_camera;
    float dx, dy, dz;
    float distance;
    float limit;
    float swing;
    float damping;
    float wrapped;
    float rank_blend;
    double yaw;
    double pitch;
    RwMatrix *matrix;

    camera_get_target_position(&target, view);
    position.x = rec->placement.pos.x;
    position.y = rec->placement.pos.y;
    position.z = rec->placement.pos.z;
    dx = target.x - position.x;
    dy = target.y - position.y;
    dz = target.z - position.z;
    offset_to_camera.x = position.x - target.x;
    offset_to_camera.y = position.y - target.y;
    offset_to_camera.z = position.z - target.z;
    distance = RwV3dLength(&offset_to_camera);
    if (rec->distance_param <= CHASE_DISTANCE_MODE) {
        float d = CHASE_MAX_DISTANCE - distance;

        if (d < CHASE_MIN_DISTANCE)
            d = CHASE_MIN_DISTANCE;
        rec->turn_limit = d / rec->distance_param;
    } else {
        rec->turn_limit = rec->distance_param - CHASE_DISTANCE_MODE;
    }
    limit = rec->turn_limit * CHASE_LIMIT_SCALE;
    if (limit > CHASE_LIMIT_MAX)
        limit = CHASE_LIMIT_MAX;

    /* yaw towards the target */
    yaw = heading_degrees(dx, dz);
    rec->yaw = (float)yaw;
    swing = step * CHASE_SWING_RATE;
    wrapped = wrap_degrees(0.0f);       /* the original wraps a difference it has just made zero: both swing branches are dead */
    if (wrapped > 0.0f) {
        if (wrapped <= limit)
            rec->yaw_velocity = (1.0f - step * CHASE_DAMPING_RATE) * rec->yaw_velocity - (1.0f - (limit - wrapped) / limit) * swing;
        else
            rec->yaw_velocity = rec->yaw_velocity - swing;
    }
    if (wrapped < 0.0f) {
        if (-limit <= wrapped)
            rec->yaw_velocity = (1.0f - (limit + wrapped) / limit) * swing + (1.0f - step * CHASE_DAMPING_RATE) * rec->yaw_velocity;
        else
            rec->yaw_velocity = swing + rec->yaw_velocity;
    }
    if (rec->fixed_orientation != 1)
        rec->yaw = (float)(yaw + rec->yaw_velocity);
    damping = 1.0f - step * CHASE_VELOCITY_DAMPING;
    rec->yaw_velocity = damping * rec->yaw_velocity;

    /* pitch towards the target */
    pitch = pitch_degrees(dx, dy, dz);
    rec->pitch = (float)(DEGREES_FULL - pitch);
    wrapped = wrap_degrees(0.0f);
    if (wrapped > 0.0f) {
        if (wrapped <= limit)
            rec->pitch_velocity = (1.0f - step * CHASE_DAMPING_RATE) * rec->pitch_velocity - (1.0f - (limit - wrapped) / limit) * swing;
        else
            rec->pitch_velocity = rec->pitch_velocity - swing;
    }
    if (wrapped < 0.0f) {
        if (-limit <= wrapped)
            rec->pitch_velocity = (1.0f - (limit + wrapped) / limit) * swing + (1.0f - step * CHASE_DAMPING_RATE) * rec->pitch_velocity;
        else
            rec->pitch_velocity = swing + rec->pitch_velocity;
    }

    /* blend: 0 while the followed player's rank key lies inside [rank_from, rank_to) (wrapping), else 100 */
    {
        float rank = player_rank_key(view[8]);
        int below_from = !(rank >= rec->rank_from);      /* also true for a NaN rank key */
        int below_to = rank < rec->rank_to;
        int inside;

        if (!(rec->rank_from > rec->rank_to))
            inside = !below_from && below_to;
        else
            inside = !below_from || below_to;
        rank_blend = inside ? 0.0f : CHASE_BLEND_OUT_OF_RANGE;
    }
    if (rec->fixed_orientation != 1)
        rec->pitch = rec->pitch_velocity + rec->pitch;
    rec->pitch_velocity = damping * rec->pitch_velocity;
    if (rank_blend < 0.0f)
        rank_blend = -rank_blend;
    rec->blend = rank_blend;

    matrix = &rec->orientation;
    camera_orientation_reset(rec);
    RwMatrixRotate(matrix, &g_car_62ac74, rec->yaw, COMBINE_POSTCONCAT);
    RwMatrixRotate(matrix, &matrix->right, rec->pitch, COMBINE_POSTCONCAT);
    RwMatrixRotate(matrix, &matrix->at, rec->roll, COMBINE_POSTCONCAT);
    if (rec->fixed_orientation != 1) {
        /* the eye sits a little above and in front of the camera position */
        RwMatrix turn;
        RwV3d eye;

        turn.flags = turn.flags | MATRIX_ORTHONORMAL_FLAGS;
        turn.at.z = 1.0f;
        turn.up.y = 1.0f;
        turn.right.x = 1.0f;
        turn.up.x = 0.0f;
        turn.right.z = 0.0f;
        turn.right.y = 0.0f;
        turn.at.y = 0.0f;
        turn.at.x = 0.0f;
        turn.up.z = 0.0f;
        turn.pos.z = 0.0f;
        turn.pos.y = 0.0f;
        turn.pos.x = 0.0f;
        RwMatrixRotate(&turn, &g_car_62ac74, rec->yaw, COMBINE_POSTCONCAT);
        eye.x = 0.0f;
        eye.y = CHASE_EYE_OFFSET_Y;
        eye.z = CHASE_EYE_OFFSET_Z;
        RwV3dTransformVectors(&eye, &eye, 1, &turn);
        position.x = position.x + eye.x;
        position.y = position.y + eye.y;
        position.z = position.z + eye.z;
    }
    RwMatrixTranslate(matrix, &position, COMBINE_POSTCONCAT);
}

#define FOLLOW_SPEED_SCALE 0.00025f
#define FOLLOW_SWAY_SPEED 0.00006666667f
#define FOLLOW_SWAY_GAIN_UP 1.2f
#define FOLLOW_SWAY_GAIN_DOWN 0.15f
#define FOLLOW_SWAY_REACH 3.75f
#define FOLLOW_CAR_RIGHT_WEIGHT 0.375f
#define FOLLOW_CAR_UP_WEIGHT 0.2f
#define FOLLOW_HEIGHT_TOLERANCE 0.05f
#define FOLLOW_HEIGHT_RATE 20.0f

/* Follow camera of the local player: sits behind the car with a little speed-dependent sway, springs to the car height,
   and keeps a minimum distance. */
void camera_follow_update(CameraRecord *rec, int *view)
{
    float step = (float)g_tick_count * CHASE_STEP_PER_MS;
    float *car;
    float speed;
    double speed_scale;
    double wave;
    float sway;
    double pull_exact;
    float pull;
    RwV3d aim;
    float height_error;
    float height_damping;
    RwV2d to_camera;
    RwV2d direction;
    float distance;
    float push;
    RwV3d move;
    RwV3d look;
    float inertia;
    float brake;
    float gain;
    float wrapped;
    float blend_in;
    double offset;
    float velocity_damping;
    double yaw;
    double pitch;
    double dx_exact;
    float dx, dy, dz;

    g_camera_follow_6a06f4 = g_camera_follow_6a06f4 + (float)g_tick_count;
    car_current_matrix(&car, intro_banner_get_flag());
    car_get_speed(&speed, intro_banner_get_flag());
    speed_scale = speed * FOLLOW_SPEED_SCALE;
    if (speed_scale > 1.0f)
        speed_scale = 1.0f;
    wave = sin(g_camera_follow_6a06f4 * FOLLOW_SWAY_SPEED);
    sway = float_barrier((float)(wave * (wave < 0.0 ? FOLLOW_SWAY_GAIN_DOWN : FOLLOW_SWAY_GAIN_UP) * speed_scale));
    pull_exact = sway * FOLLOW_SWAY_REACH;
    pull = float_barrier(pull_exact);
    /* the aim point: the car position plus 0.375 of its right vector, 0.2 of its up vector and the sway along its at vector
       (move is the offset vector here and the translation below) */
    move.x = car[0] * FOLLOW_CAR_RIGHT_WEIGHT;
    move.y = car[1] * FOLLOW_CAR_RIGHT_WEIGHT;
    move.z = car[2] * FOLLOW_CAR_RIGHT_WEIGHT;
    move.x = float_barrier(car[4] * FOLLOW_CAR_UP_WEIGHT) + move.x;
    move.y = car[5] * FOLLOW_CAR_UP_WEIGHT + move.y;
    move.z = car[6] * FOLLOW_CAR_UP_WEIGHT + move.z;
    move.x = float_barrier(pull_exact * car[8]) + move.x;
    move.y = pull * car[9] + move.y;
    move.z = pull * car[10] + move.z;
    aim.x = car[12] + move.x;
    aim.y = car[13] + move.y;
    aim.z = car[14] + move.z;

    /* vertical spring towards the aim height */
    height_error = (rec->placement.pos.y - aim.y) - 1.0f;
    if (height_error < FOLLOW_HEIGHT_TOLERANCE)
        rec->velocity[1] = step * FOLLOW_HEIGHT_RATE + rec->velocity[1];
    if (FOLLOW_HEIGHT_TOLERANCE < height_error)
        rec->velocity[1] = rec->velocity[1] - step * FOLLOW_HEIGHT_RATE;
    height_damping = 1.0f - step;
    brake = height_damping;
    if (height_error < 1.0f && -1.0f < height_error)
        brake = 1.0f - (step + step);
    if (height_error < 0.25f && -0.25f < height_error)
        brake = 1.0f - step * 8.0f;
    if (height_error < 0.1f && -0.1f < height_error)
        brake = 1.0f - step * 16.0f;
    rec->velocity[1] = brake * rec->velocity[1];

    /* horizontal approach: away from the aim point while it is close, towards it while it is far */
    to_camera.x = rec->placement.pos.x - aim.x;
    to_camera.y = rec->placement.pos.z - aim.z;
    RwV2dNormalize(&direction, &to_camera);
    distance = RwV2dLength(&to_camera);
    push = distance;
    if (rec->arrived == 0) {
        double rate = step * 250.0f;
        float reach_d;

        if (distance < 0.0f)
            rec->arrived = 1;
        if (distance < 0.0f) {
            push = 0.0f;
            reach_d = 0.0f;
        } else {
            reach_d = distance;
            if (10.0f < distance)
                reach_d = 10.0f;
        }
        reach_d = rate * reach_d * 0.1f;
        rec->velocity[0] = rec->velocity[0] - direction.x * reach_d;
        rec->velocity[2] = rec->velocity[2] - direction.y * reach_d;
    }
    if (rec->arrived != 0) {
        double rate = step * 60.0f;
        float away;

        if (10.0f < push)
            rec->arrived = 0;
        away = 7.0f - push;
        if (away < 0.0f)
            away = 0.0f;
        away = rate * away * 0.14285715f;
        rec->velocity[0] = direction.x * away + rec->velocity[0];
        rec->velocity[2] = direction.y * away + rec->velocity[2];
    }
    inertia = step * 5.0f;
    brake = 1.0f - inertia;
    rec->velocity[0] = brake * rec->velocity[0];
    rec->velocity[2] = brake * rec->velocity[2];
    move.x = rec->velocity[0] * step;
    move.y = rec->velocity[1] * step;
    move.z = rec->velocity[2] * step;
    RwMatrixTranslate(&rec->placement, &move, COMBINE_POSTCONCAT);

    /* look at the target */
    camera_get_target_position(&aim, view);       /* the same vector: the aim point is replaced by the target */
    dx_exact = (double)aim.x - (double)rec->placement.pos.x;      /* the yaw uses the unrounded difference */
    dx = (float)dx_exact;
    rec->turn_limit = 0.6f;
    dy = aim.y - rec->placement.pos.y;
    dz = aim.z - rec->placement.pos.z;
    yaw = heading_degrees(dx_exact, dz);
    gain = inertia;
    if (view[0] != 0 && 2.0f < distance)
        gain = step * 10.0f;
    rec->yaw = (float)yaw;
    wrapped = wrap_degrees(0.0f);
    if (wrapped <= 5.0f) {
        if (wrapped < 0.0f)
            wrapped = -wrapped;
        if (wrapped <= 5.0f)
            gain = rec->yaw_velocity * 0.5f;
        else
            gain = gain + rec->yaw_velocity;
    } else {
        gain = rec->yaw_velocity - gain;
    }
    rec->yaw_velocity = gain;
    blend_in = wrapped * 0.2f;
    if (blend_in < 4.0f)
        push = 100.0f;
    else if (blend_in < 10.0f)
        push = float_barrier((10.0f - blend_in) * 16.666666f);
    else
        push = 0.0f;
    look.x = rec->placement.pos.x - aim.x;
    look.y = rec->placement.pos.y - aim.y;
    look.z = rec->placement.pos.z - aim.z;
    offset = RwV3dLength(&look) - rec->height_target;
    if (offset < 0.0f)
        offset = -offset;
    velocity_damping = push - (offset + offset);
    if (velocity_damping < 0.0f)
        velocity_damping = 0.0f;
    rec->blend = velocity_damping;
    rec->yaw = rec->yaw + rec->yaw_velocity;
    velocity_damping = height_damping;
    if (offset < 10.0f)
        velocity_damping = 1.0f - step * 4.0f;
    rec->yaw_velocity = velocity_damping * rec->yaw_velocity;

    pitch = pitch_degrees(dx, dy, dz);
    rec->pitch = (float)(DEGREES_FULL - pitch);
    wrapped = wrap_degrees(0.0f);
    if (wrapped <= 5.0f) {
        if (wrapped < -5.0f) {
            rec->pitch_velocity = inertia + rec->pitch_velocity;
            wrapped = -wrapped;
        } else {
            rec->pitch_velocity = rec->pitch_velocity * 0.5f;
        }
    } else {
        rec->pitch_velocity = rec->pitch_velocity - inertia;
    }
    rec->pitch = (float)((DEGREES_FULL - pitch) + rec->pitch_velocity);
    velocity_damping = height_damping;
    if (wrapped < 10.0f)
        velocity_damping = 1.0f - step * 4.0f;
    rec->pitch_velocity = velocity_damping * rec->pitch_velocity;
    rec->roll = 0.0f;
    camera_orientation_apply(rec, &rec->placement.pos);
}

#define RECORD_NEAR_DISTANCE 7.0f
#define RECORD_HEIGHT_TOLERANCE 0.1f
#define COURSE_CITY 26

/* Record camera (race start / replay): drifts towards `view` with a velocity, keeps a target height, and turns smoothly
   to look at the point. view[0] != 0 selects the second mode (point given by the view's own floats). */
void camera_record_update(CameraRecord *rec, int *view)
{
    float *viewf = (float *)view;
    double step_ms = g_tick_count;
    float step;
    RwV3d aim;
    RwV2d to_camera;
    RwV2d direction;
    RwV3d move;
    RwV3d look;
    float height_error;
    float height_damping;
    double brake;
    float distance;
    float approach;
    float limit;
    float swing;
    float gain;
    float wrapped;
    float blend_in;
    double offset;
    double damping;
    float dxf, dyf, dzf;
    double dx;
    double yaw;
    double pitch;
    double roll;
    double wobble;
    int i;

    if (view[0] != 0) {
        step_ms = float_barrier((float)g_tick_count);      /* the original doubles the rounded float of the tick count */
        step_ms = step_ms + step_ms;
    }
    step = step_ms * CHASE_STEP_PER_MS;
    camera_get_target_position(&aim, view);
    if (view[0] == 0) {
        rec->height_target = 12.0f;
        if (current_course_id() == COURSE_CITY)
            rec->height_target = 20.0f;
    } else {
        float reach;

        aim.x = viewf[1];
        aim.z = viewf[3];
        to_camera.x = rec->placement.pos.x - aim.x;
        to_camera.y = rec->placement.pos.z - aim.z;
        reach = RwV2dLength(&to_camera);
        if (4.0f < reach)
            reach = 4.0f;
        reach = reach * 0.33333334f;
        rec->height_target = viewf[2];
        aim.x = aim.x + reach;
        aim.z = reach + aim.z;
    }
    height_error = rec->placement.pos.y - rec->height_target;
    if (height_error < -RECORD_HEIGHT_TOLERANCE)
        rec->velocity[1] = step * 3.0f + rec->velocity[1];
    if (RECORD_HEIGHT_TOLERANCE < height_error)
        rec->velocity[1] = rec->velocity[1] - step * 3.0f;
    brake = 1.0f - step;
    height_damping = float_barrier((float)brake);      /* the later uses read the rounded copy */
    if (height_error < 1.0f && -1.0f < height_error)
        brake = 1.0f - (step + step);
    rec->velocity[1] = brake * rec->velocity[1];
    to_camera.x = rec->placement.pos.x - aim.x;
    to_camera.y = rec->placement.pos.z - aim.z;
    RwV2dNormalize(&direction, &to_camera);
    distance = RwV2dLength(&to_camera);
    approach = distance;
    if (rec->arrived == 0) {
        float floor_distance = 0.0f;
        float speed_gain;
        float gap;
        float reach;
        double rate;
        float k;

        if (view[0] == 0) {
            floor_distance = RECORD_NEAR_DISTANCE;
            speed_gain = 14.0f;
            if (distance < RECORD_NEAR_DISTANCE)
                rec->arrived = 1;
        } else {
            if (distance < 0.02f)
                menu_set_car_8c7460_and_reset(0);
            for (i = 0; i < 8; i++)
                if (g_player_inputs[i].menu_select != 0 && g_player_inputs_previous[i].menu_select == 0)
                    menu_set_car_8c7460_and_reset(0);
            speed_gain = 6.0f;
        }
        rate = step * speed_gain;
        gap = distance - floor_distance;
        if (view[0] != 0 && gap < 0.75f)
            gap = 0.75f;
        approach = distance;
        if (distance < 0.0f)
            approach = 0.0f;
        reach = 0.0f;
        if (0.0f <= gap) {
            reach = gap;
            if (5.0f < gap)
                reach = 5.0f;
        }
        k = reach * rate * 0.2f;
        rec->velocity[0] = rec->velocity[0] - direction.x * k;
        rec->velocity[2] = rec->velocity[2] - direction.y * k;
    }
    if (rec->arrived != 0) {
        double rate = step * 60.0f;
        float away;

        if (10.0f < approach)
            rec->arrived = 0;
        if (view[0] != 0)
            rec->arrived = 0;
        away = 7.0f - approach;
        if (away < 0.0f)
            away = 0.0f;
        away = rate * away * 0.14285715f;
        rec->velocity[0] = direction.x * away + rec->velocity[0];
        rec->velocity[2] = direction.y * away + rec->velocity[2];
    }
    damping = height_damping;
    if (approach < 10.0f && rec->arrived == 0)
        damping = 1.0f - (step + step);
    rec->velocity[0] = damping * rec->velocity[0];
    rec->velocity[2] = damping * rec->velocity[2];
    move.x = rec->velocity[0] * step;
    move.y = rec->velocity[1] * step;
    move.z = rec->velocity[2] * step;
    RwMatrixTranslate(&rec->placement, &move, COMBINE_POSTCONCAT);
    if (view[0] != 0) {
        camera_get_target_position(&aim, view);
        if (distance < 8.0f) {
            float pull = float_barrier((8.0f - distance) * 0.125f);
            float rise = float_barrier(aim.z - viewf[6]);

            aim.x = aim.x - float_barrier((aim.x - viewf[4]) * pull);
            aim.y = aim.y - (aim.y - viewf[5]) * pull;
            aim.z = aim.z - rise * pull;
        }
    }

    /* look at the point */
    dx = (double)aim.x - (double)rec->placement.pos.x;
    dyf = aim.y - rec->placement.pos.y;
    dzf = aim.z - rec->placement.pos.z;
    wobble = sin((double)((g_tick_number - 0x200) & 0x3ff) * 0.003067871f);
    if (wobble < 0.0)
        wobble = -wobble;
    if (wobble < 0.2f)
        wobble = 0.2f - (0.2f - wobble) * 0.25f;
    rec->turn_limit = (float)(wobble * 0.5f + 0.1f);
    if (view[0] != 0)
        rec->turn_limit = 0.6f;
    limit = rec->turn_limit * 15.0f;
    if (5.0f < limit)
        limit = 5.0f;
    if (view[0] != 0 && distance < 4.0f)
        limit = distance;
    if (dzf == 0.0f)
        yaw = !(dx < 0.0f) ? 90.0f : 270.0f;
    else {
        yaw = atan(dx / dzf);
        yaw = DEGREES_HALF - -(yaw * RADIANS_TO_DEGREES);
        if (0.0f < dzf)
            yaw = yaw + DEGREES_HALF;
    }
    swing = step * 5.0f;
    gain = swing;
    if (view[0] != 0 && 2.0f < distance)
        gain = step * 10.0f;
    rec->yaw = (float)yaw;
    wrapped = wrap_degrees(0.0f);
    if (wrapped <= limit) {
        if (wrapped < 0.0f)
            wrapped = -wrapped;
        if (wrapped <= limit)
            gain = rec->yaw_velocity * 0.5f;
        else
            gain = gain + rec->yaw_velocity;
    } else {
        gain = rec->yaw_velocity - gain;
    }
    rec->yaw_velocity = gain;
    blend_in = wrapped / limit;
    if (blend_in < 4.0f)
        blend_in = 100.0f;
    else if (blend_in < 10.0f)
        blend_in = float_barrier((10.0f - blend_in) * 16.666666f);
    else
        blend_in = 0.0f;
    look.x = rec->placement.pos.x - aim.x;
    look.y = rec->placement.pos.y - aim.y;
    look.z = rec->placement.pos.z - aim.z;
    offset = RwV3dLength(&look) - rec->height_target;
    if (offset < 0.0f)
        offset = -offset;
    damping = blend_in - (offset + offset);
    if (damping < 0.0f)
        damping = 0.0f;
    rec->blend = damping;
    rec->yaw = rec->yaw_velocity + rec->yaw;
    damping = height_damping;
    if (offset < limit + limit)
        damping = 1.0f - step * 4.0f;
    rec->yaw_velocity = damping * rec->yaw_velocity;

    dxf = (float)dx;
    pitch = pitch_degrees(dxf, dyf, dzf);
    rec->pitch = (float)(DEGREES_FULL - pitch);
    wrapped = wrap_degrees(0.0f);
    if (wrapped <= limit) {
        if (wrapped < -limit) {
            rec->pitch_velocity = swing + rec->pitch_velocity;
            wrapped = -wrapped;
        } else {
            rec->pitch_velocity = rec->pitch_velocity * 0.5f;
        }
    } else {
        rec->pitch_velocity = rec->pitch_velocity - swing;
    }
    rec->pitch = (float)((DEGREES_FULL - pitch) + rec->pitch_velocity);
    damping = height_damping;
    if (wrapped < limit + limit)
        damping = 1.0f - step * 4.0f;
    rec->pitch_velocity = damping * rec->pitch_velocity;
    roll = sin((double)(g_tick_number & 0x3ff) * 0.006135742f) * 30.0f;
    if (view[0] != 0 && distance < 4.0f) {
        double lift = (double)distance - 1.0f;

        if (lift < 0.0)
            lift = 0.0f;
        roll = roll * lift * 0.33333334f;
    }
    rec->roll = (float)roll;
    camera_orientation_apply(rec, &rec->placement.pos);
}

