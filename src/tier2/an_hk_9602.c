#include "mfl_tier2.h"
/*00408810*/ extern void fx_objects_reset(void);
/*00408900*/ extern void fx_objects_destroy(void);
/*00421a70*/ extern const char * __cdecl course_table_find_entry(int course);
/*0042f620*/ extern int __cdecl game_content_exists_prefixed(int prefix, const char *name, int flag);
/*0042fa80*/ extern const char * __cdecl course_content_path_build(const char *path);
/*004492b0*/ extern void lua_register_course_functions(void);
/*00464aa0*/ extern void powerups_clear(void);
/*0048cf80*/ extern int __cdecl lua_run_script_file(const char *script, const char *chunk_name, void (*register_functions)(void));
/*004a43d0*/ extern void __cdecl game_open_piz(const char *path);
/*004a4440*/ extern void game_close_piz(void);
/*007d2128*/ extern char *RwEngineInstance;
extern void *memset(void *, int, unsigned int);
/* compile: /GS */

#define COURSE_PATH_SIZE 128 /* both path buffers */

/*
 * course_run_ktc_script: runs the course's optional Lua script "KTCScript.lua" from its .piz archive. Builds
 * "toastart/tracks/<course>.piz", maps it to the content path, opens it as the current archive, and runs the script
 * with the course Lua functions registered when the archive contains one.
 *
 * Frame layout of the original (kept as ONE struct so that lua_run_script_file sees the same memory behind its argument):
 * the chunk name passed to Lua is the address of the 4 bytes IN FRONT of the tracks path, the original never writes
 * them (stack residue of the caller; Lua only uses the name for error messages), so `chunk_name_residue` is left
 * uninitialised on purpose and the call passes its address.
 */
/* TIER2: unvalidated 2026-10-23 authority=diffemu */
void course_run_ktc_script(int course)
{
  struct {
    int chunk_name_residue;                  /* uninitialised, see above */
    char tracks_path[COURSE_PATH_SIZE];      /* "toastart/tracks/<course>.piz" */
    char content_path[COURSE_PATH_SIZE];     /* the path course_content_path_build maps it to */
  } frame;
  const char *course_name;

  memset(frame.tracks_path, 0, COURSE_PATH_SIZE);
  course_name = course_table_find_entry(course);
  memset(frame.content_path, 0, COURSE_PATH_SIZE);
  rwstrcpy(frame.tracks_path, "toastart/tracks/");
  rwstrcat(frame.tracks_path, course_name);
  rwstrcat(frame.tracks_path, ".piz");
  rwstrcpy(frame.content_path, course_content_path_build(frame.tracks_path));
  powerups_clear();
  fx_objects_destroy();
  game_open_piz(frame.content_path);
  if (game_content_exists_prefixed(0, "KTCScript.lua", 1) != 0) {
    lua_run_script_file("KTCScript.lua", (const char *)&frame.chunk_name_residue, lua_register_course_functions);
    fx_objects_reset();
  }
  game_close_piz();
}
