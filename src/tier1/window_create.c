#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall AdjustWindowRect(int, int, int);
extern __declspec(dllimport) int __stdcall CoInitialize(int);
extern __declspec(dllimport) int __stdcall CreateWindowExA(int, int, int, int, int, int, int, int, int, int, int, int);
extern __declspec(dllimport) int __stdcall GetStockObject(int);
extern __declspec(dllimport) int __stdcall LoadCursorA(int, int);
extern __declspec(dllimport) int __stdcall RegisterClassA(int);
extern char LAB_004a9830[];
/*004a8830*/ extern int app_get_app_62c5c8();
/*004a8840*/ extern int app_get_app_62c5cc();
/*004a94d0*/ extern int os_version_log();
/*00771908*/ extern undefined g_app_771908;
/*007e7700*/ extern undefined4 g_app_7e7700;
/*007e7704*/ extern undefined4 g_app_7e7704;
#define IDC_ARROW_ID (0x7f00)
#define WINDOW_STYLE_OVERLAPPED (0xcf0000)
#define CREATE_USE_DEFAULT (0x80000000)
#define WNDCLASSA_WORD_COUNT (10)
/* Registers the "DtS" window class and creates the main game window titled
 * "Drive to Survive [D3D9] [Release]".
 * hinstance: module instance handle; also stored in g_app_7e7700.
 * unused_arg: not referenced (inferred: leftover parameter).
 * app_name: NUL-terminated string copied into g_app_771908 (inferred: app name buffer).
 * The window client size comes from app_get_app_62c5c8 (width) and app_get_app_62c5cc (height) (inferred).
 * Returns 1 on success, 0 if RegisterClassA fails. */
undefined4 window_create(HINSTANCE hinstance,char *unused_arg,char *app_name)
{
  char ch;
  ATOM atom;
  int i;
  WNDCLASSA *wc_cursor;
  tagRECT window_rect;
  WNDCLASSA wc;

  /* copy app_name (including the terminating NUL) into g_app_771908 (inferred) */
  i = (char *)&g_app_771908 - (char *)app_name;
  do {
    ch = *app_name;
    app_name[i] = ch;
    app_name = app_name + 1;
  } while (ch != '\0');
  CoInitialize((LPVOID)0x0);
  /* zero the whole WNDCLASSA before filling in the fields we use */
  wc_cursor = &wc;
  for (i = WNDCLASSA_WORD_COUNT; i != 0; i = i + -1) {
    wc_cursor->style = 0;
    wc_cursor = (WNDCLASSA *)&wc_cursor->lpfnWndProc;
  }
  wc.style = 0x2000;
  wc.hInstance = hinstance;
  wc.lpszClassName = "DtS";
  wc.lpfnWndProc = (WNDPROC)&LAB_004a9830;
  wc.hIcon = (HICON)0x0;
  wc.hCursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)IDC_ARROW_ID);
  wc.lpszMenuName = (LPCSTR)0x0;
  wc.hbrBackground = GetStockObject(0);
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  atom = RegisterClassA(&wc);
  if (atom == 0) {
    return 0;
  }
  /* client rect at (0,0) sized to the app dimensions; expand it to the outer
     window rect for WINDOW_STYLE_OVERLAPPED (menu flag: lpszMenuName != NULL) */
  window_rect.left = 0;
  window_rect.top = 0;
  window_rect.right = app_get_app_62c5c8();
  window_rect.bottom = app_get_app_62c5cc();
  if (wc.lpszMenuName != (LPCSTR)0x0) {
    AdjustWindowRect(&window_rect,WINDOW_STYLE_OVERLAPPED,1);
  }
  else {
    AdjustWindowRect(&window_rect,WINDOW_STYLE_OVERLAPPED,0);
  }
  /* create at default position (CREATE_USE_DEFAULT) with the expanded size; remember the handle in g_app_7e7704 */
  g_app_7e7704 = CreateWindowExA(0,"DtS","Drive to Survive [D3D9] [Release]",WINDOW_STYLE_OVERLAPPED,-CREATE_USE_DEFAULT,
                                 -CREATE_USE_DEFAULT,window_rect.right - window_rect.left,
                                 window_rect.bottom - window_rect.top,(HWND)0x0,(HMENU)0x0,hinstance,
                                 (LPVOID)0x0);
  g_app_7e7700 = hinstance;
  os_version_log();
  return 1;
}
