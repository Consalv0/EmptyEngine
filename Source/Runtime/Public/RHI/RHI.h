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
        RHIDevice device;
        RHIInstance instance;

    public:
        virtual ~DynamicRHI() = default;

        virtual EDynamicRHI GetType() const = 0;

        virtual bool Initialize() = 0;

        virtual bool CreatePresentContext( Window* window, RHIPresentContext& context ) = 0;
        virtual bool CreateSurface( const SurfaceDescription& description, RHISurface& outWindowSurface ) const = 0;
        virtual bool CreateSwapChain( const SwapChainDescription& description, class Window* window, RHISwapChain& outSwapChain ) const = 0;
        virtual bool CreateCommandBuffer( const CommandBufferDescription& description, RHICommandBuffer& outCommandBuffer ) const = 0;
        virtual bool CreateBuffer( const BufferDescription& description, RHIBuffer& outBuffer ) const = 0;
        virtual bool CreateTexture( const TextureDescription& description, RHITexture& outTexture ) const = 0;
        virtual bool CreateSampler( const SamplerDescription& description, RHISampler& outSampler ) const = 0;
        virtual bool CreateShaderStage( EShaderStage stage, const void* pShaderBytecode, size_t bytecodeLength, RHIShaderStage& outShaderStage ) const = 0;

        // virtual void BindScissorRects( uint32 numRects, const Box2* rects ) = 0;
        // // virtual void BindViewports( uint32 NumViewports, const Viewport* pViewports ) = 0;
        // virtual void BindResource( EShaderStage stage, const RHIResource* resource, uint32 slot, int subresource = -1 ) = 0;
        // virtual void BindResources( EShaderStage stage, const RHIResource* const* resources, uint32 slot, uint32 count ) = 0;
        // virtual void UnbindResources( uint32 slot, uint32 num ) = 0;
        // virtual void UnbindUAVs( uint32 slot, uint32 num ) = 0;
        // virtual void BindSampler( EShaderStage stage, const RHISampler* sampler, uint32 slot ) = 0;
        // virtual void BindConstantBuffer( EShaderStage stage, const RHIBuffer* buffer, uint32 slot ) = 0;
        // virtual void BindVertexBuffers( const RHIBuffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets ) = 0;
        // virtual void BindIndexBuffer( const RHIBuffer* indexBuffer, uint32 offset ) = 0;
        // virtual void BindStencilRef( uint32 value ) = 0;
        // virtual void BindBlendFactor( float r, float g, float b, float a ) = 0;
        // virtual void Clear( EClearFlags flags, Vector4f clearColor, float depth, uint64 stencil ) = 0;
        // virtual void Draw( uint32 vertexCount, uint32 startVertexLocation ) = 0;
        // virtual void DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation ) = 0;
        // virtual void DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation ) = 0;
        // virtual void DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation ) = 0;
        // virtual void DrawInstancedIndirect( const RHIBuffer* args, uint32 args_offset ) = 0;
        // virtual void DrawIndexedInstancedIndirect( const RHIBuffer* args, uint32 args_offset ) = 0;
        // virtual void CopyResource( const RHIResource* pDst, const RHIResource* pSrc ) = 0;
        // virtual void UpdateBuffer( const RHIBuffer* buffer, const void* data, int dataSize = -1 ) = 0;
        // virtual void UpdateTexture( const RHITexture& texture, const void* data ) = 0;
        // // virtual void QueryBegin( const GPUQueryHeap* heap, uint32 index ) = 0;
        // // virtual void QueryEnd( const GPUQueryHeap* heap, uint32 index ) = 0;
        // // virtual void QueryResolve( const GPUQueryHeap* heap, uint32 index, uint32 count ) {}
        // // virtual void Barrier( const GPUBarrier* barriers, uint32 numBarriers ) = 0;
        // virtual void PushConstants( const void* data, uint32 size ) {}
        // 
        // virtual void EventBegin( const EName& name ) = 0;
        // virtual void EventEnd() = 0;
        // virtual void SetMarker( const EName& name ) = 0;

        virtual EShaderFormat GetShaderFormat() const { return ShaderFormat_None; }

        virtual RHITexture* GetBackBuffer( const RHISwapChain& swapchain ) const = 0;

        const RHIDevice& GetDevice() { return device; };
        const RHIInstance& GetInstance() { return instance; };
        virtual WString const& GetName() const = 0;
    };

    extern DynamicRHI* GDynamicRHI;

    const RHIPresentContext& GetPresentContextOfWindow( Window* window );

    const RHIPresentContext& CreatePresentContextOfWindow( Window* window );

    void FreePresentContextOfWindow( Window* window );

    //* Called to create the instance of the dynamic RHI,
    //* this is implemented for each platform.
    DynamicRHI* PlatformCreateDynamicRHI();
}