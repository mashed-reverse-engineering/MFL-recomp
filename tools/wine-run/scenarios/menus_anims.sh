# debug: dump g_menu_item_anims (30 * 0x34) at rest in the main menu, in a sub page, and after Escape back
scenario() {
  to_single; to_main; echo "wait 2"; echo "dump anims_main 0x8c80a0 0x618"
  echo "key Return"; echo "wait 2"; echo "dump anims_page 0x8c80a0 0x618"
  echo "key Escape"; echo "wait 1.5"; echo "dump anims_back 0x8c80a0 0x618"
  echo "shot final"
}
