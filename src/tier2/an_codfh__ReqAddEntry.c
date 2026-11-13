/* _ReqAddEntry (MFL.exe 004f2590). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2dep.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helper _ReqSearch4Cluster inlined by hand (MFL.exe has it inlined), the function under its codfh name _ReqAddEntry (as the callers
 * use it), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the
 * release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses.
 * Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct rxReq rxReq;
/* ---- end of the generated declarations ---- */

typedef struct RwReqEntry RwReqEntry;
typedef struct RwScopeTrace RwScopeTrace;
struct rxReq
{
    RwUInt32             numused;
    RwUInt32             numalloced;
    RwUInt32             unavailslots;
        
    RwReqEntry          *reqentries;
        
    RxPipelineNode      *node;
};
struct RwReqEntry
{
        
    RxClusterDefinition *clusterDef;
    RxClusterValidityReq required;
    RwUInt32             inputs;

    RwScopeTrace        *st;

    RwReqEntry          *next;
        
    rxReq               *req;
    RwUInt32             outbf;
    RwUInt32             assignedslot;

    RxPipelineNode      *originatingNode;
};
struct RwScopeTrace
{
        
    RwReqEntry          *head;
        
    RwScopeTrace        *continuation;
        
    RwScopeTrace        *next;

    RwScopeTrace        *parent;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rxReq, reqentries) == 0xc && sizeof(RwReqEntry) == 0x24 && RW_OFFSETOF(RwReqEntry, inputs) == 8 && RW_OFFSETOF(RwReqEntry, st) == 0xc && RW_OFFSETOF(RwReqEntry, next) == 0x10 && RW_OFFSETOF(RwReqEntry, req) == 0x14 && RW_OFFSETOF(RwReqEntry, outbf) == 0x18 && RW_OFFSETOF(RwReqEntry, assignedslot) == 0x1c && RW_OFFSETOF(RwReqEntry, originatingNode) == 0x20);      /* MFL.exe 0x4f25a0 reqentries [esi+0xc], 0x4f25b8 stride 0x24 (ecx*9*4), 0x4f25c9..0x4f25f4 the nine fields */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(RwScopeTrace) == 0x10);      /* MFL.exe 0x4e65bb (RxLockedPipeUnlock): per node and cluster 0x24 + 0x10 */

/* TIER2: unvalidated authority=diffemu */
RwReqEntry * _ReqAddEntry(rxReq *req, RxClusterDefinition *clusterDef, RxClusterValidityReq required, RwUInt32 inputs, RxPipelineNode *originatingNode)
{
    RwReqEntry *reqentry = (RwReqEntry *) NULL;

    /* _ReqSearch4Cluster(req, clusterDef), static in p2dep.c, inlined in MFL.exe */
    if (req->numused != 0)
    {
        RwReqEntry *entry = &(req->reqentries[0]);
        RwUInt32    n;

        for (n = 0;n < req->numused;++n)
        {
            if (entry->clusterDef == clusterDef)
            {
                reqentry = entry;
                break;
            }
            ++entry;
        }
    }

    if (reqentry != NULL)
    {
        if (required == rxCLREQ_REQUIRED)
        {
            reqentry->required = rxCLREQ_REQUIRED;
            reqentry->originatingNode = originatingNode;
        }

        return (reqentry);
    }

    reqentry = &req->reqentries[req->numused];
    ++req->numused;

    reqentry->clusterDef = clusterDef;
    reqentry->required = required;
    reqentry->inputs = inputs;
    reqentry->next = (RwReqEntry *)NULL;
    reqentry->req = req;
    reqentry->st = (RwScopeTrace *)NULL;
    reqentry->outbf = 0U;
    reqentry->assignedslot = (RwUInt32) - 1;
    reqentry->originatingNode = originatingNode;

    return (reqentry);
}
