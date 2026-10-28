# What "replaced" and "verified" mean here

The private working tree replaces application functions of `MFL.exe` with C that is compiled by the original compiler (MSVC 13.00.9466) and patched back into the executable.
Not all of that source is published; the progress table in the README separates the two numbers:

* **Whole pipeline**: everything the working tree has replaced, in every tier below.
* **Published**: the C source that is in this repository (`src/*.c`, `src/tier1/`, `src/automatch/`, `src/tier2/`, `src/clusters/`). The table counts each function once (a function with both a tier 2 and a cluster source counts once) and only application code (VA < 0x4B0000); published sources of linked library code are listed separately. "Byte identical" published = tier 1 + automatch + hand-written `MATCH`.

## Tiers

| Tier | Evidence | Published here |
|---|---|---|
| Tier 1 / automatch / agent | The compiled object is **byte identical** to the original function after relocation (`cmpfn`, strict build with relocation check). | `src/tier1/`, `src/automatch/` (all of them, written against `mfl_tier2.h`), plus the hand-written ones in `src/*.c`. |
| Tier 2 | **Behaviour verified**, bytes differ (other register allocation, other frame layout). Written as readable C against `mfl_tier2.h`. | `src/tier2/` (installed functions; a few sources that still need the decompiler's type dump stay private). |
| Clusters | A group of functions that pass arguments in registers: natural-C body plus a generated entry thunk, verified together. | `src/clusters/` (`enabled.txt` = installed set; clusters still on the decompiler header are skipped). |

Source files that include anything other than the curated headers (`mfl_tier2.h`, `mfl_consts.h`, `mfl_types.h`, `mfl_externs.h`, `rwrs.h`) are never exported; `tools/export_public.py` enforces this and also refuses game data, binaries, SDK structure text and absolute paths.
Named constants are explained in [ENUMS.md](ENUMS.md).

## How tier 2 and clusters are verified

1. **diffemu** (`tools/diffemu/`): our compiled function and the original bytes of `MFL.exe` run in two Unicorn x86 emulators over identical, lazily seeded memory (file data, random, boundary values, several stack fills).
   Every callee and import is replaced by a stub with a deterministic result; the stub's argument words are recorded. Compared per case: EAX, x87 stack, stack delta, callee-saved registers, all non-stack memory writes, and the ordered call trace including arguments.
   Verdict EQUAL needs at least 20 completed cases and basic-block coverage of at least 0.8 on both sides; otherwise the function is reported as not validated.
2. **Game gates**: a hybrid executable with the replacements is run under Wine through the courses of the game (union sweep, crash checks, shadow sweep). A gate must pass before an install is kept.
3. **Static lints** (`tools/check_all.sh`): `cheatgrep` (no inline asm, pragmas, raw address pointers or byte tables copied from the original), `defectlint` (signedness, float/int bit confusion, wrong prototypes), `declcheck` (call convention and argument counts consistent across our own files), `stack_lint` (frame overflow through callee stores, calls with fewer arguments than the callee reads).
4. **Ledger** (`data/tier_ledger.csv`): per function the strongest evidence (bytes-reloc > diffemu > shadow > static). Evidence for a changed source is stale and drops to static.

### Opt-in harness switches

The default diffemu run is unchanged; these environment switches make it reach more code, and are set per function when the default stubs are the limit. Both sides always see the same inputs.

| Switch | Effect |
|---|---|
| `OUTSTRUCT=1` | Out-structs written by stubs get plausible values (floats the original compares with, zero, colours) instead of a canary pattern. |
| `STDCALL_IND=1` | An indirect call with no `add esp` after it is a stdcall COM method: the stub pops the pushes of that call. |
| `INDARGC="1:3,2:5"` | Exact stdcall argument count per stubbed indirect call (ordinal = Nth such call of the run); needs `STDCALL_IND`. |
| `INDOUT="ord:arg:val"` | The stubbed indirect call with that ordinal stores `val` through its stack argument `arg` (0 = this). |
| `FRETPOOL=1` | Float and zero return values and out-pool values (0, 1, -1, 0.5, ...) for stubs, so compares on callee results see both branches. |
| `MEMMASK="lo-hex-hi-hex,.."` | Address ranges excluded from the memory-write comparison (known uninitialised stack copies). Use sparingly, with a reason. |
| `DIFFEMU_MAXINS=n` | Instruction cap per run (default 300000) for long loops. |

Spill slots and frame words are still compared; nothing is masked to hide a float-spill difference.

### NaN forms (`nancmp`)

An x87 compare is `fnstsw` + `test ah, mask` + a conditional jump, and the mask / jump pair decides what happens for NaN. `nancmp` lists, for the original and for our compiled function, the multiset of behaviours (taken for greater, less, equal, unordered; canonical under operand swap and branch inversion) of every such pair. A different multiset means a compare was written in the wrong form. Differences caused by inlining or loop unrolling by the original compiler are triaged by hand and recorded as artefacts. Because diffemu rarely feeds NaN, this catches what sampling does not.

## Defect classes found by the checks

Real source defects the checks exposed (anonymous, simplified):

1. NaN form. `if (a <= b)` and `if (!(a > b))` differ for NaN. The original takes the "less or equal" branch for NaN, so the source must be the second form:
   `if (!(distance_param > LIMIT)) { ... }`
2. Int as float. A word that holds an id, flag or colour was compared as a float. For some bit patterns (signalling NaN) the compare can never match:
   `if (surface_colour == SURFACE_ARGB_ROAD)` with `int surface_colour`, not `float`.
3. x87 rounding / evaluation count. The original keeps one extended-precision value; computing `now - stored` twice (once to compare, once to add) rounds differently from storing it once:
   `double elapsed = now - stored; if (elapsed > T) total += elapsed;`
4. Emulated status word. C that reproduced FPU status bits by hand (`fnstsw` masks) was replaced with the plain compare it implements: `if (volume > 1.0f) volume = 1.0f;`

## Limits (honest)

* EQUAL means "no difference found in the sampled cases", not a proof. Paths that the seeded memory never reaches, and loops/traps in the original, are not covered; coverage is reported, not hidden.
* Callees are stubs: a wrong callee is not detected, only the arguments we pass and what we do with the result. Behind-pointer frame content is compared only where a function writes through a pointer argument.
* Frame layout and spill slots differ by construction in tier 2; unread frame words are not compared one by one.
* Some tier 2 rows in the ledger only have `static` or `shadow` authority (checked by the lints and the game gates, not by diffemu). The ledger lists the authority of each function.
* The lints are pattern based; they find the defect classes seen so far and can produce false positives (documented allow-lists) as well as miss new classes.
* The published sources are syntax-checked with the original compiler (`tools/public_syntax.sh`), but a full build and the verification need `MFL.exe`, the toolchain and the private declaration set; they cannot be reproduced from this repository alone.
