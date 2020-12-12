#pragma once
#include "base/Config.h"

#include "cocos/bindings/jswrapper/SeApi.h"

extern se::Object* __jsb_cc_pipeline_RenderQueueDesc_proto;
extern se::Class* __jsb_cc_pipeline_RenderQueueDesc_class;

bool js_register_cc_pipeline_RenderQueueDesc(se::Object* obj);
bool register_all_pipeline(se::Object* obj);

extern se::Object* __jsb_cc_pipeline_RenderWindow_proto;
extern se::Class* __jsb_cc_pipeline_RenderWindow_class;

bool js_register_cc_pipeline_RenderWindow(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_RenderWindow_getFramebuffer);

extern se::Object* __jsb_cc_pipeline_RenderPipelineInfo_proto;
extern se::Class* __jsb_cc_pipeline_RenderPipelineInfo_class;

bool js_register_cc_pipeline_RenderPipelineInfo(se::Object* obj);
bool register_all_pipeline(se::Object* obj);

extern se::Object* __jsb_cc_pipeline_RenderPipeline_proto;
extern se::Class* __jsb_cc_pipeline_RenderPipeline_class;

bool js_register_cc_pipeline_RenderPipeline(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_activate);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_render);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_getShadows);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_setValue);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_setShadows);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_initialize);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_destroy);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_getDevice);
SE_DECLARE_FUNC(js_pipeline_RenderPipeline_getInstance);

extern se::Object* __jsb_cc_pipeline_ForwardPipeline_proto;
extern se::Class* __jsb_cc_pipeline_ForwardPipeline_class;

bool js_register_cc_pipeline_ForwardPipeline(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_ForwardPipeline_setFog);
SE_DECLARE_FUNC(js_pipeline_ForwardPipeline_getSphere);
SE_DECLARE_FUNC(js_pipeline_ForwardPipeline_setRenderObjects);
SE_DECLARE_FUNC(js_pipeline_ForwardPipeline_setSkybox);
SE_DECLARE_FUNC(js_pipeline_ForwardPipeline_setAmbient);
SE_DECLARE_FUNC(js_pipeline_ForwardPipeline_ForwardPipeline);

extern se::Object* __jsb_cc_pipeline_RenderFlowInfo_proto;
extern se::Class* __jsb_cc_pipeline_RenderFlowInfo_class;

bool js_register_cc_pipeline_RenderFlowInfo(se::Object* obj);
bool register_all_pipeline(se::Object* obj);

extern se::Object* __jsb_cc_pipeline_RenderFlow_proto;
extern se::Class* __jsb_cc_pipeline_RenderFlow_class;

bool js_register_cc_pipeline_RenderFlow(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_RenderFlow_activate);
SE_DECLARE_FUNC(js_pipeline_RenderFlow_initialize);
SE_DECLARE_FUNC(js_pipeline_RenderFlow_getTag);

extern se::Object* __jsb_cc_pipeline_ForwardFlow_proto;
extern se::Class* __jsb_cc_pipeline_ForwardFlow_class;

bool js_register_cc_pipeline_ForwardFlow(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_ForwardFlow_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_ForwardFlow_ForwardFlow);

extern se::Object* __jsb_cc_pipeline_RenderStageInfo_proto;
extern se::Class* __jsb_cc_pipeline_RenderStageInfo_class;

bool js_register_cc_pipeline_RenderStageInfo(se::Object* obj);
bool register_all_pipeline(se::Object* obj);

extern se::Object* __jsb_cc_pipeline_RenderStage_proto;
extern se::Class* __jsb_cc_pipeline_RenderStage_class;

bool js_register_cc_pipeline_RenderStage(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_RenderStage_activate);
SE_DECLARE_FUNC(js_pipeline_RenderStage_initialize);
SE_DECLARE_FUNC(js_pipeline_RenderStage_getTag);
SE_DECLARE_FUNC(js_pipeline_RenderStage_getFlow);

extern se::Object* __jsb_cc_pipeline_ForwardStage_proto;
extern se::Class* __jsb_cc_pipeline_ForwardStage_class;

bool js_register_cc_pipeline_ForwardStage(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_ForwardStage_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_ForwardStage_ForwardStage);

extern se::Object* __jsb_cc_pipeline_ShadowFlow_proto;
extern se::Class* __jsb_cc_pipeline_ShadowFlow_class;

bool js_register_cc_pipeline_ShadowFlow(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_ShadowFlow_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_ShadowFlow_ShadowFlow);

extern se::Object* __jsb_cc_pipeline_ShadowStage_proto;
extern se::Class* __jsb_cc_pipeline_ShadowStage_class;

bool js_register_cc_pipeline_ShadowStage(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_ShadowStage_setUseData);
SE_DECLARE_FUNC(js_pipeline_ShadowStage_setFramebuffer);
SE_DECLARE_FUNC(js_pipeline_ShadowStage_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_ShadowStage_ShadowStage);

extern se::Object* __jsb_cc_pipeline_UIFlow_proto;
extern se::Class* __jsb_cc_pipeline_UIFlow_class;

bool js_register_cc_pipeline_UIFlow(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_UIFlow_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_UIFlow_UIFlow);

extern se::Object* __jsb_cc_pipeline_UIStage_proto;
extern se::Class* __jsb_cc_pipeline_UIStage_class;

bool js_register_cc_pipeline_UIStage(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_UIStage_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_UIStage_UIStage);

extern se::Object* __jsb_cc_pipeline_RenderView_proto;
extern se::Class* __jsb_cc_pipeline_RenderView_class;

bool js_register_cc_pipeline_RenderView(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_RenderView_setEnable);
SE_DECLARE_FUNC(js_pipeline_RenderView_setExecuteFlows);
SE_DECLARE_FUNC(js_pipeline_RenderView_onGlobalPipelineStateChanged);
SE_DECLARE_FUNC(js_pipeline_RenderView_getVisibility);
SE_DECLARE_FUNC(js_pipeline_RenderView_setVisibility);
SE_DECLARE_FUNC(js_pipeline_RenderView_getWindow);
SE_DECLARE_FUNC(js_pipeline_RenderView_initialize);
SE_DECLARE_FUNC(js_pipeline_RenderView_destroy);
SE_DECLARE_FUNC(js_pipeline_RenderView_getFlows);
SE_DECLARE_FUNC(js_pipeline_RenderView_setWindow);
SE_DECLARE_FUNC(js_pipeline_RenderView_RenderView);

extern se::Object* __jsb_cc_pipeline_InstancedBuffer_proto;
extern se::Class* __jsb_cc_pipeline_InstancedBuffer_class;

bool js_register_cc_pipeline_InstancedBuffer(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_InstancedBuffer_destroy);
SE_DECLARE_FUNC(js_pipeline_InstancedBuffer_setDynamicOffset);
SE_DECLARE_FUNC(js_pipeline_InstancedBuffer_get);
SE_DECLARE_FUNC(js_pipeline_InstancedBuffer_InstancedBuffer);

extern se::Object* __jsb_cc_pipeline_DeferredPipeline_proto;
extern se::Class* __jsb_cc_pipeline_DeferredPipeline_class;

bool js_register_cc_pipeline_DeferredPipeline(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_setHDR);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getShadows);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getDepth);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_updateUBOs);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getRenderObjects);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getLightIndices);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getSkybox);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_setFog);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getCommandBuffers);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getSphere);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getLightBuffers);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getShadowObjects);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getOrCreateRenderPass);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_setRenderObjects);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getQuadIA);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_isHDR);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getShadowFramebuffer);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_setShadowObjects);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getAmbient);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getFpScale);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_setDepth);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getLightIndexOffsets);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getLightsUBO);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getFog);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getShadingScale);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_setSkybox);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_setAmbient);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_getValidLights);
SE_DECLARE_FUNC(js_pipeline_DeferredPipeline_DeferredPipeline);

extern se::Object* __jsb_cc_pipeline_GbufferFlow_proto;
extern se::Class* __jsb_cc_pipeline_GbufferFlow_class;

bool js_register_cc_pipeline_GbufferFlow(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_GbufferFlow_getFrameBuffer);
SE_DECLARE_FUNC(js_pipeline_GbufferFlow_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_GbufferFlow_GbufferFlow);

extern se::Object* __jsb_cc_pipeline_GbufferStage_proto;
extern se::Class* __jsb_cc_pipeline_GbufferStage_class;

bool js_register_cc_pipeline_GbufferStage(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_GbufferStage_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_GbufferStage_GbufferStage);

extern se::Object* __jsb_cc_pipeline_LightingFlow_proto;
extern se::Class* __jsb_cc_pipeline_LightingFlow_class;

bool js_register_cc_pipeline_LightingFlow(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_LightingFlow_getLightingFrameBuffer);
SE_DECLARE_FUNC(js_pipeline_LightingFlow_createRenderPass);
SE_DECLARE_FUNC(js_pipeline_LightingFlow_createFrameBuffer);
SE_DECLARE_FUNC(js_pipeline_LightingFlow_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_LightingFlow_LightingFlow);

extern se::Object* __jsb_cc_pipeline_LightingStage_proto;
extern se::Class* __jsb_cc_pipeline_LightingStage_class;

bool js_register_cc_pipeline_LightingStage(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_LightingStage_initLightingBuffer);
SE_DECLARE_FUNC(js_pipeline_LightingStage_getInitializeInfo);
SE_DECLARE_FUNC(js_pipeline_LightingStage_LightingStage);

extern se::Object* __jsb_cc_pipeline_PostprocessStage_proto;
extern se::Class* __jsb_cc_pipeline_PostprocessStage_class;

bool js_register_cc_pipeline_PostprocessStage(se::Object* obj);
bool register_all_pipeline(se::Object* obj);
SE_DECLARE_FUNC(js_pipeline_PostprocessStage_PostprocessStage);

