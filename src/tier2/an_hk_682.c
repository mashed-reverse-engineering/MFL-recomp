#include "mfl_tier2.h"
extern char PTR_s_Training1_00607d00[];
extern char PTR_s_Egypt1_00608390[];
extern char PTR_s_Neustein1_006083b8[];
extern char PTR_s_Tierra_PiedraNight1_006083e0[];
extern char PTR_s_Training1_00608228[];
/*0040d110*/ extern undefined __cdecl find_texture_dict_d(RwChar *p0);
/*00434bb0*/ extern int get_event_type();
/*00607adc*/ extern undefined g_menu_texture_607adc;
/*00607ae0*/ extern undefined g_menu_texture_607ae0;
/*0069d688*/ extern undefined4 g_career_row;
/*0069d690*/ extern undefined4 g_menu_event_column;

/* Offsets into the texture name tables (inferred: second and first set of entries), not struct fields */
#define TEXTURE_SET_B_OFFSET (0x3c)
#define TEXTURE_SET_A_OFFSET (0x1e)
/* Event type that picks between the two menu textures (inferred) */
#define EVENT_TYPE_SPLIT_TEXTURE (0x11)
/* menu_texture_dict_load
 * Loads the texture dictionary for one menu entry of the current menu column, via find_texture_dict_d.
 * item_index: index of the entry within the column (inferred), 0 is the first entry.
 * Returns nothing. Column ids are the raw g_menu_event_column values, their meanings are not named yet.
 * (was: "loads the texture dictionary for menu id 3 / 4 / 5", only the first branch matches that)
 */

void menu_texture_dict_load(int item_index)

{
  int event_column;
  int texture_index;
  
  event_column = (int)(g_menu_event_column);
  /* Columns 3, 4, 5: textures come from PTR_s_Training1_00607d00. Column 5 uses TEXTURE_SET_B_OFFSET, column 4 TEXTURE_SET_A_OFFSET, column 3 no offset. */
  if (((g_menu_event_column == 3) || (g_menu_event_column == 4)) || (g_menu_event_column == 5)) {
    texture_index = item_index + g_career_row * 2;
    if (g_menu_event_column == 5) {
      find_texture_dict_d((RwChar *)(((undefined4 *)PTR_s_Training1_00607d00)[item_index + TEXTURE_SET_B_OFFSET +g_career_row * 2]));
      return;
    }
    if (g_menu_event_column == 4) {
      texture_index = item_index + TEXTURE_SET_A_OFFSET +g_career_row * 2;
    }
    find_texture_dict_d((RwChar *)(((undefined4 *)PTR_s_Training1_00607d00)[texture_index]));
    return;
  }
  /* Other columns: if get_event_type() == EVENT_TYPE_SPLIT_TEXTURE, item 0 gets the first texture and the others the second. */
  texture_index = get_event_type();
  if (texture_index == EVENT_TYPE_SPLIT_TEXTURE) {
    if (item_index == 0) {
      find_texture_dict_d(&g_menu_texture_607adc);
      return;
    }
    find_texture_dict_d(&g_menu_texture_607ae0);
    return;
  }
  /* Columns 0xe, 0xf, 0x10: one fixed texture table each, indexed by item_index + g_career_row * 2. */
  texture_index = item_index + g_career_row * 2;
  if (event_column == 0xe) {
    find_texture_dict_d((RwChar *)(((undefined4 *)PTR_s_Egypt1_00608390)[texture_index]));
    return;
  }
  if (event_column == 0xf) {
    find_texture_dict_d((RwChar *)(((undefined4 *)PTR_s_Neustein1_006083b8)[texture_index]));
    return;
  }
  if (event_column == 0x10) {
    find_texture_dict_d((RwChar *)(((undefined4 *)PTR_s_Tierra_PiedraNight1_006083e0)[texture_index]));
    return;
  }
  /* Columns 8, 0xd, 2 (goto LAB_0043ce61): PTR_s_Training1_00608228 with TEXTURE_SET_B_OFFSET. */
  if (event_column == 8) {
LAB_0043ce61:
    find_texture_dict_d((RwChar *)(((undefined4 *)PTR_s_Training1_00608228)[item_index + TEXTURE_SET_B_OFFSET +g_career_row * 2]));
    return;
  }
  /* Columns 1, 7, 0xc: TEXTURE_SET_A_OFFSET. Columns 6, 0xb and all others: no offset (goto LAB_0043ce7f). */
  if (event_column != 7) {
    if (event_column == 6) goto LAB_0043ce7f;
    if (event_column == 0xd) goto LAB_0043ce61;
    if (event_column != 0xc) {
      if (event_column == 0xb) goto LAB_0043ce7f;
      if (event_column == 2) goto LAB_0043ce61;
      if (event_column != 1) goto LAB_0043ce7f;
    }
  }
  texture_index = item_index + TEXTURE_SET_A_OFFSET +g_career_row * 2;
LAB_0043ce7f:
  find_texture_dict_d((RwChar *)(((undefined4 *)PTR_s_Training1_00608228)[texture_index]));
  return;
}



