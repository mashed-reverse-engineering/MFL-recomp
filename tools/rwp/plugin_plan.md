
## Plan for lifting the plugin

### Can the plugin be byte-identical? Honest answer: mostly no

The ledger gives the empirical rate for this compiler on this game (tier 1, hand and agent-bytes = byte-identical, over all functions the ledger knows, split by how many x87 instructions the original has):

| x87 instructions in the function | tier 1 / hand (byte-identical) | tier 2 or lower |
|---|---|---|
| 0 | 1737 of 2228 (78%) | 491 |
| 1-9 | 150 of 290 (52%) | 140 |
| 10-49 | 33 of 228 (14%) | 195 |
| 50+ | 6 of 57 (11%) | 51 |

The plugin is the worst case of that table: see the profile above (82 functions / 90 KB (64% of the bytes) sit in the 50+ bucket; 10 KB single function 580740, 5 KB 586f20, 3.5 KB 571c60). x87 byte-identity needs the exact operand order, spill slots, fxch placement and register allocation of an unknown source with unknown inlining, and every miss in a 2-10 KB function is a whole-function miss. Expectation: only the x87-free and few-x87 functions (the bookkeeping / dispatch layer: 190 functions but only 24% of the bytes) are realistic byte targets, plus a handful of small leaf math routines. A byte-identical plugin of 130 KB is not realistic and is not planned. The plugin has no source anywhere (Rwp37Active was never published), so the readability goal is met by tier 2 (readable C, behaviour proven by diffemu), which is what `tools/bytematch_*` already treats as the normal state of x87 code.

### Method per object file (tier 2, readable)

1. Order: lift leaves first (the `Leaf functions` count above), then the per-volume function tables (the 17 vtables), then the callers. Order of files: RwpNull, RwpTriangle, RwpSphere, RwpBox, RwpCylinder, RwpCapsule (volume shape virtuals: small, vtable-reached, easy to diff against the vtable slot signature), RwpMath, RwpGjk, RwpGrid, RwpTrilist, RwpAggregate (+AggChildren, GeneralPrimitive), RwpConvexHullMassProps, RwpFarfield, RwpPipeline, RwpBatch, RwpConstraints last (3 functions are about 30 KB of the 140 KB and all of the solver state).
2. Per function: Ghidra decompile (headless, one file per RCS object, the existing tier-2 chunk format `recomp/automatch_near/an_*.c`), clean into readable C with named struct fields (the volume record, the body record, the constraint row), `__cdecl` unless a register convention is seen (callee list of check_one.py), keep float as float (the code is single precision x87 with 80-bit intermediates: use `float` locals and `double` only where the original stores a double).
3. Proof: `tools/diffemu` per object (one function per case set, the cases seeded from live game state through the existing `seed.py` machinery; the vtable tables give every dispatch target a stable signature). Gate: tier 2 (`gate.sh tier2`) with `diffemu` EQUAL and coverage reported, spill slots compared (no masking, see the diffemu-compares-spills rule).
4. Attempt byte-identity per function only for the x87-free / few-x87 buckets, with the RWP patterns of `public/docs/BYTEMATCH_PATTERNS.md`, one bounded attempt each (the same rule as for the game side: `/O2`, plain, no `/G5`).
5. Boundary: the game side of Rwp (the `rwp_*` wrappers, about 60 entry points) already is tier 1/2 C; the plugin only has to expose the same entry points, so every file can be switched from overlay bytes to C independently (the overlay keeps the not yet lifted objects).

### Work estimate (honest)

About 340 functions, 137 KB. The leaf shape code (Sphere, Box, Cylinder, Capsule, Triangle, Null, Grid, Math, Gjk: about 25 KB) is a few days of work with diffemu. RwpAggregate (20 KB), RwpPipeline (12 KB), RwpBatch (31 KB), RwpConstraints (18 KB, one 10.5 KB function) are the long poles; the 10.5 KB function 580740 will probably need to be split by hand into the solver stages the decompiler shows as loops before it can be named or verified. A realistic result is a fully readable and diffemu-verified plugin in tier 2, with a few percent of byte-identical functions.

### Open questions that need evidence before lifting

- File boundaries come from the RCS id link order and the vtable runs; confidence is low for RwpMgr, RwpDyn, RwpVolume, RwpGeneralPrimitive, RwpAggChildren, RwpGjk. The `game` tag on 56e890-571840 contradicts the vtable data (see the island section) and must be resolved first (check callers of 56f020..571830 from outside the block).
- Slot meaning of the 17 vtables is not named (each volume type has a primary table plus secondary tables 5f6470, 5f6740, 5f6820, 5f69e4, 5f6e24: probably volume vs shape-query vs mass-properties); naming them is the first deliverable of the Sphere/Box lift.
- Mixed `fs`/`fstp` use and the single indirect jump (jump table) in the block still need a look in RwpConstraints/RwpBatch.
