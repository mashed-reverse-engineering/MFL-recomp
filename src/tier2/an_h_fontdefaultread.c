#include "mfl_tier2.h"
/* RenderWare 2D toolkit (rt2d): FontDefaultRead (0x5663c0), the reader for the text font metrics files (<path><name>.met on
   each ';'-separated entry of the font path; the first line names the format METRICS1 / 2 / 3).  Written from the
   disassembly of MFL.exe (this build calls the file functions directly and passes four arguments to the metrics readers).
   RwFgets and the three metrics readers are trap rows the original calls here too (tools/delink/trap_caller_allow.txt). */

#define rt2dFONT_MEMORY_HINT 0x10190      /* rwMEMHINTDUR_FUNCTION | rwID_2DPLUGIN */
#define rt2dFONT_LINE_SIZE 256
#define rt2dFONT_FILENAME_SIZE 128

typedef struct Rt2dFont Rt2dFont;

/*007d2128*/ extern char *RwEngineInstance;
/*00947b60*/ extern char Rt2dGlobals[];
#define Rt2dFontPath (*(RwChar **)(Rt2dGlobals + 0x50c))      /* Rt2dGlobals.fontPath: ';'-separated directories */

/*0063a474*/ extern RwChar rt2dFontFileExtension[];      /* the metrics file suffix */
/*0062d914*/ extern RwChar rt2dFontFileMode[];           /* RwFopen access mode */
/*0063a468*/ extern RwChar rt2dFontMetrics1Tag[];        /* "METRICS1" */
/*0063a45c*/ extern RwChar rt2dFontMetrics2Tag[];        /* "METRICS2" */
/*0063a450*/ extern RwChar rt2dFontMetrics3Tag[];        /* "METRICS3" */

/*00565c80*/ extern Rt2dFont *FontCreate2d(void);
/*005662e0*/ extern RwBool Rt2dFontDestroy(Rt2dFont *font);
/*00560d10*/ extern void *RwFopen(const RwChar *name, const RwChar *access);
/*00560f80*/ extern RwChar *RwFgets(RwChar *buffer, RwInt32 maxLen, void *fp);
/*00560e70*/ extern RwInt32 RwFclose(void *fp);
/*005665a0*/ extern Rt2dFont *FontReadMetrics2d1(Rt2dFont *font, RwChar *filename, void *fp, RwChar *line);
/*00566aa0*/ extern Rt2dFont *FontReadMetrics2d2(Rt2dFont *font, RwChar *filename, void *fp, RwChar *line);
/*00567240*/ extern Rt2dFont *FontReadMetrics2d3(Rt2dFont *font, RwChar *filename, void *fp, RwChar *line);

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
Rt2dFont *FontDefaultRead(const RwChar *name)
{
  Rt2dFont *font, *result;
  RwChar *fullName;
  const RwChar *path;
  void *fp;
  RwInt32 length, i;
  RwChar c;
  RwChar line[rt2dFONT_LINE_SIZE];
  RwChar filename[rt2dFONT_FILENAME_SIZE];

  font = FontCreate2d();
  if (font == NULL) {
    return NULL;
  }
  length = rwstrlen(Rt2dFontPath);
  fullName = (RwChar *)RwMalloc(length + rwstrlen(name) + 5, rt2dFONT_MEMORY_HINT);
  if (fullName == NULL) {
    Rt2dFontDestroy(font);
    return NULL;
  }

  /* AttemptToOpenFont (inlined in the exe): <dir><name><ext> for each directory of the font path until one opens */
  path = Rt2dFontPath;
  fp = NULL;
  i = 0;
  do {
    c = *path;
    if (c != ';' && c != '\0') {
      fullName[i++] = c;
    } else {
      fullName[i] = '\0';
      rwstrcat(fullName, name);
      rwstrcat(fullName, rt2dFontFileExtension);
      fp = RwFopen(fullName, rt2dFontFileMode);
      i = 0;
    }
    path++;
  } while (c != '\0' && fp == NULL);

  result = NULL;
  if (fp != NULL) {
    RwFgets(line, sizeof(line), fp);
    if (!rwstrcmp(line, rt2dFontMetrics1Tag)) {
      result = FontReadMetrics2d1(font, filename, fp, line);
    } else if (!rwstrcmp(line, rt2dFontMetrics2Tag)) {
      result = FontReadMetrics2d2(font, filename, fp, line);
    } else if (!rwstrcmp(line, rt2dFontMetrics3Tag)) {
      result = FontReadMetrics2d3(font, filename, fp, line);
    }
    RwFclose(fp);
  }
  RwFree(fullName);

  if (result != font) {
    Rt2dFontDestroy(font);
    return NULL;
  }
  return font;
}
