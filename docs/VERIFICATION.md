# What "replaced" and "verified" mean here

The private working tree replaces application functions of `MFL.exe` with C that is compiled by the original compiler (MSVC 13.00.9466) and patched back into the executable.
Not all of that source is published; the progress table in the README separates the two numbers:

* **Whole pipeline**: everything the working tree has replaced, in every tier below.
* **Published**: the C source that is in this repository (`src/*.c`, `src/tier2/`, `src/clusters/`).

## Tiers

| Tier | Evidence | Published here |
|---|---|---|
| Tier 1 / automatch / agent | The compiled object is **byte identical** to the original function after relocation (`cmpfn`, strict build with relocation check). | Mostly not: those sources still use the decompiler's type dump, which cannot be published. The hand-written ones are in `src/*.c`. |
| Tier 2 | **Behaviour verified**, bytes differ (other register allocation, other frame layout). Written as readable C against `mfl_tier2.h`. | `src/tier2/` (installed functions whose source needs no unpublished header). |
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

## Limits (honest)

* EQUAL means "no difference found in the sampled cases", not a proof. Paths that the seeded memory never reaches, and loops/traps in the original, are not covered; coverage is reported, not hidden.
* Callees are stubs: a wrong callee is not detected, only the arguments we pass and what we do with the result. Behind-pointer frame content is compared only where a function writes through a pointer argument.
* Frame layout and spill slots differ by construction in tier 2; unread frame words are not compared one by one.
* Some tier 2 rows in the ledger only have `static` or `shadow` authority (checked by the lints and the game gates, not by diffemu). The ledger lists the authority of each function.
* The lints are pattern based; they find the defect classes seen so far and can produce false positives (documented allow-lists) as well as miss new classes.
* The published sources are syntax-checked with the original compiler (`tools/public_syntax.sh`), but a full build and the verification need `MFL.exe`, the toolchain and the private declaration set; they cannot be reproduced from this repository alone.
