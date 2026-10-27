#include "mfl_tier2.h"
/*0048ce70*/ extern int lua_run_buffer_ok();
/*004c5490*/ extern RwInt32 RwFclose_00560e70(void *p0);
/*004c54a0*/ extern size_t RwFwrite_00560eb0(void *p0, size_t p1, size_t p2, void *p3);
/*004c54b0*/ extern RwInt32 __cdecl RwFseek_00560f10(void *p0, long p1, RwInt32 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*005e0450*/ extern undefined g_piz_5e0450;
#define PIZ_SCRIPT_BUF_SIZE (0x8000)
#define RW_SEEK_END (2)
/* lua_run_piz_script */
/* compile: /GS */
/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Reads a whole .piz file into a stack buffer and runs its contents as a Lua buffer.
 * piz_path: file name, opened by thunk_FUN_00560d10 with the mode in g_piz_5e0450 (inferred: fopen-style open).
 * lua_state: passed unchanged to lua_run_buffer_ok as its third argument (inferred: Lua state).
 * Returns nothing; lua_run_buffer_ok's result is ignored.
 * No bound check: a file larger than PIZ_SCRIPT_BUF_SIZE bytes overflows script_buf (inferred from the code, not verified in disassembly).
 * (was: "reads the script tail from a piz file and runs it through the Lua state" - it reads the whole file, not a tail)
 */
void __cdecl lua_run_piz_script(undefined4 piz_path,undefined4 lua_state)
{
  void *piz_file;
  size_t script_len;
  undefined1 script_buf [PIZ_SCRIPT_BUF_SIZE];

  /* Zero the whole buffer first so bytes past the script are 0. */
  mem_zero(script_buf,PIZ_SCRIPT_BUF_SIZE);
  piz_file = (void *)thunk_FUN_00560d10(piz_path,&g_piz_5e0450);
  /* Seek to the end, then take the position as the file size (thunk_FUN_00544430, inferred: ftell). */
  RwFseek_00560f10(piz_file,0,RW_SEEK_END);
  script_len = thunk_FUN_00544430(piz_file);
  /* Rewind, then read the whole file into script_buf. RwFwrite_00560eb0 gets (buffer, 1, count, file),
     which is fread's shape: the Ghidra name says Write, but data flows into script_buf (inferred).
     script_len becomes the number of items read. */
  RwFseek_00560f10(piz_file,0,0);
  script_len = RwFwrite_00560eb0(script_buf,1,script_len,piz_file);
  RwFclose_00560e70(piz_file);
  /* Run the buffer as Lua code; script_len is the byte count. */
  lua_run_buffer_ok(script_buf,script_len,lua_state);
  return;
}
