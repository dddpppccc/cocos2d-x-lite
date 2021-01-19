#pragma once

#include "../RenderFlow.h"

namespace cc {
namespace pipeline {

class Camera;
class GbufferStage;

class CC_DLL GbufferFlow : public RenderFlow {
public:
    static const RenderFlowInfo &getInitializeInfo();

    GbufferFlow() = default;
    virtual ~GbufferFlow();

    virtual bool initialize(const RenderFlowInfo &info) override;
    virtual void activate(RenderPipeline *pipeline) override;
    virtual void destroy() override;
    virtual void render(Camera *camera) override;
    gfx::Framebuffer *getFrameBuffer() {return _gbufferFrameBuffer;}

private:
    void createRenderPass(gfx::Device *device);
    void createRenderTargets(gfx::Device *device);

private:
    static RenderFlowInfo _initInfo;

    gfx::RenderPass *_gbufferRenderPass = nullptr;
    gfx::TextureList _gbufferRenderTargets;
    gfx::Framebuffer *_gbufferFrameBuffer = nullptr;
    gfx::Texture *_depth = nullptr;
    uint _width;
    uint _height;

    GbufferStage *_GbufferStage = nullptr;
};

} // namespace pipeline
} // namespace cc
