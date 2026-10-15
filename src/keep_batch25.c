/* functions of batch25.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch25.c is kept as it was */
#pragma auto_inline(off)
#include "rwrs.h"
extern int g_load_652dd8, g_effects_652ddc, g_effects_652de0, g_effects_652de4;
extern int RwTexDictionaryFindNamedTexture(int dict, const char *name);
extern void message_show(int, int, int, int, int, int);
extern int g_gamesave_message_8d8bfc;
extern void clump_set_envmap(int, void *);
extern int g_replay_buffer;
extern void course_camera_remove_from_collision_world(int);
extern void RwCameraSetProjection(int, int);
extern void RwCameraSetViewWindow(int, void *);
extern void RwFrameTransform(int, void *, int);
extern int g_camera_652e88;
extern char g_camera_652e8c[], g_camera_652e08[];
extern char g_trail_653258[];
extern void particle_system_setup_ptank(void *, int, int, int);
extern void ptank_set_blend_modes(void *, int, int);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x0040f900
void camera_restore(int camera)
{
    int frame;
    course_camera_remove_from_collision_world(camera);
    frame = *(int *)(camera + 4);
    RwCameraSetProjection(camera, g_camera_652e88);
    RwCameraSetViewWindow(camera, g_camera_652e8c);
    RwFrameTransform(frame, g_camera_652e08, 0);
}
