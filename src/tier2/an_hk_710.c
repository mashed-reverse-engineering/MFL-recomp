#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*0042f860*/ extern int __cdecl texdict_load_named_b(char *p0, undefined4 p1, undefined4 p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004f6bf0*/ extern int FUN_004f6bf0(undefined4 p0);
/*006592f0*/ extern undefined4 g_load_6592f0;
/*00659340*/ extern undefined g_kc_659340;
/*00659448*/ extern undefined4 g_load_endpoint_659448;
static int endpoint_panel_attach_clump_r(int reg_eax, void *reg_ebx);
extern void endpoint_panel_attach_clump(void);
#define ENDPOINT_PANEL_COUNT (4)          /* panel slots, inferred from the loop bound */
#define ENDPOINT_RECORD_SIZE (0x114)      /* bytes per slot record in g_kc_659340 (inferred) */
#define ENDPOINT_RECORD_DWORDS (0x45)     /* same record in dwords, g_load_endpoint_659448: 0x45 * 4 == 0x114 */
__declspec(naked) int endpoint_panel_attach_clump_r(int reg_eax, void *reg_ebx)
{
    __asm {
        push ebx
        mov eax, dword ptr [esp + 8]
        mov ebx, dword ptr [esp + 12]
        call endpoint_panel_attach_clump
        pop ebx
        ret
    }
}

/* load_endpoint_panel */

/* endpointpanel.txd/.dff */

/*
 * Loads the endpoint panel texture dictionary (endpointpanel.txd) into g_load_6592f0 and the
 * model (endpointpanel.dff), then attaches a clump to each of the ENDPOINT_PANEL_COUNT slots.
 * Takes no parameters, returns nothing.
 * Per slot: the slot index is stored as the first dword of that slot's record in
 * g_load_endpoint_659448 (inferred), and the record in g_kc_659340 is passed to
 * endpoint_panel_attach_clump_r (inferred).
 */
void load_endpoint_panel(void)

{
  undefined4 endpoint_clump;
  undefined4 slot_clump;
  int slot;

  /* Load the texture dictionary, then clear the current dictionary (inferred). */
  g_load_6592f0 = texdict_load_named_b("endpointpanel.txd", 0x0, 0x0);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  endpoint_clump = clump_load_by_name("endpointpanel.dff", 0x0, 0x0);
  slot = 0;
  /* One pass per panel slot. */
  do {
    slot_clump = endpoint_clump;
    /* Slot 0 uses the loaded clump; every other slot gets FUN_004f6bf0(clump) (inferred: a copy). */
    if (slot != 0) {
      slot_clump = FUN_004f6bf0(endpoint_clump);
    }
    endpoint_panel_attach_clump_r(slot_clump,(void *)(&g_kc_659340 + slot * ENDPOINT_RECORD_SIZE));
    (&g_load_endpoint_659448)[slot * ENDPOINT_RECORD_DWORDS] = slot;
    slot = slot + 1;
  } while (slot < ENDPOINT_PANEL_COUNT);
  return;
}


