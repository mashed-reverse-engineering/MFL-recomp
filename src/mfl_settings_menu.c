/* mfl_settings_menu.c: the Preferences page of the Options menu (MFL_FIX_PREFS; new code, not an original function).
 * build_overlay.py compiles this file only when MFL_FIX_PREFS is on, so every other build keeps its object list.
 *
 * The page is an ordinary menu page of the game: an item list in the game's own format in page slot MFL_PAGE_PREFS (0x1b, empty in the
 * original table), opened from a fourth row "Preferences" added to a copy of the Options page list (both lists are built at run time
 * from the original one, so no table of the game is copied here).  The game draws the title, the buttons and the prompts, moves the
 * selection and goes back with Escape, exactly as on its other option pages.  Ours are only:
 *   - the texts: ids from MFL_TEXT_FIRST on, answered by dat_get_string (hook) from the table below, in the game's language
 *     (g_language_offset; USA = English).  The values On / Off are the game's own texts 0x5d / 0x5c, so they follow its translation.
 *   - mfl_prefs_menu_update (hook in menu_update): Left / Right change the selected row, Return steps it as Right does; the Language
 *     row offers only languages whose speech folder exists (mfl_lang_available); Escape writes mflng_settings.cfg.
 *   - mfl_prefs_menu_draw (hook in hud_menu_draw_all_panels): the value beside each row, with the arrows of the game's own toggle
 *     panels (menu_panel_draw) around the selected one, and "applies at the next start" under the rows where that is so.
 * Rows whose key is read only at start (display mode, language, intro movies, unlock all) change the file only; the others change
 * mfl_prefs at once too.  unlock_all is never switched live: mfl_unlock_all_force keeps the real save words only once per run. */
#include "mfl_fix.h"
#if MFL_FIX_PREFS      /* (build_overlay.py leaves the file out without the switch; this keeps any other build of it empty) */
#include "mfl_consts.h"
#include "mfl_types.h"

/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069cbd0*/ extern int g_menu_current;
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/*0060aca8*/ extern char *g_menu_page_item_lists[];
/*0076e874*/ extern int g_language_offset;      /* game language id 0..5 (en fr de es it us) */
/*0069d6a8*/ extern int g_menu_select_pressed;
/*004300b0*/ extern int menu_back_pressed_any(void);
/*00430510*/ extern int menu_left_pressed_any(void);
/*00430740*/ extern int menu_right_pressed_any(void);
/*0042c780*/ extern int ui_text_draw_fade_alpha(int id, float x, float y, unsigned int colour, float scale, int param, int counter);
/*0042c8e0*/ extern double text_width(int text_id, float scale);
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*0040d0f0*/ extern void *find_texture_dict_c(const char *name);
/*00483130*/ extern void draw_sprite(void *texture, float x, float y, float width, float height, unsigned int colour, int flags);
/*004832d0*/ extern void draw_sprite_uv(void *texture, float x, float y, float width, float height, unsigned int colour, float z, float u_left, float v_top, float u_right, int a, int b);

/* item list words (the game's format, mfl_tier2.h MENU_ITEM_LIST_* / MENU_INPUT_*) */
#define L_FRAME 0xff000000u      /* + title text id */
#define L_PROMPTS 0xff080000u    /* + prompt set */
#define L_ROW 0xff040000u        /* + text id: a button row */
#define L_ACTION 0xff140000u     /* + token handed to menu_update on Return (0xffffffff: none) */
#define L_PAGE 0xff050000u       /* + page id opened on Return */
#define L_ITEM_END 0xff060000u
#define L_INPUT_BEGIN 0xff090000u
#define L_UP 0xff0b0000u
#define L_DOWN 0xff0c0000u
#define L_LEFT 0xff0d0000u
#define L_RIGHT 0xff0e0000u
#define L_BACK 0xff110000u
#define L_SELECT 0xff100000u
#define L_INPUT_END 0xff0a0000u
#define L_END 0xff070000u
#define PROMPTS_LEFT_RIGHT 4     /* the prompt set of the game's slider / toggle pages (sound, autosave, widescreen) */
#define TEXT_OFF 0x5c            /* the game's "Off" / "On" */
#define TEXT_ON 0x5d

enum text {
  T_TITLE, T_DISPLAY, T_LANGUAGE, T_INTRO, T_SAVE_WAIT, T_FLYOVER, T_WINNER_CAM, T_UNLOCK, T_NEXT_START,
  T_FULLSCREEN, T_WINDOWED, T_BORDERLESS, T_AUTO, T_ENGLISH, T_FRENCH, T_GERMAN, T_SPANISH, T_ITALIAN, T_USA, T_COUNT
};
/* en, fr, de, es, it; Latin-1 escapes (the game's strings are UTF-16 with the same code points).  Translations: ours, not the game's
 * (need a native speaker's check); the language names are each language's own name. */
static const char *const k_text[T_COUNT][5] = {
  { "Preferences", "Pr\xe9" "f\xe9rences", "Einstellungen", "Preferencias", "Preferenze" },
  { "Display", "Affichage", "Anzeige", "Pantalla", "Schermo" },
  { "Language", "Langue", "Sprache", "Idioma", "Lingua" },
  { "Intro movies", "Vid\xe9os d'intro", "Intro-Videos", "V\xed" "deos de intro", "Filmati iniziali" },
  { "Save delay", "Pause sauvegarde", "Speicherpause", "Pausa al guardar", "Pausa salvataggio" },
  { "Course fly-over", "Survol du circuit", "Streckenvorschau", "Vista del circuito", "Volo sul circuito" },
  { "Winner cam spin", "Rotation cam\xe9ra", "Kamerarotation", "Giro de c\xe1mara", "Rotazione camera" },
  { "Unlock all (cheat)", "Tout d\xe9" "bloquer (triche)", "Alles frei (Cheat)", "Desbloquear todo (truco)", "Sblocca tutto (trucco)" },
  { "Applies at the next start", "Au prochain d\xe9marrage", "Ab dem n\xe4" "chsten Start", "Al reiniciar el juego", "Al prossimo avvio" },
  { "Fullscreen", "Plein \xe9" "cran", "Vollbild", "Pantalla completa", "Schermo intero" },
  { "Windowed", "Fen\xea" "tre", "Fenster", "Ventana", "Finestra" },
  { "Borderless", "Sans bordure", "Randlos", "Sin bordes", "Senza bordi" },
  { "Auto", "Auto", "Automatisch", "Autom\xe1tico", "Automatica" },
  { "English", "English", "English", "English", "English" },
  { "Fran\xe7" "ais", "Fran\xe7" "ais", "Fran\xe7" "ais", "Fran\xe7" "ais", "Fran\xe7" "ais" },
  { "Deutsch", "Deutsch", "Deutsch", "Deutsch", "Deutsch" },
  { "Espa\xf1ol", "Espa\xf1ol", "Espa\xf1ol", "Espa\xf1ol", "Espa\xf1ol" },
  { "Italiano", "Italiano", "Italiano", "Italiano", "Italiano" },
  { "English (US)", "English (US)", "English (US)", "English (US)", "English (US)" },
};

/* the rows: key, number of values, first value (language: -1 = auto), shown inverted (the key says "skip", the row "on"), applies at once */
static const struct { int text, key, count, first, invert, live; } k_rows[] = {
  { T_DISPLAY, MFL_PREF_DISPLAY_MODE, 3, 0, 0, 0 },
#if MFL_FIX_LANG
  { T_LANGUAGE, MFL_PREF_LANGUAGE, 7, -1, 0, 0 },
#endif
  { T_INTRO, MFL_PREF_SKIP_INTRO, 2, 0, 1, 0 },
  { T_SAVE_WAIT, MFL_PREF_SKIP_SAVE_WAIT, 2, 0, 1, 1 },
  { T_FLYOVER, MFL_PREF_SKIP_FLYOVER, 2, 0, 1, 1 },
  { T_WINNER_CAM, MFL_PREF_WINNER_CAM_ROTATION, 2, 0, 0, 1 },
  { T_UNLOCK, MFL_PREF_UNLOCK_ALL, 2, 0, 0, 0 },
};
#define ROWS ((int)(sizeof k_rows / sizeof k_rows[0]))
#define VALUE_X 390.0f      /* 640-space column of the values: right of the buttons (60..370), as menu_panel_draw's 356 + 20 */

static unsigned int options_list[64];      /* the Options page list plus the Preferences row */
static unsigned int page_list[24 + 5 * ROWS];
static int values[MFL_PREF_COUNT];         /* the file's values as the page shows them */
static int installed, dirty;

static int text_id(int t) { return MFL_TEXT_FIRST + t; }

const unsigned short *mfl_prefs_text(int id)
{
  static unsigned short ring[4][48];      /* a few strings may be in use at once (draw + width) */
  static int next;
  unsigned short *s = ring[next++ & 3];
  int lang = g_language_offset >= 1 && g_language_offset <= 4 ? g_language_offset : 0, n = 0;
  const char *c = id - MFL_TEXT_FIRST < T_COUNT ? k_text[id - MFL_TEXT_FIRST][lang] : "";

  while (c[n] != '\0' && n < 47) { s[n + 1] = (unsigned char)c[n]; n++; }
  s[0] = (unsigned short)n;      /* length first, as the .dat strings */
  return s;
}

static void install(void)
{
  const unsigned int *src = (const unsigned int *)g_menu_page_item_lists[MENU_PAGE_OPTIONS];
  int i = 0, n = 0, r;

  installed = 1;
  mfl_pref(0);
  for (r = 0; r < MFL_PREF_COUNT; r++) values[r] = mfl_prefs[r];
  while (src[i] != L_INPUT_BEGIN && n < 40) options_list[n++] = src[i++];
  options_list[n++] = L_ROW; options_list[n++] = text_id(T_TITLE); options_list[n++] = L_PAGE; options_list[n++] = MFL_PAGE_PREFS;
  options_list[n++] = L_ITEM_END;
  while (src[i] != L_END && n < 63) options_list[n++] = src[i++];
  options_list[n] = L_END;
  n = 0;
  page_list[n++] = L_FRAME; page_list[n++] = text_id(T_TITLE);
  page_list[n++] = L_PROMPTS; page_list[n++] = PROMPTS_LEFT_RIGHT;
  for (i = 4; src[i] != L_ROW; i++) page_list[n++] = src[i];      /* the layout words of the Options page (0xff020000 / 0xff030000 pairs) */
  for (r = 0; r < ROWS; r++) {
    page_list[n++] = L_ROW; page_list[n++] = text_id(k_rows[r].text); page_list[n++] = L_ACTION; page_list[n++] = 0xffffffffu;
    page_list[n++] = L_ITEM_END;
  }
  page_list[n++] = L_INPUT_BEGIN; page_list[n++] = L_UP; page_list[n++] = L_DOWN; page_list[n++] = L_LEFT; page_list[n++] = L_RIGHT;
  page_list[n++] = L_SELECT; page_list[n++] = L_BACK; page_list[n++] = L_INPUT_END; page_list[n] = L_END;
  g_menu_page_item_lists[MENU_PAGE_OPTIONS] = (char *)options_list;
  g_menu_page_item_lists[MFL_PAGE_PREFS] = (char *)page_list;
}

void mfl_prefs_menu_update(void)
{
  static int was_on_page;      /* this page was the current one at the previous call (the Return that opened it is not a step) */
  struct MenuRecord *rec = &g_menu_records[g_menu_current];
  int dir, key, r, n, was = was_on_page;

  if (!installed) install();
  was_on_page = rec->page_id == MFL_PAGE_PREFS;
  if (!was_on_page || !was) return;
  if (menu_back_pressed_any() && g_menu_select_pressed == 0 && dirty) {      /* the game's back block runs right after this */
    mfl_settings_save(values);
    dirty = 0;
    return;
  }
  /* Return (menu_update set g_menu_select_pressed this frame; the rows' token 0xffffffff does nothing there) = Right */
  dir = g_menu_select_pressed == 1 ? 1 : (menu_right_pressed_any() != 0) - (menu_left_pressed_any() != 0);
  r = rec->selected;
  if (dir == 0 || r < 0 || r >= ROWS) return;
  key = k_rows[r].key;
  for (n = 0; n < k_rows[r].count; n++) {
    values[key] = k_rows[r].first + (values[key] - k_rows[r].first + dir + k_rows[r].count) % k_rows[r].count;
#if MFL_FIX_LANG
    if (key == MFL_PREF_LANGUAGE && !mfl_lang_available(values[key])) continue;      /* no speech folder: not offered */
#endif
    break;
  }
  if (k_rows[r].live) mfl_prefs[key] = values[key];
  dirty = 1;
}

static int value_text(int r)
{
  int v = values[k_rows[r].key];

  if (k_rows[r].key == MFL_PREF_DISPLAY_MODE) return text_id(T_FULLSCREEN + v);
  if (k_rows[r].key == MFL_PREF_LANGUAGE) return text_id(v < 0 ? T_AUTO : T_ENGLISH + v);
  return (v != 0) != (k_rows[r].invert != 0) ? TEXT_ON : TEXT_OFF;
}

/* the arrows of menu_panel_draw (cl_menu_panel_draw_69ca08.c) left of the value at x - 20 and right after it */
static void arrows(int text, float y_menu)
{
  float sw = (float)screen_width(), sh = (float)screen_height();
  float y = (y_menu - 8.0f) * sh * (1.0f / 480.0f), w = sw * 0.025f, h = sh * (1.0f / 30.0f);

  draw_sprite_uv(find_texture_dict_c("Arrow"), sw * (VALUE_X - 20.0f) * (1.0f / 640.0f), y, w, h, 0xff000000u, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
  draw_sprite(find_texture_dict_c("Arrow"), sw * ((float)(int)text_width(text, 0.7f) + VALUE_X + 6.0f) * (1.0f / 640.0f), y, w, h, 0xff000000u, 1);
}

void mfl_prefs_menu_draw(void)
{
  struct MenuItemAnim *it;
  int i, id, r, note = 0, ours = installed && g_menu_records[g_menu_current].page_id == MFL_PAGE_PREFS;
  float last_y = 0.0f;

  for (i = 0; i < 30 && installed; i++) {
    it = &g_menu_item_anims[i];
    if (it->item_id != (int)L_ROW) continue;
    id = it->state == 0 || it->state == 2 ? it->ref_b : it->ref_a;      /* the text frontend_draw shows for this button */
    for (r = 0; r < ROWS && id != text_id(k_rows[r].text); r++) ;
    if (r == ROWS) continue;
    ui_text_draw_fade_alpha(value_text(r), VALUE_X, it->y, 0xff000000u, 0.7f, 0, it->anim_counter);
    if (ours && it->ref_a == id && r == g_menu_records[g_menu_current].selected) {
      arrows(value_text(r), it->y);
      note = !k_rows[r].live;
    }
    if (it->y > last_y) last_y = it->y;
  }
  if (ours && note) {      /* the selected row applies at the next start: say so under the last row */
    ui_text_draw_fade_alpha(text_id(T_NEXT_START), 64.0f, last_y + 34.0f, 0xff000000u, 0.6f, 0, 0x1ff);
  }
}
#endif
