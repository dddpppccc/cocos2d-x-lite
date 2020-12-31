#pragma once

#include "../RenderStage.h"
#include "../../core/gfx/GFXFramebuffer.h"

namespace cc {
namespace pipeline {

class RenderFlow;
class Camera;
class RenderBatchedQueue;
class RenderInstancedQueue;
class RenderAdditiveLightQueue;
class PlanarShadowQueue;

class CC_DLL GbufferStage : public RenderStage {
public:
    static const RenderStageInfo &getInitializeInfo();

    GbufferStage();
    ~GbufferStage();

    virtual bool initialize(const RenderStageInfo &info) override;
    virtual void activate(RenderPipeline *pipeline, RenderFlow *flow) override;
    virtual void destroy() override;
    virtual void render(Camera *camera) override;

private:
    static RenderStageInfo _initInfo;
    PlanarShadowQueue *_planarShadowQueue = nullptr;
    RenderBatchedQueue *_batchedQueue = nullptr;
    RenderInstancedQueue *_instancedQueue = nullptr;
    gfx::Rect _renderArea;
    uint _phaseID = 0;
};

} // namespace pipeline
} // namespace cc