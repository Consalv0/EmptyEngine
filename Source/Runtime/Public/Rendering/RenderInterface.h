#pragma once

#include "Math/CoreMath.h"
#include "Rendering/Common.h"
#include "Rendering/Objects.h"

namespace EE
{
    typedef uint8_t CommandList;
    static const CommandList COMMANDLIST_COUNT = 32;
    static const CommandList INVALID_COMMAND_LIST = COMMANDLIST_COUNT;

    class RenderInterface
    {
    protected:
        RenderDevice device;
        RenderInstance instance;

    public:
        virtual ~RenderInterface() = default;

        virtual ERenderInterface GetType() const = 0;

        virtual bool Initialize() = 0;

        virtual bool CreatePresentContext( Window* window, PresentContext& context ) = 0;
        virtual bool CreateSurface( const SurfaceDescription& description, Surface& outWindowSurface ) const = 0;
        virtual bool CreateSwapChain( const SwapChainDescription& description, class Window* window, SwapChain& outSwapChain ) const = 0;
        virtual bool CreateBuffer( const BufferDescription& description, Buffer& outBuffer ) const = 0;
        virtual bool CreateTexture( const TextureDescription& description, Texture& outTexture ) const = 0;
        virtual bool CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const = 0;
        virtual bool CreateShaderStage( EShaderStage stage, const void* pShaderBytecode, size_t bytecodeLength, ShaderStage& outShaderStage ) const = 0;

        // Begin a new command list for GPU command recording.
        //	This will be valid until SubmitCommandLists() is called.
        virtual CommandList BeginCommandList( ECommandQueueType queue = ComandQueue_Graphics ) = 0;
        // Submit all command list that were used with BeginCommandList before this call.
        //	This will make every command list to be in "available" state and restarts them
        virtual void SubmitCommandLists() = 0;

        virtual void WaitForDevice() const = 0;
        virtual void ClearPipelineStateCache() {};

        virtual EShaderFormat GetShaderFormat() const { return ShaderFormat_None; }

        virtual Texture* GetBackBuffer( const SwapChain& swapchain ) const = 0;

        virtual void WaitCommandList( CommandList cmd, CommandList wait_for ) {}
        virtual void RenderPassBegin( const SwapChain& swapchain, CommandList cmd ) = 0;
        // virtual void RenderPassBegin( const RenderPass* renderpass, CommandList cmd ) = 0;
        virtual void RenderPassEnd( const SwapChain& swapchain, CommandList cmd ) = 0;
        virtual void BindScissorRects( uint32 numRects, const Box2* rects, CommandList cmd ) = 0;
        // virtual void BindViewports( uint32 NumViewports, const Viewport* pViewports, CommandList cmd ) = 0;
        virtual void BindResource( EShaderStage stage, const RenderResource* resource, uint32 slot, CommandList cmd, int subresource = -1 ) = 0;
        virtual void BindResources( EShaderStage stage, const RenderResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) = 0;
        virtual void BindUAV( EShaderStage stage, const RenderResource* resource, uint32 slot, CommandList cmd, int subresource = -1 ) = 0;
        virtual void BindUAVs( EShaderStage stage, const RenderResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) = 0;
        virtual void UnbindResources( uint32 slot, uint32 num, CommandList cmd ) = 0;
        virtual void UnbindUAVs( uint32 slot, uint32 num, CommandList cmd ) = 0;
        virtual void BindSampler( EShaderStage stage, const Sampler* sampler, uint32 slot, CommandList cmd ) = 0;
        virtual void BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32 slot, CommandList cmd ) = 0;
        virtual void BindVertexBuffers( const Buffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd ) = 0;
        virtual void BindIndexBuffer( const Buffer* indexBuffer, uint32 offset, CommandList cmd ) = 0;
        virtual void BindStencilRef( uint32 value, CommandList cmd ) = 0;
        virtual void BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) = 0;
        // virtual void BindShadingRate( SHADING_RATE rate, CommandList cmd ) {}
        // virtual void BindPipelineState( const PipelineState* pso, CommandList cmd ) = 0;
        virtual void Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd ) = 0;
        virtual void DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd ) = 0;
        virtual void DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd ) = 0;
        virtual void DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd ) = 0;
        virtual void DrawInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) = 0;
        virtual void DrawIndexedInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) = 0;
        virtual void Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) = 0;
        virtual void DispatchIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) = 0;
        virtual void DispatchMesh( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) {}
        virtual void DispatchMeshIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) {}
        virtual void CopyResource( const RenderResource* pDst, const RenderResource* pSrc, CommandList cmd ) = 0;
        virtual void UpdateBuffer( const Buffer* buffer, const void* data, CommandList cmd, int dataSize = -1 ) = 0;
        virtual void UpdateTexture( const Texture& texture, const void* data ) = 0;
        // virtual void QueryBegin( const GPUQueryHeap* heap, uint32 index, CommandList cmd ) = 0;
        // virtual void QueryEnd( const GPUQueryHeap* heap, uint32 index, CommandList cmd ) = 0;
        // virtual void QueryResolve( const GPUQueryHeap* heap, uint32 index, uint32 count, CommandList cmd ) {}
        // virtual void Barrier( const GPUBarrier* barriers, uint32 numBarriers, CommandList cmd ) = 0;
        virtual void PushConstants( const void* data, uint32 size, CommandList cmd ) {}

        virtual void EventBegin( const char* name, CommandList cmd ) = 0;
        virtual void EventEnd( CommandList cmd ) = 0;
        virtual void SetMarker( const char* name, CommandList cmd ) = 0;

        RenderDevice& GetDevice() { return device; };
        RenderInstance& GetInstance() { return instance; };
        virtual WString const& GetName() const = 0;
    };

    //* Creates a handler
    RenderInterface* CreateRenderingInterface();
}