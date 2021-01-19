#include "GbufferFlow.h"
#include "DeferredPipeline.h"
#include "GbufferStage.h"
#include "../SceneCulling.h"
#include "../../core/gfx/GFXDevice.h"
#include "../../core/gfx/GFXDescriptorSet.h"
#include "../../core/gfx/GFXSampler.h"
#include "gfx/GFXRenderPass.h"

namespace cc {
namespace pipeline {
RenderFlowInfo GbufferFlow::_initInfo = {
    "GbufferFlow",
    static_cast<uint>(DeferredFlowPriority::GBUFFER),
    static_cast<uint>(RenderFlowTag::SCENE),
    {},
};
const RenderFlowInfo &GbufferFlow::getInitializeInfo() { return GbufferFlow::_initInfo; }

GbufferFlow::~GbufferFlow() {
}

bool GbufferFlow::initialize(const RenderFlowInfo &info) {
    RenderFlow::initialize(info);

    if (_stages.size() == 0) {
        GbufferStage *gbufferStage = CC_NEW(GbufferStage);
        gbufferStage->initialize(GbufferStage::getInitializeInfo());
        _stages.emplace_back(gbufferStage);
    }

    return true;
}

void GbufferFlow::createRenderPass(gfx::Device *device) {
    if (_gbufferRenderPass != nullptr) {
        return;
    }

    gfx::RenderPassInfo info;

    gfx::ColorAttachment color = {
        gfx::Format::RGBA16F,
        1,
        gfx::LoadOp::CLEAR,
        gfx::StoreOp::STORE,
        gfx::TextureLayout::UNDEFINED,
        gfx::TextureLayout::COLOR_ATTACHMENT_OPTIMAL
    };

    for (int i = 0; i < 4; i++) {
        info.colorAttachments.push_back(color);
    }

    gfx::DepthStencilAttachment depth = {
        device->getDepthStencilFormat(),
        1,
        gfx::LoadOp::CLEAR,
        gfx::StoreOp::STORE,
        gfx::LoadOp::CLEAR,
        gfx::StoreOp::STORE,
        gfx::TextureLayout::UNDEFINED,
        gfx::TextureLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    info.depthStencilAttachment = depth;
    _gbufferRenderPass = device->createRenderPass(info);
}

void GbufferFlow::createRenderTargets(gfx::Device *device) {
    gfx::TextureInfo info = {
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::SAMPLED,
        gfx::Format::RGBA16F,
        _width,
        _height,
    };

    if (_gbufferRenderTargets.empty()) {
        for (int i = 0; i < 4; i++) {
            gfx::Texture *tex = device->createTexture(info);
            _gbufferRenderTargets.push_back(tex);
        }
    }

    if (!_depth) {
        info.usage = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT;
        info.format = device->getDepthStencilFormat();
        _depth = device->createTexture(info);

        DeferredPipeline *pp = dynamic_cast<DeferredPipeline*>(_pipeline);
        pp->setDepth(_depth);
    }

    if (!_gbufferFrameBuffer) {
        gfx::FramebufferInfo fbInfo = {
            _gbufferRenderPass,
            _gbufferRenderTargets,
            _depth,
        };

        _gbufferFrameBuffer = device->createFramebuffer(fbInfo);
    }
}

void GbufferFlow::activate(RenderPipeline *pipeline) {
    RenderFlow::activate(pipeline);

    gfx::Device *device = pipeline->getDevice();
    if (device->getSurfaceTransform() == gfx::SurfaceTransform::IDENTITY || 
        device->getSurfaceTransform() == gfx::SurfaceTransform::ROTATE_180) {
            _width = device->getWidth();
            _height = device->getHeight();
    }
    else {
            _width = device->getHeight();
            _height = device->getWidth();
    }

    createRenderPass(device);
    createRenderTargets(device);

    pipeline->getDescriptorSet()->bindTexture(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_ALBEDOMAP), _gbufferFrameBuffer->getColorTextures()[0]);
    pipeline->getDescriptorSet()->bindTexture(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_POSITIONMAP), _gbufferFrameBuffer->getColorTextures()[1]);
    pipeline->getDescriptorSet()->bindTexture(
       static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_NORMALMAP), _gbufferFrameBuffer->getColorTextures()[2]);
    pipeline->getDescriptorSet()->bindTexture(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_EMISSIVEMAP), _gbufferFrameBuffer->getColorTextures()[3]);

    gfx::SamplerInfo sInfo = {
        gfx::Filter::LINEAR,
        gfx::Filter::LINEAR,
        gfx::Filter::NONE,
        gfx::Address::CLAMP,
        gfx::Address::CLAMP,
        gfx::Address::CLAMP,
    };

    uint hash = genSamplerHash(sInfo);
    gfx::Sampler *gbufferSampler = getSampler(hash);
    pipeline->getDescriptorSet()->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_ALBEDOMAP), gbufferSampler);
    pipeline->getDescriptorSet()->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_POSITIONMAP), gbufferSampler);
    pipeline->getDescriptorSet()->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_NORMALMAP), gbufferSampler);
    pipeline->getDescriptorSet()->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_EMISSIVEMAP), gbufferSampler);
}

void GbufferFlow::render(Camera *camera) {
    auto pipeline = static_cast<DeferredPipeline *>(_pipeline);
    sceneCulling(pipeline, camera);
    pipeline->updateCameraUBO(camera, true);
    RenderFlow::render(camera);
}

void GbufferFlow::destroy() {
    RenderFlow::destroy();
}

} // namespace pipeline
} // namespace cc