// cluster-entries: rx_pipeline_set_render_callback rx_pipeline_set_callback_5003b0 rx_pipeline_set_callback_5009c0
/* Installed cluster rx_pipeline_set_render_callback (tools/cluster_install.py from output/cluster_pilot5/rx_pipeline_set_render_callback.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004fb460*/ extern void RxD3D9AllInOneSetRenderCallBack(struct RxPipelineNode *p0, RxD3D9AllInOneRenderCallBack p1);
/*004e7070*/ extern struct RxPipelineNode * RxPipelineFindNodeByName(struct RxPipeline *p0, RwChar *p1, struct RxPipelineNode *p2, RwInt32 *p3);
/* DECLS-END */
extern RxNodeDefinition *RxNodeDefinitionGetD3D9AtomicAllInOne(void);
extern RxNodeDefinition *RxNodeDefinitionGetD3D9WorldSectorAllInOne(void);
/* Cluster rx_pipeline_set_render_callback: sets the D3D9 all-in-one render callback on the node of a pipeline (found by the node definition's name). */

/* was: EAX = node definition (name pointer at +0), EDX = pipeline, callback on the stack */
void rx_pipeline_set_render_callback_n(RxNodeDefinition *definition, RxPipeline *pipeline, RxD3D9AllInOneRenderCallBack callback)
{
    RxPipelineNode *node = RxPipelineFindNodeByName(pipeline, RxNodeDefinition_name(definition), 0, 0);
    RxD3D9AllInOneSetRenderCallBack(node, callback);
}

void rx_pipeline_set_callback_5003b0(RxPipeline *pipeline, RxD3D9AllInOneRenderCallBack callback)
{
    rx_pipeline_set_render_callback_n(RxNodeDefinitionGetD3D9AtomicAllInOne(), pipeline, callback);
}

void rx_pipeline_set_callback_5009c0(RxPipeline *pipeline, RxD3D9AllInOneRenderCallBack callback)
{
    rx_pipeline_set_render_callback_n(RxNodeDefinitionGetD3D9WorldSectorAllInOne(), pipeline, callback);
}

/* entry thunk at the original address of rx_pipeline_set_render_callback: register inputs (eax,edx,s0) -> natural C rx_pipeline_set_render_callback_n; ret 0 */
__declspec(naked) void rx_pipeline_set_render_callback(void)
{
    __asm {
        push dword ptr [esp + 4]
        push edx
        push eax
        call rx_pipeline_set_render_callback_n
        add esp, 12
        ret
    }
}
