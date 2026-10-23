#include "mfl_tier2.h"
/*004b2a60*/ extern int sprintf();
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/* compile: /GS */

/* A texture animation: circular singly linked list of frames, header {first, last, frame count, unused}. */
typedef struct TexFrame { struct TexFrame *next; struct RwTexture *texture; } TexFrame;
typedef struct TexAnim { TexFrame *first; TexFrame *last; int count; int reserved; } TexAnim;

/* course_helper_4849e0 */

/*
 * Builds a texture animation from the numbered textures "<name>0", "<name>1", ... of a texture
 * dictionary (names are the base name followed by a two digit frame number).
 * Frames are collected until the next number is missing; the list is made circular and the frame
 * count stored. Allocated with the RenderWare memory functions. Returns the animation header.
 */
/* TIER2: unvalidated 2026-10-23 authority=diffemu */
TexAnim *course_helper_4849e0(char *base_name, RwTexDictionary *dict)
{
  TexAnim *anim;
  TexFrame *frame;
  TexFrame *next;
  struct RwTexture *texture;
  int index;
  RwChar name [64];

  anim = (TexAnim *)RwMalloc(sizeof(TexAnim),0);
  anim->first = 0;
  anim->last = 0;
  anim->count = 0;
  anim->reserved = 0;
  frame = (TexFrame *)RwMalloc(sizeof(TexFrame),0);
  anim->first = frame;
  anim->last = frame;
  sprintf(name,"%s%.2d",base_name,0);
  texture = RwTexDictionaryFindNamedTexture(dict,name);
  if (texture != 0) {
    frame->texture = texture;
    frame->next = 0;
  }
  index = 1;
  while (1) {
    sprintf(name,"%s%.2d",base_name,index);
    texture = RwTexDictionaryFindNamedTexture(dict,name);
    if (texture == 0) break;
    next = (TexFrame *)RwMalloc(sizeof(TexFrame),0);
    frame->next = next;
    next->texture = texture;
    next->next = 0;
    index = index + 1;
    frame = next;
  }
  frame->next = anim->first;
  anim->count = index;
  return anim;
}
