/* mfl_settings_menu.c: the Preferences page of the Options menu (MFL_FIX_PREFS; new code, not an original function).
 * build_overlay.py compiles this file only when MFL_FIX_PREFS is on, so every other build keeps its object list.
 *
 * The page is an ordinary menu page of the game: an item list in the game's own format in page slot MFL_PAGE_PREFS (0x1b, empty in the
 * original table), opened from a first row "Preferences" added to a copy of the Options page list: a category list (Graphics, Comfort,
 * Cheats, Other), each opening a sub-page of its rows (all lists are built at run time from the original one, so no table of the game
 * is copied here).  The game draws the title, the buttons and the prompts, moves the selection and goes back with Escape, exactly as
 * on its other option pages.  Ours are only:
 *   - the texts: ids from MFL_TEXT_FIRST on, answered by dat_get_string (hook) from the tables below, in the game's language
 *     (g_language_offset; USA = English).  The values On / Off are the game's own texts 0x5d / 0x5c, so they follow its translation.
 *   - mfl_prefs_menu_update (hook in menu_update): Left / Right change the selected row, Return steps it as Right does; the Language
 *     row offers only languages whose speech folder exists (mfl_lang_available); Escape writes mflng_settings.cfg.
 *   - mfl_prefs_menu_draw (hook in hud_menu_draw_all_panels): the value beside each row, with the arrows of the game's own toggle
 *     panels (menu_panel_draw) around the selected one, and on the right a box that explains the selected row and says when it applies
 *     (on the category list: the rows of the selected category).
 *   - mfl_menu_update_scaled (hook in app_state_frontend_frame): menu_speed.
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
/*007ef7ec*/ extern float g_frame_seconds;
/*0076e7f4*/ extern int g_app_state;
/*004463e0*/ extern void menu_update(void);
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
/*004824a0*/ extern int draw_rect(float x, float y, float w, float h, unsigned int colour);      /* 640-space, as frontend_draw's button faces */

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
  T_TITLE, T_DISPLAY, T_LANGUAGE, T_INTRO, T_SAVE_WAIT, T_FLYOVER, T_WINNER_CAM, T_UNLOCK, T_MENU_SPEED, T_RESOLUTION, T_NEXT_START,
  T_AT_ONCE, T_FULLSCREEN, T_WINDOWED, T_BORDERLESS, T_AUTO, T_ENGLISH, T_FRENCH, T_GERMAN, T_SPANISH, T_ITALIAN, T_USA, T_X1, T_X2, T_X4,
  T_CAT_GRAPHICS, T_CAT_COMFORT, T_CAT_CHEATS, T_CAT_OTHER, T_COUNT
};
#define T_LINE0 0x100      /* text ids T_LINE0.. = the word-wrapped lines of the description box (line_buf) */
#define T_MODE_NAME 0x180  /* text id of the Resolution row's value (mode_buf) */
#define LINES 10
#define LINE_CHARS 64

/* en, fr, de, es, it; Latin-1 escapes (the game's strings are UTF-16 with the same code points).  Translations: ours, not the game's
 * (need a native speaker's check); the language names are each language's own name.  Captions <= 22 characters (row scale 0.65). */
static const char *const k_text[T_COUNT][5] = {
  { "Preferences", "Pr\xe9" "f\xe9rences", "Einstellungen", "Preferencias", "Preferenze" },
  { "Display mode", "Mode d'affichage", "Anzeigemodus", "Modo de pantalla", "Modalit\xe0 schermo" },
  { "Language", "Langue", "Sprache", "Idioma", "Lingua" },
  { "Intro movies", "Vid\xe9os d'intro", "Intro-Videos", "V\xed" "deos de intro", "Filmati iniziali" },
  { "Wait after saving", "Attente sauvegarde", "Warten beim Speichern", "Espera al guardar", "Attesa salvataggio" },
  { "Course flyover", "Survol avant course", "Kameraflug vor Rennen", "Vuelo de c\xe1mara", "Volo sul circuito" },
  { "Winner camera spin", "Cam\xe9ra du vainqueur", "Siegerkamera kreist", "Giro al ganador", "Giro sul vincitore" },
  { "Unlock all (cheat)", "Tout ouvrir (triche)", "Alles frei (Cheat)", "Todo abierto (truco)", "Sblocca tutto (trucco)" },
  { "Menu speed", "Vitesse des menus", "Men\xfc-Tempo", "Velocidad de men\xfas", "Velocit\xe0 dei menu" },
  { "Resolution", "R\xe9solution", "Aufl\xf6sung", "Resoluci\xf3n", "Risoluzione" },
  { "Applies at the next start.", "Effet au prochain d\xe9marrage.", "Gilt ab dem n\xe4" "chsten Start.", "Se aplica al reiniciar.",
    "Vale dal prossimo avvio." },
  { "Applies at once.", "Effet imm\xe9" "diat.", "Gilt sofort.", "Se aplica al momento.", "Vale subito." },
  { "Fullscreen", "Plein \xe9" "cran", "Vollbild", "Completa", "Schermo intero" },
  { "Windowed", "Fen\xeatr\xe9", "Fenster", "Ventana", "Finestra" },
  { "Borderless", "Sans bordure", "Randlos", "Sin bordes", "Senza bordi" },
  { "Auto", "Auto", "Automatisch", "Autom\xe1tico", "Automatica" },
  { "English", "English", "English", "English", "English" },
  { "Fran\xe7" "ais", "Fran\xe7" "ais", "Fran\xe7" "ais", "Fran\xe7" "ais", "Fran\xe7" "ais" },
  { "Deutsch", "Deutsch", "Deutsch", "Deutsch", "Deutsch" },
  { "Espa\xf1ol", "Espa\xf1ol", "Espa\xf1ol", "Espa\xf1ol", "Espa\xf1ol" },
  { "Italiano", "Italiano", "Italiano", "Italiano", "Italiano" },
  { "English (US)", "English (US)", "English (US)", "English (US)", "English (US)" },
  { "1x", "1x", "1x", "1x", "1x" },
  { "2x", "2x", "2x", "2x", "2x" },
  { "4x", "4x", "4x", "4x", "4x" },
  { "Graphics", "Graphismes", "Grafik", "Gr\xe1" "ficos", "Grafica" },
  { "Comfort", "Confort", "Komfort", "Comodidad", "Comfort" },
  { "Cheats", "Triches", "Cheats", "Trucos", "Trucchi" },
  { "Other", "Autres", "Sonstiges", "Otros", "Altro" },
};

/* the description box text of each row (same order as the row captions T_DISPLAY..T_RESOLUTION); "Off" words = the game's own Off text */
static const char *const k_desc[T_RESOLUTION - T_DISPLAY + 1][5] = {
  { "How the game is shown. Fullscreen: the video mode chosen in the setup. Windowed: a window on the desktop. "
    "Borderless: a window over the whole screen, quick Alt+Tab.",
    "Comment le jeu s'affiche. Plein \xe9" "cran : le mode vid\xe9o choisi \xe0 l'installation. Fen\xeatr\xe9 : une fen\xeatre "
    "sur le bureau. Sans bordure : une fen\xeatre sur tout l'\xe9" "cran, Alt+Tab rapide.",
    "Wie das Spiel erscheint. Vollbild: der im Setup gew\xe4hlte Videomodus. Fenster: ein Fenster auf dem Desktop. "
    "Randlos: ein Fenster \xfc" "ber den ganzen Bildschirm, schnelles Alt+Tab.",
    "C\xf3mo se muestra el juego. Completa: el modo de v\xed" "deo elegido en la configuraci\xf3n. Ventana: una ventana en el "
    "escritorio. Sin bordes: una ventana en toda la pantalla, Alt+Tab r\xe1pido.",
    "Come appare il gioco. Schermo intero: la modalit\xe0 video scelta nella configurazione. Finestra: una finestra sul desktop. "
    "Senza bordi: una finestra su tutto lo schermo, Alt+Tab rapido." },
  { "Language of the texts and voices. Auto: the Windows language if the game has it, else English. Only installed languages are offered.",
    "Langue des textes et des voix. Auto : la langue de Windows si le jeu l'a, sinon l'anglais. Seules les langues install\xe9" "es sont propos\xe9" "es.",
    "Sprache der Texte und Stimmen. Automatisch: die Windows-Sprache, wenn das Spiel sie hat, sonst Englisch. Nur installierte Sprachen "
    "werden angeboten.",
    "Idioma de los textos y las voces. Autom\xe1tico: el idioma de Windows si el juego lo tiene; si no, ingl\xe9s. Solo se ofrecen los "
    "idiomas instalados.",
    "Lingua dei testi e delle voci. Automatica: la lingua di Windows se il gioco la ha, altrimenti l'inglese. Sono offerte solo le "
    "lingue installate." },
  { "The company logo movies when the game starts. Off: straight to the title screen.",
    "Les vid\xe9os des logos au lancement du jeu. Non : directement \xe0 l'\xe9" "cran titre.",
    "Die Firmenlogo-Videos beim Spielstart. Aus: direkt zum Titelbild.",
    "Los v\xed" "deos de logotipos al iniciar el juego. No: directo a la pantalla de t\xedtulo.",
    "I filmati dei loghi all'avvio del gioco. No: subito alla schermata del titolo." },
  { "The saving and loading messages stay on screen for at least one second. Off: they close as soon as the save is written or loaded.",
    "Les messages de sauvegarde et de chargement restent au moins une seconde. Non : ils se ferment d\xe8s que la sauvegarde est "
    "\xe9" "crite ou charg\xe9" "e.",
    "Die Meldungen beim Speichern und Laden bleiben mindestens eine Sekunde stehen. Aus: sie schlie\xdf" "en, sobald gespeichert oder "
    "geladen ist.",
    "Los mensajes de guardar y cargar se ven al menos un segundo. No: se cierran en cuanto la partida se guarda o se carga.",
    "I messaggi di salvataggio e caricamento restano almeno un secondo. No: si chiudono appena il salvataggio \xe8 scritto o caricato." },
  { "The camera flight over the course before each race (about 7 seconds). Off: the countdown starts at once.",
    "Le vol de la cam\xe9ra au-dessus du circuit avant chaque course (environ 7 secondes). Non : le compte \xe0 rebours commence tout de suite.",
    "Der Kameraflug \xfc" "ber die Strecke vor jedem Rennen (etwa 7 Sekunden). Aus: der Countdown beginnt sofort.",
    "El vuelo de la c\xe1mara sobre el circuito antes de cada carrera (unos 7 segundos). No: la cuenta atr\xe1s empieza enseguida.",
    "Il volo della telecamera sul circuito prima di ogni gara (circa 7 secondi). No: il conto alla rovescia parte subito." },
  { "At the end of a round the camera slowly circles the winning car. Off: it moves in to the car and stays still.",
    "En fin de manche, la cam\xe9ra tourne lentement autour de la voiture gagnante. Non : elle s'approche de la voiture et reste fixe.",
    "Am Ende einer Runde kreist die Kamera langsam um das Siegerauto. Aus: sie f\xe4hrt nur heran und bleibt stehen.",
    "Al final de cada ronda la c\xe1mara gira despacio alrededor del coche ganador. No: se acerca al coche y se queda quieta.",
    "A fine round la telecamera gira lentamente intorno all'auto vincente. No: si avvicina all'auto e resta ferma." },
  { "Cheat: opens all cups, cars and bonus features. Your save keeps its real progress; progress won while this is on is not saved.",
    "Triche : ouvre toutes les coupes, voitures et bonus. La sauvegarde garde ta vraie progression ; ce que tu gagnes pendant ce temps "
    "n'est pas sauvegard\xe9.",
    "Cheat: \xf6" "ffnet alle Cups, Autos und Bonus-Extras. Dein Spielstand beh\xe4lt den echten Fortschritt; was du in dieser Zeit gewinnst, "
    "wird nicht gespeichert.",
    "Truco: abre todas las copas, coches y extras. Tu partida guardada conserva el progreso real; lo que ganes mientras tanto no se guarda.",
    "Trucco: apre tutte le coppe, le auto e i bonus. Il salvataggio tiene i progressi reali; quelli ottenuti nel frattempo non vengono "
    "salvati." },
  { "How fast the menus move: page changes, fades and sliding buttons. The pause menu in a race keeps its normal speed.",
    "Vitesse des menus : changements de page, fondus et boutons qui glissent. Le menu pause en course garde sa vitesse normale.",
    "Wie schnell die Men\xfcs ablaufen: Seitenwechsel, \xdc" "berblendungen und gleitende Schaltfl\xe4" "chen. Das Pausenmen\xfc im Rennen "
    "bleibt normal schnell.",
    "Rapidez de los men\xfas: cambios de p\xe1gina, fundidos y botones que se deslizan. El men\xfa de pausa en carrera mantiene su "
    "velocidad normal.",
    "Velocit\xe0 dei menu: cambi di pagina, dissolvenze e pulsanti che scorrono. Il menu di pausa in gara resta alla velocit\xe0 normale." },
  { "The picture size in windowed and fullscreen mode. Borderless always uses the desktop size.",
    "La taille de l'image en mode fen\xeatr\xe9 et plein \xe9" "cran. Sans bordure utilise toujours la taille du bureau.",
    "Die Bildgr\xf6\xdf" "e im Fenster- und Vollbildmodus. Randlos nutzt immer die Desktopgr\xf6\xdf" "e.",
    "El tama\xf1o de la imagen en ventana y pantalla completa. Sin bordes usa siempre el tama\xf1o del escritorio.",
    "La dimensione dell'immagine in finestra e a schermo intero. Senza bordi usa sempre la dimensione del desktop." },
};

/* The page tree: Options -> Preferences (category list) -> one sub-page per category.  All of them are page id MFL_PAGE_PREFS (the only
 * free slot of g_menu_page_item_lists): a menu record keeps its own item list, so the slot is pointed at the list the next Return will
 * open (sync_slot) and Escape pops back one level through the game's own back path. */
enum category { CAT_GRAPHICS, CAT_COMFORT, CAT_CHEATS, CAT_OTHER, CATS };
static const int k_cat_text[CATS] = { T_CAT_GRAPHICS, T_CAT_COMFORT, T_CAT_CHEATS, T_CAT_OTHER };
#define KEY_RESOLUTION (-1)      /* the Resolution row: values from mfl_video_modes_api (GFX), not a key of mfl_prefs */

/* the rows: category, caption (= description index), key, number of values, first value (language: -1 = auto), shown inverted (the key
 * says "skip", the row "on"), applies at once.  A new row = one line here (+ its texts); a new category = one enum value + its name. */
static const struct { int cat, text, key, count, first, invert, live; } k_rows[] = {
  { CAT_GRAPHICS, T_DISPLAY, MFL_PREF_DISPLAY_MODE, 3, 0, 0, 0 },
  { CAT_GRAPHICS, T_RESOLUTION, KEY_RESOLUTION, 0, 0, 0, 0 },      /* shown only when the video code registered its mode list */
  { CAT_COMFORT, T_INTRO, MFL_PREF_SKIP_INTRO, 2, 0, 1, 0 },
  { CAT_COMFORT, T_FLYOVER, MFL_PREF_SKIP_FLYOVER, 2, 0, 1, 1 },
  { CAT_COMFORT, T_WINNER_CAM, MFL_PREF_WINNER_CAM_ROTATION, 2, 0, 0, 1 },
  { CAT_COMFORT, T_MENU_SPEED, MFL_PREF_MENU_SPEED, 3, 0, 0, 1 },      /* values 1, 2, 4 (speed_index) */
  { CAT_CHEATS, T_UNLOCK, MFL_PREF_UNLOCK_ALL, 2, 0, 0, 0 },
#if MFL_FIX_LANG
  { CAT_OTHER, T_LANGUAGE, MFL_PREF_LANGUAGE, 7, -1, 0, 0 },
#endif
  { CAT_OTHER, T_SAVE_WAIT, MFL_PREF_SKIP_SAVE_WAIT, 2, 0, 1, 1 },
};
#define ROWS ((int)(sizeof k_rows / sizeof k_rows[0]))
/* 640-space layout (the game's buttons are fixed at x 60..270 plus a gradient to 370; the widescreen fix centres this space 4:3) */
#define ROW_SCALE 0.65f     /* our row captions (the game's rows: 0.8) */
#define VALUE_X 300.0f      /* the values, over the buttons' gradient strip */
#define VALUE_SCALE 0.6f
#define BOX_X 424.0f        /* the description box: x 424..632 */
#define BOX_W 208.0f
#define BOX_Y 132.0f
#define BOX_PAD 8.0f
#define BOX_SCALE 0.5f
#define BOX_LINE 16.0f
#define PROMPTS_SELECT_BACK 2    /* the Options page's prompt set */

const struct mfl_video_modes *mfl_video_modes_api;      /* set by the video code (GFX gf3) at start; 0 = no Resolution row */
static unsigned int options_list[64];      /* the Options page list with the Preferences row first */
static unsigned int cat_list[24 + 5 * CATS];
static unsigned int sub_list[CATS][24 + 5 * ROWS];
static int sub_rows[CATS][ROWS], sub_count[CATS];      /* k_rows index of each row of a sub-page */
static int values[MFL_PREF_COUNT];         /* the file's values as the page shows them */
static int installed, dirty;
static unsigned short line_buf[LINES][LINE_CHARS + 1];      /* length-prefixed UTF-16 lines of the description box */
static unsigned short mode_buf[24];         /* the Resolution row's value */

static int text_id(int t) { return MFL_TEXT_FIRST + t; }
static int language(void) { return g_language_offset >= 1 && g_language_offset <= 4 ? g_language_offset : 0; }

const unsigned short *mfl_prefs_text(int id)
{
  static unsigned short ring[4][48];      /* a few strings may be in use at once (draw + width) */
  static int next;
  unsigned short *s = ring[next++ & 3];
  int n = 0;
  const char *c;

  if (id - MFL_TEXT_FIRST == T_MODE_NAME) return mode_buf;
  if (id - MFL_TEXT_FIRST >= T_LINE0) return line_buf[(id - MFL_TEXT_FIRST - T_LINE0) % LINES];
  c = id - MFL_TEXT_FIRST < T_COUNT ? k_text[id - MFL_TEXT_FIRST][language()] : "";
  while (c[n] != '\0' && n < 47) { s[n + 1] = (unsigned char)c[n]; n++; }
  s[0] = (unsigned short)n;      /* length first, as the .dat strings */
  return s;
}

/* menu_speed 1 / 2 / 4 -> 0 / 1 / 2 (3 counts as 2x) */
static int speed_index(int v) { return v >= 4 ? 2 : v >= 2 ? 1 : 0; }

/* menu_speed: app_state_frontend_frame's menu_update call of the frontend (app state 1: title, main menu and its pages) runs with
 * g_frame_seconds x 1 / 2 / 4, so every timed step of menu_update (fades, page slides, item animations, gamesave_update's message
 * timer) runs that much faster; frame-counted steps (slider repeat) and the drawing (credits scroll) are unchanged.  The pause menu
 * in a race (app state 4, its own call) and the race states keep the original time. */
void mfl_menu_update_scaled(void)
{
  float saved = g_frame_seconds;

  if (g_app_state == APP_STATE_MAIN_LOOP_INIT) g_frame_seconds = saved * (float)(1 << speed_index(mfl_pref(MFL_PREF_MENU_SPEED)));
  menu_update();
  g_frame_seconds = saved;
}

static int row_shown(int k)
{
  return k_rows[k].key != KEY_RESOLUTION || (mfl_video_modes_api != 0 && mfl_video_modes_api->count() > 0);
}

/* list = frame (title), prompts, the Options page's layout words (src[4] up to its first row), rows, inputs */
static int list_head(unsigned int *list, int title, int prompts, const unsigned int *src)
{
  int i, n = 0;

  list[n++] = L_FRAME; list[n++] = text_id(title); list[n++] = L_PROMPTS; list[n++] = prompts;
  for (i = 4; src[i] != L_ROW; i++) list[n++] = src[i];
  return n;
}

static void install(void)
{
  const unsigned int *src = (const unsigned int *)g_menu_page_item_lists[MENU_PAGE_OPTIONS];
  unsigned int *l;
  int i = 0, n = 0, c, k;

  installed = 1;
  mfl_pref(0);
  for (k = 0; k < MFL_PREF_COUNT; k++) values[k] = mfl_prefs[k];
  while (src[i] != L_ROW && n < 40) options_list[n++] = src[i++];      /* header, then our row first */
  options_list[n++] = L_ROW; options_list[n++] = text_id(T_TITLE); options_list[n++] = L_PAGE; options_list[n++] = MFL_PAGE_PREFS;
  options_list[n++] = L_ITEM_END;
  while (src[i] != L_END && n < 63) options_list[n++] = src[i++];
  options_list[n] = L_END;
  n = list_head(cat_list, T_TITLE, src[3], src);
  for (c = 0; c < CATS; c++) {
    cat_list[n++] = L_ROW; cat_list[n++] = text_id(k_cat_text[c]); cat_list[n++] = L_PAGE; cat_list[n++] = MFL_PAGE_PREFS;
    cat_list[n++] = L_ITEM_END;
  }
  cat_list[n++] = L_INPUT_BEGIN; cat_list[n++] = L_UP; cat_list[n++] = L_DOWN; cat_list[n++] = L_SELECT; cat_list[n++] = L_BACK;
  cat_list[n++] = L_INPUT_END; cat_list[n] = L_END;
  for (c = 0; c < CATS; c++) {
    l = sub_list[c];
    n = list_head(l, k_cat_text[c], PROMPTS_LEFT_RIGHT, src);
    for (k = 0; k < ROWS; k++) {
      if (k_rows[k].cat != c || !row_shown(k)) continue;
      sub_rows[c][sub_count[c]++] = k;
      l[n++] = L_ROW; l[n++] = text_id(k_rows[k].text); l[n++] = L_ACTION; l[n++] = 0xffffffffu; l[n++] = L_ITEM_END;
    }
    l[n++] = L_INPUT_BEGIN; l[n++] = L_UP; l[n++] = L_DOWN; l[n++] = L_LEFT; l[n++] = L_RIGHT;
    l[n++] = L_SELECT; l[n++] = L_BACK; l[n++] = L_INPUT_END; l[n] = L_END;
  }
  g_menu_page_item_lists[MENU_PAGE_OPTIONS] = (char *)options_list;
  g_menu_page_item_lists[MFL_PAGE_PREFS] = (char *)cat_list;
}

/* menu record rec shows list l (MenuRecord.item_list is a 32-bit address word in mfl_types.h) */
#define SHOWS(rec, l) ((rec)->item_list == (unsigned int)(l))

/* the sub-page shown by the current record, or -1 */
static int current_sub(void)
{
  int c;

  for (c = 0; c < CATS && !SHOWS(&g_menu_records[g_menu_current], sub_list[c]); c++) ;
  return c < CATS ? c : -1;
}

/* the page slot = the list the next Return opens: the selected category's rows on the category list, else the category list */
static void sync_slot(void)
{
  struct MenuRecord *rec = &g_menu_records[g_menu_current];

  g_menu_page_item_lists[MFL_PAGE_PREFS] = (char *)(SHOWS(rec, cat_list) && rec->selected >= 0 && rec->selected < CATS
                                                    ? sub_list[rec->selected] : cat_list);
}

/* the row of button anim it (the text frontend_draw shows for it), or ROWS when it is not one of ours */
static int anim_row(const struct MenuItemAnim *it)
{
  int r, id = it->state == 0 || it->state == 2 ? it->ref_b : it->ref_a;

  for (r = 0; r < ROWS && (it->item_id != (int)L_ROW || id != text_id(k_rows[r].text)); r++) ;
  return r;
}

/* our captions are longer than the game's: smaller text in the same buttons (run before frontend_draw, in menu_update) */
static void row_scales(void)
{
  int i;

  for (i = 0; i < 30; i++)
    if (anim_row(&g_menu_item_anims[i]) < ROWS) g_menu_item_anims[i].scale = ROW_SCALE;
}

static void step(int k, int dir)
{
  int key = k_rows[k].key, n;

  if (key == KEY_RESOLUTION) {
    n = mfl_video_modes_api->count();
    mfl_video_modes_api->choose((mfl_video_modes_api->current() + dir + n) % n);
    return;
  }
  if (key == MFL_PREF_MENU_SPEED) {
    values[key] = 1 << (speed_index(values[key]) + dir + 3) % 3;      /* 1, 2, 4 */
  } else {
    for (n = 0; n < k_rows[k].count; n++) {
      values[key] = k_rows[k].first + (values[key] - k_rows[k].first + dir + k_rows[k].count) % k_rows[k].count;
#if MFL_FIX_LANG
      if (key == MFL_PREF_LANGUAGE && !mfl_lang_available(values[key])) continue;      /* no speech folder: not offered */
#endif
      break;
    }
  }
  if (k_rows[k].live) mfl_prefs[key] = values[key];
}

void mfl_prefs_menu_update(void)
{
  static unsigned int last_list;      /* the current record's list at the previous call (the Return that opened a page is not a step) */
  struct MenuRecord *rec = &g_menu_records[g_menu_current];
  int dir, r, c, was = last_list == rec->item_list;

  if (!installed) install();
  sync_slot();
  row_scales();
  last_list = rec->item_list;
  if (rec->page_id != MFL_PAGE_PREFS || !was) return;
  if (menu_back_pressed_any() && g_menu_select_pressed == 0 && dirty) {      /* the game's back block runs right after this */
    mfl_settings_save(values);
    dirty = 0;
    return;
  }
  c = current_sub();
  if (c < 0) return;      /* the category list: Return opens the slot's sub-page (menu_update) */
  /* Return (menu_update set g_menu_select_pressed this frame; the rows' token 0xffffffff does nothing there) = Right */
  dir = g_menu_select_pressed == 1 ? 1 : (menu_right_pressed_any() != 0) - (menu_left_pressed_any() != 0);
  r = rec->selected;
  if (dir == 0 || r < 0 || r >= sub_count[c]) return;
  step(sub_rows[c][r], dir);
  dirty = 1;
}

static int value_text(int k)
{
  int v = k_rows[k].key >= 0 ? values[k_rows[k].key] : 0, n = 0;
  const char *s;

  if (k_rows[k].key == KEY_RESOLUTION) {      /* "1280x720" from the video code */
    for (s = mfl_video_modes_api->name(mfl_video_modes_api->current()); s[n] != '\0' && n < 23; n++) mode_buf[n + 1] = (unsigned char)s[n];
    mode_buf[0] = (unsigned short)n;
    return text_id(T_MODE_NAME);
  }
  if (k_rows[k].key == MFL_PREF_MENU_SPEED) return text_id(T_X1 + speed_index(v));
  if (k_rows[k].key == MFL_PREF_DISPLAY_MODE) return text_id(T_FULLSCREEN + v);
  if (k_rows[k].key == MFL_PREF_LANGUAGE) return text_id(v < 0 ? T_AUTO : T_ENGLISH + v);
  return (v != 0) != (k_rows[k].invert != 0) ? TEXT_ON : TEXT_OFF;
}

/* the arrows of menu_panel_draw (cl_menu_panel_draw_69ca08.c) left of the value at x - 20 and right after it */
static void arrows(int text, float y_menu)
{
  float sw = (float)screen_width(), sh = (float)screen_height();
  float y = (y_menu - 8.0f) * sh * (1.0f / 480.0f), w = sw * 0.025f, h = sh * (1.0f / 30.0f);

  draw_sprite_uv(find_texture_dict_c("Arrow"), sw * (VALUE_X - 20.0f) * (1.0f / 640.0f), y, w, h, 0xff000000u, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
  draw_sprite(find_texture_dict_c("Arrow"), sw * ((float)(int)text_width(text, VALUE_SCALE) + VALUE_X + 6.0f) * (1.0f / 640.0f), y, w, h, 0xff000000u, 1);
}

/* s word-wrapped into line_buf (greedy, by the game font's width at BOX_SCALE); returns the number of lines */
static int wrap(const char *s, float width)
{
  const char *e, *p;
  unsigned short *b;
  int line, n, len, k;

  for (line = 0; line < LINES; line++) {
    while (*s == ' ') s++;
    if (*s == '\0') break;
    b = line_buf[line];
    n = 0;
    for (;;) {
      for (e = s; *e != '\0' && *e != ' '; e++) ;
      len = n + (n != 0) + (int)(e - s);
      if (len > LINE_CHARS) {
        if (n != 0) break;
        e = s + LINE_CHARS; len = LINE_CHARS;      /* one word longer than a line: cut */
      }
      if (n != 0) b[1 + n] = ' ';      /* char j of the line is b[1 + j] */
      for (k = len - (int)(e - s), p = s; p < e; p++, k++) b[1 + k] = (unsigned char)*p;
      b[0] = (unsigned short)len;
      if (n != 0 && text_width(text_id(T_LINE0 + line), BOX_SCALE) > width) { b[0] = (unsigned short)n; break; }
      n = len;
      s = e;
      if (*s == '\0') break;
      while (*s == ' ') s++;
    }
  }
  return line;
}

/* the box right of the rows (translucent dark panel): a heading, then `lines` text ids from first (step 1) or the given ids, then a footer */
static void box(int heading, int lines, const int *ids, int footer)
{
  float y = BOX_Y + BOX_PAD + BOX_LINE * 0.5f;
  int i;

  draw_rect(BOX_X, BOX_Y, BOX_W, BOX_PAD * 2.0f + BOX_LINE * (float)(lines + (footer >= 0 ? 3 : 1)), 0xb0101010u);
  ui_text_draw_fade_alpha(text_id(heading), BOX_X + BOX_PAD, y, 0xfff0b040u, 0.55f, 0, 0x1ff);
  for (i = 0; i < lines; i++)
    ui_text_draw_fade_alpha(ids[i], BOX_X + BOX_PAD, y + BOX_LINE * (float)(i + 1) + 4.0f, 0xffffffffu, BOX_SCALE, 0, 0x1ff);
  if (footer >= 0)
    ui_text_draw_fade_alpha(text_id(footer), BOX_X + BOX_PAD, y + BOX_LINE * (float)(lines + 2), 0xffa0d0ffu, BOX_SCALE, 0, 0x1ff);
}

void mfl_prefs_menu_draw(void)
{
  struct MenuRecord *rec = &g_menu_records[g_menu_current];
  struct MenuItemAnim *it;
  int i, k, n, ids[LINES], sel = -1, idle = 0, c = installed ? current_sub() : -1;

  if (!installed) return;
  sync_slot();
  for (i = 0; i < 30; i++) {
    it = &g_menu_item_anims[i];
    if (it->item_id == (int)L_ROW && it->state != 0 && it->state != 2) idle = 1;      /* the current page's buttons are in place */
    k = anim_row(it);
    if (k == ROWS) continue;
    ui_text_draw_fade_alpha(value_text(k), VALUE_X, it->y, 0xff000000u, VALUE_SCALE, 0, it->anim_counter);
    if (c >= 0 && it->state != 0 && it->state != 2 && rec->selected >= 0 && rec->selected < sub_count[c] && k == sub_rows[c][rec->selected]) {
      arrows(value_text(k), it->y);
      sel = k;
    }
  }
  if (sel >= 0) {      /* a sub-page row: what it does and when it applies */
    n = wrap(k_desc[k_rows[sel].text - T_DISPLAY][language()], BOX_W - 2.0f * BOX_PAD);
    for (i = 0; i < n; i++) ids[i] = text_id(T_LINE0 + i);
    box(k_rows[sel].text, n, ids, k_rows[sel].live ? T_AT_ONCE : T_NEXT_START);
  } else if (idle && SHOWS(rec, cat_list) && rec->selected >= 0 && rec->selected < CATS) {      /* the category list: its rows */
    for (n = 0; n < sub_count[rec->selected] && n < LINES; n++) ids[n] = text_id(k_rows[sub_rows[rec->selected][n]].text);
    box(k_cat_text[rec->selected], n, ids, -1);
  }
  row_scales();      /* rows that slide in while menu_update's input part does not run (from the next frame on) */
}
#endif
