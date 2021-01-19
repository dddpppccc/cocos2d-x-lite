#pragma once

#include <array>

#include "../RenderPipeline.h"
#include "../helper/SharedMemory.h"
#include "../../core/gfx/GFXBuffer.h"
#include "../../core/gfx/GFXInputAssembler.h"

namespace cc {
namespace pipeline {
struct UBOGlobal;
struct UBOShadow;
struct Fog;
struct Ambient;
struct Skybox;
struct Shadows;
struct Sphere;
class Framebuffer;
class Camera;

class CC_DLL DeferredPipeline : public RenderPipeline {
public:
    DeferredPipeline() = default;
    ~DeferredPipeline() = default;

    virtual bool initialize(const RenderPipelineInfo &info) override;
    virtual void destroy() override;
    virtual bool activate() override;
    virtual void render(const vector<uint> &cameras) override;
    void updateGlobalUBO();
    void updateCameraUBO(Camera *camera, bool hasOffScreenAttachments = false);
    void updateShadowUBO(Camera *camera);
    CC_INLINE void setHDR(bool isHDR) { _isHDR = isHDR; }

    gfx::RenderPass *getOrCreateRenderPass(gfx::ClearFlags clearFlags);
    void setFog(uint);
    void setAmbient(uint);
    void setSkybox(uint);
    //void setShadows(uint);
        void destroyShadowFrameBuffers();
    CC_INLINE void setShadowFramebuffer(const Light *light, gfx::Framebuffer *framebuffer) { _shadowFrameBufferMap.emplace(light, framebuffer); }
    CC_INLINE const std::unordered_map<const Light *, gfx::Framebuffer *> &getShadowFramebufferMap() const { return _shadowFrameBufferMap; }
    gfx::InputAssembler *getQuadIAOnScreen(){return _quadIAOnscreen;}
    gfx::InputAssembler *getQuadIAOffScreen(){return _quadIAOffscreen;}

    CC_INLINE gfx::Buffer *getLightsUBO() const { return _lightsUBO; }
    CC_INLINE const LightList &getValidLights() const { return _validLights; }
    CC_INLINE const gfx::BufferList &getLightBuffers() const { return _lightBuffers; }
    CC_INLINE const UintList &getLightIndexOffsets() const { return _lightIndexOffsets; }
    CC_INLINE const UintList &getLightIndices() const { return _lightIndices; }
    CC_INLINE const RenderObjectList &getRenderObjects() const { return _renderObjects; }
    CC_INLINE const RenderObjectList &getShadowObjects() const { return _shadowObjects; }
    CC_INLINE const gfx::CommandBufferList &getCommandBuffers() const { return _commandBuffers; }
    CC_INLINE float getShadingScale() const { return _shadingScale; }
    CC_INLINE float getFpScale() const { return _fpScale; }
    CC_INLINE bool isHDR() const { return _isHDR; }
    CC_INLINE const Fog *getFog() const { return _fog; }
    CC_INLINE const Ambient *getAmbient() const { return _ambient; }
    CC_INLINE const Skybox *getSkybox() const { return _skybox; }
    //CC_INLINE Shadows *getShadows() const { return _shadows; }
    CC_INLINE Sphere *getSphere() const { return _sphere; }
    CC_INLINE std::array<float, UBOShadow::COUNT> getShadowUBO() const { return _shadowUBO; }

    void setRenderObjects(const RenderObjectList &ro) { _renderObjects = std::move(ro); }
    void setShadowObjects(const RenderObjectList &ro) { _shadowObjects = std::move(ro); }
    float getFpScale() {return _fpScale;}

    void setDepth(gfx::Texture *tex) {_depth = tex;}
    gfx::Texture *getDepth(){return _depth;}
    gfx::Rect getRenderArea(Camera *view, bool onScreen);

private:
    bool activeRenderer();
    bool createQuadInputAssembler(gfx::Buffer* &quadIB, gfx::Buffer* &quadVB, gfx::InputAssembler* &quadIA,
        gfx::SurfaceTransform surfaceTransform);
    void destroyQuadInputAssembler();

private:
    const Fog *_fog = nullptr;
    const Ambient *_ambient = nullptr;
    const Skybox *_skybox = nullptr;
    //Shadows *_shadows = nullptr;
    gfx::Buffer *_lightsUBO = nullptr;
    LightList _validLights;
    gfx::BufferList _lightBuffers;
    UintList _lightIndexOffsets;
    UintList _lightIndices;
    RenderObjectList _renderObjects;
    RenderObjectList _shadowObjects;
    map<gfx::ClearFlags, gfx::RenderPass *> _renderPasses;
    std::array<float, UBOGlobal::COUNT> _globalUBO;
    std::array<float, UBOCamera::COUNT> _cameraUBO;
    std::array<float, UBOShadow::COUNT> _shadowUBO;
    Sphere *_sphere = nullptr;

    // light stage
    gfx::Buffer *_quadIB = nullptr;
    gfx::Buffer *_quadVBOnscreen = nullptr;
    gfx::Buffer *_quadVBOffscreen = nullptr;
    gfx::InputAssembler *_quadIAOnscreen = nullptr;
    gfx::InputAssembler *_quadIAOffscreen = nullptr;

    gfx::Texture *_depth = nullptr;

    float _shadingScale = 1.0f;
    bool _isHDR = false;
    float _fpScale = 1.0f / 1024.0f;

    std::unordered_map<const Light *, gfx::Framebuffer *> _shadowFrameBufferMap;
};

} // namespace pipeline
} // namespace cc
