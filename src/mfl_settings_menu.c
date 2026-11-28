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
 *   - mfl_prefs_menu_update (hook in menu_update): Left / Right change the selected row; the Language row offers only languages whose
 *     speech folder exists (mfl_lang_available).  As on the game's own option pages, Return on a value row accepts (writes
 *     mflng_settings.cfg / videocfg.bin) and goes back one page, Escape gives back the values the page was entered with (snapshot).
 *   - mfl_prefs_menu_draw (hook in hud_menu_draw_all_panels): the value beside each row, with the arrows of the game's own toggle
 *     panels (menu_panel_draw) around the selected one, and on the right a box that explains the selected row and says when it applies
 *     (on the category list: the rows of the selected category).
 *   - mfl_menu_update_scaled (hook in app_state_frontend_frame): menu_speed.
 * Rows whose key is read only at start (display mode, language, intro movies, unlock all) change the file only; the others change
 * mfl_prefs at once too.  The Performance category and the Display adapter row are the original setup dialog's controls: their values stay in
 * videocfg.bin (g_video_cfg, written by the game's own video_cfg_save when the page is left), their captions are the dialog's own strings.
 * unlock_all is never switched live: mfl_unlock_all_force keeps the real save words only once per run.
 *
 * MFL_FIX_CONTROLS: the category Controls.  Its rows show and change the game's own controller table g_controller_cfg[4] of the player
 * chosen in the first row (the race and the menus read that table every frame, so a change is live) and write that player's
 * contcfg<n>.bin at once with the game's own controller_cfg_save_n: the file format stays the original setup dialog's.  Two of its rows
 * open a further page each (the 9 button actions, the 4 directions); Return on an action row waits for the next key or button
 * (capture_run).  mfl_controls_load is the start-up loader with the wider acceptance rule; the dead zone row (on the directions page,
 * beside the live direction preview) is the key pad_deadzone.  mfl_controls_menu_input (hook in controller_poll_player) keeps the
 * Preferences pages steerable from the keyboard and every pad whatever the players' devices are, and the other frontend pages whose
 * input is "any player" from the devices no player has (fallback_page). */
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
/* the setup dialog's own state (clusters/cl_video_config_dialog.c): videocfg.bin as loaded, the adapter and mode lists */
/*007712f8*/ extern struct VideoCfg g_video_cfg;
/*00771500*/ extern int g_app_771500;               /* number of adapters */
/*007712ec*/ extern char *g_video_7712ec;           /* adapter names, 0x50 bytes each */
/*007712e8*/ extern int *g_video_mode_counts;       /* modes per adapter */
/*007714f8*/ extern char **g_video_mode_lists;      /* per adapter, 0x18 bytes per mode: width, height, depth, flags, ... */
/*007e7700*/ extern int g_app_7e7700;               /* resource module handle (the original exe's strings) */
/*0064d8a0*/ extern int g_race_64d8a0;              /* environmental effects off (copied from videocfg.bin once at start) */
/*0064d89c*/ extern int g_round_64d89c;
/*005fb098*/ extern int g_course_5fb098;            /* trilinear (course texture format) */
extern int video_cfg_save(void);                    /* the original's writer: g_video_cfg, 0x200 bytes */
extern __declspec(dllimport) int __stdcall LoadStringA(int module, unsigned int id, char *text, int size);
/*004463e0*/ extern void menu_update(void);
/*00444c10*/ extern void menu_page_enter(int page, int kind);      /* kind 1: back one page (the page argument is not read) */
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
#if MFL_FIX_CONTROLS
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007711c4*/ extern unsigned char g_app_7711c4[];                /* pad button bits of this frame, 4 bytes per pad */
/*0077120c*/ extern float g_app_77120c[];                        /* pad stick x, y (after the dead zone; y > 0 = up), 2 per pad */
/*0077122c*/ extern unsigned char g_app_77122c[];                /* key bits of this frame, by DirectInput key code (32 bytes) */
/*00770f60*/ extern float g_app_770f60;                          /* stick dead zone (joypads_init: 0.35) */
/*007711a4*/ extern char g_controller_dialog_7711a4[];           /* "Keyboard" in the game language (controller_dialog_run) */
/*007711e8*/ extern char g_controller_dialog_7711e8[];           /* "No Controller" */
/*004a48e0*/ extern int app_get_app_770f64(void);                /* number of pads found at start */
/*004a4980*/ extern int controller_dialog_copy_name(int pad, char *name);      /* product name of a found pad */
/*004a6e20*/ extern int controller_cfg_load_n(int slot);         /* contcfg<slot>.bin -> g_controller_cfg[slot]; 0 without a file */
/*004a6ea0*/ extern int controller_cfg_save_n(int slot);
/*004a6f80*/ extern unsigned char controller_action_pressed(int player, int action);      /* the binding of g_controller_cfg[player] on its device, now */
/*0069d1e0*/ extern int g_frontend_state;                        /* 0: no menu on screen */
/*0042bd00*/ extern unsigned char *dat_get_string(int index);    /* a text of the game's own table, in its language */
extern __declspec(dllimport) void *__stdcall GetModuleHandleA(const char *name);
extern __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
typedef int (__stdcall *key_name_fn)(long key, char *text, int size);      /* GetKeyNameTextA (the original exe does not import it) */
#endif

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
  T_TITLE, T_DISPLAY, T_LANGUAGE, T_INTRO, T_SAVE_WAIT, T_FLYOVER, T_WINNER_CAM, T_WINNER_ZOOM, T_UNLOCK, T_MENU_SPEED, T_FPS_CAP, T_VSYNC, T_RESOLUTION,
  T_ADAPTER, T_SHADOWS, T_MULTITEX, T_TRILINEAR, T_ENV,
  T_CTL_PLAYER, T_CTL_DEVICE, T_CTL_BUTTONS, T_CTL_DIRECTIONS, T_CTL_RESET, T_DEADZONE, T_MUTE,      /* the rows' captions, in the order of k_desc */
  T_NEXT_START, T_AT_ONCE, T_NEXT_RACE, T_RESTART, T_FULLSCREEN, T_WINDOWED, T_BORDERLESS, T_AUTO, T_ENGLISH, T_FRENCH, T_GERMAN, T_SPANISH, T_ITALIAN, T_USA, T_X1, T_X2, T_X4,
  T_FPS_60, T_FPS_30, T_FPS_UNLIMITED,
  T_CAT_GRAPHICS, T_CAT_COMFORT, T_CAT_CHEATS, T_CAT_OTHER, T_CAT_PERF, T_CAT_CONTROLS,
  T_ACTION,      /* the 13 controller actions, in the order of ControllerCfg.bindings */
  T_PRESS = T_ACTION + 13, T_STICK, T_BUTTON, T_COUNT
};
#define T_LINE0 0x100      /* text ids T_LINE0.. = the word-wrapped lines of the description box (line_buf) */
#define T_ROW_VALUE 0x200  /* text ids T_ROW_VALUE + row = the value of a Controls row (row_buf) */
#define T_MODE_NAME 0x180  /* text id of the Resolution row's value (mode_buf) */
#define T_ADAPTER_NAME 0x181  /* the Display adapter row's value (adapter_buf) */
#define T_PERCENT 0x184    /* text ids T_PERCENT + direction = the numbers in the direction preview (percent_buf) */
#define LINES 16
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
  { "Zoom to winner", "Zoom sur le vainqueur", "Zoom auf Sieger", "Zoom al ganador", "Zoom sul vincitore" },      /* ours, NEED A NATIVE SPEAKER'S CHECK */
  { "Unlock all (cheat)", "Tout ouvrir (triche)", "Alles frei (Cheat)", "Todo abierto (truco)", "Sblocca tutto (trucco)" },
  { "Menu speed", "Vitesse des menus", "Men\xfc-Tempo", "Velocidad de men\xfas", "Velocit\xe0 dei menu" },
  { "Frame rate limit", "Limite d'images", "Bildratenlimit", "L\xedmite de FPS", "Limite FPS" },
  { "Vsync", "Synchro verticale", "VSync", "Sincronizaci\xf3n vertical", "Sincronia verticale" },
  { "Resolution", "R\xe9solution", "Aufl\xf6sung", "Resoluci\xf3n", "Risoluzione" },
  { "Display adapter", "Carte graphique", "Grafikkarte", "Tarjeta gr\xe1" "fica", "Scheda video" },
  /* the next four are the original dialog's own strings at run time (k_orig); these are only the fallback when the resource is missing or too long */
  { "Dynamic shadows", "Ombres dynamiques", "Dyn. Schatten", "Sombras", "Ombre" },
  { "Multi-texturing", "Multi-texturage", "Multi-Texturing", "Multitexturas", "Multitexture" },
  { "Trilinear filtering", "Filtrage trilin\xe9" "aire", "Trilinearer Filter", "Filtrado trilineal", "Filtro trilineare" },
  { "Environment effects", "Effets du d\xe9" "cor", "Umgebungseffekte", "Efectos del entorno", "Effetti ambiente" },
  /* MFL_FIX_CONTROLS.  French, German, Spanish, Italian: ours, NEED A NATIVE SPEAKER'S CHECK.  "Player" is the game's own text at run time (k_game_text) */
  { "Player", "Joueur", "Spieler", "Jugador", "Giocatore" },
  { "Device", "P\xe9riph\xe9rique", "Ger\xe4t", "Dispositivo", "Dispositivo" },
  { "Buttons", "Boutons", "Tasten", "Botones", "Pulsanti" },
  { "Direction keys", "Touches de direction", "Richtungstasten", "Teclas de direcci\xf3n", "Tasti direzionali" },
  { "Reset to defaults", "R\xe9initialiser", "Zur\xfc" "cksetzen", "Restablecer", "Ripristina" },
  { "Stick dead zone", "Zone morte du stick", "Stick-Totzone", "Zona muerta del stick", "Zona morta dello stick" },
  { "Mute in background", "Muet en arri\xe8re-plan", "Stumm im Hintergrund", "Mudo en segundo plano", "Muto in secondo piano" },      /* ours, NEED A NATIVE SPEAKER'S CHECK */
  { "Applies at the next start.", "Effet au prochain d\xe9marrage.", "Gilt ab dem n\xe4" "chsten Start.", "Se aplica al reiniciar.",
    "Vale dal prossimo avvio." },
  { "Applies at once.", "Effet imm\xe9" "diat.", "Gilt sofort.", "Se aplica al momento.", "Vale subito." },
  { "Applies from the next race.", "Effet d\xe8s la prochaine course.", "Gilt ab dem n\xe4" "chsten Rennen.", "Se aplica desde la pr\xf3xima carrera.",
    "Vale dalla prossima gara." },
  /* shown on every Preferences page while a changed row waits for the next start.  Translations: ours (need a native speaker's check) */
  { "Restart the game to apply.", "Red\xe9marre le jeu pour appliquer.", "Zum \xdc" "bernehmen Spiel neu starten.", "Reinicia el juego para aplicar.",
    "Riavvia il gioco per applicare." },
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
  { "60", "60", "60", "60", "60" },
  { "30", "30", "30", "30", "30" },
  { "Unlimited", "Illimit\xe9", "Unbegrenzt", "Sin l\xedmite", "Illimitato" },
  { "Graphics", "Graphismes", "Grafik", "Gr\xe1" "ficos", "Grafica" },
  { "Comfort", "Confort", "Komfort", "Comodidad", "Comfort" },
  { "Cheats", "Triches", "Cheats", "Trucos", "Trucchi" },
  { "Other", "Autres", "Sonstiges", "Otros", "Altro" },
  { "Performance", "Performances", "Leistung", "Rendimiento", "Prestazioni" },      /* the original's string 2240 is used when it is short enough (k_orig) */
  /* MFL_FIX_CONTROLS.  French, German, Spanish, Italian: ours, NEED A NATIVE SPEAKER'S CHECK.  Where the game has the text (k_game_text:
   * Controllers, Accelerate, Brake, Fire weapon, Drop weapon, Start/Pause, Insults) its own is shown and these are the fallback */
  { "Controls", "Commandes", "Steuerung", "Controles", "Comandi" },
  { "Accelerate", "Acc\xe9l\xe9rer", "Gas geben", "Acelerar", "Accelera" },
  { "Brake", "Freiner", "Bremsen", "Frenar", "Frena" },
  { "Fire weapon", "Tirer", "Waffe abfeuern", "Disparar arma", "Usa arma" },
  { "Drop weapon", "L\xe2" "cher l'arme", "Waffe abwerfen", "Soltar arma", "Lascia arma" },
  { "Menu: select", "Menu : valider", "Men\xfc: Ausw\xe4hlen", "Men\xfa: aceptar", "Menu: conferma" },
  { "Menu: back", "Menu : retour", "Men\xfc: Zur\xfc" "ck", "Men\xfa: atr\xe1s", "Menu: indietro" },
  { "Pause", "Pause", "Pause", "Pausa", "Pausa" },
  { "Menu: next", "Menu : suivant", "Men\xfc: Weiter", "Men\xfa: siguiente", "Menu: avanti" },
  { "Insult", "Insulte", "Beleidigung", "Insulto", "Insulto" },
  { "Left", "Gauche", "Links", "Izquierda", "Sinistra" },
  { "Right", "Droite", "Rechts", "Derecha", "Destra" },
  { "Up", "Haut", "Oben", "Arriba", "Su" },
  { "Down", "Bas", "Unten", "Abajo", "Gi\xf9" },
  { "Press a key or button", "Appuie sur une touche", "Taste oder Knopf dr\xfc" "cken", "Pulsa una tecla o bot\xf3n", "Premi un tasto" },
  { "Stick / D-pad", "Stick / croix", "Stick / Steuerkreuz", "Stick / cruceta", "Stick / croce" },
  { "Button", "Bouton", "Knopf", "Bot\xf3n", "Pulsante" },      /* + the button's number as Windows counts it (from 1) */
};

/* the description box text of each row (same order as the row captions T_DISPLAY..T_RESOLUTION); "Off" words = the game's own Off text */
static const char *const k_desc[T_MUTE - T_DISPLAY + 1][5] = {
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
  { "At the end of a round the camera moves in to the winning car. Off: the race view stays, the round goes on as usual.",
    "En fin de manche, la cam\xe9ra s'approche de la voiture gagnante. Non : la vue de course reste, la manche continue normalement.",
    "Am Ende einer Runde f\xe4hrt die Kamera an das Siegerauto heran. Aus: die Rennansicht bleibt, die Runde l\xe4uft normal weiter.",
    "Al final de cada ronda la c\xe1mara se acerca al coche ganador. No: se queda la vista de carrera y la ronda sigue igual.",
    "A fine round la telecamera si avvicina all'auto vincente. No: resta la vista di gara, il round prosegue come sempre." },
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
  { "The most pictures per second. The game runs at the same speed at every frame rate.",
    "Le nombre maximal d'images par seconde. Le jeu garde la m\xeame vitesse \xe0 chaque fr\xe9quence d'images.",
    "Die h\xf6" "chste Zahl an Bildern pro Sekunde. Das Spiel l\xe4uft bei jeder Bildrate gleich schnell.",
    "El m\xe1ximo de im\xe1genes por segundo. El juego va a la misma velocidad con cualquier tasa de im\xe1genes.",
    "Il massimo di immagini al secondo. Il gioco va alla stessa velocit\xe0 con ogni frequenza di immagini." },
  { "Each picture waits for the screen's refresh: no tearing. Off: the picture can tear, the frame rate limit alone sets the rate.",
    "Chaque image attend le rafra\xee" "chissement de l'\xe9" "cran : pas de d\xe9" "chirure. Non : l'image peut se d\xe9" "chirer, seule la "
    "limite d'images fixe la cadence.",
    "Jedes Bild wartet auf den Bildaufbau des Monitors: kein Zerrei\xdf" "en. Aus: das Bild kann zerrei\xdf" "en, nur das Bildratenlimit "
    "bestimmt das Tempo.",
    "Cada imagen espera al refresco de la pantalla: sin cortes. No: la imagen puede cortarse, solo el l\xedmite de FPS fija la cadencia.",
    "Ogni immagine attende l'aggiornamento dello schermo: niente strappi. No: l'immagine pu\xf2 strapparsi, solo il limite FPS decide il "
    "ritmo." },
  { "The picture size in windowed and fullscreen mode. Borderless always uses the desktop size.",
    "La taille de l'image en mode fen\xeatr\xe9 et plein \xe9" "cran. Sans bordure utilise toujours la taille du bureau.",
    "Die Bildgr\xf6\xdf" "e im Fenster- und Vollbildmodus. Randlos nutzt immer die Desktopgr\xf6\xdf" "e.",
    "El tama\xf1o de la imagen en ventana y pantalla completa. Sin bordes usa siempre el tama\xf1o del escritorio.",
    "La dimensione dell'immagine in finestra e a schermo intero. Senza bordi usa sempre la dimensione del desktop." },
  /* the video options of videocfg.bin (the original setup dialog's boxes).  Translations: ours (need a native speaker's check) */
  { "The graphics card the game uses. Listed only when the PC has more than one. The mode is reset to a full-screen mode of that card.",
    "La carte graphique utilis\xe9" "e par le jeu. Propos\xe9" "e seulement s'il y en a plusieurs. Le mode repasse \xe0 un mode plein \xe9" "cran de cette carte.",
    "Die vom Spiel benutzte Grafikkarte. Nur sichtbar, wenn der PC mehrere hat. Der Modus wird auf einen Vollbildmodus dieser Karte zur\xfc" "ckgesetzt.",
    "La tarjeta gr\xe1" "fica que usa el juego. Solo aparece si el PC tiene varias. El modo vuelve a uno de pantalla completa de esa tarjeta.",
    "La scheda video usata dal gioco. Compare solo se il PC ne ha pi\xf9 di una. La modalit\xe0 torna a una a schermo intero di quella scheda." },
  { "The shadows under the cars. Off: no shadows, a little faster on old graphics cards.",
    "Les ombres sous les voitures. Non : pas d'ombres, un peu plus rapide sur les anciennes cartes graphiques.",
    "Die Schatten unter den Autos. Aus: keine Schatten, auf alten Grafikkarten etwas schneller.",
    "Las sombras bajo los coches. No: sin sombras, algo m\xe1s r\xe1pido en tarjetas gr\xe1" "ficas antiguas.",
    "Le ombre sotto le auto. No: niente ombre, un po' pi\xf9 veloce sulle vecchie schede video." },
  { "Extra texture layers (shine and detail) on the course and the cars. Off: plain textures, a little faster on old graphics cards.",
    "Couches de texture suppl\xe9" "mentaires (reflets et d\xe9" "tails) sur le circuit et les voitures. Non : textures simples, un peu plus rapide sur "
    "les anciennes cartes.",
    "Zus\xe4" "tzliche Texturebenen (Glanz und Details) auf Strecke und Autos. Aus: einfache Texturen, auf alten Grafikkarten etwas schneller.",
    "Capas de textura extra (brillo y detalle) en el circuito y los coches. No: texturas simples, algo m\xe1s r\xe1pido en tarjetas gr\xe1" "ficas antiguas.",
    "Livelli di texture aggiuntivi (lucentezza e dettagli) su pista e auto. No: texture semplici, un po' pi\xf9 veloce sulle vecchie schede video." },
  { "How the course textures are stored and filtered. It is the Trilinear Filtering box of the original setup program.",
    "Comment les textures du circuit sont stock\xe9" "es et filtr\xe9" "es. C'est la case Filtrage trilin\xe9" "aire du programme d'installation d'origine.",
    "Wie die Streckentexturen gespeichert und gefiltert werden. Entspricht dem Kontrollk\xe4" "stchen Trilinearer Filter des urspr\xfc" "nglichen Setups.",
    "C\xf3mo se guardan y filtran las texturas del circuito. Es la casilla Filtrado trilineal del programa de instalaci\xf3n original.",
    "Come vengono memorizzate e filtrate le texture della pista. \xc8 la casella Filtering trilineare del programma di installazione originale." },
  { "Environmental effects such as boost flames and wheel marks. Off: fewer effects.",
    "Effets d'environnement comme les flammes du turbo et les traces de roues. Non : moins d'effets.",
    "Umgebungseffekte wie Boost-Flammen und Radspuren. Aus: weniger Effekte.",
    "Efectos ambientales como las llamas del turbo y las marcas de las ruedas. No: menos efectos.",
    "Effetti ambientali come le fiamme del turbo e i segni delle ruote. No: meno effetti." },
  /* MFL_FIX_CONTROLS: Player, Device, Buttons (also shown on each of its action rows), Direction keys (the same), Reset, Stick dead
   * zone.  French, German, Spanish, Italian: ours, NEED A NATIVE SPEAKER'S CHECK.  No percent sign: the game's texts never draw one */
  { "The player whose controls the rows below show and change.",
    "Le joueur dont les lignes ci-dessous montrent et changent les commandes.",
    "Der Spieler, dessen Steuerung die Zeilen darunter zeigen und \xe4ndern.",
    "El jugador cuyos controles muestran y cambian las filas de abajo.",
    "Il giocatore di cui le righe sotto mostrano e cambiano i comandi." },
  { "What this player plays with: nothing, the keyboard or a connected gamepad. A new device starts with its default keys or buttons. "
    "Gamepads are found when the game starts.",
    "Avec quoi ce joueur joue : rien, le clavier ou une manette branch\xe9" "e. Un nouveau p\xe9riph\xe9rique commence avec ses touches ou "
    "boutons par d\xe9" "faut. Les manettes sont d\xe9tect\xe9" "es au lancement du jeu.",
    "Womit dieser Spieler spielt: nichts, Tastatur oder ein angeschlossenes Gamepad. Ein neues Ger\xe4t beginnt mit seinen Standardtasten. "
    "Gamepads werden beim Spielstart erkannt.",
    "Con qu\xe9 juega este jugador: nada, el teclado o un mando conectado. Un dispositivo nuevo empieza con sus teclas o botones por "
    "defecto. Los mandos se detectan al iniciar el juego.",
    "Con cosa gioca questo giocatore: niente, la tastiera o un gamepad collegato. Un nuovo dispositivo parte con i tasti o pulsanti "
    "predefiniti. I gamepad vengono rilevati all'avvio del gioco." },
  { "The key or button of each action. Return on an action, then press the new key or button; Escape cancels. A key already in use "
    "swaps with the old one. The three menu buttons are a group of their own: they may share a key with a race action.",
    "La touche ou le bouton de chaque action. Entr\xe9" "e sur une action, puis la nouvelle touche ou le nouveau bouton ; \xc9" "chap "
    "annule. Une touche d\xe9j\xe0 utilis\xe9" "e \xe9" "change avec l'ancienne. Les trois boutons de menu forment un groupe \xe0 part : "
    "ils peuvent partager une touche avec une action de course.",
    "Die Taste oder der Knopf jeder Aktion. Eingabe auf einer Aktion, dann die neue Taste oder den neuen Knopf dr\xfc" "cken; Esc bricht "
    "ab. Eine schon benutzte Taste tauscht mit der alten. Die drei Men\xfc-Tasten sind eine eigene Gruppe: sie d\xfcrfen dieselbe "
    "Taste wie eine Rennaktion haben.",
    "La tecla o el bot\xf3n de cada acci\xf3n. Intro en una acci\xf3n y luego la tecla o el bot\xf3n nuevo; Esc cancela. Una tecla ya "
    "usada se intercambia con la anterior. Los tres botones de men\xfa son un grupo aparte: pueden compartir tecla con una acci\xf3n "
    "de carrera.",
    "Il tasto o il pulsante di ogni azione. Invio su un'azione, poi il nuovo tasto o pulsante; Esc annulla. Un tasto gi\xe0 usato si "
    "scambia con il precedente. I tre pulsanti dei menu sono un gruppo a parte: possono condividere un tasto con un'azione di gara." },
  { "The keys for left, right, up and down: steering and the menus. Return on a row, then the new key; Escape cancels. A gamepad always "
    "uses its stick or D-pad.",
    "Les touches gauche, droite, haut et bas : direction et menus. Entr\xe9" "e sur une ligne, puis la nouvelle touche ; \xc9" "chap annule. "
    "Une manette utilise toujours son stick ou sa croix.",
    "Die Tasten f\xfcr links, rechts, oben und unten: Lenken und Men\xfcs. Eingabe auf einer Zeile, dann die neue Taste; Esc bricht ab. "
    "Ein Gamepad benutzt immer Stick oder Steuerkreuz.",
    "Las teclas de izquierda, derecha, arriba y abajo: direcci\xf3n y men\xfas. Intro en una fila y luego la tecla nueva; Esc cancela. "
    "Un mando usa siempre su stick o su cruceta.",
    "I tasti per sinistra, destra, su e gi\xf9: sterzo e menu. Invio su una riga, poi il nuovo tasto; Esc annulla. Un gamepad usa sempre "
    "lo stick o la croce direzionale." },
  { "Return: this player gets the device and the keys or buttons the game gives it by default.",
    "Entr\xe9" "e : ce joueur reprend le p\xe9riph\xe9rique et les touches ou boutons que le jeu lui donne par d\xe9" "faut.",
    "Eingabe: dieser Spieler bekommt wieder das Ger\xe4t und die Tasten, die das Spiel ihm standardm\xe4\xdfig gibt.",
    "Intro: este jugador recupera el dispositivo y las teclas o botones que el juego le da por defecto.",
    "Invio: questo giocatore riprende il dispositivo e i tasti o pulsanti che il gioco gli d\xe0 per impostazione predefinita." },
  { "How far a gamepad stick must move before it counts, in percent, for all gamepads. 35 is the original. Smaller: more sensitive; "
    "raise it if a car steers by itself.",
    "De combien le stick d'une manette doit bouger avant de compter, en pour cent, pour toutes les manettes. 35 est la valeur d'origine. "
    "Plus petit : plus sensible ; augmente-la si une voiture tourne toute seule.",
    "Wie weit ein Gamepad-Stick bewegt werden muss, bevor er z\xe4hlt, in Prozent, f\xfcr alle Gamepads. 35 ist das Original. Kleiner: "
    "empfindlicher; erh\xf6he den Wert, wenn ein Auto von selbst lenkt.",
    "Cu\xe1nto debe moverse el stick de un mando para contar, en porcentaje, para todos los mandos. 35 es el original. Menor: m\xe1s "
    "sensible; s\xfa" "belo si un coche gira solo.",
    "Di quanto deve muoversi lo stick di un gamepad prima di contare, in percentuale, per tutti i gamepad. 35 \xe8 l'originale. Pi\xf9 "
    "piccolo: pi\xf9 sensibile; aumentalo se un'auto sterza da sola." },
  /* Mute in background.  French, German, Spanish, Italian: ours, NEED A NATIVE SPEAKER'S CHECK */
  { "Silences the game sound and the menu music while another window has the focus. Off: the sound plays on, as in the original.",
    "Coupe le son du jeu et la musique des menus quand une autre fen\xeatre est active. Non : le son continue, comme dans l'original.",
    "Schaltet Spielton und Men\xfcmusik stumm, solange ein anderes Fenster aktiv ist. Aus: der Ton l\xe4uft weiter wie im Original.",
    "Silencia el sonido del juego y la m\xfasica de los men\xfas mientras otra ventana est\xe1 activa. No: el sonido sigue, como en el original.",
    "Silenzia il suono del gioco e la musica dei menu mentre un'altra finestra \xe8 attiva. No: il suono continua, come nell'originale." },
};

/* The page tree: Options -> Preferences (category list) -> one sub-page per category.  All of them are page id MFL_PAGE_PREFS (the only
 * free slot of g_menu_page_item_lists): a menu record keeps its own item list, so the slot is pointed at the list the next Return will
 * open (sync_slot) and Escape pops back one level through the game's own back path. */
enum category {
  CAT_GRAPHICS, CAT_PERF,
#if MFL_FIX_CONTROLS
  CAT_CONTROLS,
#endif
  CAT_COMFORT, CAT_CHEATS, CAT_OTHER, CATS,
  PAGE_BUTTONS = CATS, PAGE_DIRECTIONS, PAGES      /* MFL_FIX_CONTROLS: the pages two rows of Controls open (not categories; empty without the switch) */
};
static const int k_cat_text[PAGES] = {      /* the title of each page */
  T_CAT_GRAPHICS, T_CAT_PERF,
#if MFL_FIX_CONTROLS
  T_CAT_CONTROLS,
#endif
  T_CAT_COMFORT, T_CAT_CHEATS, T_CAT_OTHER, T_CTL_BUTTONS, T_CTL_DIRECTIONS
};
#define KEY_RESOLUTION (-1)      /* the Resolution row: values from mfl_video_modes_api (GFX), not a key of mfl_prefs */
/* the rows of videocfg.bin (the original setup dialog's controls; the file stays the one home of these values, written by the game's own video_cfg_save) */
#define KEY_ADAPTER (-2)         /* g_video_cfg.subsystem_index (shown only when the PC has more than one adapter) */
#define KEY_VC_SHADOWS (-3)      /* no_dynamic_shadows (stored inverted) */
#define KEY_VC_MULTITEX (-4)     /* no_multitexture (inverted) */
#define KEY_VC_TRILINEAR (-5)    /* trilinear */
#define KEY_VC_ENV (-6)          /* no_env_effects (inverted) */
/* the rows of Controls (MFL_FIX_CONTROLS): the game's controller table, not keys of mfl_prefs */
#define KEY_CTL_PLAYER (-7)      /* which player the other rows show: 1..4 */
#define KEY_CTL_DEVICE (-8)      /* ControllerCfg.device / joystick / name */
#define KEY_CTL_BUTTONS (-9)     /* opens PAGE_BUTTONS */
#define KEY_CTL_DIRECTIONS (-10) /* opens PAGE_DIRECTIONS */
#define KEY_CTL_RESET (-11)
#define KEY_BIND (-12)           /* KEY_BIND - action: ControllerCfg.bindings[action], Return = capture */
#define LIVE_START 0             /* the footer of a row: applies at the next start / at once / from the next race */
#define LIVE_NOW 1
#define LIVE_RACE 2

/* the rows: category, caption (= description index), key, number of values, first value (language: -1 = auto), shown inverted (the key
 * says "skip", the row "on"), applies at once.  A new row = one line here (+ its texts); a new category = one enum value + its name. */
static const struct { int cat, text, key, count, first, invert, live; } k_rows[] = {
  { CAT_GRAPHICS, T_DISPLAY, MFL_PREF_DISPLAY_MODE, 3, 0, 0, 0 },
  { CAT_GRAPHICS, T_RESOLUTION, KEY_RESOLUTION, 0, 0, 0, 0 },      /* shown only when the video code registered its mode list */
#if MFL_FIX_FIXED_STEP && MFL_FIX_FPS_CAP
  { CAT_GRAPHICS, T_FPS_CAP, MFL_PREF_FPS_CAP, 3, 0, 0, 1 },      /* values 60, 30, unlimited (cl_app_main_loop.c k_fps_cap) */
#endif
  { CAT_GRAPHICS, T_ADAPTER, KEY_ADAPTER, 0, 0, 0, LIVE_START },      /* shown only with more than one adapter */
  { CAT_GRAPHICS, T_VSYNC, MFL_PREF_VSYNC, 2, 0, 0, 0 },
  { CAT_PERF, T_SHADOWS, KEY_VC_SHADOWS, 2, 0, 0, LIVE_START },      /* the shadow cameras are built once at boot (car_shadow_setup) */
  { CAT_PERF, T_MULTITEX, KEY_VC_MULTITEX, 2, 0, 0, LIVE_START },    /* also read at start (an_ok_070) */
  { CAT_PERF, T_TRILINEAR, KEY_VC_TRILINEAR, 2, 0, 0, LIVE_RACE },   /* read when a course loads */
  { CAT_PERF, T_ENV, KEY_VC_ENV, 2, 0, 0, LIVE_RACE },               /* per-frame gates (boost flames, wheel marks): copied on leaving the page, not mid-race */
  { CAT_COMFORT, T_INTRO, MFL_PREF_SKIP_INTRO, 2, 0, 1, 0 },
  { CAT_COMFORT, T_FLYOVER, MFL_PREF_SKIP_FLYOVER, 2, 0, 1, 1 },
  { CAT_COMFORT, T_WINNER_CAM, MFL_PREF_WINNER_CAM_ROTATION, 2, 0, 0, 1 },
  { CAT_COMFORT, T_WINNER_ZOOM, MFL_PREF_WINNER_CAM_ZOOM, 2, 0, 0, 1 },
  { CAT_COMFORT, T_MENU_SPEED, MFL_PREF_MENU_SPEED, 3, 0, 0, 1 },      /* values 1, 2, 4 (speed_index) */
  { CAT_CHEATS, T_UNLOCK, MFL_PREF_UNLOCK_ALL, 2, 0, 0, 0 },
#if MFL_FIX_LANG
  { CAT_OTHER, T_LANGUAGE, MFL_PREF_LANGUAGE, 7, -1, 0, 0 },
#endif
  { CAT_OTHER, T_SAVE_WAIT, MFL_PREF_SKIP_SAVE_WAIT, 2, 0, 1, 1 },
  { CAT_OTHER, T_MUTE, MFL_PREF_MUTE_UNFOCUSED, 2, 0, 0, LIVE_NOW },
#if MFL_FIX_CONTROLS
  { CAT_CONTROLS, T_CTL_PLAYER, KEY_CTL_PLAYER, 0, 0, 0, LIVE_NOW },
  { CAT_CONTROLS, T_CTL_DEVICE, KEY_CTL_DEVICE, 0, 0, 0, LIVE_NOW },
  { CAT_CONTROLS, T_CTL_BUTTONS, KEY_CTL_BUTTONS, 0, 0, 0, LIVE_NOW },
  { CAT_CONTROLS, T_CTL_DIRECTIONS, KEY_CTL_DIRECTIONS, 0, 0, 0, LIVE_NOW },
  { CAT_CONTROLS, T_CTL_RESET, KEY_CTL_RESET, 0, 0, 0, LIVE_NOW },
  { PAGE_BUTTONS, T_ACTION + 0, KEY_BIND - 0, 0, 0, 0, LIVE_NOW },      /* accelerate, brake, fire, drop */
  { PAGE_BUTTONS, T_ACTION + 1, KEY_BIND - 1, 0, 0, 0, LIVE_NOW },
  { PAGE_BUTTONS, T_ACTION + 2, KEY_BIND - 2, 0, 0, 0, LIVE_NOW },
  { PAGE_BUTTONS, T_ACTION + 3, KEY_BIND - 3, 0, 0, 0, LIVE_NOW },
  { PAGE_BUTTONS, T_ACTION + 8, KEY_BIND - 8, 0, 0, 0, LIVE_NOW },      /* insult, pause, then the three menu buttons */
  { PAGE_BUTTONS, T_ACTION + 6, KEY_BIND - 6, 0, 0, 0, LIVE_NOW },
  { PAGE_BUTTONS, T_ACTION + 4, KEY_BIND - 4, 0, 0, 0, LIVE_NOW },
  { PAGE_BUTTONS, T_ACTION + 5, KEY_BIND - 5, 0, 0, 0, LIVE_NOW },
  { PAGE_BUTTONS, T_ACTION + 7, KEY_BIND - 7, 0, 0, 0, LIVE_NOW },
  { PAGE_DIRECTIONS, T_ACTION + 9, KEY_BIND - 9, 0, 0, 0, LIVE_NOW },      /* left, right, up, down */
  { PAGE_DIRECTIONS, T_ACTION + 10, KEY_BIND - 10, 0, 0, 0, LIVE_NOW },
  { PAGE_DIRECTIONS, T_ACTION + 11, KEY_BIND - 11, 0, 0, 0, LIVE_NOW },
  { PAGE_DIRECTIONS, T_ACTION + 12, KEY_BIND - 12, 0, 0, 0, LIVE_NOW },
  { PAGE_DIRECTIONS, T_DEADZONE, MFL_PREF_PAD_DEADZONE, 10, 0, 0, LIVE_NOW },      /* values 5 .. 50 (index 0..9); here because this page shows the live directions */
#endif
};
#define ROWS ((int)(sizeof k_rows / sizeof k_rows[0]))
#if MFL_FIX_CONTROLS
#define ROW_OPENS_PAGE(k) (k_rows[k].key == KEY_CTL_BUTTONS || k_rows[k].key == KEY_CTL_DIRECTIONS)
#define ROW_STEPS(k) (k_rows[k].key > KEY_CTL_BUTTONS)      /* Left / Right change it: the value gets the arrows */
#else
#define ROW_OPENS_PAGE(k) 0
#define ROW_STEPS(k) 1
#endif
/* 640-space layout (the game's buttons are fixed at x 60..270 plus a gradient to 370; the widescreen fix centres this space 4:3) */
#define ROW_SCALE 0.65f     /* our row captions (the game's rows: 0.8) */
#define ROW_DEFAULT_SCALE 0.8f      /* menu_page_enter's row scale */
#define VALUE_X 300.0f      /* the values: at most here (over the buttons' gradient strip), else VALUE_GAP right of the page's widest caption */
#define VALUE_GAP 30.0f     /* room for the left arrow (20) and air */
#define VALUE_SCALE 0.6f
#define BOX_X 424.0f        /* the description box: x 424..632 */
#define BOX_W 208.0f
#define BOX_Y 100.0f
#define BOX_PAD 8.0f
#define BOX_SCALE 0.5f
#define BOX_LINE 16.0f
#define PROMPTS_SELECT_BACK 2    /* the Options page's prompt set */

const struct mfl_video_modes *mfl_video_modes_api;      /* set by the video code (GFX gf3) at start; 0 = no Resolution row */
static unsigned int options_list[64];      /* the Options page list with the Preferences row first */
static unsigned int cat_list[24 + 5 * CATS];
static unsigned int sub_list[PAGES][24 + 5 * ROWS];
static int sub_rows[PAGES][ROWS], sub_count[PAGES];      /* k_rows index of each row of a sub-page */
static int values[MFL_PREF_COUNT];         /* the file's values as the page shows them */
static int installed, dirty;
static float caption_width[PAGES];         /* the widest row caption of each page (640-space, ROW_SCALE) */
static float value_room;                   /* the width a value may take in the row being drawn (up to the description box) */
static int start_values[MFL_PREF_COUNT], start_mode;      /* what this run started with: a LIVE_START row that differs = restart hint */
static struct VideoCfg start_video;
static unsigned short line_buf[LINES][LINE_CHARS + 1];      /* length-prefixed UTF-16 lines of the description box */
static unsigned short mode_buf[24];         /* the Resolution row's value */
static unsigned short adapter_buf[24];      /* the Display adapter row's value */
static int quality_dirty;                   /* a videocfg.bin row changed: video_cfg_save on leaving the page */
#if MFL_FIX_CONTROLS
#define CONTROLLERS 4
#define ACTIONS 13               /* ControllerCfg.bindings: 0..8 buttons, 9..12 left right up down */
#define PAD_ACTIONS 9            /* a pad's directions are its stick: bindings 9..12 are not read */
#define ACTION_LEFT 9
#define DEVICE_NONE 0            /* ControllerCfg.device */
#define DEVICE_PAD 1
#define DEVICE_KEYBOARD 2
#define CFG_VERSION 6
#define PAD_BUTTONS 16           /* button bits the game reads per pad */
#define KEY_ESCAPE 0x01          /* DirectInput key code */
#define DEADZONE_ORIGINAL 6      /* index of 35 percent */
#define VALUE_CHARS 31           /* a row value's buffer; on screen it is cut to value_room */
#define MENU_ACTION(a) ((a) == 4 || (a) == 5 || (a) == 7)      /* menu select, back, next: their own group in the swap rule */
#define CAPTURE_SECONDS 10.0f    /* a capture ends by itself after this (a key that reads as stuck must not lock the menu) */
enum { CAPTURE_OFF, CAPTURE_RELEASE_FIRST, CAPTURE_WAIT, CAPTURE_RELEASE_LAST };      /* Return up, the new key down, that key up */
static unsigned short row_buf[ROWS][VALUE_CHARS + 6];      /* the Controls rows' values, length first */
static int ctl_player;                       /* the player the Controls rows show, 0..3 */
static unsigned short percent_buf[4][5];     /* the direction preview's numbers, length first */
static int capture_phase, capture_action;
static float capture_time;
static struct ControllerCfg device_undo[CONTROLLERS];      /* the profile a player had before the Device row changed it ... */
static int device_undo_set[CONTROLLERS];                   /* ... so stepping back to that device brings its keys back */
#endif

static int text_id(int t) { return MFL_TEXT_FIRST + t; }
static int language(void) { return g_language_offset >= 1 && g_language_offset <= 4 ? g_language_offset : 0; }

/* the captions the original setup dialog has: its string table id (+ g_language_offset, 6 languages) and the longest text that fits the row / button */
static const struct { int text, base, max_chars; } k_orig[] = {
  { T_CAT_PERF, 2240, 14 },      /* "Performance Settings": only where it is short enough for the category button (scale 0.8) */
  { T_SHADOWS, 2250, 22 }, { T_MULTITEX, 2260, 22 }, { T_TRILINEAR, 2270, 22 }, { T_ENV, 2280, 22 },
};

#if MFL_FIX_CONTROLS
/* the captions the game's own text table has (dat_get_string index, the same in every language) and the longest text that fits */
static const struct { int text, dat, max_chars; } k_game_text[] = {
  { T_CAT_CONTROLS, 570, 14 },      /* "Controllers" */
  { T_CTL_PLAYER, 164, 22 },        /* "Player" */
  { T_ACTION + 0, 548, 22 },        /* "Accelerate" */
  { T_ACTION + 1, 549, 22 },        /* "Brake" */
  { T_ACTION + 2, 668, 22 },        /* "Fire weapon" */
  { T_ACTION + 3, 667, 22 },        /* "Drop weapon" */
  { T_ACTION + 6, 546, 22 },        /* "Start/Pause" */
  { T_ACTION + 8, 352, 22 },        /* "Insults" */
};
#endif

const unsigned short *mfl_prefs_text(int id)
{
  static unsigned short ring[4][48];      /* a few strings may be in use at once (draw + width) */
  static int next;
  unsigned short *s = ring[next++ & 3];
  int n = 0, k;
  char original[48];
  const char *c;

  if (id - MFL_TEXT_FIRST == T_MODE_NAME) return mode_buf;
  if (id - MFL_TEXT_FIRST == T_ADAPTER_NAME) return adapter_buf;
#if MFL_FIX_CONTROLS
  if (id - MFL_TEXT_FIRST >= T_ROW_VALUE) return row_buf[(id - MFL_TEXT_FIRST - T_ROW_VALUE) % ROWS];
  if (id - MFL_TEXT_FIRST >= T_PERCENT && id - MFL_TEXT_FIRST < T_PERCENT + 4) return percent_buf[id - MFL_TEXT_FIRST - T_PERCENT];
  for (k = 0; k < (int)(sizeof k_game_text / sizeof k_game_text[0]); k++) {      /* the game's own text when it fits the row */
    const unsigned short *game;

    if (id - MFL_TEXT_FIRST != k_game_text[k].text) continue;
    game = (const unsigned short *)dat_get_string(k_game_text[k].dat);
    if (game[0] != 0 && game[0] <= k_game_text[k].max_chars) return game;
    break;
  }
#endif
  if (id - MFL_TEXT_FIRST >= T_LINE0) return line_buf[(id - MFL_TEXT_FIRST - T_LINE0) % LINES];
  for (k = 0; k < (int)(sizeof k_orig / sizeof k_orig[0]); k++) {      /* the game's own string, in its language (the original exe's resource) */
    if (id - MFL_TEXT_FIRST != k_orig[k].text) continue;
    n = LoadStringA(g_app_7e7700, k_orig[k].base + (g_language_offset >= 0 && g_language_offset <= 5 ? g_language_offset : 0), original, sizeof original);
    if (n > 0 && n <= k_orig[k].max_chars) {
      for (k = 0; k < n; k++) s[k + 1] = (unsigned char)original[k];
      s[0] = (unsigned short)n;
      return s;
    }
    n = 0;
    break;
  }
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
float mfl_menu_real_seconds;      /* the frame time before the factor, for the timers that keep real time (the Training / Track selection hold repeat) */

void mfl_menu_update_scaled(void)
{
  float saved = g_frame_seconds;

  mfl_menu_real_seconds = saved;
  if (g_app_state == APP_STATE_MAIN_LOOP_INIT) g_frame_seconds = saved * (float)(1 << speed_index(mfl_pref(MFL_PREF_MENU_SPEED)));
  menu_update();
  g_frame_seconds = saved;
}

static int row_shown(int k)
{
  if (k_rows[k].key == KEY_ADAPTER) return g_app_771500 > 1;
  return k_rows[k].key != KEY_RESOLUTION || (mfl_video_modes_api != 0 && mfl_video_modes_api->count() > 0);
}

/* the videocfg.bin word of a quality row, and the row's On / Off (the words are stored "off" for shadows, multi-texturing and effects, "on" for trilinear) */
static unsigned int *quality_word(int key)
{
  return key == KEY_VC_SHADOWS ? &g_video_cfg.no_dynamic_shadows : key == KEY_VC_MULTITEX ? &g_video_cfg.no_multitexture
         : key == KEY_VC_TRILINEAR ? &g_video_cfg.trilinear : &g_video_cfg.no_env_effects;
}

static int quality_on(int key) { return (*quality_word(key) != 0) == (key == KEY_VC_TRILINEAR); }

/* the mode record i of adapter sub: width, height, depth, flags (RwVideoMode) */
static const int *adapter_mode_record(int sub, int i) { return (const int *)(g_video_mode_lists[sub] + i * 0x18); }

/* the mode the original would start an adapter in: the first plain 32 bit full-screen mode of at least 640 x 480, else any such full-screen mode, else 0 */
static int adapter_first_mode(int sub)
{
  int i, any = 0, found = 0;
  const int *m;

  for (i = 0; i < g_video_mode_counts[sub]; i++) {
    m = adapter_mode_record(sub, i);
    if ((m[3] & 1) == 0 || m[0] < 640 || m[1] < 480) continue;
    if (m[3] == 1 && m[2] == 32) return i;
    if (!found) { any = i; found = 1; }
  }
  return any;
}

/* a new adapter: its first mode (the dialog does the same), so the file never names a mode the adapter does not have */
static void adapter_choose(int sub)
{
  const int *m;
  int mode;

  if (g_video_mode_counts[sub] <= 0) return;      /* an adapter without modes: not selectable */
  mode = adapter_first_mode(sub);
  m = adapter_mode_record(sub, mode);
  g_video_cfg.subsystem_index = (unsigned int)sub;
  g_video_cfg.mode_index = (unsigned int)mode;
  g_video_cfg.width = (unsigned int)m[0];
  g_video_cfg.height = (unsigned int)m[1];
  g_video_cfg.depth = (unsigned int)m[2];
  g_video_cfg.fullscreen = (unsigned int)(m[3] & 1);
}

#if MFL_FIX_CONTROLS
/* ---- MFL_FIX_CONTROLS: the game's controller table g_controller_cfg[4] and its files contcfg<n>.bin ---- */

/* the defaults of controller_dialog_run (automatch_near/an_hk_999.c), by action: keys S X A D Return Escape Escape A C and the four
 * arrows; pad buttons 0 1 2 3 0 1 4 2 5 (a pad's directions are its stick) */
static const unsigned char k_default_keys[ACTIONS] = { 0x1f, 0x2d, 0x1e, 0x20, 0x1c, 0x01, 0x01, 0x1e, 0x2e, 0xcb, 0xcd, 0xc8, 0xd0 };
static const unsigned char k_default_buttons[ACTIONS] = { 0, 1, 2, 3, 0, 1, 4, 2, 5, 0, 0, 0, 0 };

static int same_text(const char *a, const char *b)
{
  while (*a != '\0' && *a == *b) { a++; b++; }
  return *a == *b;
}

static int any_device(void)
{
  int slot, any = 0;

  for (slot = 0; slot < CONTROLLERS; slot++) any |= g_controller_cfg[slot].device != DEVICE_NONE;
  return any;
}

/* a player's slot filled as controller_dialog_run fills it: the device, its name (the acceptance key of the original's loader) and its
 * default keys or buttons */
static void slot_set(int slot, unsigned int device, int pad)
{
  static const struct ControllerCfg k_empty = { 0 };
  struct ControllerCfg *cfg = &g_controller_cfg[slot];
  const char *keyboard = g_controller_dialog_7711a4;
  int k;

  *cfg = k_empty;
  cfg->version = CFG_VERSION;
  cfg->device = device;
  if (device == DEVICE_NONE) return;
  if (device == DEVICE_PAD) {
    cfg->joystick = (unsigned int)pad;
    controller_dialog_copy_name(pad, cfg->name);
  } else {
    for (k = 0; keyboard[k] != '\0' && k < (int)sizeof cfg->name - 1; k++) cfg->name[k] = keyboard[k];
  }
  for (k = 0; k < ACTIONS; k++) cfg->bindings[k] = device == DEVICE_PAD ? k_default_buttons[k] : k_default_keys[k];
}

/* the key pad_deadzone into the game's dead zone (joypads_init set 0.35) */
static void deadzone_apply(void)
{
  int index = mfl_pref(MFL_PREF_PAD_DEADZONE);

  g_app_770f60 = index == DEADZONE_ORIGINAL ? 0.35f : (float)(index + 1) * 0.05f;      /* 35: the original's own constant, bit for bit */
}

/* Start (the last step of controller_dialog_run, which has made its defaults and kept the files its own rule accepts): every
 * contcfg<n>.bin is read again and kept when it has version 6, key codes in range and a device that exists now: none, the keyboard
 * (under any name: the original's rule dropped the profile when the game language, and so the word "Keyboard", changed), or a pad whose
 * name is one of the pads found (the profile follows its pad when Windows lists the pads in another order).  The players without such
 * a file get the devices left over in the original's order: the pads, then the keyboard, then none.  Without files that is exactly the
 * original's table.  The original exe reads the same files with its stricter rule and falls back to its defaults where that fails. */
void mfl_controls_load(void)
{
  int pads = app_get_app_770f64(), slot, pad, k, kept[CONTROLLERS], pads_used = 0, keyboard_used = 0;
  struct ControllerCfg *cfg;
  char name[sizeof cfg->name];

  if (pads > CONTROLLERS) pads = CONTROLLERS;      /* the pad tables hold 4 (MFL_FIX_PAD_LIMIT stops the enumeration there) */
  for (slot = 0; slot < CONTROLLERS; slot++) {
    cfg = &g_controller_cfg[slot];
    kept[slot] = controller_cfg_load_n(slot) != 0 && cfg->version == CFG_VERSION && cfg->device <= DEVICE_KEYBOARD;
    cfg->name[sizeof cfg->name - 1] = '\0';
    for (k = 0; k < ACTIONS; k++) kept[slot] &= cfg->bindings[k] < (cfg->device == DEVICE_PAD ? PAD_BUTTONS : 256u);
    if (kept[slot] && cfg->device == DEVICE_PAD) {
      /* the pad of that name: the file's own number first, else the first one no earlier player has */
      for (k = -1, pad = cfg->joystick < (unsigned int)pads ? (int)cfg->joystick : 0; k < pads; pad = ++k) {
        name[0] = '\0';
        if (pad < pads && (pads_used >> pad & 1) == 0 && controller_dialog_copy_name(pad, name) && same_text(name, cfg->name)) break;
      }
      kept[slot] = k < pads;
      if (kept[slot]) { cfg->joystick = (unsigned int)pad; pads_used |= 1 << pad; }
    }
    keyboard_used |= kept[slot] && cfg->device == DEVICE_KEYBOARD;
  }
  for (slot = 0; slot < CONTROLLERS; slot++) {
    if (kept[slot]) continue;
    for (pad = 0; pad < pads && (pads_used >> pad & 1) != 0; pad++) ;
    if (pad < pads) { slot_set(slot, DEVICE_PAD, pad); pads_used |= 1 << pad; }
    else if (!keyboard_used) { slot_set(slot, DEVICE_KEYBOARD, 0); keyboard_used = 1; }
    else slot_set(slot, DEVICE_NONE, 0);
  }
  if (!any_device()) slot_set(0, DEVICE_KEYBOARD, 0);      /* four files that all say "none": never a game nobody can steer */
  deadzone_apply();
}

/* ---- MFL_FIX_CONTROLS: the rows ---- */

/* s (cut to the room between the value column and the description box) as the value text of row k */
static int row_value(int k, const char *s)
{
  int n;

  for (n = 0; s[n] != '\0' && n < VALUE_CHARS; n++) row_buf[k][n + 1] = (unsigned char)s[n];
  row_buf[k][0] = (unsigned short)n;
  while (n > 1 && text_width(text_id(T_ROW_VALUE + k), VALUE_SCALE) > value_room) row_buf[k][0] = (unsigned short)--n;
  return text_id(T_ROW_VALUE + k);
}

/* the name of a key as Windows names it in the user's keyboard layout (DirectInput key codes are scan codes, bit 7 = extended key);
 * without a name "Key" and the code */
static void key_name(unsigned int code, char *out)
{
  static key_name_fn get_name;

  if (get_name == 0) get_name = (key_name_fn)GetProcAddress(GetModuleHandleA("user32.dll"), "GetKeyNameTextA");
  if (get_name != 0 && get_name((long)((code & 0x7f) << 16 | (code & 0x80) << 17), out, VALUE_CHARS + 1) > 0) return;
  out[0] = 'K'; out[1] = 'e'; out[2] = 'y'; out[3] = ' ';
  out[4] = "0123456789ABCDEF"[code >> 4 & 15]; out[5] = "0123456789ABCDEF"[code & 15]; out[6] = '\0';
}

static int ctl_value_text(int k)
{
  const struct ControllerCfg *cfg = &g_controller_cfg[ctl_player];
  int key = k_rows[k].key, action = KEY_BIND - key, n, number;
  char name[VALUE_CHARS + 8];
  const char *s;

  name[0] = (char)('1' + ctl_player); name[1] = '\0';
  if (key == KEY_CTL_PLAYER) return row_value(k, name);
  if (key == KEY_CTL_DEVICE)
    return row_value(k, cfg->device == DEVICE_PAD ? cfg->name : cfg->device == DEVICE_KEYBOARD ? g_controller_dialog_7711a4 : g_controller_dialog_7711e8);
  if (key > KEY_BIND) return row_value(k, "");      /* the rows that open a page, Reset */
  if (capture_phase == CAPTURE_WAIT && action == capture_action) return row_value(k, "?");
  if (cfg->device == DEVICE_NONE) return row_value(k, "-");
  if (cfg->device == DEVICE_KEYBOARD) {
    key_name(cfg->bindings[action], name);
    return row_value(k, name);
  }
  if (action >= ACTION_LEFT) return row_value(k, k_text[T_STICK][language()]);
  for (n = 0, s = k_text[T_BUTTON][language()]; s[n] != '\0'; n++) name[n] = s[n];
  number = (int)cfg->bindings[action] + 1;      /* 1..16 */
  name[n++] = ' ';
  if (number >= 10) name[n++] = '1';
  name[n++] = (char)('0' + number % 10);
  name[n] = '\0';
  return row_value(k, name);
}

/* the lowest set bit of a bit set, or -1 */
static int first_bit(const unsigned char *bits, int count)
{
  int n;

  for (n = 0; n < count && (bits[n >> 3] >> (n & 7) & 1) == 0; n++) ;
  return n < count ? n : -1;
}

/* A capture in progress (Return on an action row started it): returns 1 while it owns the input.  The menu must not act on the keys
 * of these frames (the Return that started it, the new key, Escape), so the menu fields of every player's input are cleared before
 * menu_update's back / direction blocks read them, until all keys and buttons are up again.  The new key or button is the lowest one
 * down on the player's device; an action that had it gets the old one of this action (nothing is left without a key).  Escape cancels. */
static int capture_run(void)
{
  struct ControllerCfg *cfg = &g_controller_cfg[ctl_player];
  int key = first_bit(g_app_77122c, 256), found = key, held = key >= 0, pad, k, count = ACTIONS;
  unsigned int old;

  if (capture_phase == CAPTURE_OFF) return 0;
  for (k = 0; k < CONTROLLERS; k++) {
    struct PlayerInput *in = &g_player_inputs[k];

    in->menu_select = in->menu_back = in->menu_next = in->pause = 0;
    in->direction_flags[0] = in->direction_flags[1] = in->direction_flags[2] = in->direction_flags[3] = 0;
  }
  for (pad = 0; pad < CONTROLLERS; pad++) held |= first_bit(g_app_7711c4 + pad * 4, PAD_BUTTONS) >= 0;
  capture_time += g_frame_seconds;
  if (capture_time > CAPTURE_SECONDS) capture_phase = CAPTURE_OFF;
  else if (capture_phase != CAPTURE_WAIT) {      /* waiting for every key and button to be up */
    if (!held) capture_phase = capture_phase == CAPTURE_RELEASE_FIRST ? CAPTURE_WAIT : CAPTURE_OFF;
  } else if (held) {
    if (cfg->device == DEVICE_PAD) { found = first_bit(g_app_7711c4 + cfg->joystick * 4, PAD_BUTTONS); count = PAD_ACTIONS; }
    if (key != KEY_ESCAPE && found >= 0) {
      old = cfg->bindings[capture_action];
      /* the swap, inside the action's group only: race actions and menu buttons may share a key (the defaults do: accelerate and
       * select, brake and back, fire and next on a pad); a direction shares with nobody */
      for (k = 0; k < count; k++)
        if (cfg->bindings[k] == (unsigned int)found
            && (k >= ACTION_LEFT || capture_action >= ACTION_LEFT || MENU_ACTION(k) == MENU_ACTION(capture_action))) cfg->bindings[k] = old;
      cfg->bindings[capture_action] = (unsigned int)found;
      device_undo_set[ctl_player] = 0;
      controller_cfg_save_n(ctl_player);
    }
    if (key == KEY_ESCAPE || found >= 0) capture_phase = CAPTURE_RELEASE_LAST;
  }
  return 1;
}

/* Left / Right / Return on a Controls row (the dead zone row is an ordinary key of mfl_prefs).  A change is in force at once and
 * the player's file is written at once. */
static void ctl_step(int key, int dir)
{
  struct ControllerCfg *cfg = &g_controller_cfg[ctl_player];
  int pads = app_get_app_770f64(), choice, action = KEY_BIND - key;

  if (pads > CONTROLLERS) pads = CONTROLLERS;
  if (key == KEY_CTL_PLAYER) { ctl_player = (ctl_player + dir + CONTROLLERS) % CONTROLLERS; return; }
  if (key == KEY_CTL_DEVICE) {      /* none, keyboard, pad 0 .. */
    if (!device_undo_set[ctl_player]) { device_undo[ctl_player] = *cfg; device_undo_set[ctl_player] = 1; }
    choice = cfg->device == DEVICE_PAD ? 2 + (int)cfg->joystick : cfg->device == DEVICE_KEYBOARD ? 1 : 0;
    choice = (choice + dir + pads + 2) % (pads + 2);
    slot_set(ctl_player, choice == 0 ? DEVICE_NONE : choice == 1 ? DEVICE_KEYBOARD : DEVICE_PAD, choice - 2);
    if (!any_device()) slot_set(ctl_player, DEVICE_KEYBOARD, 0);      /* the last device cannot be switched off: the menu would be dead */
    if (cfg->device == device_undo[ctl_player].device && (cfg->device != DEVICE_PAD || cfg->joystick == device_undo[ctl_player].joystick))
      *cfg = device_undo[ctl_player];      /* back on the device he had: with his keys */
  } else if (key == KEY_CTL_RESET && g_menu_select_pressed == 1) {      /* the device and keys the game gives this player without a file */
    slot_set(ctl_player, ctl_player < pads ? DEVICE_PAD : ctl_player == pads ? DEVICE_KEYBOARD : DEVICE_NONE, ctl_player);
    if (!any_device()) slot_set(ctl_player, DEVICE_KEYBOARD, 0);
    device_undo_set[ctl_player] = 0;
  } else {
    if (key <= KEY_BIND && g_menu_select_pressed == 1 && cfg->device != DEVICE_NONE && (cfg->device == DEVICE_KEYBOARD || action < ACTION_LEFT)) {
      capture_action = action;
      capture_phase = CAPTURE_RELEASE_FIRST;
      capture_time = 0.0f;
    }
    return;
  }
  controller_cfg_save_n(ctl_player);
}

/* The pages whose readers of g_player_inputs only ask "any record" (agents/mw2_w17/S1a-clusters.md, S1a-automatch.md): there the
 * fallback below cannot let one device steer another player's choice.  Off (the original input): pause 0, team colour 4, results 5,
 * teams 9, Xbox Live / lobby / join 0xb..0xe, ability 0xf, team select 0x10 (per-player cursors), move screen 0x1a (adds every record's
 * directions), the unidentified 0x14..0x17 0x19 0x1d 0x26 0x27, and every page outside the frontend (a race and its pause menu, which
 * reach Options too; g_input_mode cannot tell: single player sets mode 1 in the frontend).  Preferences: everywhere, as before.  A message
 * box sits on the page it opened over and keeps that page's choice (its select / back readers are "any" ones). */
static int fallback_page(int page)
{
  if (page == MFL_PAGE_PREFS) return 1;
  if (g_app_state != APP_STATE_MAIN_LOOP_INIT) return 0;
  switch (page) {
  case MENU_PAGE_MAIN: case MENU_PAGE_SINGLE_PLAYER: case MENU_PAGE_MULTIPLAYER: case MENU_PAGE_CHALLENGE_SELECT_A:
  case MENU_PAGE_CHALLENGE_SELECT_B: case MENU_PAGE_OPTIONS: case MENU_PAGE_CUSTOMISE_PLAY: case MENU_PAGE_CUSTOM_ABILITIES:
  case MENU_PAGE_GAME_SETUP_MULTI: case MENU_PAGE_SOUND: case MENU_PAGE_GAME_SETUP_SINGLE: case MENU_PAGE_VIBRATION:
    return 1;
  }
  return page >= MENU_PAGE_GAMMA && page <= MENU_PAGE_LOAD_SAVE;      /* 0x1e..0x25: gamma .. load / save */
}

/* controller_poll_player, last step, while a menu page of fallback_page is on screen: the menu fields of the player's input also follow the
 * keyboard's default keys (arrows, Return, Escape) and every found pad's stick and default buttons, whatever device and keys the
 * players have.  The menus read only the players' own bindings, so the Device row could leave no player on the device in the user's
 * hand (the keyboard went dead as soon as no player had it) and a rebound menu key could be forgotten: here that cannot lock the page.
 * The game's own reader does the test, on the player's slot filled with each device's defaults for the moment. */
void mfl_controls_menu_input(int player)
{
  static unsigned char last[CONTROLLERS][6];      /* the previous frame's result: select, back, left, right, up, down */
  static int stale[CONTROLLERS] = { 1, 1, 1, 1 };
  struct PlayerInput *in = &g_player_inputs[player], *before = &g_player_inputs_previous[player];
  struct ControllerCfg saved;
  unsigned char now[6] = { 0, 0, 0, 0, 0, 0 };
  int pads = app_get_app_770f64(), device, d, page, bound = 0;

  if (player < 0 || player >= CONTROLLERS) return;
  if (!installed || g_frontend_state == 0) { stale[player] = 1; return; }
  if (pads > CONTROLLERS) pads = CONTROLLERS;
  page = g_menu_records[g_menu_current].page_id;
  /* outside Preferences only the devices no player has: a bound device already steers its own record, and ORed into every record
   * it would win "who pressed" pickers (ui_input_reset on Main gives slot 0 to the first record with a fresh select) */
  if (page != MFL_PAGE_PREFS)
    for (d = 0; d < CONTROLLERS; d++) {
      if (g_controller_cfg[d].device == DEVICE_KEYBOARD) bound |= 1;
      if (g_controller_cfg[d].device == DEVICE_PAD && g_controller_cfg[d].joystick < CONTROLLERS) bound |= 2 << g_controller_cfg[d].joystick;
    }
  saved = g_controller_cfg[player];
  for (device = -1; device < pads; device++) {      /* -1: the keyboard */
    if (bound >> (device + 1) & 1) continue;
    slot_set(player, device < 0 ? DEVICE_KEYBOARD : DEVICE_PAD, device);
    now[0] |= controller_action_pressed(player, 4);
    now[1] |= controller_action_pressed(player, 5);
    for (d = 0; d < 4; d++) if (controller_action_pressed(player, ACTION_LEFT + d) != 0) now[2 + d] = 0xff;
  }
  g_controller_cfg[player] = saved;
  if (stale[player]) {      /* first frame after a gap (no menu / not installed): last[] is old, a key held now is not a new press */
    for (d = 0; d < 6; d++) last[player][d] = now[d];
    stale[player] = 0;
  }
  if (fallback_page(page)) {
    /* the previous frame's record gets the previous result too: the Return still held from opening the page is not a new press */
    in->menu_select |= now[0]; before->menu_select |= last[player][0];
    in->menu_back |= now[1]; before->menu_back |= last[player][1];
    for (d = 0; d < 4; d++) { in->direction_flags[d] |= now[2 + d]; before->direction_flags[d] |= last[player][2 + d]; }
  }
  for (d = 0; d < 6; d++) last[player][d] = now[d];
}

/* the live directions of the shown player as the game reads them (a pad: after the dead zone): a cross of four squares, each filled from
 * below as far as its direction is pressed, with that value in percent inside (a key: 0 or 100) */
static void direction_preview(float x, float y)
{
  static const float k_dx[4] = { -20.0f, 20.0f, 0.0f, 0.0f }, k_dy[4] = { 0.0f, 0.0f, -20.0f, 20.0f };      /* left, right, up, down */
  const struct ControllerCfg *cfg = &g_controller_cfg[ctl_player];
  float sx, sy, a, scale, w;
  int d, v, n;

  for (d = 0; d < 4; d++) {
    v = controller_action_pressed(ctl_player, ACTION_LEFT + d);      /* 0 / 255; a pad's left / right: the deflection 0..255 */
    if (cfg->device == DEVICE_PAD && d >= 2) {      /* a pad's up / down read 0 / 255 there: the stick's y as the deflection */
      a = g_app_77120c[cfg->joystick * 2 + 1] * (d == 2 ? 255.0f : -255.0f);
      v = a <= 0.0f ? 0 : a >= 255.0f ? 255 : (int)a;
    }
    v = (v * 100 + 127) / 255;      /* percent */
    sx = x + 20.0f + k_dx[d];
    sy = y + 20.0f + k_dy[d];
    draw_rect(sx, sy, 18.0f, 18.0f, 0x60ffffffu);
    if (v != 0) draw_rect(sx, sy + 18.0f * (float)(100 - v) * 0.01f, 18.0f, 18.0f * (float)v * 0.01f, 0xff40c0ffu);
    n = 0;
    if (v >= 100) percent_buf[d][++n] = '1';
    if (v >= 10) percent_buf[d][++n] = (unsigned short)('0' + v / 10 % 10);
    percent_buf[d][++n] = (unsigned short)('0' + v % 10);
    percent_buf[d][0] = (unsigned short)n;
    scale = 0.35f;
    w = (float)text_width(text_id(T_PERCENT + d), scale);
    if (w > 16.0f) { scale *= 16.0f / w; w = 16.0f; }
    ui_text_draw_fade_alpha(text_id(T_PERCENT + d), sx + (18.0f - w) * 0.5f, sy + 9.0f, 0xffffffffu, scale, 0, 0x1ff);
  }
}
#define PREVIEW_LINES 4      /* the cross's height (58) in box lines */
#endif

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
  for (k = 0; k < MFL_PREF_COUNT; k++) start_values[k] = values[k] = mfl_prefs[k];
  start_video = g_video_cfg;
  start_mode = mfl_video_modes_api != 0 ? mfl_video_modes_api->current() : 0;
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
  for (c = 0; c < PAGES; c++) {
    l = sub_list[c];
    n = list_head(l, k_cat_text[c], PROMPTS_LEFT_RIGHT, src);
    for (k = 0; k < ROWS; k++) {
      if (k_rows[k].cat != c || !row_shown(k)) continue;
      sub_rows[c][sub_count[c]++] = k;
      if ((float)text_width(text_id(k_rows[k].text), ROW_SCALE) > caption_width[c]) caption_width[c] = (float)text_width(text_id(k_rows[k].text), ROW_SCALE);
      l[n++] = L_ROW; l[n++] = text_id(k_rows[k].text);
      if (ROW_OPENS_PAGE(k)) { l[n++] = L_PAGE; l[n++] = MFL_PAGE_PREFS; }      /* the slot holds that row's page while it is selected (sync_slot) */
      else { l[n++] = L_ACTION; l[n++] = 0xffffffffu; }
      l[n++] = L_ITEM_END;
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

  for (c = 0; c < PAGES && !SHOWS(&g_menu_records[g_menu_current], sub_list[c]); c++) ;
  return c < PAGES ? c : -1;
}

/* the page slot = the list the next Return opens: the selected category's rows on the category list, the page of a Controls row that
 * opens one, else the category list */
static void sync_slot(void)
{
  struct MenuRecord *rec = &g_menu_records[g_menu_current];
  unsigned int *opens = cat_list;

  if (SHOWS(rec, cat_list) && rec->selected >= 0 && rec->selected < CATS) opens = sub_list[rec->selected];
#if MFL_FIX_CONTROLS
  else if (SHOWS(rec, sub_list[CAT_CONTROLS]) && rec->selected >= 0 && rec->selected < sub_count[CAT_CONTROLS]) {
    int key = k_rows[sub_rows[CAT_CONTROLS][rec->selected]].key;

    if (key == KEY_CTL_BUTTONS) opens = sub_list[PAGE_BUTTONS];
    if (key == KEY_CTL_DIRECTIONS) opens = sub_list[PAGE_DIRECTIONS];
  }
#endif
  g_menu_page_item_lists[MFL_PAGE_PREFS] = (char *)opens;
}

/* the text id frontend_draw shows on button anim it */
static int anim_text_id(const struct MenuItemAnim *it)
{
  return it->state == 0 || it->state == 2 ? it->ref_b : it->ref_a;
}

/* the row of button anim it (the text frontend_draw shows for it), or ROWS when it is not one of ours */
static int anim_row(const struct MenuItemAnim *it)
{
  int r, id = anim_text_id(it);

  for (r = 0; r < ROWS && (it->item_id != (int)L_ROW || id != text_id(k_rows[r].text)); r++) ;
  return r;
}

/* our captions are longer than the game's: smaller text in the same buttons.  Hook at the start of frontend_draw's item loop, so the
 * scale is set from the state the items are drawn with in this frame (set in menu_update or after the draw, it was one frame late
 * when a slot went idle: the new page's caption flashed in the old page's size) */
#define ROW_X 64.0f         /* menu_page_enter's row x (frontend_draw draws the button face from x - 4, its round cap 13 further left) */
#define ROW_SHIFT 37.0f     /* our rows' buttons this much further left: the cap of the selected one starts at x 8 (frontend_draw: box at
                             60 - 37 = 23, button_border moves g.x 2 left, the cap 13 left of that: 23 - 2 - 13 = 8) */

float mfl_prefs_row_shift(void) { return ROW_SHIFT; }

/* a row button that shows one of our captions (category list and sub-pages; not the Preferences row of the Options page) */
static int our_row(const struct MenuItemAnim *it)
{
  int id = anim_text_id(it);

  return it->item_id == (int)L_ROW && id > text_id(T_TITLE) && id < text_id(T_COUNT);
}

void mfl_prefs_menu_scales(void)
{
  int i;

  if (!installed) return;
  for (i = 0; i < 30; i++) {
    struct MenuItemAnim *it = &g_menu_item_anims[i];

    /* x as the scale below: from the text the slot shows now, so a slot moves when its caption changes between a game page and ours */
    if (our_row(it)) it->x = ROW_X - ROW_SHIFT;
    else if (it->item_id == (int)L_ROW && it->x == ROW_X - ROW_SHIFT) it->x = ROW_X;

    if (anim_row(it) < ROWS) it->scale = ROW_SCALE;
    /* a slot shows the old page's row text while it appears and the new page's once idle (one scale for both): when it changes
     * from one of our rows to a row of another page (sub-page -> category list: the first rows) give it back the game's scale */
    else if (it->item_id == (int)L_ROW && it->scale == ROW_SCALE) it->scale = ROW_DEFAULT_SCALE;
  }
}

/* the Resolution row while the page's Display mode is Borderless: that mode always has the desktop size (resolution= is ignored), so the row
   shows "Desktop" in grey without arrows and does not step; the stored resolution is kept for the other modes */
static int resolution_locked(int k) { return k_rows[k].key == KEY_RESOLUTION && values[MFL_PREF_DISPLAY_MODE] == MFL_DISPLAY_BORDERLESS; }

static void step(int k, int dir)
{
  int key = k_rows[k].key, n;

  if (resolution_locked(k)) return;
  if (key == KEY_RESOLUTION) {
    n = mfl_video_modes_api->count();
    mfl_video_modes_api->choose((mfl_video_modes_api->current() + dir + n) % n);
    return;
  }
  if (key == KEY_ADAPTER) {
    adapter_choose(((int)g_video_cfg.subsystem_index + dir + g_app_771500) % g_app_771500);
    quality_dirty = 1;
    return;
  }
#if MFL_FIX_CONTROLS
  if (key <= KEY_CTL_PLAYER) {
    ctl_step(key, dir);
    return;
  }
#endif
  if (key <= KEY_VC_SHADOWS) {      /* a videocfg.bin box: a toggle of its word (any non-zero value counts as set) */
    *quality_word(key) = *quality_word(key) == 0;
    quality_dirty = 1;
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
  if (k_rows[k].live == LIVE_NOW) mfl_prefs[key] = values[key];
#if MFL_FIX_CONTROLS
  if (key == MFL_PREF_PAD_DEADZONE) deadzone_apply();
#endif
}

/* leaving the page after a videocfg.bin row changed: the game's own writer saves g_video_cfg (the file the original dialog shares); the effects that only
 * gate a course load or a frame are copied to their globals in the frontend (no race running), the others take the file at the next start */
static void quality_save(void)
{
  if (g_app_state == APP_STATE_MAIN_LOOP_INIT) {
    g_course_5fb098 = (int)g_video_cfg.trilinear;
    g_race_64d8a0 = (int)g_video_cfg.no_env_effects;
    g_round_64d89c = (int)g_video_cfg.no_env_effects;
  }
  video_cfg_save();
  quality_dirty = 0;
}

/* the state a sub-page was entered with (taken on its first frame, also when a deeper page returns to it): Escape gives it back */
static struct {
  int values[MFL_PREF_COUNT], prefs[MFL_PREF_COUNT];      /* prefs: the LIVE_NOW keys and resolution= (mfl_video_modes_api->choose) */
  struct VideoCfg video;
#if MFL_FIX_CONTROLS
  struct ControllerCfg controllers[CONTROLLERS];      /* restored on the Controls page only: a captured key (Buttons / Direction keys) and
                                                         Reset are confirmed actions and stay (plan D2) */
#endif
} snap;

static void snapshot(void)
{
  int k;

  for (k = 0; k < MFL_PREF_COUNT; k++) { snap.values[k] = values[k]; snap.prefs[k] = mfl_prefs[k]; }
  snap.video = g_video_cfg;
#if MFL_FIX_CONTROLS
  for (k = 0; k < CONTROLLERS; k++) snap.controllers[k] = g_controller_cfg[k];
#endif
  dirty = quality_dirty = 0;
}

static void revert(int c)
{
  int k;

  for (k = 0; k < MFL_PREF_COUNT; k++) { values[k] = snap.values[k]; mfl_prefs[k] = snap.prefs[k]; }
  g_video_cfg = snap.video;
#if MFL_FIX_CONTROLS
  deadzone_apply();
  if (c == CAT_CONTROLS)
    for (k = 0; k < CONTROLLERS; k++) { g_controller_cfg[k] = snap.controllers[k]; controller_cfg_save_n(k); }      /* ctl_step wrote the files */
#endif
  dirty = quality_dirty = 0;
}

void mfl_prefs_menu_update(void)
{
  static unsigned int last_list;      /* the current record's list at the previous call (the Return that opened a page is not a step) */
  struct MenuRecord *rec = &g_menu_records[g_menu_current];
  int dir, r, c, k, was = last_list == rec->item_list;

  if (!installed) install();
  sync_slot();
#if MFL_FIX_CONTROLS
  if (capture_run()) return;     /* waiting for the new key of an action: the menu gets no input */
#endif
  last_list = rec->item_list;
  if (rec->page_id != MFL_PAGE_PREFS) return;
  c = current_sub();
  if (c < 0) return;      /* the category list: Return opens the slot's sub-page (menu_update) */
  if (!was) { snapshot(); return; }
  if (menu_back_pressed_any() && g_menu_select_pressed == 0) {      /* the game's back block runs right after this and goes up */
    if (dirty) revert(c);
    return;
  }
  r = rec->selected;
  if (r < 0 || r >= sub_count[c]) return;
  k = sub_rows[c][r];
  /* Return: menu_update set g_menu_select_pressed this frame (the rows' token 0xffffffff dispatches nothing; not MENU_ITEM_RESUME: its
   * dispatch runs before this hook and would pop the page first, agents/mw2_w17/S1b.md).  On a value row: accept and up one page as the
   * game's option pages do (their Select token RESUME = menu_page_enter(MENU_PAGE_PAUSE, 1); g_menu_select_pressed keeps the back block
   * from restoring anything).  On an action row (capture a key, Reset) it acts as before. */
  if (g_menu_select_pressed == 1) {
    if (!ROW_STEPS(k)) {
      step(k, 1);
#if MFL_FIX_CONTROLS
      if (k_rows[k].key == KEY_CTL_RESET) snap.controllers[ctl_player] = g_controller_cfg[ctl_player];
#endif
      return;
    }
    if (dirty) mfl_settings_save(values);
    if (quality_dirty) quality_save();
    dirty = 0;
    menu_page_enter(MENU_PAGE_PAUSE, 1);
    return;
  }
  dir = (menu_right_pressed_any() != 0) - (menu_left_pressed_any() != 0);
  if (dir == 0) return;
  step(k, dir);
  dirty = 1;
}

static int value_text(int k)
{
  int v = k_rows[k].key >= 0 ? values[k_rows[k].key] : 0, n = 0;
  const char *s;

  if (k_rows[k].key == KEY_RESOLUTION) {      /* "1280x720" from the video code */
    for (s = mfl_video_modes_api->name(resolution_locked(k) ? 0 : mfl_video_modes_api->current()); s[n] != '\0' && n < 23; n++) mode_buf[n + 1] = (unsigned char)s[n];
    mode_buf[0] = (unsigned short)n;
    return text_id(T_MODE_NAME);
  }
  if (k_rows[k].key == KEY_ADAPTER) {      /* the adapter's name from the engine, cut to the value column */
    for (s = g_video_7712ec + (int)g_video_cfg.subsystem_index % g_app_771500 * 0x50; s[n] != '\0' && n < 18; n++) adapter_buf[n + 1] = (unsigned char)s[n];
    adapter_buf[0] = (unsigned short)n;
    return text_id(T_ADAPTER_NAME);
  }
#if MFL_FIX_CONTROLS
  if (k_rows[k].key <= KEY_CTL_PLAYER) return ctl_value_text(k);
  if (k_rows[k].key == MFL_PREF_PAD_DEADZONE) {      /* 5 .. 50 */
    char number[3];

    number[0] = (char)('0' + (v + 1) / 2); number[1] = (char)('0' + (v + 1) % 2 * 5); number[2] = '\0';
    return row_value(k, number + (v == 0));
  }
#endif
  if (k_rows[k].key <= KEY_VC_SHADOWS) return quality_on(k_rows[k].key) ? TEXT_ON : TEXT_OFF;
  if (k_rows[k].key == MFL_PREF_MENU_SPEED) return text_id(T_X1 + speed_index(v));
  if (k_rows[k].key == MFL_PREF_FPS_CAP) return text_id(T_FPS_60 + v);
  if (k_rows[k].key == MFL_PREF_DISPLAY_MODE) return text_id(T_FULLSCREEN + v);
  if (k_rows[k].key == MFL_PREF_LANGUAGE) return text_id(v < 0 ? T_AUTO : T_ENGLISH + v);
  return (v != 0) != (k_rows[k].invert != 0) ? TEXT_ON : TEXT_OFF;
}

/* the arrows of menu_panel_draw (cl_menu_panel_draw_69ca08.c) left of the value at x - 20 and right after it */
static void arrows(int text, float x, float y_menu)
{
  float sw = (float)screen_width(), sh = (float)screen_height();
  float y = (y_menu - 8.0f) * sh * (1.0f / 480.0f), w = sw * 0.025f, h = sh * (1.0f / 30.0f);

  draw_sprite_uv(find_texture_dict_c("Arrow"), sw * (x - 20.0f) * (1.0f / 640.0f), y, w, h, 0xff000000u, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
  draw_sprite(find_texture_dict_c("Arrow"), sw * ((float)(int)text_width(text, VALUE_SCALE) + x + 6.0f) * (1.0f / 640.0f), y, w, h, 0xff000000u, 1);
}

/* a changed row that the running game does not follow before its next start (not Intro movies: they only play at a start anyway) */
static int restart_needed(void)
{
  int k, key, need = start_video.subsystem_index != g_video_cfg.subsystem_index
                     || (start_video.no_dynamic_shadows != 0) != (g_video_cfg.no_dynamic_shadows != 0)
                     || (start_video.no_multitexture != 0) != (g_video_cfg.no_multitexture != 0)
                     || (mfl_video_modes_api != 0 && mfl_video_modes_api->current() != start_mode);

  for (k = 0; k < ROWS; k++) {
    key = k_rows[k].key;
    need |= key >= 0 && key != MFL_PREF_SKIP_INTRO && k_rows[k].live == LIVE_START && values[key] != start_values[key];
  }
  return need;
}

/* s word-wrapped into line_buf from `line` on (greedy, by the game font's width at BOX_SCALE); returns the line after the last one */
static int wrap(const char *s, float width, int line)
{
  const char *e, *p;
  unsigned short *b;
  int n, len, k;

  for (; line < LINES; line++) {
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

/* one box line, made smaller when it is wider than the box */
static void box_line(int id, float y, unsigned int colour, float scale)
{
  float w = (float)text_width(id, scale);

  if (w > BOX_W - 2.0f * BOX_PAD) scale *= (BOX_W - 2.0f * BOX_PAD) / w;
  ui_text_draw_fade_alpha(id, BOX_X + BOX_PAD, y, colour, scale, 0, 0x1ff);
}

/* the box right of the rows (translucent dark panel): a heading, ids[0 .. lines) in white, after an empty line `extra` free lines for the
 * caller (returns their top y), after an empty line ids[lines .. lines + notes) (when the row applies) and, while a changed row waits for
 * it, the restart hint */
static float box(int heading, const int *ids, int lines, int notes, int extra)
{
  float y = BOX_Y + BOX_PAD + BOX_LINE * 0.5f;
  int i, gap = extra != 0 ? extra + 1 : 0, body = 1 + lines + gap + (notes != 0 ? notes + 1 : 0), restart = restart_needed();

  draw_rect(BOX_X, BOX_Y, BOX_W, BOX_PAD * 2.0f + BOX_LINE * (float)(body + (restart ? 2 : 0)), 0xb0101010u);
  box_line(text_id(heading), y, 0xfff0b040u, 0.55f);
  for (i = 0; i < lines; i++) box_line(ids[i], y + BOX_LINE * (float)(i + 1) + 4.0f, 0xffffffffu, BOX_SCALE);
  for (; i < lines + notes; i++) box_line(ids[i], y + BOX_LINE * (float)(i + 2 + gap), 0xffa0d0ffu, BOX_SCALE);
  if (restart) box_line(text_id(T_RESTART), y + BOX_LINE * (float)(body + 1), 0xff4040ffu, BOX_SCALE);
  return BOX_Y + BOX_PAD + BOX_LINE * (float)(lines + 2);
}

void mfl_prefs_menu_draw(void)
{
  struct MenuRecord *rec = &g_menu_records[g_menu_current];
  struct MenuItemAnim *it;
  int i, k, n, ids[LINES], sel = -1, idle = 0, c = installed ? current_sub() : -1, desc, heading, notes, preview = 0;
  float x, y;

  if (!installed) return;
  sync_slot();
  for (i = 0; i < 30; i++) {
    it = &g_menu_item_anims[i];
    if (it->item_id == (int)L_ROW && it->state != 0 && it->state != 2) idle = 1;      /* the current page's buttons are in place */
    k = anim_row(it);
    if (k == ROWS) continue;
    /* the value column of a page: right of its widest caption (the values get the room up to the description box), at most VALUE_X */
    x = it->x + caption_width[k_rows[k].cat] + VALUE_GAP;
    if (x > VALUE_X + it->x - ROW_X) x = VALUE_X + it->x - ROW_X;      /* the cap moves with the rows (ROW_SHIFT) */
    value_room = BOX_X - 26.0f - x;      /* the right arrow (6 + 16) ends before the box */
    ui_text_draw_fade_alpha(value_text(k), x, it->y, resolution_locked(k) ? 0xff808080u : 0xff000000u, VALUE_SCALE, 0, it->anim_counter);
    if (c >= 0 && it->state != 0 && it->state != 2 && rec->selected >= 0 && rec->selected < sub_count[c] && k == sub_rows[c][rec->selected]) {
      if (ROW_STEPS(k) && !resolution_locked(k)) arrows(value_text(k), x, it->y);
      sel = k;
    }
  }
  if (sel >= 0) {      /* a sub-page row: what it does and when it applies */
    desc = heading = k_rows[sel].text;
#if MFL_FIX_CONTROLS
    if (k_rows[sel].key <= KEY_BIND) {      /* an action row: the text of the row that opened its page; while it waits for the key, say so */
      desc = k_cat_text[k_rows[sel].cat];
      if (capture_phase == CAPTURE_WAIT) heading = T_PRESS;
    }
#endif
    n = wrap(k_desc[desc - T_DISPLAY][language()], BOX_W - 2.0f * BOX_PAD, 0);
#if MFL_FIX_CONTROLS
    preview = k_rows[sel].key == KEY_CTL_DIRECTIONS || k_rows[sel].cat == PAGE_DIRECTIONS ? PREVIEW_LINES : 0;
#endif
    notes = wrap(k_text[k_rows[sel].live == LIVE_NOW ? T_AT_ONCE : k_rows[sel].live == LIVE_RACE ? T_NEXT_RACE : T_NEXT_START][language()],
                 BOX_W - 2.0f * BOX_PAD, n) - n;
    for (i = 0; i < n + notes; i++) ids[i] = text_id(T_LINE0 + i);
    y = box(heading, ids, n, notes, preview);
#if MFL_FIX_CONTROLS
    if (preview != 0) direction_preview(BOX_X + BOX_PAD, y);
#endif
  } else if (idle && SHOWS(rec, cat_list) && rec->selected >= 0 && rec->selected < CATS) {      /* the category list: its rows */
    for (n = 0; n < sub_count[rec->selected] && n < LINES; n++) ids[n] = text_id(k_rows[sub_rows[rec->selected][n]].text);
    box(k_cat_text[rec->selected], ids, n, 0, 0);
  }
}
#endif
