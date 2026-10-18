# developer-only code reached by poking its switch in a running Quick Battle (not a user action): build info overlay pages (g_build_info_69bb30 = 0 1 3 4 -> build_info_draw_page_*, draw_frame)
# and the AI editor (g_player_slots[3].extra = 1 @0x7f0238: ai_editor_update runs the paint / cursor / brush handlers, ai_debug_draw_lines -> collision_debug_draw); keys steer the editor cursor
SCEN_DRIVE=${SCEN_DRIVE-8}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"; echo "waitmem $PHASE 6 90"
  echo "poke 0x7f0238 1"; echo "wait 1.5"; echo "shot editor_on"
  keys 0.4 Left Left Right Up Down Return Return Left Up Return Down Right Return Tab space
  echo "shot editor_keys"
  for p in 1 3 4 0; do echo "poke 0x69bb30 $p"; echo "wait 1.2"; echo "shot build_$p"; done
  race_freeze ${SCEN_FRAMES:-300}
}
