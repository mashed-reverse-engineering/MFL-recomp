# Matching field notes (cl 13.00.9466 /O2, MFL.exe)
Idioms that decided byte matches in `ghidra/recomp`; look here before fiddling (idea: Echo's mutation rules and the byte-tactics "solved patterns" index).
Check a function with `python3 ghidra/tools/cmpfn.py <file.c> <symbol> <VA>`; the output classifies a mismatch (`REGISTERS` = only register names differ, `OPERANDS`, `STRUCTURE`) with a score.

## Compare / branch forms
- Index checks: `if (i >= 0x10) return 0;` (not `> 0xf`); `i >= 4`, `slot >= 0x80`. `i >= 0 && i < N` compiles to `test; jl` (not `-1 < i`).
- Pairs of range checks are `i >= 0 && i < N`; an early-out `if (a < 0 || a >= N) return 0;` gives a different block order (the table load after the `xor eax,eax`).
- `x <= 0` gives `test; setle`, `x < 1` gives `cmp 1; setl`.
- A boolean `return a == b;` as the whole expression becomes `sub; neg; sbb; inc`; inside `&&` it becomes jumps.
- A `switch` with gaps gets an index byte table; if the original has none, it had more case labels (every value of the range).
- `mov eax,[m]; dec eax; jnz` for a test against 1 means the source was a one-case `switch (x) { case 1: ... break; }`, not `if (x == 1)` (which gives `cmp [m],1`).
- `rwCOMBINEREPLACE` is 0 (RwFrameTransform's third argument), render state ids and blend ids are in `rwrs.h`.
## Memory access
- `struct T *p = &arr[i]; p->f` gives `add eax, table` then `[eax+off]`; a direct `arr[i].f` folds the table into the displacement (`[eax+table+off]`). Use the form the original shows.
- A flat `int` array with computed index (`g[v * 0xab + part]`) gives `imul; add; mov [eax*4+table]`.
- A 16-int copy is `rep movs` through a struct copy (`*out = *(struct Mat16 *)src`); 12 bytes (3 ints) are copied through three temporaries when written as a struct assignment.
- Float globals: `g = 0.0f` is an immediate store (`mov [m], 0`), the int form is `xor eax,eax; mov [m], eax`.
- Byte stores: `b | 4` on a `unsigned char` field with int promotion needs `movzx`; write `*(unsigned char*)p = *(unsigned char*)p | 4`.
## Calls
- `RwEngineInstance` is a pointer global; `(*(T**)(RwEngineInstance + off))(...)`: `+0x20` RwRenderStateSet (cdecl, args pushed right to left), `+0x108` alloc, `+0x10c` free, `+0xf4` strlen. `rwrs.h` has `RS_SET`.
- A tail call (`pop esi; jmp f`) appears when the callee has the same or fewer stack args: declare the real parameter list (a hidden argument such as `menu_event_0x11_reset_page(0)`).
- Functions that take arguments in registers (decompiler shows `in_EAX`) cannot be written in plain C; skip them or use `__fastcall` only for ecx/edx.
## Register allocation (the near-miss tail)
- Local declaration order and the order of the first use decide which register a value gets; `int a, b;` vs `int b, a;` is the first thing to try.
- Copy the pointer into a local before use when the original keeps it in a register across a call.
- Loop pointer + counter: `p++` vs `p += n`, `do { } while` (original loops are bottom tested).
- tools/permute.py tries these automatically (operand swaps, comparison flips, declaration order, signedness, `x != 0` forms, `++x`, temporaries).
- A fill loop that writes -1/0 to consecutive words becomes `rep stosd` only if written as `int *p; for (n = N; n != 0; n--) *p++ = value;` (byte or struct-field forms stay loops).
- A table walk with a clear loop inside (`course_destroy_extra_clumps`) wants two pointers: `rec` for the destroy test, `p = rec` for the fill, then `rec = p` (the original keeps esi and edi).
- A pointer into a record plus several field stores (`int *d = (int*)(base + i * stride); d[0] = ..; d[1] = ..`) gives `add eax, base` then `[eax+n]`; indexing the base with the displacement folded does not.

## Named in the struct spec (2026-10-08, from live dumps and the decompilation)
- `Wheel +0x30 in_contact` (non-zero = touching; gates the surface colour effects), `+0x3C spin_rate` (integer factor of the wheel angle update), `+0x88 surface_color` (RGBA of the touched track triangle; -0xff0001 / -0xfcfcfd / -0xff01 / -0xe5cc80 select stop / slow / push / boost; 0xffc88080 on the start grid).
- `Car +0x9C8 up[3]` = (0, 1, 0) in a live race for both cars (dot with a triangle normal > 0.3 in the wheel contact test).
- `Car +0xBFC contact_key[32]`, `+0xC7C contact_valid[32]` (i32[32] each, loop bound 0x20 in `car_wheel_contact_find_index`; `car_set_wheel_contact` writes both). `car_wheel_accessors.c` / `batch22.c` use the names and stay byte identical.
