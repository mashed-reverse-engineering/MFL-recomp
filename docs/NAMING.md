# Naming workflow (MFL.exe)

Naming results are kept as data files in `names/` and re-applied to a Ghidra project with the scripts in `scripts/` (headless):

1. `RenameFns.java names/MFL_*.csv` - function names (and `@label` data labels, `!` comments, see the script header).
2. `ApplyStructs.java names/MFL_structs*.txt` - struct types (e.g. `PlayerStatRecord`, `MenuRecord`, `VehicleRecord`, `MenuItemAnim`, `AiBotRecord`). Apply before the globals.
3. `RenameVars.java names/MFL_vars*.csv` / `MFL_globals*.csv` - globals and locals.

Conventions: `<subsystem>_fn_<addr>` is an auto name for a function not yet understood; `g_<subsystem>_<addr>` for globals; `k_f_*` for float constants.
Names describe what the body does; weaker ones carry a `?` in the CSV comment.

Statically linked libraries are named from their real sources: Lua 4.0 (`MFL_lua40*.csv`), qhull 2003.1 (`MFL_qhull2003*.csv`), and the RenderWare 3.7 / MSVC CRT functions matched by function-ID databases (`lib_renames_applied.csv`).
