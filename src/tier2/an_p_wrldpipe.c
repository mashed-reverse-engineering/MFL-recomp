/* The D3D9 world / atomic platform pipeline module of MFL.exe (RenderWare 3.7 wrldpipe, D3D9 build): _rpD3D9AddDynamicGeometry (00504430),
 * _rpD3D9RemoveDynamicGeometry (005044a0), _rxWorldDevicePluginAttach (00504500), _rpCreatePlatformWorldSectorPipelines (00504570),
 * _rpDestroyPlatformWorldSectorPipelines (005045e0), _rpCreatePlatformAtomicPipelines (00504620), _rpDestroyPlatformAtomicPipelines (005046a0).
 * Written from the MFL.exe disassembly (lane P W8, _rpD3D9AddDynamicGeometry lane P W9; the codfh-win32 file of this module is the Xbox one,
 * so it was not used); RenderWare SDK types from rw37_slim.h.  Compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"

/* the dynamic geometry list: one node per object whose vertex buffers are rebuilt on a device restore */
typedef struct rpD3D9DynamicGeometry rpD3D9DynamicGeometry;
struct rpD3D9DynamicGeometry
{
    void                   *object;
    rpD3D9DynamicGeometry  *next;
};
RW_LAYOUT_ASSERT(p_wp_0, sizeof(rpD3D9DynamicGeometry) == 8 && RW_OFFSETOF(rpD3D9DynamicGeometry, next) == 4);      /* MFL.exe 0x5044b3 [eax] object, 0x5044b9 [eax+4] next */
extern rpD3D9DynamicGeometry *D3D9DynamicGeometry;
extern RwFreeList *D3D9DynamicGeometryFreeList;

typedef void (*rwD3D9DeviceRestoreCallBack)(void);
extern rwD3D9DeviceRestoreCallBack D3D9OldRestoreDeviceCallback;
extern void D3D9WorldRestoreCallback(void);
extern void _rwD3D9DeviceSetRestoreCallback(rwD3D9DeviceRestoreCallBack callback);
extern rwD3D9DeviceRestoreCallBack _rwD3D9DeviceGetRestoreCallback(void);
extern RwBool _rpD3D9LightPluginAttach(void);
extern RwBool _rpD3D9UsageFlagPluginAttach(void);

extern RxPipeline *RxPipelineCreate(void);
extern void _rxPipelineDestroy(RxPipeline *pipeline);
typedef RxPipeline RxLockedPipe;
extern RxLockedPipe *RxPipelineLock(RxPipeline *pipeline);
extern RxPipeline *RxLockedPipeUnlock(RxLockedPipe *pipeline);
extern RxLockedPipe *RxLockedPipeAddFragment(RxLockedPipe *pipeline, RwUInt32 *firstIndex, RxNodeDefinition *nodeDef0, ...);
extern RxNodeDefinition *RxNodeDefinitionGetD3D9WorldSectorAllInOne(void);
extern RxNodeDefinition *RxNodeDefinitionGetD3D9AtomicAllInOne(void);
extern RxPipeline *RpWorldSetDefaultSectorPipeline(RxPipeline *pipeline);
extern RxPipeline *RpAtomicSetDefaultPipeline(RxPipeline *pipeline);
extern RwBool _rwD3D9LightsOpen(void);
extern void _rwD3D9LightsClose(void);
extern RwBool _rpD3D9VertexShaderCacheOpen(void);
extern void _rpD3D9VertexShaderCachePurge(void);
extern RwBool RwFreeListDestroy(RwFreeList *freelist);

#define rpWRLDPIPE_WORLDPLUGINID 2      /* pipe->pluginId = rwID_WORLDPLUGIN (MFL.exe 0x50457e / 0x50462e) */

extern RwFreeList *RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint);
#define rpWRLDPIPE_DYNGEOM_PER_BLOCK 128      /* MFL.exe 0x504459 */
#define rpWRLDPIPE_DYNGEOM_ALIGN 4            /* MFL.exe 0x504457 */
#define rpWRLDPIPE_DYNGEOM_HINT (rwMEMHINTDUR_EVENT | 0x50d)      /* MFL.exe 0x504452 / 0x50446d: event duration, module id 0x50d */

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
rpD3D9DynamicGeometry * _rpD3D9AddDynamicGeometry(void *object)
{
    rpD3D9DynamicGeometry *cur;
    RwFreeList *freeList;

    for (cur = D3D9DynamicGeometry; cur != NULL; cur = cur->next)
    {
        if (cur->object == object)
        {
            return (cur);      /* already listed */
        }
    }

    freeList = D3D9DynamicGeometryFreeList;
    if (freeList == NULL)
    {
        freeList = RwFreeListCreate(sizeof(rpD3D9DynamicGeometry), rpWRLDPIPE_DYNGEOM_PER_BLOCK, rpWRLDPIPE_DYNGEOM_ALIGN,
                                    rpWRLDPIPE_DYNGEOM_HINT);
        D3D9DynamicGeometryFreeList = freeList;
    }

    cur = (rpD3D9DynamicGeometry *)RwFreeListAlloc(freeList, rpWRLDPIPE_DYNGEOM_HINT);      /* no NULL test, as in MFL.exe */
    cur->object = object;
    cur->next = D3D9DynamicGeometry;
    D3D9DynamicGeometry = cur;

    return (cur);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwFreeList * _rpD3D9RemoveDynamicGeometry(void *object)
{
    rpD3D9DynamicGeometry *head = D3D9DynamicGeometry;
    rpD3D9DynamicGeometry *prev = (rpD3D9DynamicGeometry *)NULL;
    rpD3D9DynamicGeometry *cur;

    for (cur = head; cur != NULL; prev = cur, cur = cur->next)
    {
        if (cur->object == object)
        {
            if (cur == head)
            {
                D3D9DynamicGeometry = cur->next;
            }
            else
            {
                prev->next = cur->next;
            }
            cur->object = NULL;
            return (RwFreeListFree(D3D9DynamicGeometryFreeList, cur));
        }
    }
    return ((RwFreeList *)NULL);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool _rxWorldDevicePluginAttach(void)
{
    RwBool result;

    result = (_rpD3D9LightPluginAttach() && _rpD3D9UsageFlagPluginAttach()) ? TRUE : FALSE;

    D3D9OldRestoreDeviceCallback = _rwD3D9DeviceGetRestoreCallback();
    _rwD3D9DeviceSetRestoreCallback(D3D9WorldRestoreCallback);

    return (result);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool _rpCreatePlatformWorldSectorPipelines(void)
{
    RxPipeline *pipe;

    pipe = RxPipelineCreate();
    if (pipe)
    {
        RxLockedPipe *lpipe;

        pipe->pluginId = rpWRLDPIPE_WORLDPLUGINID;
        lpipe = RxPipelineLock(pipe);
        if (lpipe &&
            RxLockedPipeUnlock(RxLockedPipeAddFragment(lpipe, (RwUInt32 *)NULL, RxNodeDefinitionGetD3D9WorldSectorAllInOne(), (RxNodeDefinition *)NULL)))
        {
            RXPIPELINEGLOBAL(platformWorldSectorPipeline) = pipe;
            RpWorldSetDefaultSectorPipeline(pipe);
            return (TRUE);
        }
        _rxPipelineDestroy(pipe);
    }
    return (FALSE);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void _rpDestroyPlatformWorldSectorPipelines(void)
{
    RpWorldSetDefaultSectorPipeline((RxPipeline *)NULL);
    if (RXPIPELINEGLOBAL(platformWorldSectorPipeline))
    {
        _rxPipelineDestroy(RXPIPELINEGLOBAL(platformWorldSectorPipeline));
        RXPIPELINEGLOBAL(platformWorldSectorPipeline) = (RxPipeline *)NULL;
    }
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool _rpCreatePlatformAtomicPipelines(void)
{
    RxPipeline *pipe;

    pipe = RxPipelineCreate();
    if (pipe)
    {
        RxLockedPipe *lpipe;

        pipe->pluginId = rpWRLDPIPE_WORLDPLUGINID;
        lpipe = RxPipelineLock(pipe);
        if (lpipe &&
            RxLockedPipeUnlock(RxLockedPipeAddFragment(lpipe, (RwUInt32 *)NULL, RxNodeDefinitionGetD3D9AtomicAllInOne(), (RxNodeDefinition *)NULL)))
        {
            RXPIPELINEGLOBAL(platformAtomicPipeline) = pipe;
            RpAtomicSetDefaultPipeline(pipe);
            if (_rwD3D9LightsOpen())
            {
                _rpD3D9VertexShaderCacheOpen();
                return (TRUE);
            }
        }
        _rxPipelineDestroy(pipe);
    }
    return (FALSE);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void _rpDestroyPlatformAtomicPipelines(void)
{
    rpD3D9DynamicGeometry *cur = D3D9DynamicGeometry;

    if (cur)
    {
        do
        {
            cur->object = NULL;
            RwFreeListFree(D3D9DynamicGeometryFreeList, cur);
            cur = cur->next;      /* read after the free, as in MFL.exe (0x5046c4) */
        }
        while (cur);
        D3D9DynamicGeometry = cur;
    }

    if (D3D9DynamicGeometryFreeList)
    {
        RwFreeListDestroy(D3D9DynamicGeometryFreeList);
        D3D9DynamicGeometryFreeList = (RwFreeList *)NULL;
    }

    _rpD3D9VertexShaderCachePurge();
    _rwD3D9LightsClose();

    RpAtomicSetDefaultPipeline((RxPipeline *)NULL);
    if (RXPIPELINEGLOBAL(platformAtomicPipeline))
    {
        _rxPipelineDestroy(RXPIPELINEGLOBAL(platformAtomicPipeline));
        RXPIPELINEGLOBAL(platformAtomicPipeline) = (RxPipeline *)NULL;
    }
}
