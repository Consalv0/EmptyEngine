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
        virtual RHICommandBuffer* CreateRHICommandBuffer( const RHICommandBufferCreateInfo& info ) const = 0;
        virtual RHIBuffer* CreateRHIBuffer( const RHIBufferCreateInfo& info ) const = 0;
        virtual RHITexture* CreateRHITexture( const RHITextureCreateInfo& info ) const = 0;
        virtual RHISampler* CreateRHISampler( const RHISamplerCreateInfo& info ) const = 0;
        virtual RHIShaderStage* CreateRHIShaderStage( const RHIShaderStageCreateInfo& info ) const = 0;
        virtual RHIGraphicsPipeline* CreateRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info ) const = 0;

        virtual const RHIDevice* GetRHIDevice() const = 0;

        virtual EShaderFormat GetShaderFormat() const = 0;

        virtual WString const& GetName() const = 0;

        const RHIPresentContext* GetRHIPresentContextOfWindow( Window* window ) const;

        const RHIPresentContext* CreateRHIPresentContextOfWindow( Window* window ) const;

        void FreeRHIPresentContextOfWindow( Window* window ) const;

        void BeginWindowRender( Window* window ) const;

        void EndWindowRender( Window* window ) const;
    };

    extern DynamicRHI* GDynamicRHI;

    //* Called to create the instance of the dynamic RHI,
    //* this is implemented for each platform.
    DynamicRHI* PlatformCreateDynamicRHI( EDynamicRHI rhi );
}