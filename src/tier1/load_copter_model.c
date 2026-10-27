#include "mfl_tier2.h"
/*00429c40*/ extern int clump_check_atomics_lookup();
/*00488230*/ extern int __cdecl clump_load_logged(void *p0, RwChar *p1);
/*004b2a60*/ extern int sprintf();
/*004c2b6f*/ extern int __cdecl __stricmp(char *p0, char *p1);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*005de408*/ extern undefined g_game_5de408;
/*00664414*/ extern undefined4 g_load_664414;
/*008c8860*/ extern undefined4 g_course_8c8860[0x1000];
/*008c8ae0*/ extern char g_player_stats_race[];
#define MODEL_SLOT_WORDS (0x14) /* 20 words per model slot in g_course_8c8860 */
#define MODEL_SLOT_KIND (0x13) /* word in the slot holding the model kind (inferred) */
#define MODEL_PATH_SIZE (64) /* size of the file path buffer */
/* load_copter_model */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* JetRanger */
/* Loads the copter model named by model_name (JetRanger, Copter or CivHuey) into the first free
   model slot of g_course_8c8860 (the global name says course, here it holds model slots) and stores
   the model kind in that slot. Returns 1 always. If no slot is free, free_slot stays NULL and the
   store to *free_slot crashes. */
undefined4 load_copter_model(char *model_name)
{
  int *slot_scan;
  undefined4 clump;
  int slot_index;
  undefined4 *free_slot;
  undefined1 path [MODEL_PATH_SIZE];
  
  free_slot = (undefined4 *)0x0;
  /* Make the texture dictionary g_load_664414 current while the model loads; cleared at the end. */
  RwTexDictionarySetCurrent(g_load_664414);
  /* Scan the slots from the start of the table for the first one whose first word is 0.
     The table ends where g_player_stats_race begins. */
  slot_index = 0;
  slot_scan = g_course_8c8860;
  do {
    if (*slot_scan == 0) {
      free_slot = g_course_8c8860 + slot_index * MODEL_SLOT_WORDS;
      break;
    }
    slot_scan = slot_scan + MODEL_SLOT_WORDS;
    slot_index = slot_index + 1;
  } while ((int)slot_scan < ((int)&g_player_stats_race));
  /* Path is ToastArt/common/perm/Copters/<model_name><g_game_5de408> (tail inferred: file extension). */
  sprintf(path,"%s%s%s%s","ToastArt/common/perm/","Copters/",model_name,&g_game_5de408);
  clump = clump_load_logged(path,(RwChar *)0x0);
  *free_slot = clump;
  clump_check_atomics_lookup(free_slot);
  /* Model kind: 0 JetRanger, 1 Copter, 2 CivHuey. slot_index is reused as the __stricmp result
     from here on. The CivHuey branch is branchless (inferred from the bit trick, check
     disassembly): a CivHuey match gives 2, any other name gives -1. */
  slot_index = __stricmp(model_name,"JetRanger");
  if (slot_index == 0) {
    free_slot[MODEL_SLOT_KIND] = 0;
  }
  else {
    slot_index = __stricmp(model_name,"Copter");
    if (slot_index == 0) {
      free_slot[MODEL_SLOT_KIND] = 1;
    }
    else {
      slot_index = __stricmp(model_name,"CivHuey");
      free_slot[MODEL_SLOT_KIND] = (slot_index != 0) ? -1 : 2;
    }
  }
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  return 1;
}
