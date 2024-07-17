#pragma once

#include "Engine/Window.h"
#include "Rendering/Common.h"
#include "Math/CoreMath.h"
#include "Core/Name.h"

#include "RHI/RHIResources.h"

namespace EE
{
    //* Base Render Hardware Interface
    class DynamicRHI
    {
    protected:

    public:
        DynamicRHI();
        virtual ~DynamicRHI() = default;

        virtual EDynamicRHI GetType() const = 0;

        virtual RHIPresentContext* CreateRHIPresentContext( Window* window ) const = 0;
        virtual RHISurface* CreateRHISurface( Window* window ) const = 0;
        virtual RHISwapChain* CreateRHISwapChain( const RHISwapChainCreateDescription& description, class Window* window ) const = 0;
        virtual RHICommandBuffer* CreateRHICommandBuffer( const RHICommandBufferCreateDescription& description ) const = 0;
        virtual RHIBuffer* CreateRHIBuffer( const RHIBufferCreateDescription& description ) const = 0;
        virtual RHITexture* CreateRHITexture( const RHITextureCreateDescription& description ) const = 0;
        virtual RHISampler* CreateRHISampler( const RHISamplerCreateDescription& description ) const = 0;
        virtual RHIShaderStage* CreateRHIShaderStage( EShaderStage stage, const void* pShaderBytecode, size_t bytecodeLength ) const = 0;

        virtual const RHIDevice* GetRHIDevice() const = 0;

        virtual EShaderFormat GetShaderFormat() const = 0;

        virtual WString const& GetName() const = 0;

        const RHIPresentContext* GetRHIPresentContextOfWindow( Window* window ) const;

        const RHIPresentContext* CreateRHIPresentContextOfWindow( Window* window ) const;

        void FreeRHIPresentContextOfWindow( Window* window ) const;
    };

    extern DynamicRHI* GDynamicRHI;

    //* Called to create the instance of the dynamic RHI,
    //* this is implemented for each platform.
    DynamicRHI* PlatformCreateDynamicRHI( EDynamicRHI rhi );
}