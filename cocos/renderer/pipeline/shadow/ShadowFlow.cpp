/****************************************************************************
Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "ShadowFlow.h"

#include "../Define.h"
#include "../forward/ForwardPipeline.h"
#include "../helper/SharedMemory.h"
#include "ShadowStage.h"
#include "gfx/GFXDescriptorSet.h"
#include "gfx/GFXDevice.h"
#include "gfx/GFXFramebuffer.h"
#include "gfx/GFXRenderPass.h"
#include "gfx/GFXTexture.h"
#include "../SceneCulling.h"

namespace cc {
namespace pipeline {
RenderFlowInfo ShadowFlow::_initInfo = {
    "ShadowFlow",
    static_cast<uint>(ForwardFlowPriority::SHADOW),
    static_cast<uint>(RenderFlowTag::SCENE),
    {}};
const RenderFlowInfo &ShadowFlow::getInitializeInfo() { return ShadowFlow::_initInfo; }

ShadowFlow::~ShadowFlow() {
}

bool ShadowFlow::initialize(const RenderFlowInfo &info) {
    RenderFlow::initialize(info);
    if (_stages.empty()) {
        auto shadowStage = CC_NEW(ShadowStage);
        shadowStage->initialize(ShadowStage::getInitializeInfo());
        _stages.emplace_back(shadowStage);
    }

    return true;
}

void ShadowFlow::activate(RenderPipeline *pipeline) {
    RenderFlow::activate(pipeline);
}

void ShadowFlow::render(Camera *camera) {
    const auto sceneData = _pipeline->getPipelineSceneData();
    const auto *shadowInfo = sceneData->getSharedData()->getShadows();
    if (!shadowInfo->enabled || shadowInfo->getShadowType() != ShadowType::SHADOWMAP) return;

    lightCollecting(camera, _validLights);
    shadowCollecting(_pipeline, camera);
    if (sceneData->getShadowObjects().size() == 0) return;

    const auto &shadowFramebufferMap = sceneData->getShadowFramebufferMap();
    for (const auto *light : _validLights) {
        if (!shadowFramebufferMap.count(light)) {
            initShadowFrameBuffer(_pipeline, light);
        }

        auto *shadowFrameBuffer = shadowFramebufferMap.at(light);
        if (shadowInfo->shadowMapDirty) {
            resizeShadowMap(light, (uint)shadowInfo->size.x, (uint)shadowInfo->size.y);
        }
        for (auto *_stage : _stages) {
            auto *shadowStage = static_cast<ShadowStage *>(_stage);
            shadowStage->setUseData(light, shadowFrameBuffer);
            shadowStage->render(camera);
        }
    }

    // After the shadowMap rendering of all lights is completed,
    // restore the ShadowUBO data of the main light.
    _pipeline->getPipelineUBO()->updateShadowUBO(camera);
}

void ShadowFlow::resizeShadowMap(const Light *light, const uint width, const uint height) const {
    auto sceneData = _pipeline->getPipelineSceneData();

    if (sceneData->getShadowFramebufferMap().count(light)) {
        auto *framebuffer = sceneData->getShadowFramebufferMap().at(light);

        if (!framebuffer) {
            return;
        }

        auto &renderTargets = framebuffer->getColorTextures();
        for (auto *renderTarget : renderTargets) {
            renderTarget->resize(width, height);
        }

        auto *depth = framebuffer->getDepthStencilTexture();
        if (depth) {
            depth->resize(width, height);
        }

        framebuffer->destroy();
        framebuffer->initialize({
            _renderPass,
            renderTargets,
            depth,
            {},
        });
    }
}

void ShadowFlow::initShadowFrameBuffer(RenderPipeline *pipeline, const Light *light) {
    const auto device = gfx::Device::getInstance();
    const auto sceneData = _pipeline->getPipelineSceneData();
    const auto *shadowInfo = sceneData->getSharedData()->getShadows();
    const auto shadowMapSize = shadowInfo->size;
    const auto width = (uint)shadowMapSize.x;
    const auto height = (uint)shadowMapSize.y;

    if (!_renderPass) {
        gfx::RenderPassInfo rpInfo;
        rpInfo.depthStencilAttachment.depthStoreOp = gfx::StoreOp::DISCARD;
        rpInfo.depthStencilAttachment.stencilStoreOp = gfx::StoreOp::DISCARD;
        _renderPass = device->createRenderPass(rpInfo);
    }

    vector<gfx::Texture *> renderTargets;
    renderTargets.emplace_back(device->createTexture({
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::SAMPLED,
        gfx::Format::RGBA8,
        width,
        height,
    }));

    gfx::Texture *depth = device->createTexture({
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT,
        device->getDepthStencilFormat(),
        width,
        height,
    });

    gfx::Framebuffer *framebuffer = device->createFramebuffer({
        _renderPass,
        renderTargets,
        depth,
        {}, //colorMipmapLevels
    });

    pipeline->getPipelineSceneData()->setShadowFramebuffer(light, framebuffer);
}

void ShadowFlow::destroy() {
    if (_renderPass) {
        _renderPass->destroy();
        _renderPass = nullptr;
    }

    _validLights.clear();

    RenderFlow::destroy();
}

} // namespace pipeline
} // namespace cc
