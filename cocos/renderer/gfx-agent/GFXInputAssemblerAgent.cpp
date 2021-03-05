/****************************************************************************
 Copyright (c) 2020-2021 Xiamen Yaji Software Co., Ltd.

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

#include "base/CoreStd.h"
#include "base/threading/MessageQueue.h"

#include "GFXBufferAgent.h"
#include "GFXDeviceAgent.h"
#include "GFXInputAssemblerAgent.h"

namespace cc {
namespace gfx {

InputAssemblerAgent::~InputAssemblerAgent() {
    ENQUEUE_MESSAGE_1(
        ((DeviceAgent *)_device)->getMessageQueue(),
        InputAssemblerDestruct,
        actor, _actor,
        {
            CC_DELETE(actor);
        });
}

bool InputAssemblerAgent::initialize(const InputAssemblerInfo &info) {
    _attributes = info.attributes;
    _vertexBuffers = info.vertexBuffers;
    _indexBuffer = info.indexBuffer;
    _indirectBuffer = info.indirectBuffer;

    if (_indexBuffer) {
        _indexCount = _indexBuffer->getCount();
        _firstIndex = 0;
    } else if (_vertexBuffers.size()) {
        _vertexCount = _vertexBuffers[0]->getCount();
        _firstVertex = 0;
        _vertexOffset = 0;
    }
    _attributesHash = computeAttributesHash();

    InputAssemblerInfo actorInfo = info;
    for (uint i = 0u; i < actorInfo.vertexBuffers.size(); ++i) {
        actorInfo.vertexBuffers[i] = ((BufferAgent *)actorInfo.vertexBuffers[i])->getActor();
    }
    if (actorInfo.indexBuffer) {
        actorInfo.indexBuffer = ((BufferAgent *)actorInfo.indexBuffer)->getActor();
    }
    if (actorInfo.indirectBuffer) {
        actorInfo.indirectBuffer = ((BufferAgent *)actorInfo.indirectBuffer)->getActor();
    }

    ENQUEUE_MESSAGE_2(
        ((DeviceAgent *)_device)->getMessageQueue(),
        InputAssemblerInit,
        actor, getActor(),
        info, actorInfo,
        {
            actor->initialize(info);
        });

    return true;
}

void InputAssemblerAgent::destroy() {
    ENQUEUE_MESSAGE_1(
        ((DeviceAgent *)_device)->getMessageQueue(),
        InputAssemblerDestroy,
        actor, getActor(),
        {
            actor->destroy();
        });
}

} // namespace gfx
} // namespace cc
