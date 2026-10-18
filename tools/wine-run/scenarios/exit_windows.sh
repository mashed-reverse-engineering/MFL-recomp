# main menu > Exit To Windows > Yes: the shutdown path (app_shutdown_resources, *_destroy, rw_engine_shutdown...); the process ends, counters.bin stays empty (gdb coverage only)
scenario() { to_single; to_main; exit_windows; }
scenario_footer() { echo "wait 1"; }
