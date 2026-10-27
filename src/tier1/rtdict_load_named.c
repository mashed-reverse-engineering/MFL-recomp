#include "mfl_tier2.h"
extern int _rpUVAnimDictSchema;
extern char *PTR_g_rtdict_load_5de918_00609a0c;
/*0042f6e0*/ extern int game_content_find();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*005693c0*/ extern struct RtDict * __cdecl RtDictSchemaStreamReadDict(struct RtDictSchema *p0, struct RwStream *p1);
/*0069c478*/ extern undefined g_rtdict_load_69c478;
/*0069c678*/ extern undefined g_rtdict_load_69c678;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* character that marks a file extension in the requested name ('.') */
#define FILE_EXTENSION_SEPARATOR (0x2e)
/* chunk id searched for in the stream before reading the dict (inferred: header enum maps 43 to rwID_COLLTREE, verify against the schema) */
#define DICT_STREAM_CHUNK_TYPE (0x2b)
/* rtdict_load_named */
/* "game_content_find + RtDictSchemaStreamReadDict" */
/*
 * Loads the named RtDict (read with _rpUVAnimDictSchema) from a game content file.
 * name: requested file name, copied into the scratch buffer g_rtdict_load_69c678.
 * length_out / version_out: receive the length and version of the dict chunk (inferred).
 * st_arg4: stack argument, unused here.
 * Returns the dict, or NULL if the content file is not found, cannot be opened, or has no dict chunk.
 */
RtDict * __cdecl rtdict_load_named(char *name, RwUInt32 *length_out, RwUInt32 *version_out, undefined4 st_arg4)
{
  char *extension_dot;
  int content_found;
  RwStream *stream;
  RwBool chunk_found;
  RtDict *result;

  result = (RtDict *)0x0;
  rwstrcpy(&g_rtdict_load_69c678,name);
  /* no '.' in the name: append the default extension */
  extension_dot = _strchr(name,FILE_EXTENSION_SEPARATOR);
  if (extension_dot == (char *)0x0) {
    rwstrcat
              (&g_rtdict_load_69c678,PTR_g_rtdict_load_5de918_00609a0c);
  }
  /* resolve the name to a content file; return NULL if it is missing */
  content_found = game_content_find(&g_rtdict_load_69c478,&g_rtdict_load_69c678);
  if (content_found != 0) {
    stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,&g_rtdict_load_69c478);
    if (stream != (RwStream *)0x0) {
      /* find the dict chunk; on success its length and version are written to the out params */
      chunk_found = RwStreamFindChunk(stream,DICT_STREAM_CHUNK_TYPE,length_out,version_out);
      if (chunk_found != 0) {
        result = RtDictSchemaStreamReadDict((RtDictSchema *)&_rpUVAnimDictSchema,stream);
      }
      RwStreamClose(stream,(void *)0x0);
    }
    return result;
  }
  return (RtDict *)0x0;
}
