# Readability pass for ONE tier 2 function (cheap-model recipe)

You get one C file (`work.c`) with one decompiled function (Ghidra style: `param_1`, `iVar3`, `local_1c`, magic numbers, no comments) plus its headers
(`prelude.h`, `ghidra_types.h`) and a dossier (`DOSSIER.md`: callee prototypes, struct fields, strings). The file compiles to code that is NOT byte identical to the
original game, so the only guard is: **your edit must compile to exactly the same code** (same bytes, same relocation targets) as `before.c`.
Goal: someone can read the file, understand what the function does, and fix a bug in it.

## Do
1. Rename parameters and locals (`param_1`, `iVar3`, `pfVar4`, `local_1c`, `uVar5` ...) to meaningful snake_case names, from how they are used and from the names of the callees they are
   passed to / returned from (`dossier` lists them). Rename all uses (word-boundary edit; never rename inside strings or a longer identifier).
2. One purpose comment in front of the function (what it does, what the parameters mean, what it returns), plus short block comments (`/* ... */`) in front of each loop / major branch.
   Keep the comments the file already has; improve them when wrong. Mark guesses with `(inferred)`.
3. Replace magic numbers by named constants at the TOP of the file, after the includes / externs: `#define NAME (value)` or `enum { NAME = value };` (flags, counts, strides,
   offsets that are not struct fields, sentinel values; NOT 0 / 1 / loop steps). Same value, same type context: write `#define MAX_CARS (16)`, do not add casts or suffixes (`16u`) unless the literal already had it.
4. Where `ghidra_types.h` knows a struct and the code does `*(float *)(param_1 + 0x19c)` / `puVar[7]`, you MAY use the member (`car->body_pos.x`) - only if `readable_check` stays SAME
   (a changed pointer type often changes the code; revert then). Struct fields are in `DOSSIER.md`.
5. A local that is a pure alias may get a more precise TYPE (`undefined4 *` -> `float *`) only if it stays SAME; otherwise keep the type, rename only.

## Never
- Change the structure or ORDER of expressions / statements / declarations, operand order, `if`/loop shape, `&&`/`||` nesting, casts, local variable types or their declaration order
  (order of declarations can decide register allocation), the function signature (name, parameter types and order, calling convention), or any `extern` declaration / global name
  (other files use them). Do not add, remove or merge code, no helper functions, no `static`/`inline`, no inline asm, no `#pragma`.
- Rename a global, a callee, or a struct/typedef name. Rename only parameters and locals (and add new constant names).
- Reformat the whole file (keep Ghidra's layout; whitespace-only changes are fine only inside lines you edit).

## What the check does NOT see (your job)
`SAME` only proves the code is unchanged. It does NOT prove a name is right: `(void *)0x1` is CULL_NONE in one render state call and VERTEXALPHAENABLE=1 in the next, and both
compile identically, so a global replace `(void *)0x1` -> `CULL_NONE` stays SAME and is wrong. Name a constant only from the call/field it appears in (render state id, callee parameter,
struct member), one occurrence group at a time; never replace a number everywhere. Same for renames: a name must say what the value IS, not what it is called in Ghidra.
Other things the dossier gives you: `Known enums` (course / weapon / menu ids: `current_course_id() != 38` -> `COURSE_HIGHWAY`) and `Member offsets` (byte offset of every struct member,
so `*(float *)((char *)&player_car[i] + 0x9c8)` -> `player_car[i].up[0]`; these edits were SAME in the pilot, also retyping a `char *` parameter to `struct Car *` - then
`param + 0x19c` must become `(char *)car + 0x19c`).
Place `#define`s after the last `extern` / `typedef` line above your function (not at the very top: an extern list can follow).

## Check loop (mandatory, after EVERY edit round)
```
python3 tools/readable_check.py before.c work.c
```
`SAME` = keep going. Anything else (`DIFF ...`, compile error) = revert the last edit (or fix the typo), then redo it smaller. Typical causes (pilot): retyped integer parameter/local (`uint` -> `int`: different shifts/compares), a pointer local retyped to another element type, a local renamed to the name of a global it uses (shadows it),
a missing declaration (typo, define placed below its first use), an array member used without index, and a constant that changed type
(`(0x10)` vs unsigned), a struct member access instead of a cast, a retyped local, a renamed name that collides with a global/macro, a `#define` that clashes with a header name.
Work in 3-4 rounds (1: locals/params renames, 2: constants, 3: comments + struct members), checking each; if a round fails twice, drop that round's idea and finish with what was SAME.
Edit only `work.c`. Never touch `before.c`, the headers or other files. One file at a time. When done print one line: `DONE <function> <renames> <constants> <struct-members>`.
