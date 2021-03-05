/****************************************************************************
 Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "DeferredPipeline.h"
#include "../helper/SharedMemory.h"
#include "GbufferFlow.h"
#include "LightingFlow.h"
#include "../shadow/ShadowFlow.h"
#include "../SceneCulling.h"
#include "gfx-base/GFXBuffer.h"
#include "gfx-base/GFXCommandBuffer.h"
#include "gfx-base/GFXDescriptorSet.h"
#include "gfx-base/GFXDevice.h"
#include "gfx-base/GFXQueue.h"
#include "gfx-base/GFXRenderPass.h"
#include "gfx-base/GFXTexture.h"
#include "gfx-base/GFXFramebuffer.h"
#include "gfx-base/GFXSampler.h"
#include "platform/Application.h"

namespace cc {
namespace pipeline {
namespace {
#define TO_VEC3(dst, src, offset) \
    dst[offset] = src.x;          \
    dst[offset + 1] = src.y;      \
    dst[offset + 2] = src.z;
#define TO_VEC4(dst, src, offset) \
    dst[offset] = src.x;          \
    dst[offset + 1] = src.y;      \
    dst[offset + 2] = src.z;      \
    dst[offset + 3] = src.w;
} // namespace

gfx::RenderPass *DeferredPipeline::getOrCreateRenderPass(gfx::ClearFlags clearFlags) {
    if (_renderPasses.find(clearFlags) != _renderPasses.end()) {
        return _renderPasses[clearFlags];
    }

    auto device = gfx::Device::getInstance();
    gfx::ColorAttachment colorAttachment;
    gfx::DepthStencilAttachment depthStencilAttachment;
    colorAttachment.format = device->getColorFormat();
    depthStencilAttachment.format = device->getDepthStencilFormat();
    depthStencilAttachment.stencilStoreOp = gfx::StoreOp::DISCARD;
    depthStencilAttachment.depthStoreOp = gfx::StoreOp::DISCARD;

    if (!(clearFlags & gfx::ClearFlagBit::COLOR)) {
        if (clearFlags & static_cast<gfx::ClearFlagBit>(SKYBOX_FLAG)) {
            colorAttachment.loadOp = gfx::LoadOp::DISCARD;
        } else {
            colorAttachment.loadOp = gfx::LoadOp::LOAD;
            colorAttachment.beginAccesses = {gfx::AccessType::PRESENT};
        }
    }

    if (static_cast<gfx::ClearFlagBit>(clearFlags & gfx::ClearFlagBit::DEPTH_STENCIL) != gfx::ClearFlagBit::DEPTH_STENCIL) {
        if (!(clearFlags & gfx::ClearFlagBit::DEPTH)) depthStencilAttachment.depthLoadOp = gfx::LoadOp::LOAD;
        if (!(clearFlags & gfx::ClearFlagBit::STENCIL)) depthStencilAttachment.stencilLoadOp = gfx::LoadOp::LOAD;
        depthStencilAttachment.beginAccesses = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};
    }

    auto renderPass = device->createRenderPass({
        {colorAttachment},
        depthStencilAttachment,
    });
    _renderPasses[clearFlags] = renderPass;

    return renderPass;
}

bool DeferredPipeline::initialize(const RenderPipelineInfo &info) {
    RenderPipeline::initialize(info);

    if (_flows.size() == 0) {
        auto shadowFlow = CC_NEW(ShadowFlow);
        shadowFlow->initialize(ShadowFlow::getInitializeInfo());
        _flows.emplace_back(shadowFlow);

        auto gbufferFlow = CC_NEW(GbufferFlow);
        gbufferFlow->initialize(GbufferFlow::getInitializeInfo());
        _flows.emplace_back(gbufferFlow);

        auto lightingFlow = CC_NEW(LightingFlow);
        lightingFlow->initialize(LightingFlow::getInitializeInfo());
        _flows.emplace_back(lightingFlow);
    }

    return true;
}

bool DeferredPipeline::activate() {
    _macros.setValue("CC_PIPELINE_TYPE", static_cast<float>(1.0));

    if (!RenderPipeline::activate()) {
        CC_LOG_ERROR("RenderPipeline active failed.");
        return false;
    }

    if (!activeRenderer()) {
        CC_LOG_ERROR("DeferredPipeline startup failed!");
        return false;
    }

    return true;
}

void DeferredPipeline::render(const vector<uint> &cameras) {
	if (cameras.size() == 0) return;
    _commandBuffers[0]->begin();
    _pipelineUBO->updateGlobalUBO();
    for (const auto cameraId: cameras) {
        Camera *camera = GET_CAMERA(cameraId);
        _pipelineUBO->updateCameraUBO(camera, true);
        for (const auto flow : _flows) {
            flow->render(camera);
        }
    }
    _commandBuffers[0]->end();
    _device->getQueue()->submit(_commandBuffers);
}

bool DeferredPipeline::createQuadInputAssembler(gfx::Buffer* &quadIB, gfx::Buffer* &quadVB, gfx::InputAssembler* &quadIA, gfx::SurfaceTransform surfaceTransform) {
    // step 1 create vertex buffer
    uint vbStride = sizeof(float) * 4;
    uint vbSize = vbStride * 4;
    quadVB = _device->createBuffer({gfx::BufferUsageBit::VERTEX | gfx::BufferUsageBit::TRANSFER_DST,
              gfx::MemoryUsageBit::HOST | gfx::MemoryUsageBit::DEVICE, vbSize, vbStride});
    if (!quadVB) {
        return false;
    }

    float vbData[16] = {0};
    int n = 0;
    switch (surfaceTransform) {
        case (gfx::SurfaceTransform::IDENTITY):
            n = 0;
            vbData[n++] = -1.0; vbData[n++] = -1.0; vbData[n++] = 0.0; vbData[n++] = 1.0;
            vbData[n++] = 1.0; vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 1.0;
            vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 0.0; vbData[n++] = 0.0;
            vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 0.0;
            break;
        case (gfx::SurfaceTransform::ROTATE_90):
            n = 0;
            vbData[n++] = -1.0; vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 1.0;
            vbData[n++] = 1.0; vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 0.0;
            vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 0.0; vbData[n++] = 1.0;
            vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 0.0; vbData[n++] = 0.0;
            break;
        case (gfx::SurfaceTransform::ROTATE_180):
            n = 0;
            vbData[n++] = -1.0; vbData[n++] = -1.0; vbData[n++] = 0.0; vbData[n++] = 0.0;
            vbData[n++] = 1.0; vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 0.0;
            vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 0.0; vbData[n++] = 1.0;
            vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 1.0;
            break;
        case (gfx::SurfaceTransform::ROTATE_270):
            n = 0;
            vbData[n++] = -1.0; vbData[n++] = -1.0; vbData[n++] = 0.0; vbData[n++] = 0.0;
            vbData[n++] = 1.0; vbData[n++] = -1.0; vbData[n++] = 0.0; vbData[n++] = 1.0;
            vbData[n++] = -1.0; vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 0.0;
            vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 1.0; vbData[n++] = 1.0;
            break;
        default:
            break;
    }

    quadVB->update(vbData, sizeof(vbData));

    // step 2 create index buffer
    uint ibStride = 4;
    uint ibSize = ibStride * 6;

    quadIB =_device->createBuffer({gfx::BufferUsageBit::INDEX | gfx::BufferUsageBit::TRANSFER_DST,
             gfx::MemoryUsageBit::HOST | gfx::MemoryUsageBit::DEVICE, ibSize, ibStride});

    if (!quadIB) {
        return false;
    }

    unsigned int ibData[] = {0, 1, 2, 1, 3, 2};
    quadIB->update(ibData, sizeof(ibData));

    // step 3 create input assembler
    gfx::InputAssemblerInfo info;
    info.attributes.push_back({"a_position", gfx::Format::RG32F});
    info.attributes.push_back({"a_texCoord", gfx::Format::RG32F});
    info.vertexBuffers.push_back(quadVB);
    info.indexBuffer = quadIB;
    quadIA = _device->createInputAssembler(info);
    if (!quadIA) {
        return false;
    }

    return true;
}

gfx::Rect DeferredPipeline::getRenderArea(Camera *camera, bool onScreen) {
    gfx::Rect renderArea;
    uint w, h;
    if (onScreen) {
        w = camera->getWindow()->hasOnScreenAttachments && (uint)_device->getSurfaceTransform() % 2 ? camera->height : camera->width;
        h = camera->getWindow()->hasOnScreenAttachments && (uint)_device->getSurfaceTransform() % 2 ? camera->width : camera->height;
    }
    else {
        w = camera->width;
        h = camera->height;
    }

    renderArea.x = camera->viewportX * w;
    renderArea.y = camera->viewportY * h;
    renderArea.width = camera->viewportWidth * w * _pipelineSceneData->getSharedData()->shadingScale;
    renderArea.height = camera->viewportHeight * h * _pipelineSceneData->getSharedData()->shadingScale;
    return renderArea;
}

DeferredRenderData *DeferredPipeline::getDeferredRenderData(Camera *view) {
    if (_deferredRenderDatas.find(view) == _deferredRenderDatas.end()) {
        generateDeferredRenderData(view);
    }
    
    return _deferredRenderDatas[view];
}

void DeferredPipeline::destroyQuadInputAssembler() {
    if (_quadIB) {
        _quadIB->destroy();
        _quadIB = nullptr;
    }

    if (_quadVBOnscreen) {
        _quadVBOnscreen->destroy();
        _quadVBOnscreen = nullptr;
    }

    if (_quadVBOffscreen) {
        _quadVBOffscreen->destroy();
        _quadVBOffscreen = nullptr;
    }

    if (_quadIAOnscreen) {
        _quadIAOnscreen->destroy();
        _quadIAOnscreen = nullptr;
    }

    if (_quadIAOffscreen) {
        _quadIAOffscreen->destroy();
        _quadIAOffscreen = nullptr;
    }
}

bool DeferredPipeline::activeRenderer() {
    _commandBuffers.push_back(_device->getCommandBuffer());
    const auto sharedData = _pipelineSceneData->getSharedData();

    gfx::SamplerInfo info{
        gfx::Filter::LINEAR,
        gfx::Filter::LINEAR,
        gfx::Filter::NONE,
        gfx::Address::CLAMP,
        gfx::Address::CLAMP,
        gfx::Address::CLAMP,
    };
    const auto samplerHash = genSamplerHash(std::move(info));
    const auto sampler = getSampler(samplerHash);

    // Main light sampler binding
    this->_descriptorSet->bindSampler(SHADOWMAP::BINDING, sampler);
    this->_descriptorSet->bindTexture(SHADOWMAP::BINDING, getDefaultTexture());

    // Spot light sampler binding
    this->_descriptorSet->bindSampler(SPOT_LIGHTING_MAP::BINDING, sampler);
    this->_descriptorSet->bindTexture(SPOT_LIGHTING_MAP::BINDING, getDefaultTexture());

    _descriptorSet->update();

    // update global defines when all states initialized.
    _macros.setValue("CC_USE_HDR", static_cast<bool>(sharedData->isHDR));
    _macros.setValue("CC_SUPPORT_FLOAT_TEXTURE", _device->hasFeature(gfx::Feature::TEXTURE_FLOAT));

    if (!createQuadInputAssembler(_quadIB, _quadVBOffscreen, _quadIAOffscreen, gfx::SurfaceTransform::IDENTITY)) {
        return false;
    }

    if (!createQuadInputAssembler(_quadIB, _quadVBOnscreen, _quadIAOnscreen, _device->getSurfaceTransform())) {
        return false;
    }
    
    gfx::RenderPassInfo gbufferPass;

    gfx::ColorAttachment color = {
        gfx::Format::RGBA16F,
        gfx::SampleCount::X1,
        gfx::LoadOp::CLEAR,
        gfx::StoreOp::STORE,
        {},
        {gfx::AccessType::COLOR_ATTACHMENT_WRITE},
    };

    for (int i = 0; i < 4; i++) {
        gbufferPass.colorAttachments.push_back(color);
    }

    gfx::DepthStencilAttachment depth = {
        _device->getDepthStencilFormat(),
        gfx::SampleCount::X1,
        gfx::LoadOp::CLEAR,
        gfx::StoreOp::STORE,
        gfx::LoadOp::CLEAR,
        gfx::StoreOp::STORE,
    };

    gbufferPass.depthStencilAttachment = depth;
    _gbufferRenderPass = _device->createRenderPass(gbufferPass);
    
    gfx::ColorAttachment cAttch = {
        gfx::Format::RGBA16F,
        gfx::SampleCount::X1,
        gfx::LoadOp::CLEAR,
        gfx::StoreOp::STORE,
        {},
        {gfx::AccessType::COLOR_ATTACHMENT_WRITE},
    };
    
    gfx::RenderPassInfo lightPass;
    lightPass.colorAttachments.push_back(cAttch);
    
    lightPass.depthStencilAttachment = {
        _device->getDepthStencilFormat(),
        gfx::SampleCount::X1,
        gfx::LoadOp::LOAD,
        gfx::StoreOp::DISCARD,
        gfx::LoadOp::DISCARD,
        gfx::StoreOp::DISCARD,
        {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE},
    };

    _lightingRenderPass = _device->createRenderPass(lightPass);
    
    _descriptorSet->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_ALBEDOMAP), sampler);
    _descriptorSet->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_POSITIONMAP), sampler);
    _descriptorSet->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_NORMALMAP), sampler);
    _descriptorSet->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_EMISSIVEMAP), sampler);
    _descriptorSet->bindSampler(
        static_cast<uint>(PipelineGlobalBindings::SAMPLER_LIGHTING_RESULTMAP), sampler);
    
    if (_device->getSurfaceTransform() == gfx::SurfaceTransform::IDENTITY ||
        _device->getSurfaceTransform() == gfx::SurfaceTransform::ROTATE_180) {
            _width = _device->getWidth();
            _height = _device->getHeight();
    }
    else {
            _width = _device->getHeight();
            _height = _device->getWidth();
    }

    return true;
}

void DeferredPipeline::generateDeferredRenderData(Camera *view) {
    DeferredRenderData* data = CC_NEW(DeferredRenderData);
    
    gfx::TextureInfo info = {
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::SAMPLED,
        gfx::Format::RGBA16F,
        _width,
        _height,
    };

    for (int i = 0; i < 4; i++) {
        gfx::Texture *tex = _device->createTexture(info);
        data->gbufferRenderTargets.push_back(tex);
    }

    info.usage = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT;
    info.format = _device->getDepthStencilFormat();
    data->depthTex = _device->createTexture(info);

    gfx::FramebufferInfo gbufferInfo = {
        _gbufferRenderPass,
        data->gbufferRenderTargets,
        data->depthTex,
    };

    data->gbufferFrameBuffer = _device->createFramebuffer(gbufferInfo);
    
    gfx::TextureInfo rtInfo = {
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::SAMPLED,
        gfx::Format::RGBA16F,
        _width,
        _height,
    };
    data->lightingRenderTarget = _device->createTexture(rtInfo);

    gfx::FramebufferInfo lightingInfo;
    lightingInfo.renderPass = _lightingRenderPass;
    lightingInfo.colorTextures.push_back(data->lightingRenderTarget);
    lightingInfo.depthStencilTexture = data->depthTex;
    data->lightingFrameBuff = _device->createFramebuffer(lightingInfo);

    _deferredRenderDatas[view] = data;
}

void DeferredPipeline::destroy() {
    destroyQuadInputAssembler();

    if (_descriptorSet) {
        _descriptorSet->getBuffer(UBOGlobal::BINDING)->destroy();
        _descriptorSet->getBuffer(UBOCamera::BINDING)->destroy();
        _descriptorSet->getBuffer(UBOShadow::BINDING)->destroy();
        _descriptorSet->getSampler(SHADOWMAP::BINDING)->destroy();
        _descriptorSet->getTexture(SHADOWMAP::BINDING)->destroy();
        _descriptorSet->getSampler(SPOT_LIGHTING_MAP::BINDING)->destroy();
        _descriptorSet->getTexture(SPOT_LIGHTING_MAP::BINDING)->destroy();
    }

    for (auto &it : _renderPasses) {
        it.second->destroy();
    }
    _renderPasses.clear();
    
    _deferredRenderDatas.clear();

    _commandBuffers.clear();
    
    _gbufferRenderPass = nullptr;
    _lightingRenderPass = nullptr;

    RenderPipeline::destroy();
}

} // namespace pipeline
} // namespace cc
