#pragma auto_inline(off)
/* MFL.exe small helpers, batch 4 (cup scene, movie texture, gamesave file io, fx objects, player records). */
extern int game_mode;
extern float k_f_0;
extern float g_course_7ef7cc;
extern int g_cup_64cbe8[];
extern int *g_cup_64d880;
extern int g_cup_scene_5dd380[];
extern int cup_scene_setup_lights(void);
extern void RpClumpRender(int clump);
extern int RpClumpCreate(void);
extern int RwFrameCreate(void);
extern int RpAtomicCreate(void);
extern int RpAtomicSetFrame(int atomic, int frame);
extern int RpAtomicSetGeometry(int atomic, int geometry, int flags);
extern int RpClumpAddAtomic(int clump, int atomic);
extern int RwFrameAddChild(int parent, int child);
extern int RpGeometryDestroy(int geometry);
extern int geometry_build_arrow(void);
extern int geometry_build_pointer(void);
extern volatile int g_attract_timer, g_attract_timer_64d8a8, g_attract_timer_64d8ac, g_scene_64d8b4, g_attract_timer_64d8b8;
extern int video_check_texture_caps(int);
extern int g_movie_64d8c8, *g_app_64d8c0;
extern void RwRasterDestroy(int);
extern int clump_load_by_name(const char *, int, int);
extern int g_crown_64d950;
extern int vehicle_is_active(int);
extern int vehicle_part_ltm(int, int);
extern int RwFrameTransform(int frame, int matrix, int op);
extern int atomic_plugin_get_flags(int);
extern void atomic_plugin_set_flags(int, int);
extern int get_viewport_camera_matrix(int);
extern void RpClumpForAllAtomics(int clump, int (*cb)(int, int), int data);
extern void stream_read_file(void *, void *, int);
extern void stream_write_file(void *, void *, int);
extern char g_gamesave_5dd9b4[], g_savegame[];
extern void gamesave_set_result_code(int);
extern void object_set_pipeline_7c(int);
extern int g_scene_651294[], g_fx_object_count;
extern int g_fx_651260[], g_fx_651298[];
extern void fx_object_timer_subtract_thunk(int, int);
extern char g_fx_651970[];
extern void fx_system_render_no_zwrite(int);
extern int rwp_object_get_matrix(int);
extern int g_player_8d8f74[], g_player_8d8ca0[];
extern char g_fx_651ab0[];

// FUNCTION: MFL 0x00401eb0
void cup_find_entry(int id)
{
    int *p = g_cup_64cbe8;
    int n = 0x1f;
    g_cup_64d880 = g_cup_64cbe8;
    do {
        if (p[0xe] == id)
            g_cup_64d880 = p;
        p += 0x1a;
        n--;
    } while (n != 0);
}

// FUNCTION: MFL 0x00402830
void cup_scene_render_with_overlay(int id)
{
    cup_find_entry(id);
    cup_scene_setup_lights();
    if (*g_cup_64d880 != 0) {
        RpClumpRender(*g_cup_64d880);
        return;
    }
}

// FUNCTION: MFL 0x00402b90
int clump_from_geometry_a(void)
{
    int geometry = geometry_build_arrow();
    int clump = RpClumpCreate();
    int frame = RwFrameCreate();
    int atomic;
    *(int *)(clump + 4) = frame;
    atomic = RpAtomicCreate();
    frame = RwFrameCreate();
    RpAtomicSetFrame(atomic, frame);
    RpAtomicSetGeometry(atomic, geometry, 0);
    RpClumpAddAtomic(clump, atomic);
    RwFrameAddChild(*(int *)(clump + 4), frame);
    RpGeometryDestroy(geometry);
    return clump;
}

// FUNCTION: MFL 0x00402ee0
int clump_from_geometry_b(void)
{
    int geometry = geometry_build_pointer();
    int clump = RpClumpCreate();
    int frame = RwFrameCreate();
    int atomic;
    *(int *)(clump + 4) = frame;
    atomic = RpAtomicCreate();
    frame = RwFrameCreate();
    RpAtomicSetFrame(atomic, frame);
    RpAtomicSetGeometry(atomic, geometry, 0);
    RpClumpAddAtomic(clump, atomic);
    RwFrameAddChild(*(int *)(clump + 4), frame);
    RpGeometryDestroy(geometry);
    return clump;
}

// FUNCTION: MFL 0x00403830
void attract_timer_init(void)
{
    g_attract_timer_64d8a8 = 0;
    g_attract_timer_64d8ac = 0;
    g_attract_timer = 0;
    g_scene_64d8b4 = 0;
    g_attract_timer_64d8b8 = 0;
    g_scene_64d8b4 = 0x42200000;
    g_attract_timer_64d8ac = 0x42700000;
}

// FUNCTION: MFL 0x00403f20
int movie_texture_open(int arg)
{
    g_movie_64d8c8 = video_check_texture_caps(arg);
    return 1;
}

// FUNCTION: MFL 0x00403f40
void movie_texture_release(void)
{
    if (g_app_64d8c0 != 0) {
        (**(void (__stdcall **)(int *))(*g_app_64d8c0 + 8))(g_app_64d8c0);
        g_app_64d8c0 = 0;
    }
    if (g_movie_64d8c8 != 0) {
        RwRasterDestroy(g_movie_64d8c8);
        g_movie_64d8c8 = 0;
    }
}

// FUNCTION: MFL 0x004042a0
void load_bomb_model(void)
{
    g_crown_64d950 = clump_load_by_name("Bomb.dff", 0, 0);
}

// FUNCTION: MFL 0x00405ca0
int atomic_clear_plugin_flag_1(int atomic)
{
    int flags = atomic_plugin_get_flags(atomic);
    if ((flags & 1) != 0)
        flags &= 0xfffffffe;
    atomic_plugin_set_flags(atomic, flags);
    return atomic;
}

// FUNCTION: MFL 0x00405cd0
void clump_clear_atomic_plugin_flag_1(int clump)
{
    int data = get_viewport_camera_matrix(0);
    RpClumpForAllAtomics(clump, atomic_clear_plugin_flag_1, data);
}

// FUNCTION: MFL 0x00405cf0
int gamesave_load(void)
{
    stream_read_file(g_gamesave_5dd9b4, g_savegame, 0x42334);
    gamesave_set_result_code(0);
    return 0;
}

// FUNCTION: MFL 0x00405df0
int gamesave_write(void)
{
    stream_write_file(g_gamesave_5dd9b4, g_savegame, 0x42334);
    gamesave_set_result_code(0);
    return 0;
}

// FUNCTION: MFL 0x004065a0
int object_set_pipeline_7c_forward(int object)
{
    object_set_pipeline_7c(object);
    return object;
}

// FUNCTION: MFL 0x00406d40
int fx_object_timer_subtract_by_id(int id, int amount)
{
    int i = 0;
    int *p;
    if (0 < g_fx_object_count) {
        p = g_scene_651294;
        do {
            if (*p == id) {
                fx_object_timer_subtract_thunk(i, amount);
                return 1;
            }
            i++;
            p += 0x3b;
        } while (i < g_fx_object_count);
    }
    return 0;
}

// FUNCTION: MFL 0x00407590
void fx_render_all(void)
{
    int *p = (int *)g_fx_651970;
    do {
        if (p[-1] != 0) {
            RpClumpRender(p[-4]);
            fx_system_render_no_zwrite(*p);
        }
        p += 8;
    } while ((int)p < ((int)&g_fx_651ab0));
}

// FUNCTION: MFL 0x004089e0
int fx_object_get_rwp_id(int i)
{
    return g_scene_651294[i * 0x3b];
}

struct FxObject { char _pad0[0x10]; int node; char _pad1[0x30]; int owner; int flag; char _pad2[0xec - 0x4c]; };
extern struct FxObject g_fx_objects[]; /* 0x651250, stride 0xEC */

int fx_object_get_sphere(int i, int *out)
{
    struct FxObject *o = &g_fx_objects[i];
    int *pos = (int *)(*(int *)(o->node + 4) + 0x40);
    int z;
    out[0] = pos[0];
    out[1] = pos[1];
    z = pos[2];
    out[3] = 0x40000000;
    out[2] = z;
    return o->flag == 0;
}

// FUNCTION: MFL 0x00408a60
int fx_object_get_position(int i)
{
    int m = rwp_object_get_matrix(g_scene_651294[i * 0x3b]);
    return m + 0x30;
}

// FUNCTION: MFL 0x00408a80
void fx_object_get_matrix_ptr(int *out, int i)
{
    *out = *(int *)(g_fx_651260[i * 0x3b] + 4) + 0x10;
}

// FUNCTION: MFL 0x00408e60
int player_record_get_value(int p, int i)
{
    return g_player_8d8f74[p * 0xc3 + i];
}

// FUNCTION: MFL 0x00408ea0
int player_record_get_8d8ca0(int p)
{
    return *(int *)((char *)g_player_8d8ca0 + p * 0x30c);
}
