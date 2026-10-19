#include "mfl_tier2.h"
/*0069bc78*/ extern undefined g_game_content_69bc78;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* platform ids passed as `platform` (inferred from the prefix string each case uses) */
#define PLATFORM_NONE (1)
#define PLATFORM_PS2 (2)
#define PLATFORM_XBOX (3)
#define PLATFORM_PC (4)
#define PLATFORM_GAMECUBE (5)

/* game_content_exists_prefixed */

/* tries pc/ xbox/ ps2/ gamecube/ prefixes
 * Builds the content path for content_name: the platform's prefix (if any) followed by content_name,
 * stored in the global g_game_content_69bc78, and checks whether that path exists.
 * platform: PLATFORM_* id (inferred); PLATFORM_NONE uses content_name as is, unknown ids skip the build and check
 *           whatever is already in g_game_content_69bc78 (inferred).
 * out_path: optional buffer, receives the found path when non-NULL.
 * Returns 1 if the content exists, 0 otherwise.
 */

undefined4 game_content_exists_prefixed(RwChar *out_path,RwChar *content_name,undefined4 platform)

{
  int found;
  char *prefix;

  /* pick the prefix for the platform; PLATFORM_NONE copies the name and skips the prefix copy */
  switch(platform) {
  case PLATFORM_NONE:
    rwstrcpy(&g_game_content_69bc78,content_name);
    goto switchD_0042f62a_default;
  case PLATFORM_PS2:
    prefix = "ps2/";
    break;
  case PLATFORM_XBOX:
    prefix = "xbox/";
    break;
  case PLATFORM_PC:
    prefix = "pc/";
    break;
  case PLATFORM_GAMECUBE:
    prefix = "gamecube/";
    break;
  default:
    goto switchD_0042f62a_default;
  }
  rwstrcpy(&g_game_content_69bc78,prefix);
  rwstrcat(&g_game_content_69bc78,content_name);
switchD_0042f62a_default:
  /* the existence check (thunk_FUN_00561060, inferred) on the built path */
  found = thunk_FUN_00561060(&g_game_content_69bc78);
  if (found == 0) {
    return 0;
  }
  if (out_path != (RwChar *)0x0) {
    rwstrcpy(out_path,&g_game_content_69bc78);
  }
  return 1;
}


