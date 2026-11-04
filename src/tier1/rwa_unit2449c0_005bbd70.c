/* rwa_unit2449c0_005bbd70 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2449C0.c, functions sub_244DC0 (0x005bbbc0, static), sub_244F70 (0x005bbd70), sub_245160 (0x005bbf50), sub_2451B0 (0x005bbfa0)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_244F70 -> rwa_unit2449c0_005bbd70; sub_245160 -> rwa_unit2449c0_005bbf50; sub_2451B0 -> rwa_unit2449c0_005bbfa0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.00 and 13.10; MFL-specific changes are noted in the code). Static functions keep their
 * linkage: the overlay replaces only the external ones, the static copies serve the callers in this file. */
#include "rwa.h"
#include "rwaint.h"
#define rwaINFINITE     0xFFFFFFFF
extern void *dword_507180;
extern void *dword_507184;
extern rwaLink dword_507188;
extern rwaCacheRequest *dword_507190;
extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern RwBool sub_2416F0(void **semaphore);
extern void sub_244B40(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event,
                       rwaCacheClient *client);
extern void sub_245060(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event,
                rwaCacheClient *client);
extern void sub_244B40(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event, rwaCacheClient *client);
extern void sub_245060(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event, rwaCacheClient *client);

/* MFL.exe 0x005bbbc0 */
static void
sub_244DC0(rwaCacheClient *client, RwUInt32 size, void *dest, rwaCacheCallBack callback,
           void *data)
{
    rwaCacheRequest *request = &client->request;

    request->state = 3;
    request->file = client->holder->file;
    request->size = size;
    request->remaining = size;
    request->dest = (RwUInt8 *)dest;
    request->callback = callback;
    request->data = data;

    WaitForSingleObject(dword_507184, rwaINFINITE);

    if (sub_2416F0(&dword_507180))
    {
        dword_507190 = request;
        ReleaseSemaphore(dword_507184, 1, NULL);
        sub_244B40(request->file, request->size, 0, 1, client);

        return;
    }

    request->link.next = dword_507188.next;
    request->link.prev = &dword_507188;
    dword_507188.next->prev = &request->link;
    dword_507188.next = &request->link;
    request->state = 2;

    ReleaseSemaphore(dword_507184, 1, NULL);
}

/* MFL.exe 0x005bbd70 */
void
rwa_unit2449c0_005bbd70(rwaCacheClient *client, void *dest, RwUInt32 size, rwaCacheCallBack callback,
           void *data)
{
    client->callback = callback;
    client->data = data;
    client->mode = 3;
    sub_244DC0(client, size, dest, callback, data);
}

/* MFL.exe 0x005bbf50 */
void
rwa_unit2449c0_005bbf50(rwaCacheFile *holder, rwaCacheClient *client, RwUInt32 unk4C, RwUInt32 *header,
           rwaCacheCallBack callback, void *data)
{
    client->holder = holder;
    client->unk4C = unk4C;
    client->header = header;
    client->seeking = FALSE;
    client->callback = callback;
    client->data = data;
    client->mode = 2;
    sub_244DC0(client, 0xC, header, (rwaCacheCallBack)sub_245060, client);
}

/* MFL.exe 0x005bbfa0 */
void
rwa_unit2449c0_005bbfa0(rwaCacheClient *client, void *dest, rwaCacheCallBack callback, void *data)
{
    client->callback = callback;
    client->data = data;
    client->mode = 3;
    sub_244DC0(client, 0xC, dest, (rwaCacheCallBack)sub_245060, client);
}
