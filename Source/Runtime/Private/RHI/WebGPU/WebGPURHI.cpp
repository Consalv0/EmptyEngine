
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Utils/Hasher.h"

#include "RHI/WebGPU/WebGPURHI.h"
#include "RHI/WebGPU/WebGPU.h"
#include <RHI/WebGPU/WebGPURHIObjects.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <set>
#include <map>

namespace EE
{
    WebGPURHI::~WebGPURHI()
    {
    }

    EDynamicRHI WebGPURHI::GetType() const
    {
        return EDynamicRHI::WebGPU;
    }

    WebGPURHI::WebGPURHI() : DynamicRHI()
    {
    }

    void WebGPURHI::SetName( RHIResource* pResource, const U8Char* name ) { }

    void WebGPURHI::WaitForDevice() const { }

    const RHIDevice* WebGPURHI::GetRHIDevice() const
    {
        return NULL;
    }

    const U8String& WebGPURHI::GetName() const
    {
        static U8String const deviceName = "WebGPURHI";
        return deviceName;
    }

    RHIPresentContext* WebGPURHI::CreateRHIPresentContext( Window* window ) const
    {
        return NULL;
    }

    RHISurface* WebGPURHI::CreateRHISurface( Window* window ) const
    {
        return NULL;
    }

    RHICommandBuffer* WebGPURHI::CreateRHICommandBuffer( const RHICommandBufferCreateInfo& info ) const
    {
        return NULL;
    }
    
    RHIBuffer* WebGPURHI::CreateRHIBuffer( const RHIBufferCreateInfo& info ) const
    {
        return NULL;
    }

    RHITexture* WebGPURHI::CreateRHITexture( const RHITextureCreateInfo& info ) const
    {
        return NULL;
    }

    RHITextureView* WebGPURHI::CreateRHITextureView( const RHITextureViewCreateInfo& info ) const
    {
        return NULL;
    }
    
    RHISampler* WebGPURHI::CreateRHISampler( const RHISamplerCreateInfo& info ) const
    {
        return NULL;
    }
    
    RHIShaderStage* WebGPURHI::CreateRHIShaderStage( const RHIShaderStageCreateInfo& info ) const
    {
        return NULL;
    }

    RHIBindGroup* WebGPURHI::CreateRHIBindGroup( const RHIBindGroupCreateInfo& info ) const
    {
        return NULL;
    }

    RHIRenderPass* WebGPURHI::CreateRHIRenderPass( const RHIRenderPassCreateInfo& info ) const
    {
        return NULL;
    }
    
    RHIGraphicsPipeline* WebGPURHI::CreateRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info ) const
    {
        return NULL;
    }

    void* GetWebGPURHIInstance()
    {
        return NULL;
    }
}